#include "MainWindow.h"

#include <MainWindowUI.h>

#include "InstrumentEnumerator.h"
#include "InstrumentOperator.h"
#include "ExperimentReader.h"

#include "Log.h"

#include <QDir>
#include <QList>

#define PREBUILT_EXP_DIR	"./prebuilt/"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new MainWindowUI(this)),
	instrumentOperator(0)
{
	LoadFonts();
    ui->CreateUI();

	LoadPrebuildExperiments();
}
MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::LoadPrebuildExperiments() {
	LOG() << "Loading prebuilt experiments";

	prebuiltExperiments.clear();

	auto expFileInfos = QDir(PREBUILT_EXP_DIR).entryInfoList(QStringList() << "*.json", QDir::Files | QDir::Readable);

	foreach(const QFileInfo &expFileInfo, expFileInfos) {
		auto filePath = expFileInfo.absoluteFilePath();

		QFile file(filePath);

		if (!file.open(QIODevice::ReadOnly)) {
			continue;
		}

		auto jsonData = file.readAll();

		file.close();

		try {
			prebuiltExperiments << ExperimentReader::GenerateExperimentContainer(jsonData);
		}
		catch (const QString &err) {
			LOG() << "Error in the file" << expFileInfo.fileName() << "-" << err;
		}
	}

	emit PrebuiltExperimentsFound(prebuiltExperiments);
}
void MainWindow::PrebuiltExperimentSelected(int index) {
	emit PrebuiltExperimentSetParameters(prebuiltExperiments.at(index));
}
void MainWindow::SearchHwVendor() {
	LOG() << "Search instruments by the manufacturer name";
	
	auto instrumentList = InstrumentEnumerator().FindInstruments();
	LOG() << "Found" << instrumentList.size() << "instruments";

	emit HardwareFound(instrumentList);
}
void MainWindow::SearchHwHandshake() {
	LOG() << "Search instruments via HANDSHAKE request";

	auto instrumentList = InstrumentEnumerator().FindInstrumentsActive();
	LOG() << "Found" << instrumentList.size() << "instruments";

	emit HardwareFound(instrumentList);
}
void MainWindow::SelectHardware(const InstrumentInfo &info, quint8 channel) {
	static QMetaObject::Connection calibrationDataReceivedConnection;
	static QMetaObject::Connection experimentalDataReceivedConnection;
	static QMetaObject::Connection experimentComletedConnection;
	LOG() << "Start working with" << info.portName;

	currentInstrument.instrumentInfo = info;
	currentInstrument.channel = channel;

	if (instrumentOperator) {
		disconnect(calibrationDataReceivedConnection);
		disconnect(experimentalDataReceivedConnection);
		disconnect(experimentComletedConnection);
		instrumentOperator->deleteLater();
	}

	instrumentOperator = new InstrumentOperator(info);
	experimentComletedConnection = QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentCompleted,
		[=]() {
			LOG() << "Experiment completed";
		}
	);

	calibrationDataReceivedConnection = QObject::connect(instrumentOperator, &InstrumentOperator::CalibrationDataReceived,
		[=](const CalibrationData &calData) {
			LOG() << "Calibration received";
		}
	);


	experimentalDataReceivedConnection = QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentalDataReceived,
		[=](quint8 channel, const ExperimentalData &expData) {
			//LOG() << "Experimental data received";
			emit DataArrived(channel, expData);
		}
	);
}
void MainWindow::RequestCalibration() {
	if (!instrumentOperator) {
		return;
	}

	LOG() << "Request calibration";
	instrumentOperator->RequestCalibrationData();
}
void MainWindow::StartExperiment() {
	if (!instrumentOperator) {
		return;
	}
	
	LOG() << "Start experiment";
	instrumentOperator->StartExperiment(currentInstrument.channel);
}
void MainWindow::StopExperiment() {
	if (!instrumentOperator) {
		return;
	}

	LOG() << "Stop experiment";
	instrumentOperator->StopExperiment(currentInstrument.channel);
}

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QMessageBox>
#include <QFontDatabase>
#include <QDir>

void MainWindow::LoadFonts() {
	QStringList families;

	foreach(const QString &font, QDir(":/GUI/Resources/fonts/").entryList()) {
		int id = QFontDatabase::addApplicationFont(":/GUI/Resources/fonts/" + font);
		families += QFontDatabase::applicationFontFamilies(id);
	}

	QString msg;
	QFontDatabase fdb;
	foreach(const QString &curFamily, families) {
		msg += "Family: \"" + curFamily + "\"\n";

		QStringList styles = fdb.styles(curFamily);
		foreach(const QString &curStyle, styles) {
			bool isSmothyScalable = fdb.isSmoothlyScalable(curFamily, curStyle);
			msg += "Style \"" + curStyle + "\" is" + (isSmothyScalable ? " " : " NOT ") + "smoothy scalable\n";
			if (!isSmothyScalable)
				continue;

			QList<int> smoothSizes = fdb.smoothSizes(curFamily, curStyle);
			QStringList smoothSizesStr;
			foreach(int curSize, smoothSizes) {
				smoothSizesStr << QString("%1").arg(curSize);
			}

			msg += "Smooth sizes: " + smoothSizesStr.join(", ") + "\n";
		}
		msg += "----------------------------\n";
	}

	//QMessageBox::information(0, "", msg);
	//*
	QFile f("FontList.txt");
	if (f.open(QIODevice::WriteOnly)) {
		f.write(msg.toLatin1());
		f.flush();
		f.close();
	}
	//*/
}
void MainWindow::ApplyStyle() {
    QFile f("./GUI.css");
    if(!f.open(QIODevice::ReadOnly))
        return;

    qobject_cast<QApplication *>(QApplication::instance())->setStyleSheet(f.readAll());

    f.close();
}
