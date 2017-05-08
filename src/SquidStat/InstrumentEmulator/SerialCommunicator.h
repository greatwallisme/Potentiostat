#pragma once

#include <QObject>
#include <QSerialPort>

#include <ExternalStructures.h>
#include <InstrumentStructures.h>

class SerialCommunicator : public QObject {
	Q_OBJECT

public:
	SerialCommunicator(QObject *parent = 0);

public slots:
	void Start();
	void Stop();

	void SendResponse(ResponseID resp, quint8 channel = 0, const QByteArray &data = QByteArray());

private slots:
	void DataArrived();

signals:
	void CommandReceived(CommandID comm, quint8 channel, const QByteArray &data);

private:
	int FindPacket(const char *start, const char *end);
	bool CheckPacket(const CommandPacket *resp);

	QSerialPort *_serialPort;
	QByteArray _rawData;
};