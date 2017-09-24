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
      LOG() << "ADCAC_DATA";
      auto *dataPtr = data.data();
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
      LOG() << "EXPERIMENT_COMPLETE";
			emit ExperimentCompleted(channel);
			break;
		
		case EXPERIMENT_PAUSED:
			emit ExperimentPaused(channel);
			break;
		
		case EXPERIMENT_RESUMED:
			emit ExperimentResumed(channel);
			break;

		case EXPERIMENT_NODE_BEGINNING:
      LOG() << "EXPERIMENT_NODE_BEGINNING";
      if (data.size() == sizeof(ExperimentNode_t)) {
				ExperimentNode_t *node = (ExperimentNode_t*)data.data();
				emit ExperimentNodeBeginning(channel, *node);
			}
			break;

		case EXPERIMENT_NODE_COMPLETE:
			LOG() << "Node complete";
			break;

		case DATA_RECEIVED_OK:
			//LOG() << "DATA_RECEIVED_OK";
			emit NodeDownloaded(channel);
			break;

    case OVERCURRENT_ERROR:
      LOG() << "The current has exceeded instrument limits! The channel has been shut off.";
      break;

    case OVERCURRENT_WARNING:
      //LOG() << "The current has exceeded its set maximum limit! New range: " << QString::number(data[0]);
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
		QTimer timer;
		timer.setInterval(1000);

		QEventLoop *loopPtr = &loop;

		#define EXIT_BY_TIMER		1
		#define EXIT_BY_RESPONCE	0

		QList<QMetaObject::Connection> connections;

		connections << connect(this, &InstrumentOperator::NodeDownloaded, [=](quint8 nodeChannel) {
			if (nodeChannel != channel) {
				return;
			}

			loopPtr->exit(EXIT_BY_RESPONCE);
		});
		connections << connect(&timer, &QTimer::timeout, [=]() {
			loopPtr->exit(EXIT_BY_TIMER);
		});

		_communicator->SendCommand((CommandID)APPEND_EXP_NODE, channel, node);

		timer.start();
		int ret = loop.exec();
		timer.stop();

		foreach(auto conn, connections) {
			disconnect(conn);
		}

		if (ret != EXIT_BY_RESPONCE) {
			LOG() << "Error while downloading experiment.";

			emit ExperimentCompleted(channel);
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
void InstrumentOperator::SoftReset() {
	_communicator->SendCommand((CommandID)RESET_TO_BOOTLOADER);

}
void InstrumentOperator::StartManualExperiment(quint8 channel) {
	_communicator->SendCommand((CommandID)SET_MANUAL_MODE, channel);
}

void InstrumentOperator::SetCompRange(quint8 channel, quint8 range) {
    _communicator->SendCommand((CommandID)SET_COMP_RANGE, channel, QByteArray((char*)&range, sizeof(range)));
}

#define TO_BYTE_ARRAY(a)	QByteArray((char*)&a, sizeof(a))

void InstrumentOperator::SetManualSamplingParams(quint8 channel, const Manual::SamplingParams &params) {
	_communicator->SendCommand((CommandID)MANUAL_SAMPLING_PARAMS_SET, channel, TO_BYTE_ARRAY(params));
}
void InstrumentOperator::SetManualGalvanoSetpoint(quint8 channel, const Manual::GalvanoSetpoint &params) {
	_communicator->SendCommand((CommandID)MANUAL_GALV_SETPOINT_SET, channel, TO_BYTE_ARRAY(params));
}
void InstrumentOperator::SetManualPotentioSetpoint(quint8 channel, const Manual::PotentioSetpoint &params) {
	_communicator->SendCommand((CommandID)MANUAL_POT_SETPOINT_SET, channel, TO_BYTE_ARRAY(params));
}
void InstrumentOperator::SetManualOcp(quint8 channel) {
	_communicator->SendCommand((CommandID)MANUAL_OCP_SET, channel);
}
void InstrumentOperator::SetCurrentRangingMode(quint8 channel, const Manual::RangingMode &params) {
	_communicator->SendCommand((CommandID)MANUAL_CURRENT_RANGING_MODE_SET, channel, TO_BYTE_ARRAY(params));
}