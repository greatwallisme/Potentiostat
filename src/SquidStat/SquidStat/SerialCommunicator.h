#pragma once

#include <QObject>

#include "ExternalStructures.h"
#include "InstrumentStructures.h"

class SerialThread;

class SerialCommunicator : public QObject {
	Q_OBJECT

public:
	SerialCommunicator(const InstrumentInfo&, QObject *parent = 0);
	~SerialCommunicator();

public slots:
	void Start();
	void Stop();

	void SendCommand(CommandID comm, quint8 channel = 0, const QByteArray &data = QByteArray());

private slots:
	void DataArrived(const QByteArray&);

signals:
	void SendData(const QByteArray&);
	void ResponseReceived(ResponseID resp, quint8 channel, const QByteArray &data);

private:
	int FindPacket(const char *start, const char *end);
	bool CheckPacket(const ResponsePacket *resp);

	QByteArray _rawData;
	SerialThread *_serialThread;
};