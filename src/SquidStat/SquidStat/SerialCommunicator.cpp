#include "SerialCommunicator.h"
#include "SerialThread.h"

#include "Log.h"

#include <QFile>

SerialCommunicator::SerialCommunicator(const InstrumentInfo &info, QObject *parent) :
	QObject(parent)
{
	_rawData.reserve(1024*1024);

	_serialThread = new SerialThread(info);

	connect(_serialThread, &SerialThread::NewData, this, &SerialCommunicator::DataArrived, Qt::QueuedConnection);
	connect(this, &SerialCommunicator::SendData, _serialThread, &SerialThread::DataToSend, Qt::QueuedConnection);
}
SerialCommunicator::~SerialCommunicator() {
	if (_serialThread->isRunning()) {
		_serialThread->quit();
		_serialThread->wait();
	}
	_serialThread->deleteLater();
}
void SerialCommunicator::Start() {
	_serialThread->start();
}
void SerialCommunicator::Stop() {
	_serialThread->quit();
}
void SerialCommunicator::SendCommand(CommandID comm, quint8 channel, const QByteArray &data) {
	QByteArray toSend(sizeof(CommandPacket) + data.size(), 0x00);
	CommandPacket *pack = (CommandPacket*)toSend.data();

	pack->frame = COMMAND_FRAIMING_BYTES;
	pack->hdr.command = comm;
	pack->hdr.channelNum = channel;
	pack->hdr.dataLength = data.size();
	memcpy(pack->data, data.data(), data.size());

	emit SendData(toSend);
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
void SerialCommunicator::DataArrived(const QByteArray &newData) {
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
			LOG() << "It is not a valid packet, skip one byte";
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