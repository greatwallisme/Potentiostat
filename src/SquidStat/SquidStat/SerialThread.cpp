#include "SerialThread.h"

#include "Config.h"

SerialThread::SerialThread(const InstrumentInfo &info) :
	_info(info)
{
	this->moveToThread(this);
}
void SerialThread::run() {
	_serialPort = new QSerialPort(this);
	_serialPort->setPortName(_info.port.name);
	_serialPort->setBaudRate(DefaultSerialPortSettings::Baudrate());
	_serialPort->setDataBits(DefaultSerialPortSettings::DataBits());
	_serialPort->setFlowControl(DefaultSerialPortSettings::FlowControl());
	_serialPort->setParity(DefaultSerialPortSettings::Parity());
	_serialPort->setStopBits(DefaultSerialPortSettings::StopBits());

	connect(_serialPort, &QSerialPort::readyRead,
		this, &SerialThread::DataArrived, Qt::QueuedConnection);

	connect(_serialPort, &QSerialPort::errorOccurred, this, &SerialThread::ErrorOnSerial);

	_serialPort->open(QIODevice::ReadWrite);

	exec();

	_serialPort->close();
	_serialPort->deleteLater();
}
void SerialThread::ErrorOnSerial(QSerialPort::SerialPortError error) {
	if (QSerialPort::NoError == error) {
		return;
	}

	this->quit();
}
void SerialThread::DataToSend(const QByteArray &data) {
	_serialPort->write(data);
  //_serialPort->waitForBytesWritten();
}
void SerialThread::DataArrived() {
	emit NewData(_serialPort->readAll());
}