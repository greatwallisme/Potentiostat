#include "SerialCommunicator.h"

#include "Config.h"

#include "Log.h"

SerialCommunicator::SerialCommunicator(const InstrumentInfo &info, QObject *parent) :
	QObject(parent),
	_instrumentInfo(info)
{
	_serialPort = new QSerialPort(this);
	_serialPort->setPortName(_instrumentInfo.portName);
	_serialPort->setBaudRate(DefaultSerialPortSettings::Baudrate());
	_serialPort->setDataBits(DefaultSerialPortSettings::DataBits());
	_serialPort->setFlowControl(DefaultSerialPortSettings::FlowControl());
	_serialPort->setParity(DefaultSerialPortSettings::Parity());
	_serialPort->setStopBits(DefaultSerialPortSettings::StopBits());

	connect(_serialPort, &QSerialPort::readyRead,
		this, &SerialCommunicator::DataArrived);
}
bool SerialCommunicator::Start() {
	return _serialPort->open(QIODevice::ReadWrite);
}
void SerialCommunicator::Stop() {
	_serialPort->close();
}
void SerialCommunicator::SendCommand(CommandID comm, quint8 channel, const QByteArray &data) {
	QByteArray toSend(sizeof(CommandPacket) + data.size(), 0x00);
	CommandPacket *pack = (CommandPacket*)toSend.data();

	pack->frame = COMMAND_FRAIMING_BYTES;
	pack->hdr.command = comm;
	pack->hdr.channelNum = channel;
	pack->hdr.dataLength = data.size();
	memcpy(pack->data, data.data(), data.size());

	_serialPort->write(toSend);
	_serialPort->flush();
}
int SerialCommunicator::FindPacket(const char *startPtr, const char *endPtr) {
	const char *dataPtr = startPtr;
	
	bool foundFraiming = false;
	const ResponsePacket *resp;
	while ((endPtr - dataPtr) >= sizeof(ResponsePacket)) {
		resp = (const ResponsePacket*)dataPtr;
		if (COMMAND_FRAIMING_BYTES == resp->hdr.frame) {
			foundFraiming = true;
			break;
		}

		++dataPtr;
	}

	int res = -1;
	if (foundFraiming) {
		res = dataPtr - startPtr;
	}

	return res;
}
bool SerialCommunicator::CheckPacket(const ResponsePacket *resp) {
	if (resp->hdr.channelNum > MAX_CHANNEL_VALUE) {
		return false;
	}

	//bool commandFound = false;
	//for (quint8 val = HANDSHAKE; val < USB_COMMAND_LAST; ++val) {
	//	if (val == resp->comm) {
	//		commandFound = true;
	//		break;
	//	}
	//}
	//if (!commandFound) {
	//	return false;
	//}

	if (resp->hdr.dataLength > MAX_DATA_LENGTH) {
		return false;
	}

	return true;
}
void SerialCommunicator::DataArrived() {
	QByteArray newData = _serialPort->readAll();
	
	_rawData += newData;

	if (_rawData.size() < sizeof(ResponsePacket)) {
		return;
	}

	char *dataPtr = _rawData.data();
	char *endPtr = _rawData.data() + _rawData.size();

	while ((endPtr - dataPtr) >= sizeof(ResponsePacket)) {
		int packetPos = 0;
		if (-1 == (packetPos = FindPacket(dataPtr, endPtr))) {
			break;
		}

		dataPtr += packetPos;
		ResponsePacket *resp = (ResponsePacket*)dataPtr;

		if (!CheckPacket(resp)) {
			++dataPtr;
			continue;
		}

		if ((endPtr - dataPtr) < (sizeof(ResponsePacket) + resp->hdr.dataLength)) {
			break;
		}

		emit ResponseReceived((ResponseID)resp->hdr.returnCode, resp->hdr.channelNum, QByteArray(resp->data, resp->hdr.dataLength));
		dataPtr += (sizeof(ResponsePacket) + resp->hdr.dataLength);
	}

	_rawData.remove(0, dataPtr - _rawData.data());
}