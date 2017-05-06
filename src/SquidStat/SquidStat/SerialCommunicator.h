#pragma once

#include <QObject>
#include <QSerialPort>

#include "ExternalStructures.h"
#include "InstrumentStructures.h"

class SerialCommunicator : public QObject {
	Q_OBJECT

public:
	SerialCommunicator(const InstrumentInfo&, QObject *parent = 0);

public slots:
	bool Start();
	void Stop();

	void SendCommand(CommandID comm, quint8 channel = 0, const QByteArray &data = QByteArray());

private slots:
	void DataArrived();

signals:
	void ResponseReceived(ResponseID resp, quint8 channel, const QByteArray &data);

private:
	int FindPacket(const char *start, const char *end);
	bool CheckPacket(const ResponsePacket *resp);

	InstrumentInfo _instrumentInfo;
	QSerialPort *_serialPort;
	QByteArray _rawData;
};