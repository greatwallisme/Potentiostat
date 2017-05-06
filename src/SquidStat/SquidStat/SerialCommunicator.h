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

	void SendCommand(USBcommand_t comm, quint8 channel, const QByteArray &data);

signals:
	void ResponseReceived(USBresponse_t resp, quint8 channel, const QByteArray &data);

private:
	InstrumentInfo _instrumentInfo;
	QSerialPort _serialPort;
};