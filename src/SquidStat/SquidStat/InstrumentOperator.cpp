#include "InstrumentOperator.h"
#include "ExternalStructures.h"

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
				CalibrationData *calData = (CalibrationData*)data.data();
				emit CalibrationDataReceived(*calData);
			}
			break;

		case ADCDC_DATA:
			if (data.size() == sizeof(ExperimentalData)) {
				ExperimentalData *expData = (ExperimentalData*)data.data();
				emit ExperimentalDataReceived(channel, *expData);
			}
			break;

		default:
			resp;
			break;
	}
}
void InstrumentOperator::RequestCalibrationData() {
	_communicator->SendCommand((CommandID)SEND_CAL_DATA);
}
void InstrumentOperator::StartExperiment(quint8 channel) {
	ExperimentNode_t exp[3];

	exp[0].isHead = true;
	exp[0].nodeType = DCNODE_SWEEP;
	exp[0].tMin = 100000;
	exp[0].tMax = 10000000000;
	exp[0].samplingParams.ADCTimerDiv = 7;
	exp[0].samplingParams.ADCTimerPeriod = 15625;
	exp[0].samplingParams.ADCBufferSize = 20;
	exp[0].DCSweep.VStart = 0;
	exp[0].DCSweep.VEnd = 1024;
	exp[0].DCSweep.dVdt = 1;

	exp[1].nodeType = DCNODE_SWEEP;
	exp[1].tMin = 100000;
	exp[1].tMax = 100000000;
	exp[1].samplingParams.ADCTimerDiv = 7;
	exp[1].samplingParams.ADCTimerPeriod = 15625;
	exp[1].samplingParams.ADCBufferSize = 20;
	exp[1].DCSweep.VStart = 1024;
	exp[1].DCSweep.VEnd = 0;
	exp[1].DCSweep.dVdt = -1;
	exp[1].MaxPlays = 3;
	exp[1].branchHeadIndex = 0;

	exp[2].nodeType = END_EXPERIMENT_NODE;

	_communicator->SendCommand((CommandID)DOWNLOAD_EXPERIMENT, channel, QByteArray((char*)exp, sizeof(exp)));
	_communicator->SendCommand((CommandID)RUN_EXPERIMENT, channel);
}
void InstrumentOperator::StopExperiment(quint8 channel) {
	//_communicator->SendCommand((CommandID)STOP_EXPERIMENT, channel);
}