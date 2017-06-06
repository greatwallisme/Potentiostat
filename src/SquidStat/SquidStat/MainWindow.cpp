#include "MainWindow.h"

#include <MainWindowUI.h>

#include "InstrumentEnumerator.h"
#include "InstrumentOperator.h"
#include "ExperimentReader.h"

#include <ExperimentFactoryInterface.h>

#include "Log.h"

#include <QDir>
#include <QList>

#include <QPluginLoader>

#include <stdlib.h>

#define PREBUILT_EXP_DIR	"./prebuilt/"

bool operator == (const InstrumentInfo &a, const InstrumentInfo &b) {
	return ((a.portName == a.portName) && (b.serial == a.serial));
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new MainWindowUI(this))
{
	prebuiltExperiments.selectedExp = 0;
	hardware.currentInstrument.handler = hardware.handlers.end();

	LoadFonts();
    ui->CreateUI();

	LoadPrebuildExperiments();
}
MainWindow::~MainWindow() {
	CleanupCurrentHardware();
    delete ui;

	CleanupExperiments();
}
void MainWindow::CleanupExperiments() {
	foreach(auto exp, prebuiltExperiments.expList) {
		delete exp;
	}

	foreach(auto loader, prebuiltExperiments.expLoaders) {
		loader->unload();
		loader->deleteLater();
	}
}
void MainWindow::CleanupCurrentHardware() {
	for (auto it = hardware.handlers.begin(); it != hardware.handlers.end(); ++it) {
		if (0 == it->oper) {
			continue;
		}

		it->oper->StopExperiment();

		foreach(const QMetaObject::Connection &conn, it->connections) {
			disconnect(conn);
		}

		it->oper->deleteLater();
	}

	hardware.handlers.clear();
}
void MainWindow::FillHardware(const InstrumentList &instrumentList) {
	for (auto it = instrumentList.constBegin(); it != instrumentList.constEnd(); ++it) {
		InstrumentHandler handler;
		handler.oper = 0;
		handler.info = *it;
		handler.experiment.busy = false;

		hardware.handlers << handler;
	}

	hardware.currentInstrument.handler = hardware.handlers.end();
}
void MainWindow::LoadPrebuildExperiments() {
	LOG() << "Loading prebuilt experiments";

	auto expFileInfos = QDir(PREBUILT_EXP_DIR).entryInfoList(QStringList() << "*.dll", QDir::Files | QDir::Readable);

	foreach(const QFileInfo &expFileInfo, expFileInfos) {
		auto filePath = expFileInfo.absoluteFilePath();

		auto loader = new QPluginLoader(filePath, this);

		if (!loader->load()) {
			loader->deleteLater();
			continue;
		}

		auto instance = qobject_cast<ExperimentFactoryInterface*>(loader->instance());

		if (0 == instance) {
			loader->unload();
			loader->deleteLater();
			continue;
		}

		prebuiltExperiments.expList << instance->CreateExperiment();
		prebuiltExperiments.expLoaders << loader;
	}

	//prebuiltExperiments.expList << new ExampleExperiment;

	emit PrebuiltExperimentsFound(prebuiltExperiments.expList);

	/*
	prebuiltExperiments.ecList.clear();

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
			prebuiltExperiments.ecList << ExperimentReader::GenerateExperimentContainer(jsonData);
		}
		catch (const QString &err) {
			LOG() << "Error in the file" << expFileInfo.fileName() << "-" << err;
		}
	}

	emit PrebuiltExperimentsFound(prebuiltExperiments.ecList);
	//*/
}
//void MainWindow::PrebuiltExperimentSelected(int index) {
void MainWindow::PrebuiltExperimentSelected(const AbstractExperiment *exp) {
	prebuiltExperiments.selectedExp = exp;

	/*
	if ( (index < 0) || (index >= prebuiltExperiments.ecList.size()) ) {
		return;
	}

	prebuiltExperiments.selectedEcIndex = index;
	emit PrebuiltExperimentSetDescription(prebuiltExperiments.ecList.at(index));
	try {
		auto ecPtrList = ExperimentReader::GetNodeListForUserInput(prebuiltExperiments.ecList[index]);
		emit PrebuiltExperimentSetParameters(ecPtrList);
	}
	catch (const QString &err) {
		LOG() << err;
	}
	//*/
}
void MainWindow::SearchHwVendor() {
	LOG() << "Search instruments by the manufacturer name";
	
	auto instrumentList = InstrumentEnumerator().FindInstruments();
	LOG() << "Found" << instrumentList.size() << "instruments";

	CleanupCurrentHardware();
	FillHardware(instrumentList);

	emit HardwareFound(instrumentList);
}
void MainWindow::SearchHwHandshake() {
	LOG() << "Search instruments via HANDSHAKE request";

	auto instrumentList = InstrumentEnumerator().FindInstrumentsActive();
	LOG() << "Found" << instrumentList.size() << "instruments";

	CleanupCurrentHardware();
	FillHardware(instrumentList);

	emit HardwareFound(instrumentList);
}
void MainWindow::SelectHardware(const InstrumentInfo &info, quint8 channel) {
	auto hwIt = hardware.handlers.begin();
	for (; hwIt != hardware.handlers.end(); ++hwIt) {
		if (info == hwIt->info) {
			break;
		}
	}
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	hardware.currentInstrument.handler = hwIt;
	hardware.currentInstrument.channel = channel;

	LOG() << "Start working with" << info.portName;
	/*
	static QMetaObject::Connection calibrationDataReceivedConnection;
	static QMetaObject::Connection experimentalDataReceivedConnection;
	static QMetaObject::Connection experimentComletedConnection;

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


	experimentalDataReceivedConnection =
		QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentalDataReceived,
			this, &MainWindow::DataArrived);
	//*/
	/*
	experimentalDataReceivedConnection = QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentalDataReceived,
		[=](quint8 channel, const ExperimentalData &expData) {
			//LOG() << "Experimental data received";
			emit DataArrived(channel, expData);
		}
	);
	//*/
}
void MainWindow::RequestCalibration() {
	/*
	if (!instrumentOperator) {
		return;
	}

	LOG() << "Request calibration";
	instrumentOperator->RequestCalibrationData();
	//*/
}
QList<MainWindow::InstrumentHandler>::iterator MainWindow::SearchForHandler(InstrumentOperator *oper) {
	QList<MainWindow::InstrumentHandler>::iterator ret = hardware.handlers.begin();

	for (; ret != hardware.handlers.end(); ++ret) {
		if (ret->oper == oper) {
			break;
		}
	}

	return ret;
}
void MainWindow::StartExperiment(QWidget *paramsWdg) {
	if (hardware.currentInstrument.handler == hardware.handlers.end()) {
		LOG() << "No instruments selected";
		return;
	}

	if (hardware.currentInstrument.handler->experiment.busy) {
		LOG() << "Current instrument is busy now";
		return;
	}

	if (0 == prebuiltExperiments.selectedExp) {
		LOG() << "No experiment selected";
		return;
	}
	
	if (0 == hardware.currentInstrument.handler->oper) {
		InstrumentOperator *instrumentOperator = new InstrumentOperator(hardware.currentInstrument.handler->info);
		hardware.currentInstrument.handler->oper = instrumentOperator;

		hardware.currentInstrument.handler->connections <<
		connect(instrumentOperator, &InstrumentOperator::ExperimentCompleted, this, [=]() {
			auto oper = qobject_cast<InstrumentOperator*>(sender());
			if (0 == oper) {
				LOG() << "Unexpected InstrumentOperator pointer";
				return;
			}

			auto handler = SearchForHandler(oper);
			if (handler == hardware.handlers.end()) {
				LOG() << "Hardware handler not found";
				return;
			}

			handler->experiment.busy = false;
			handler->experiment.id = QUuid();

			LOG() << "Experiment completed";
		});

		hardware.currentInstrument.handler->connections <<
		QObject::connect(instrumentOperator, &InstrumentOperator::CalibrationDataReceived, this, [=](const CalibrationData &calData) {
			LOG() << "Calibration received";
			
			QByteArray nodesData = prebuiltExperiments.selectedExp->GetNodesData(paramsWdg, calData);
			if (nodesData.isEmpty()) {
				LOG() << "Error while getting user input";
				return;
			}

			hardware.currentInstrument.handler->experiment.busy = true;
			hardware.currentInstrument.handler->experiment.id = QUuid::createUuid();
			hardware.currentInstrument.handler->experiment.channel = hardware.currentInstrument.channel;

			emit CreateNewDataWindow(hardware.currentInstrument.handler->experiment.id, prebuiltExperiments.selectedExp->GetShortName());

			LOG() << "Start experiment";
			hardware.currentInstrument.handler->oper->StartExperiment(nodesData, hardware.currentInstrument.channel);
		});


		hardware.currentInstrument.handler->connections <<
		QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentalDataReceived, this, [=](quint8 channel, const ExperimentalData &expData) {
			auto oper = qobject_cast<InstrumentOperator*>(sender());
			if (0 == oper) {
				LOG() << "Unexpected InstrumentOperator pointer";
				return;
			}

			auto handler = SearchForHandler(oper);
			if (handler == hardware.handlers.end()) {
				LOG() << "Hardware handler not found";
				return;
			}

			emit DataArrived(handler->experiment.id, channel, expData);
		});
	}
	

	hardware.currentInstrument.handler->oper->RequestCalibrationData();
}
void MainWindow::StopExperiment(const QUuid &id) {
	auto it = hardware.handlers.begin();
	for (; it != hardware.handlers.end(); ++it) {
		if (it->experiment.id == id) {
			break;
		}
	}

	if (it == hardware.handlers.end()) {
		return;
	}

	if (!it->experiment.busy) {
		return;
	}

	it->oper->StopExperiment(it->experiment.channel);
	/*
	if (!instrumentOperator) {
		return;
	}

	LOG() << "Stop experiment";
	instrumentOperator->StopExperiment(currentInstrument.channel);
	//*/
}
void MainWindow::SaveData(const QVector<qreal> &xData, const QVector<qreal> &yData, const QString &fileName) {
	QFile f(fileName);

	if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return;
	}

	int minCount = qMin(xData.size(), yData.size());

	for (quint64 i = 0; i < minCount; ++i) {
		f.write(QString("%1;%2\n").arg(xData[i]).arg(yData[i]).toLatin1());
	}

	f.close();
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
