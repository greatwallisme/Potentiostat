#include "InstrumentOperator.h"
#include "ExternalStructures.h"

#include "Log.h"

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

		case ADCAC_DATA:
			emit ExperimentalAcDataReceived(channel, 0);
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

		default:
			LOG() << "Unknown response";
			break;
	}
}
void InstrumentOperator::StartExperiment(const NodesData &nodesData, quint8 channel) {
	uint16_t nodesCount = nodesData.count();

	_communicator->SendCommand((CommandID)BEGIN_NEW_EXP_DOWNLOAD, channel, QByteArray((char*)&nodesCount, sizeof(nodesCount)));
	foreach(auto node, nodesData) {
		_communicator->SendCommand((CommandID)APPEND_EXP_NODE, channel, node);
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