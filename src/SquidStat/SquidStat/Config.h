#pragma once

#include <QSerialPort>

#define MANUFACTURER_NAME	"Admiral Instruments"

namespace DefaultSerialPortSettings {
	qint32 Baudrate();
	QSerialPort::DataBits DataBits();
	QSerialPort::FlowControl FlowControl();
	QSerialPort::Parity Parity();
	QSerialPort::StopBits StopBits();
}