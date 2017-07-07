#pragma once

#include <QThread>
#include <QSerialPort>

#include "InstrumentStructures.h"

class SerialThread : public QThread {
	Q_OBJECT

public:
	SerialThread(const InstrumentInfo&);

protected:
	void run();

signals:
	void NewData(const QByteArray&);

public slots:
	void DataToSend(const QByteArray&);

private slots:
	void DataArrived();
	void ErrorOnSerial(QSerialPort::SerialPortError);

private:
	QSerialPort *_serialPort;
	InstrumentInfo _info;
};