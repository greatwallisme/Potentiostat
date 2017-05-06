#include <QtCore/QCoreApplication>

#include "Log.h"

#include "InstrumentEnumerator.h"
#include "ExternalStructures.h"

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);

	InstrumentEnumerator instrumentEnumerator;

	LOG() << "Search instruments by the manufacturer name";
	auto instrumentList = instrumentEnumerator.FindInstruments();

	LOG() << "Found" << instrumentList.size() << "instruments";

	foreach(const InstrumentInfo &info, instrumentList) {
		LOG() << info.portName << ": " << info.serial;
	}

	LOG() << "Search instruments via HANDSHAKE request";
	instrumentList = instrumentEnumerator.FindInstrumentsActive();

	LOG() << "Found" << instrumentList.size() << "instruments";

	foreach(const InstrumentInfo &info, instrumentList) {
		LOG() << info.portName << ": " << info.serial;
	}

	foreach(const InstrumentInfo &info, instrumentList) {
		bool ok;
		CalibrationData calData = instrumentEnumerator.GetCalibrationData(info, &ok);

		if (ok) {
			LOG() << "Calibration received for" << info.portName;
		}
		else {
			LOG() << "Calibration NOT received";
		}
	}

	;

	return a.exec();
}
