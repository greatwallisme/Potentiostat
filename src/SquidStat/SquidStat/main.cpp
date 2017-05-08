#include <QApplication>

#include "Log.h"

#include "InstrumentEnumerator.h"
#include "InstrumentOperator.h"
#include "ExternalStructures.h"

#include "MainWindow.h"

#include <QTimer>
#include <QtGlobal>


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	qInstallMessageHandler(LogMessageHandler);

	MainWindow w;

	w.show();

	return a.exec();

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

	InstrumentOperator *instrumentOperator = 0;
	if (instrumentList.size()) {
		InstrumentInfo &info(instrumentList[0]);

		LOG() << "Start working with" << info.portName;

		instrumentOperator = new InstrumentOperator(info);

		QObject::connect(instrumentOperator, &InstrumentOperator::CalibrationDataReceived,
			[=](const CalibrationData &calData) {
				LOG() << "Calibration received";

				instrumentOperator->StartExperiment();
			}
		);


		QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentalDataReceived,
			[=](quint8 channel, const ExperimentalData &expData) {
				LOG() << "Experimental data received";
			}
		);

		QTimer::singleShot(500, instrumentOperator, &InstrumentOperator::RequestCalibrationData);
		QTimer::singleShot(5000, [=]() {
			instrumentOperator->StopExperiment();
		});
	}

	if (instrumentOperator) {
		delete instrumentOperator;
	}

	return 0;
}
