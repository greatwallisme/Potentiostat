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
		case UR_SEND_CAL_DATA:
			if (data.size() == sizeof(CalibrationData)) {
				CalibrationData *calData = (CalibrationData*)data.data();
				emit CalibrationDataReceived(*calData);
			}
			break;

		case UR_EXPERIMENTAL_DATA:
			if (data.size() == sizeof(ExperimentalData)) {
				ExperimentalData *expData = (ExperimentalData*)data.data();
				emit ExperimentalDataReceived(channel, *expData);
			}
			break;

		default:
			break;
	}
}
void InstrumentOperator::RequestCalibrationData() {
	_communicator->SendCommand((CommandID)SEND_CAL_DATA);
}
void InstrumentOperator::StartExperiment(quint8 channel) {
	ExperimentNode_t exp[5];

	exp[0].nodeType = BRANCHNODE_HEAD;
	
	exp[1].nodeType = DCNODE_SWEEP;
	exp[1].tMin = 100000;
	exp[1].tMax = 100000000;
	exp[1].samplingMode = EDGE_SAMPLING;
	exp[1].samplingParams.ADCTimerDiv = 7;
	exp[1].samplingParams.ADCTimerPeriod = 20000;
	exp[1].samplingParams.ADCBufferSize = 20;
	exp[1].DCSweep.VStart = 0;
	exp[1].DCSweep.VEnd = 1024;
	exp[1].DCSweep.dVdt = 1;


	exp[2].nodeType = DCNODE_SWEEP;
	exp[2].tMin = 100000;
	exp[2].tMax = 100000000;
	exp[2].samplingMode = EDGE_SAMPLING;
	exp[2].samplingParams.ADCTimerDiv = 7;
	exp[2].samplingParams.ADCTimerPeriod = 20000;
	exp[2].samplingParams.ADCBufferSize = 20;
	exp[2].DCSweep.VStart = 0;
	exp[2].DCSweep.VEnd = 1024;
	exp[2].DCSweep.dVdt = 1;

	exp[3].nodeType = BRANCHNODE_TAIL;
	exp[3].MaxPlays = 3;
	exp[3].BranchNodeTail.branchHeadIndex = 0;

	exp[4].nodeType = END_EXPERIMENT_NODE;

	_communicator->SendCommand((CommandID)START_EXPERIMENT, channel, QByteArray((char*)exp, sizeof(exp)));
}
void InstrumentOperator::StopExperiment(quint8 channel) {
	_communicator->SendCommand((CommandID)STOP_EXPERIMENT, channel);
}