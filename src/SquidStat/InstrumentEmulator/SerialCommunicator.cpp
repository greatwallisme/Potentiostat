#include "SerialCommunicator.h"

#include <QHostAddress>

SerialCommunicator::SerialCommunicator(QObject *parent) :
	QObject(parent)
{
	_serialPort = new QTcpSocket(this);

	connect(_serialPort, &QTcpSocket::readyRead, this, &SerialCommunicator::DataArrived);
}
void SerialCommunicator::Start() {
	_serialPort->connectToHost(QHostAddress::LocalHost, 33333);
}
void SerialCommunicator::Stop() {
	_serialPort->close();
}
void SerialCommunicator::SendResponse(ResponseID comm, quint8 channel, const QByteArray &data) {
	QByteArray toSend(sizeof(ResponsePacket) + data.size(), 0x00);
	ResponsePacket *pack = (ResponsePacket*)toSend.data();

	pack->fraiming = RESPONSE_FRAIMING_BYTES;
	pack->comm = comm;
	pack->channel = channel;
	pack->len = data.size();
	memcpy(pack->data, data.data(), data.size());

	_serialPort->write(toSend);
	_serialPort->flush();
}
int SerialCommunicator::FindPacket(const char *startPtr, const char *endPtr) {
	const char *dataPtr = startPtr;
	
	bool foundFraiming = false;
	const CommandPacket *comm;
	while ((endPtr - dataPtr) >= sizeof(CommandPacket)) {
		comm = (const CommandPacket*)dataPtr;
		if (COMMAND_FRAIMING_BYTES == comm->fraiming) {
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
bool SerialCommunicator::CheckPacket(const CommandPacket *resp) {
	if (resp->channel > MAX_CHANNEL_VALUE) {
		return false;
	}

	bool commandFound = false;
	for (quint8 val = UR_HANDSHAKE; val < USB_RESPONCE_LAST; ++val) {
		if (val == resp->comm) {
			commandFound = true;
			break;
		}
	}
	if (!commandFound) {
		return false;
	}

	if (resp->len > MAX_DATA_LENGTH) {
		return false;
	}

	return true;
}
void SerialCommunicator::DataArrived() {
	_rawData += _serialPort->readAll();

	if (_rawData.size() < sizeof(CommandPacket)) {
		return;
	}

	char *dataPtr = _rawData.data();
	char *endPtr = _rawData.data() + _rawData.size();

	while ((endPtr - dataPtr) >= sizeof(CommandPacket)) {
		int packetPos = 0;
		if (-1 == (packetPos = FindPacket(dataPtr, endPtr))) {
			break;
		}

		dataPtr += packetPos;
		CommandPacket *comm = (CommandPacket*)dataPtr;

		if (!CheckPacket(comm)) {
			++dataPtr;
			continue;
		}

		if ((endPtr - dataPtr) < (sizeof(CommandPacket) + comm->len)) {
			continue;
		}

		emit CommandReceived((CommandID)comm->comm, comm->channel, QByteArray(comm->data, comm->len));
		dataPtr += (sizeof(CommandPacket) + comm->len);
	}

	_rawData.remove(0, dataPtr - _rawData.data());
}