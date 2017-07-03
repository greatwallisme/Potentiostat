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
#include <QFileDialog>
#include <QSettings>

#include <stdlib.h>

#define PREBUILT_EXP_DIR			"./prebuilt/"

bool operator == (const InstrumentInfo &a, const InstrumentInfo &b) {
	return ((a.port.name == a.port.name) && (b.port.serial == a.port.serial));
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

	instrumentEnumerator = new InstrumentEnumerator;

	connect(instrumentEnumerator, &InstrumentEnumerator::RemoveDisconnectedInstruments,
		this, &MainWindow::RemoveInstruments, Qt::QueuedConnection);
	connect(instrumentEnumerator, &InstrumentEnumerator::AddNewInstruments,
		this, &MainWindow::AddInstruments, Qt::QueuedConnection);

	instrumentEnumerator->start();
}
MainWindow::~MainWindow() {
	instrumentEnumerator->terminate();
	instrumentEnumerator->deleteLater();

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
void MainWindow::RemoveInstruments(InstrumentList instruments) {
	QStringList removingInstrumentsNames;

	foreach(auto instrumentToDelete, instruments) {
		for (int i = 0; i < hardware.handlers.count(); ++i) {
			InstrumentInfo &currentInfo(hardware.handlers[i].info);

			if (currentInfo == instrumentToDelete) {
				bool needToSetEnd = false;
				if ((hardware.currentInstrument.handler->info == currentInfo) || (hardware.currentInstrument.handler == hardware.handlers.end())) {
					needToSetEnd = true;
				}

				InstrumentHandler &currentHandler(hardware.handlers[i]);

				if (currentHandler.oper) {
					emit currentHandler.oper->ExperimentCompleted();

					foreach(const QMetaObject::Connection &conn, currentHandler.connections) {
						disconnect(conn);
					}

					currentHandler.oper->deleteLater();
				}

				hardware.handlers.removeAt(i);

				if (needToSetEnd) {
					hardware.currentInstrument.handler = hardware.handlers.end();
				}
				break;
			}
		}

		removingInstrumentsNames << instrumentToDelete.name;
	}

	emit RemoveDisconnectedInstruments(removingInstrumentsNames);
}
void MainWindow::AddInstruments(InstrumentList instruments) {
	QStringList addingInstrumentsNames;

	bool needToSetEnd = false;
	if (hardware.currentInstrument.handler == hardware.handlers.end()) {
		needToSetEnd = true;
	}

	foreach(auto instrumentToAdd, instruments) {
		InstrumentHandler handler;
		handler.oper = 0;
		handler.info = instrumentToAdd;
		handler.experiment.busy = false;
		handler.experiment.paused = false;

		hardware.handlers << handler;

		addingInstrumentsNames << instrumentToAdd.name;
	}

	if (needToSetEnd) {
		hardware.currentInstrument.handler = hardware.handlers.end();
	}

	emit AddNewInstruments(addingInstrumentsNames);
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
/*
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
//*/
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
void MainWindow::PrebuiltExperimentSelected(const AbstractExperiment *exp) {
	prebuiltExperiments.selectedExp = exp;
}
/*
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
//*/
void MainWindow::SelectHardware(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name, channel);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	hardware.currentInstrument.handler = hwIt;
	hardware.currentInstrument.channel = channel;

	if (hardware.currentInstrument.handler->experiment.busy) {
		emit CurrentHardwareBusy();

		if (hardware.currentInstrument.handler->experiment.paused) {
			emit CurrentExperimentPaused();
		}
	}
	else {
		emit CurrentHardwareAvaliable();
	}

	LOG() << "Start working with" << name;
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
	/*
	static HardwareVersion lastHwVersion;
	static QWidget *lastParamsWidget;
	lastParamsWidget = paramsWdg;
	//*/

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
			handler->experiment.paused = false;

			LOG() << "Experiment completed";
			
			emit ExperimentCompleted(handler->experiment.id);
			
			handler->experiment.id = QUuid();
		});

		/*
		hardware.currentInstrument.handler->connections <<
		QObject::connect(instrumentOperator, &InstrumentOperator::CalibrationDataReceived, this, [=](const CalibrationData &calData) {
			LOG() << "Calibration received";
			
			QByteArray nodesData = prebuiltExperiments.selectedExp->GetNodesData(lastParamsWidget, calData, lastHwVersion);
			if (nodesData.isEmpty()) {
				LOG() << "Error while getting user input";
				return;
			}

			MainWindowUI::ExperimentNotes notes;

			if (!MainWindowUI::GetExperimentNotes(this, notes)) {
				return;
			}

			QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);

			QString dirName = settings.value(DATA_SAVE_PATH, "").toString();
			QString tabName = prebuiltExperiments.selectedExp->GetShortName() + " (" + QTime::currentTime().toString("hh:mm:ss") + ")";
			tabName.replace(QRegExp("[\\\\/\\*\\?:\"<>|]"), "_");
			auto dialogRet = QFileDialog::getSaveFileName(this, "Save experiment data", dirName + "/" + tabName, "Data files (*.csv)");

			if (dialogRet.isEmpty()) {
				return;
			}
			dirName = QFileInfo(dialogRet).absolutePath();
			settings.setValue(DATA_SAVE_PATH, dirName);

			auto saveFile = new QFile(this);
			saveFile->setFileName(QFileInfo(dialogRet).absoluteFilePath());
			if (!saveFile->open(QIODevice::WriteOnly)) {
				saveFile->deleteLater();
				return;
			}

			hardware.currentInstrument.handler->experiment.busy = true;
			hardware.currentInstrument.handler->experiment.id = QUuid::createUuid();
			hardware.currentInstrument.handler->experiment.channel = hardware.currentInstrument.channel;

			prebuiltExperiments.selectedExp->SaveDataHeader(*saveFile);

			emit CreateNewDataWindow(hardware.currentInstrument.handler->experiment.id, prebuiltExperiments.selectedExp, saveFile, calData, lastHwVersion);

			LOG() << "Start experiment";
			hardware.currentInstrument.handler->oper->StartExperiment(nodesData, hardware.currentInstrument.channel);
		});
		//*/

		/*
		hardware.currentInstrument.handler->connections <<
		QObject::connect(instrumentOperator, &InstrumentOperator::HardwareVersionReceived, this, [=](const HardwareVersion &hwVersion) {
			LOG() << "Hardware version received";

			lastHwVersion = hwVersion;
			hardware.currentInstrument.handler->oper->RequestCalibrationData();
		});
		//*/

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

	//hardware.currentInstrument.handler->oper->RequestHardwareVersion();

	InstrumentInfo &instrumentInfo(hardware.currentInstrument.handler->info);
	auto nodesData = prebuiltExperiments.selectedExp->GetNodesData(paramsWdg, instrumentInfo.calData, instrumentInfo.hwVer);
	if (nodesData.isEmpty()) {
		LOG() << "Error while getting user input";
		return;
	}

	MainWindowUI::ExperimentNotes notes;

	if (!MainWindowUI::GetExperimentNotes(this, notes)) {
		return;
	}

	QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);

	QString dirName = settings.value(DATA_SAVE_PATH, "").toString();

	QString tabName = prebuiltExperiments.selectedExp->GetShortName() + " (" + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ")";
	tabName.replace(QRegExp("[\\\\/\\*\\?:\"<>|\\.]"), "_");
	auto dialogRet = QFileDialog::getSaveFileName(this, "Save experiment data", dirName + "/" + tabName, "Data files (*.csv)");

	if (dialogRet.isEmpty()) {
		return;
	}
	dirName = QFileInfo(dialogRet).absolutePath();
	settings.setValue(DATA_SAVE_PATH, dirName);

	auto saveFile = new QFile(this);
	saveFile->setFileName(QFileInfo(dialogRet).absoluteFilePath());
	if (!saveFile->open(QIODevice::WriteOnly)) {
		saveFile->deleteLater();
		return;
	}

	hardware.currentInstrument.handler->experiment.busy = true;
	hardware.currentInstrument.handler->experiment.id = QUuid::createUuid();
	hardware.currentInstrument.handler->experiment.channel = hardware.currentInstrument.channel;

	prebuiltExperiments.selectedExp->SaveDataHeader(*saveFile);

	emit CreateNewDataWindow(hardware.currentInstrument.handler->experiment.id, prebuiltExperiments.selectedExp, saveFile, instrumentInfo.calData, instrumentInfo.hwVer);

	LOG() << "Start experiment";
	hardware.currentInstrument.handler->oper->StartExperiment(nodesData, hardware.currentInstrument.channel);
}
QList<MainWindow::InstrumentHandler>::iterator MainWindow::SearchForHandler(const QString &name, quint8 channel) {
	auto hwIt = hardware.handlers.begin();
	for (; hwIt != hardware.handlers.end(); ++hwIt) {
		if (name == hwIt->info.name) {
			break;
		}
	}

	return hwIt;
}
QList<MainWindow::InstrumentHandler>::iterator MainWindow::SearchForHandler(const QUuid &id) {
	auto it = hardware.handlers.begin();
	for (; it != hardware.handlers.end(); ++it) {
		if (it->experiment.id == id) {
			break;
		}
	}
	return it;
}
void MainWindow::StopExperiment(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name, channel);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	StopExperiment(hwIt->experiment.id);
}
void MainWindow::PauseExperiment(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name, channel);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	PauseExperiment(hwIt->experiment.id);

}
void MainWindow::ResumeExperiment(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name, channel);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	ResumeExperiment(hwIt->experiment.id);
}
void MainWindow::PauseExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	if (!it->experiment.busy) {
		return;
	}

	it->experiment.paused = true;

	it->oper->PauseExperiment(it->experiment.channel);
}
void MainWindow::ResumeExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	if (!it->experiment.busy) {
		return;
	}

	it->experiment.paused = false;

	it->oper->ResumeExperiment(it->experiment.channel);
}
void MainWindow::StopExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	if (!it->experiment.busy) {
		return;
	}

	it->oper->StopExperiment(it->experiment.channel);
}
/*
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
//*/

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
