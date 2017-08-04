#include "InstrumentEnumerator.h"
#include "SerialCommunicator.h"
#include "Config.h"

#include <QSerialPortInfo>
#include <QByteArray>
#include <QEventLoop>
#include <QTimer>

#include "Log.h"

InstrumentList RemoveUnavaliableInstruments(InstrumentList &instruments, const QList<QSerialPortInfo> &availablePorts) {
	InstrumentList instrumentsToDelete;

	for (int i = 0; i < instruments.count(); ) {
		const InstrumentInfo &instrument(instruments[i]);

		bool found = false;
		foreach(const QSerialPortInfo &serialPortInfo, availablePorts) {
			if ((instrument.port.name == serialPortInfo.portName()) && (instrument.port.serial == serialPortInfo.serialNumber())) {
				found = true;
				break;
			}
		}

		if (!found) {
			instrumentsToDelete << instrument;
			instruments.removeAt(i);
		}
		else {
			++i;
		}
	}

	return instrumentsToDelete;
}
InstrumentList GetNewInstrumentsCandidates(InstrumentList &instruments, const QList<QSerialPortInfo> &availablePorts) {
	InstrumentList instrumentsToAdd;
	
	foreach(const QSerialPortInfo &serialPortInfo, availablePorts) {
		bool found = false;
		foreach(auto instrument, instruments) {
			if ((instrument.port.name == serialPortInfo.portName()) && (instrument.port.serial == serialPortInfo.serialNumber())) {
				found = true;
				break;
			}
		}

		if (!found) {
			InstrumentInfo info;
			info.port.name = serialPortInfo.portName();
			info.port.serial = serialPortInfo.serialNumber();

			instrumentsToAdd << info;
		}
	}

	return instrumentsToAdd;
}
InstrumentList RequestInstrumentData(InstrumentList &instrumentsCandidates) {
	InstrumentList ret;
	
	while (instrumentsCandidates.count()) {
		InstrumentInfo instrumentInfo = instrumentsCandidates.first();
		instrumentsCandidates.removeFirst();

		SerialCommunicator communicator(instrumentInfo);
		communicator.Start();
		/*
		if (!communicator.Start()) {
			continue;
		}
		//*/
		
		static QEventLoop eventLoop;
		static CalibrationData calData;
		static HardwareVersion hwVersion;
		static bool handshakeResponseArrived;
		static bool calibrationDataArrived;
		static bool hwVersionArrived;

		handshakeResponseArrived = false;
		calibrationDataArrived = false;
		hwVersionArrived = false;
		memset(&calData, 0x00, sizeof(CalibrationData));
		memset(&hwVersion, 0x00, sizeof(HardwareVersion));

		QMetaObject::Connection connection = QObject::connect(&communicator, &SerialCommunicator::ResponseReceived,
			[=](ResponseID resp, quint8 channel, const QByteArray &data) {
				switch (resp) {
					case HANDSHAKE_RESPONSE:
						handshakeResponseArrived = true;
						eventLoop.quit();
						break;

					case CAL_DATA:
						if (data.size() == sizeof(CalibrationData)) {
							memcpy(&calData, data.data(), sizeof(CalibrationData));
							
							calibrationDataArrived = true;
							eventLoop.quit();
						}
						break;

					case HW_DATA:
						if (data.size() == sizeof(HardwareVersion)) {
							memcpy(&hwVersion, data.data(), sizeof(HardwareVersion));
							
							hwVersionArrived = true;
							eventLoop.quit();
						}
						break;
				}
			}
		);

		communicator.SendCommand((CommandID)HANDSHAKE);

		QTimer::singleShot(1000, &eventLoop, &QEventLoop::quit);
		eventLoop.exec();

		if (handshakeResponseArrived) {
			communicator.SendCommand((CommandID)SEND_CAL_DATA);

			QTimer::singleShot(1000, &eventLoop, &QEventLoop::quit);
			eventLoop.exec();

			if (calibrationDataArrived) {
				instrumentInfo.calData = calData;

				communicator.SendCommand((CommandID)SEND_HW_DATA);

				QTimer::singleShot(1000, &eventLoop, &QEventLoop::quit);
				eventLoop.exec();

				if (hwVersionArrived) {
					instrumentInfo.hwVer = hwVersion;

					communicator.SendCommand((CommandID)INIT_DEFAULT_SAMPLING);

					char *_end = (char*)&hwVersion + sizeof(HardwareVersion);
					char *_ptr = hwVersion.hwName;
					while (_ptr != _end) {
						if (*_ptr == '\n') {
							*_ptr = 0x00;
							break;
						}
						++_ptr;
					}
					if (_ptr == _end) {
						--_ptr;
						*_ptr = 0x00;
					}

					instrumentInfo.name = QString(hwVersion.hwName) + " (" + instrumentInfo.port.name + ")";

					ret << instrumentInfo;
				}
			}
		}

		QObject::disconnect(connection);

		communicator.Stop();

	}
	
	return ret;
}

InstrumentEnumerator::InstrumentEnumerator() :
	QThread(0)
{
	this->moveToThread(this);
}
void InstrumentEnumerator::run() {
	while (1) {
		auto availablePorts = QSerialPortInfo::availablePorts();

		InstrumentList instrumentsToDelete = RemoveUnavaliableInstruments(instruments, availablePorts);

		if (instrumentsToDelete.count()) {
			foreach(auto instrument, instrumentsToDelete) {
				LOG() << "Instrument" << instrument.name << "disconnected";
			}
			emit RemoveDisconnectedInstruments(instrumentsToDelete);
		}

		InstrumentList instrumentsToAdd = GetNewInstrumentsCandidates(instruments, availablePorts);
		if (instrumentsToAdd.count()) {
			instrumentsToAdd = RequestInstrumentData(instrumentsToAdd);

			if (instrumentsToAdd.count()) {
				instruments << instrumentsToAdd;
				foreach(auto instrument, instrumentsToAdd) {
					LOG() << "Instrument" << instrument.name << "connected";
				}
				emit AddNewInstruments(instrumentsToAdd);
			}
		}

		QEventLoop sleepLoop;
		QTimer::singleShot(1000, &sleepLoop, &QEventLoop::quit);
		sleepLoop.exec();
	}
}
/*
InstrumentList InstrumentEnumerator::FindInstruments() {
	InstrumentList ret;

	auto availablePorts = QSerialPortInfo::availablePorts();


	foreach(const QSerialPortInfo &serialPortInfo, availablePorts) {
		auto name = serialPortInfo.portName();
		if (serialPortInfo.manufacturer() != MANUFACTURER_NAME) {
			continue;
		}

		InstrumentInfo instrumentInfo;
		instrumentInfo.serialPortName = serialPortInfo.portName();
		instrumentInfo.serial = serialPortInfo.serialNumber();

		ret << instrumentInfo;
	}

	return ret;
}
InstrumentList InstrumentEnumerator::FindInstrumentsActive() {
	InstrumentList ret;
	static bool handshakeResponseArrived;
	static QEventLoop eventLoop;

	auto availablePorts = QSerialPortInfo::availablePorts();

	foreach(const QSerialPortInfo &serialPortInfo, availablePorts) {
		InstrumentInfo instrumentInfo;
		instrumentInfo.serialPortName = serialPortInfo.portName();
		instrumentInfo.serial = serialPortInfo.serialNumber();

		SerialCommunicator communicator(instrumentInfo);

		if (!communicator.Start()) {
			continue;
		}

		handshakeResponseArrived = false;

		QMetaObject::Connection connection = QObject::connect(&communicator, &SerialCommunicator::ResponseReceived,
			[=](ResponseID resp, quint8 channel, const QByteArray &data) {
				if (resp == HANDSHAKE_RESPONSE) {
					handshakeResponseArrived = true;
					eventLoop.quit();
				}
			}
		);

		communicator.SendCommand((CommandID)HANDSHAKE);


		QTimer::singleShot(1000, &eventLoop, &QEventLoop::quit);
		eventLoop.exec();

		QObject::disconnect(connection);

		communicator.Stop();

		if (handshakeResponseArrived) {
			ret << instrumentInfo;
		}
	}

	return ret;
}
//*/