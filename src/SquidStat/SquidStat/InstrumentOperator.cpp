#include "InstrumentOperator.h"
#include "ExternalStructures.h"

#include "Log.h"

#include <QEventLoop>
#include <QTimer>
#include <QSignalMapper>

InstrumentOperator::InstrumentOperator(const InstrumentInfo &info, QObject *parent) :
	QObject(parent)
{
	_communicator = new SerialCommunicator(info, this);
	_communicator->Start();

	connect(_communicator, &SerialCommunicator::ResponseReceived, 
		this, &InstrumentOperator::ResponseReceived);
}
InstrumentOperator::~InstrumentOperator() {
	_communicator->Stop();
}
void InstrumentOperator::ResponseReceived(ResponseID resp, quint8 channel, const QByteArray &data) {
	switch (resp) {
		case ADCDC_DATA:
			if (data.size() == sizeof(ExperimentalDcData)) {
				ExperimentalDcData *expData = (ExperimentalDcData*)data.data();
				emit ExperimentalDcDataReceived(channel, *expData);
			}
			break;

		case ADCAC_DATA: {
				int arrayDataLen = data.size() - sizeof(ExperimentalAcData);
				if( ( arrayDataLen % (sizeof(int16_t)*2) ) == 0) {
					emit ExperimentalAcDataReceived(channel, data);
				}
			}
			break;

		case DEBUG_LOG_MSG: {
				QByteArray strData = data;
				strData.push_back('\0');
				LOG() << QString(strData.data());
			}
			break;

		case EXPERIMENT_COMPLETE:
			emit ExperimentCompleted();
			break;
		
		case EXPERIMENT_PAUSED:
			emit ExperimentPaused();
			break;
		
		case EXPERIMENT_RESUMED:
			emit ExperimentResumed();
			break;

		case EXPERIMENT_NODE_COMPLETE:
			LOG() << "Node complete";
			break;

		case DATA_RECEIVED_OK:
			//LOG() << "DATA_RECEIVED_OK";
			emit NodeDownloaded();
			break;

    case OVERCURRENT_ERROR:
      LOG() << "The current has exceeded instrument limits! The channel has been shut off.";
      break;

    case OVERCURRENT_WARNING:
      LOG() << "The current has exceeded its set maximum limit!";
      break;

    case ECE_OVERVOLTAGE_WARNING:
      LOG() << "Counter electrode potential out of range!";
        break;
     
    case EWE_OVERVOLTAGE_WARNING:
      LOG() << "Working electrode potential out of range!";
      break;

    case REF_OVERVOLTAGE_WARNING:
      LOG() << "Reference electrode potential out of range!";
      break;

		default:
			LOG() << "Unknown response";
			break;
	}
}
void InstrumentOperator::StartExperiment(const NodesData &nodesData, quint8 channel) {
	uint16_t nodesCount = nodesData.count();

	_communicator->SendCommand((CommandID)BEGIN_NEW_EXP_DOWNLOAD, channel, QByteArray((char*)&nodesCount, sizeof(nodesCount)));
	foreach(auto node, nodesData) {
		QEventLoop loop;
		QSignalMapper mapper;
		QTimer timer;
		timer.setInterval(1000);

		#define EXIT_BY_TIMER		1
		#define EXIT_BY_RESPONCE	0

		mapper.setMapping(this, EXIT_BY_RESPONCE);
		mapper.setMapping(&timer, EXIT_BY_TIMER);

		connect(&mapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), &loop, &QEventLoop::exit);
		connect(this, &InstrumentOperator::NodeDownloaded,
			&mapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
		connect(&timer, &QTimer::timeout,
			&mapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
		
		_communicator->SendCommand((CommandID)APPEND_EXP_NODE, channel, node);
		
		timer.start();
		int ret = loop.exec();
		timer.stop();

		if (ret != EXIT_BY_RESPONCE) {
			LOG() << "Error while downloading experiment.";

			emit ExperimentCompleted();
			return;
		}
		
	}
	_communicator->SendCommand((CommandID)END_NEW_EXP_DOWNLOAD, channel);

	_communicator->SendCommand((CommandID)RUN_EXPERIMENT, channel);
}
void InstrumentOperator::StopExperiment(quint8 channel) {
	_communicator->SendCommand((CommandID)STOP_EXPERIMENT, channel);
}
void InstrumentOperator::PauseExperiment(quint8 channel) {
	_communicator->SendCommand((CommandID)PAUSE_EXPERIMENT, channel);
}
void InstrumentOperator::ResumeExperiment(quint8 channel) {
	_communicator->SendCommand((CommandID)RESUME_EXPERIMENT, channel);
}