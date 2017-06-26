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
		case CAL_DATA:
			if (data.size() == sizeof(CalibrationData)) {
				CalibrationData calData;
				memcpy(&calData, data.data(), sizeof(CalibrationData));
				emit CalibrationDataReceived(calData);
			}
			break;

		case ADCDC_DATA:
			if (data.size() == sizeof(ExperimentalData)) {
				ExperimentalData *expData = (ExperimentalData*)data.data();
				emit ExperimentalDataReceived(channel, *expData);
			}
			break;

		case HW_DATA:
			if (data.size() == sizeof(HardwareVersion)) {
				HardwareVersion hwVersion;
				memcpy(&hwVersion, data.data(), sizeof(HardwareVersion));
				emit HardwareVersionReceived(hwVersion);
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

		case EXPERIMENT_NODE_COMPLETE:
			LOG() << "Node complete";
			break;

		default:
			LOG() << "Unknown response";
			break;
	}
}
void InstrumentOperator::RequestCalibrationData() {
	_communicator->SendCommand((CommandID)SEND_CAL_DATA);
}
void InstrumentOperator::RequestHardwareVersion() {
	_communicator->SendCommand((CommandID)SEND_HW_DATA);
}
void InstrumentOperator::StartExperiment(const QByteArray &nodesData, quint8 channel) {
	_communicator->SendCommand((CommandID)DOWNLOAD_EXPERIMENT, channel, nodesData);
	_communicator->SendCommand((CommandID)RUN_EXPERIMENT, channel);
}
void InstrumentOperator::StopExperiment(quint8 channel) {
	//_communicator->SendCommand((CommandID)STOP_EXPERIMENT, channel);
}