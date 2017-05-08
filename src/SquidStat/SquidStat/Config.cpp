#include "Config.h"

qint32 DefaultSerialPortSettings::Baudrate() {
	return QSerialPort::Baud115200;
}
QSerialPort::DataBits DefaultSerialPortSettings::DataBits() {
	return QSerialPort::Data8;
}
QSerialPort::FlowControl DefaultSerialPortSettings::FlowControl() {
	return QSerialPort::NoFlowControl;
}
QSerialPort::Parity DefaultSerialPortSettings::Parity() {
	return QSerialPort::NoParity;
}
QSerialPort::StopBits DefaultSerialPortSettings::StopBits() {
	return QSerialPort::OneStop;
}