#include "SerialCommunicator.h"

#include "Config.h"

struct CommandPacket {
	quint16 fraiming;
	quint8 comm;
	quint8 channel;
	quint16 len;
	char data[0];
};

struct ResponsePacket {
	quint8 fraiming;
	quint8 comm;
	quint8 channel;
	quint16 len;
	char data[0];
};

SerialCommunicator::SerialCommunicator(const InstrumentInfo &info, QObject *parent = 0) :
	QObject(parent),
	_instrumentInfo(info)
{
	_serialPort.setPortName(_instrumentInfo.portName);
	_serialPort.setBaudRate(DefaultSerialPortSettings::Baudrate());
	_serialPort.setDataBits(DefaultSerialPortSettings::DataBits());
	_serialPort.setFlowControl(DefaultSerialPortSettings::FlowControl());
	_serialPort.setParity(DefaultSerialPortSettings::Parity());
	_serialPort.setStopBits(DefaultSerialPortSettings::StopBits());

	//connect()
}
bool SerialCommunicator::Start() {
	return _serialPort.open(QIODevice::ReadWrite);
}
void SerialCommunicator::Stop() {
	_serialPort.close();
}
void SerialCommunicator::SendCommand(USBcommand_t comm, quint8 channel, const QByteArray &data) {
	QByteArray toSend(sizeof(CommandPacket) + data.size(), 0x00);
	CommandPacket *pack = (CommandPacket*)toSend.data();

	pack->fraiming = 0xFFEE;
	pack->comm = comm;
	pack->channel = channel;
	pack->len = data.length();
	memcpy(pack->data, data.data(), data.size());

	_serialPort.write(toSend);
}