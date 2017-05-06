#include "InstrumentEnumerator.h"
#include "SerialCommunicator.h"
#include "Config.h"

#include <QSerialPortInfo>
#include <QByteArray>
#include <QEventLoop>
#include <QTimer>

InstrumentEnumerator::InstrumentEnumerator() {

}
InstrumentList InstrumentEnumerator::FindInstruments() {
	InstrumentList ret;

	auto availablePorts = QSerialPortInfo::availablePorts();


	foreach(const QSerialPortInfo &serialPortInfo, availablePorts) {
		if (serialPortInfo.manufacturer() != MANUFACTURER_NAME) {
			continue;
		}

		InstrumentInfo instrumentInfo;
		instrumentInfo.portName = serialPortInfo.portName();
		instrumentInfo.serial = serialPortInfo.serialNumber();

		ret << instrumentInfo;
	}

	return ret;
}
InstrumentList InstrumentEnumerator::FindInstrumentsActive() {
	InstrumentList ret;
	static bool handshakeResponseArrived;
	static QEventLoop eventLoop;

	auto availablePorts = QSerialPortInfo::availablePorts();

	foreach(const QSerialPortInfo &serialPortInfo, availablePorts) {
		InstrumentInfo instrumentInfo;
		instrumentInfo.portName = serialPortInfo.portName();
		instrumentInfo.serial = serialPortInfo.serialNumber();

		SerialCommunicator communicator(instrumentInfo);

		if (!communicator.Start()) {
			continue;
		}

		handshakeResponseArrived = false;

		QMetaObject::Connection connection = QObject::connect(&communicator, &SerialCommunicator::ResponseReceived,
			[=](ResponseID resp, quint8 channel, const QByteArray &data) {
				if (resp == UR_HANDSHAKE) {
					handshakeResponseArrived = true;
					eventLoop.quit();
				}
			}
		);

		communicator.SendCommand((CommandID)HANDSHAKE);


		QTimer::singleShot(1000, &eventLoop, &QEventLoop::quit);
		eventLoop.exec();

		QObject::disconnect(connection);

		communicator.Stop();

		if (handshakeResponseArrived) {
			ret << instrumentInfo;
		}
	}

	return ret;
}
CalibrationData InstrumentEnumerator::GetCalibrationData(const InstrumentInfo &instrumentInfo, bool *ok) {
	CalibrationData ret;
	memset(&ret, 0x00, sizeof(CalibrationData));

	static QEventLoop eventLoop;

	if (ok) {
		*ok = false;
	}

	SerialCommunicator communicator(instrumentInfo);

	if (!communicator.Start()) {
		return ret;
	}

	QMetaObject::Connection connection = QObject::connect(&communicator, &SerialCommunicator::ResponseReceived,
		[=](ResponseID resp, quint8 channel, const QByteArray &data) {
			if (resp == UR_SEND_CAL_DATA) {
				if (data.size() == sizeof(CalibrationData)) {
					memcpy(const_cast<CalibrationData*>(&ret), data.data(), sizeof(CalibrationData));
					if (ok) {
						*ok = true;
					}
				}
				eventLoop.quit();
			}
		}
	);

	communicator.SendCommand((CommandID)SEND_CAL_DATA);


	QTimer::singleShot(1000, &eventLoop, &QEventLoop::quit);
	eventLoop.exec();

	QObject::disconnect(connection);

	communicator.Stop();

	return ret;
}