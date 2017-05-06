#include "InstrumentEnumerator.h"
#include "Config.h"

#include <QSerialPortInfo>
#include <QByteArray>

char helloRequest[] = {0xEE, 0xFF, 'A', 0x00, 0x00};
char helloResponse[] = { 'H', 'e', 'l', 'l', 'o' };

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

	auto availablePorts = QSerialPortInfo::availablePorts();

	foreach(const QSerialPortInfo &serialPortInfo, availablePorts) {
		QSerialPort serialPort(serialPortInfo);
		serialPort.setBaudRate(DefaultSerialPortSettings::Baudrate());
		serialPort.setDataBits(DefaultSerialPortSettings::DataBits());
		serialPort.setFlowControl(DefaultSerialPortSettings::FlowControl());
		serialPort.setParity(DefaultSerialPortSettings::Parity());
		serialPort.setStopBits(DefaultSerialPortSettings::StopBits());

		if (!serialPort.open(QIODevice::ReadWrite)) {
			continue;
		}

		serialPort.write(helloRequest, sizeof(helloRequest));
		serialPort.flush();

		serialPort.waitForReadyRead(1000);

		if (serialPort.bytesAvailable() < sizeof(helloResponse)) {
			serialPort.close();
			continue;
		}

		if (serialPort.read(sizeof(helloResponse)) != QByteArray(helloResponse, sizeof(helloResponse))) {
			serialPort.close();
			continue;
		}

		InstrumentInfo instrumentInfo;
		instrumentInfo.portName = serialPortInfo.portName();
		instrumentInfo.serial = serialPortInfo.serialNumber();

		ret << instrumentInfo;
	}

	return ret;
}
void InstrumentEnumerator::GetCalibrationData(const InstrumentInfo &instrumentInfo) {
	;
}