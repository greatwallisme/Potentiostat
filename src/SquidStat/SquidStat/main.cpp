#include <QtCore/QCoreApplication>

#include "Log.h"

#include "InstrumentEnumerator.h"
#include "InstrumentOperator.h"
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

	if (instrumentList.size()) {
		InstrumentInfo &info(instrumentList[0]);

		LOG() << "Start working with" << info.portName;

		static InstrumentOperator instrumentOperator(info);

		QObject::connect(&instrumentOperator, &InstrumentOperator::CalibrationDataReceived,
			[=](const CalibrationData &calData) {
				LOG() << "Calibration received";

				instrumentOperator.StartExperiment();
			}
		);

		instrumentOperator.RequestCalibrationData();
	}

	return a.exec();
}
