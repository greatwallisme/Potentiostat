#include "InstrumentOperator.h"

#include <QEventLoop>

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

		default:
			break;
	}
}
void InstrumentOperator::RequestCalibrationData() {
	_communicator->SendCommand((CommandID)SEND_CAL_DATA);
}
void InstrumentOperator::StartExperiment() {
	;
}