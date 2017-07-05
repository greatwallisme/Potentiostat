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
		/*
		case CAL_DATA:
			if (data.size() == sizeof(CalibrationData)) {
				CalibrationData calData;
				memcpy(&calData, data.data(), sizeof(CalibrationData));
				emit CalibrationDataReceived(calData);
			}
			break;
		//*/

		case ADCDC_DATA:
			if (data.size() == sizeof(ExperimentalData)) {
				ExperimentalData *expData = (ExperimentalData*)data.data();
				emit ExperimentalDataReceived(channel, *expData);
			}
			break;

		/*
		case HW_DATA:
			if (data.size() == sizeof(HardwareVersion)) {
				HardwareVersion hwVersion;
				memcpy(&hwVersion, data.data(), sizeof(HardwareVersion));
				emit HardwareVersionReceived(hwVersion);
			}
			break;
		//*/

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
/*
void InstrumentOperator::RequestCalibrationData() {
	_communicator->SendCommand((CommandID)SEND_CAL_DATA);
}
void InstrumentOperator::RequestHardwareVersion() {
	_communicator->SendCommand((CommandID)SEND_HW_DATA);
}
//*/
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