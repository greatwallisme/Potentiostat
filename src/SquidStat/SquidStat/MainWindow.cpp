#include "MainWindow.h"

#include <MainWindowUI.h>

#include "HidCommunicator.h"
#include "BootloaderOperator.h"
#include "InstrumentEnumerator.h"
#include "InstrumentOperator.h"
#include "ExperimentReader.h"
#include "CustomExperimentRunner.h"
#include "ManualExperimentRunner.h"

#define _MATH_DEFINES_DEFINED
#include "ExperimentCalcHelper.h"

#include <ExperimentFactoryInterface.h>
#include <BuilderElementFactoryInterface.h>

#include "Log.h"

#include <QDir>
#include <QList>

#include <QPluginLoader>
#include <QFileDialog>
#include <QSettings>

#include <Disconnector.h>

#include <stdlib.h>

#include <Disconnector.h>
#include <QEventLoop>
#include <QStandardPaths>
#include <QDesktopWidget>


#define PREBUILT_EXP_DIR		"./prebuilt/"
#define ELEMENTS_DIR			"./elements/"
#define CUSTOM_EXP_DIR			"./custom/"

bool operator == (const InstrumentInfo &a, const InstrumentInfo &b) {
	return ((a.port.name == a.port.name) && (b.port.serial == a.port.serial));
}

void RemoveMacFocusRect(QWidget *w) {
    w->setAttribute(Qt::WA_MacShowFocusRect, false);

    foreach(QObject *child, w->children()) {
        QWidget *childW = qobject_cast<QWidget*>(child);

        if(0 == childW) {
            continue;
        }

        RemoveMacFocusRect(childW);
    }
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
	LoadBuilderElements();
	UpdateCustomExperimentList();

	instrumentEnumerator = new InstrumentEnumerator;

	connect(instrumentEnumerator, &InstrumentEnumerator::RemoveDisconnectedInstruments,
		this, &MainWindow::RemoveInstruments, Qt::QueuedConnection);
	connect(instrumentEnumerator, &InstrumentEnumerator::AddNewInstruments,
		this, &MainWindow::AddInstruments, Qt::QueuedConnection);

	instrumentEnumerator->start();
	
	QRect screenSize = QDesktopWidget().availableGeometry(this);
	this->setMinimumHeight(screenSize.height() < 768 ? screenSize.height() * 0.95 : 768);
	this->setMinimumWidth(screenSize.width() < 1366 ? screenSize.width() * 0.95 : 1366);

    ApplyStyle();

    RemoveMacFocusRect(this);
}
MainWindow::~MainWindow() {
	instrumentEnumerator->terminate();
	//instrumentEnumerator->deleteLater();

	CleanupCurrentHardware();

	//ui->DisconnectAll();

	//CleanupExperiments();
	//CleanupBuilderElements();

	//delete ui;
}
void MainWindow::CleanupExperiments() {
	foreach(auto exp, prebuiltExperiments.customExpMap) {
		//delete exp;
		//((CustomExperimentRunner*)exp)->deleteLater();
		delete ((CustomExperimentRunner*)exp);
	}

	foreach(auto exp, prebuiltExperiments.expList) {
		delete exp;
	}

	delete ManualExperimentRunner::Instance();

	foreach(auto loader, prebuiltExperiments.expLoaders) {
		loader->unload();
		//loader->deleteLater();
		delete loader;
	}
}
void MainWindow::CleanupBuilderElements() {
	foreach(auto elem, builderElements.elements) {
		delete elem;
	}

	foreach(auto loader, builderElements.loaders) {
		loader->unload();
		//loader->deleteLater();
		delete loader;
	}
}
void MainWindow::RemoveInstruments(InstrumentList instruments) {
	QStringList removingInstrumentsNames;

	foreach(auto instrumentToDelete, instruments) {
		for (int i = 0; i < hardware.handlers.count(); ++i) {
			InstrumentInfo &currentInfo(hardware.handlers[i].info);

			if (currentInfo == instrumentToDelete) {
				bool needToSetEnd = false;
				if ((hardware.currentInstrument.handler == hardware.handlers.end()) || (hardware.currentInstrument.handler->info == currentInfo)) {
					needToSetEnd = true;
				}

				InstrumentHandler &currentHandler(hardware.handlers[i]);

				foreach(auto &expDescr, currentHandler.experiment) {
					if (expDescr.busy) {
						emit ExperimentCompleted(expDescr.id);
					}
				}

				if (currentHandler.oper) {
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
	QList<HardwareUiDescription> addingInstrumentsNames;

	bool needToSetEnd = false;
	if (hardware.currentInstrument.handler == hardware.handlers.end()) {
		needToSetEnd = true;
	}

	foreach(auto instrumentToAdd, instruments) {
		InstrumentHandler handler;
		handler.oper = 0;
		handler.info = instrumentToAdd;
		handler.experiment.resize(handler.info.channelAmount);
		for (auto it = handler.experiment.begin(); it != handler.experiment.end(); ++it) {
			it->busy = false;
			it->paused = false;
			it->isManual = false;
		}

		hardware.handlers << handler;
		CreateLogicForInstrument(hardware.handlers.last());

		HardwareUiDescription hwUiDescr;
		hwUiDescr.name = instrumentToAdd.name;
		hwUiDescr.channelAmount = instrumentToAdd.channelAmount;
		hwUiDescr.hwModel = instrumentToAdd.hwVer.hwModel;

		addingInstrumentsNames << hwUiDescr;
	}

	if (needToSetEnd) {
		hardware.currentInstrument.handler = hardware.handlers.end();
	}

	emit AddNewInstruments(addingInstrumentsNames);
}
void MainWindow::CreateLogicForInstrument(MainWindow::InstrumentHandler &newHandler) {
	InstrumentOperator *instrumentOperator = new InstrumentOperator(newHandler.info);
	newHandler.oper = instrumentOperator;

	auto experimentTrigger = new ExperimentTrigger(instrumentOperator);
	newHandler.trigger = experimentTrigger;

	connect(experimentTrigger, &ExperimentTrigger::StopExperiment,
		this, static_cast<void(MainWindow::*)(const QUuid&)>(&MainWindow::StopExperiment));

	newHandler.connections <<
	connect(instrumentOperator, &InstrumentOperator::ExperimentPaused, this, [=](quint8 channel) {
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

		handler->experiment[channel].paused = true;

		LOG() << "Experiment paused";

		emit ExperimentPaused(handler->experiment[channel].id);
		if (handler == hardware.currentInstrument.handler) {
			emit CurrentExperimentPaused();
		}
	});

	newHandler.connections <<
	connect(instrumentOperator, &InstrumentOperator::ExperimentResumed, this, [=](quint8 channel) {
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

		handler->experiment[channel].paused = false;

		LOG() << "Experiment resumed";

		emit ExperimentResumed(handler->experiment[channel].id);
		if (handler == hardware.currentInstrument.handler) {
			emit CurrentExperimentResumed();
		}
	});

	newHandler.connections <<
	connect(instrumentOperator, &InstrumentOperator::ExperimentCompleted, this, [=](quint8 channel) {
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

		handler->experiment[channel].busy = false;
		handler->experiment[channel].paused = false;

		LOG() << "Experiment completed";

		emit ExperimentCompleted(handler->experiment[channel].id);
		if (handler == hardware.currentInstrument.handler) {
			emit CurrentExperimentCompleted();
		}

		handler->experiment[channel].id = QUuid();
	});

	newHandler.connections <<
	connect(instrumentOperator, &InstrumentOperator::ExperimentNodeBeginning, this, [=](quint8 channel, const ExperimentNode_t &node) {
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
		emit ExperimentNodeBeginning(handler->experiment[channel].id, channel, node);
	});

	newHandler.connections <<
	connect(instrumentOperator, &InstrumentOperator::ExperimentalDcDataReceived, this, [=](quint8 channel, const ExperimentalDcData &expData) {
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

		emit DcDataArrived(handler->experiment[channel].id, expData, handler->trigger, handler->experiment[channel].paused);
	});
	newHandler.connections <<
	connect(instrumentOperator, &InstrumentOperator::ExperimentalAcDataReceived, this, [=](quint8 channel, const QByteArray &expData) {
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

		emit AcDataArrived(handler->experiment[channel].id, expData, handler->trigger, handler->experiment[channel].paused);
	});
}
void MainWindow::CleanupCurrentHardware() {
	for (auto it = hardware.handlers.begin(); it != hardware.handlers.end(); ++it) {
		if (0 == it->oper) {
			continue;
		}

		for (int i = 0; i < it->experiment.size(); ++i) {
			it->oper->StopExperiment(i);
		}

		foreach(const QMetaObject::Connection &conn, it->connections) {
			disconnect(conn);
		}

		//it->oper->deleteLater();
		delete it->oper;
	}

	hardware.handlers.clear();
}
#include <QApplication>
void MainWindow::LoadBuilderElements() {
    LOG() << "Loading builder elements";

    auto appPath = QFileInfo(QApplication::applicationFilePath()).absolutePath();

    auto expFileInfos = QDir(appPath + "/" ELEMENTS_DIR).entryInfoList(QStringList() << "*.dll" << "*.dylib", QDir::Files | QDir::Readable | QDir::NoSymLinks);

	foreach(const QFileInfo &expFileInfo, expFileInfos) {
        auto filePath = expFileInfo.absoluteFilePath();

		auto loader = new QPluginLoader(filePath, this);

		if (!loader->load()) {
			loader->deleteLater();
			continue;
		}

		auto instance = qobject_cast<BuilderElementFactoryInterface*>(loader->instance());

		if (0 == instance) {
			loader->unload();
			loader->deleteLater();
			continue;
		}

		builderElements.elements << instance->CreateElement();
		builderElements.loaders << loader;
	}

	emit BuilderElementsFound(builderElements.elements);
}
void MainWindow::LoadPrebuildExperiments() {
	LOG() << "Loading prebuilt experiments";

    auto appPath = QFileInfo(QApplication::applicationFilePath()).absolutePath();

    auto expFileInfos = QDir(appPath + "/" PREBUILT_EXP_DIR).entryInfoList(QStringList() << "*.dll" << "*.dylib", QDir::Files | QDir::Readable | QDir::NoSymLinks);

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
}
void MainWindow::PrebuiltExperimentSelected(const AbstractExperiment *exp) {
	prebuiltExperiments.selectedExp = exp;
}
void MainWindow::UpdateCurrentExperimentState() {
	auto &currentHandler(hardware.currentInstrument.handler);
	auto &currentChannel(hardware.currentInstrument.channel);
	
	if (currentHandler == hardware.handlers.end()) {
		return;
	}

	if (currentHandler->experiment[currentChannel].busy) {
		emit CurrentHardwareBusy();

		if (currentHandler->experiment[currentChannel].paused) {
			emit CurrentExperimentPaused();
		}
		else {
			emit CurrentExperimentResumed();
		}

		if (!currentHandler->experiment[currentChannel].isManual) {
			emit CurrentExperimentIsNotManual();
		}
	}
	else {
		emit CurrentExperimentCompleted();
	}
}
void MainWindow::SelectHardware(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name/*, channel*/);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	hardware.currentInstrument.handler = hwIt;
	hardware.currentInstrument.channel = channel;

#ifndef QT_NO_DEBUG
	LOG() << "Start working with" << name;
#endif
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
void MainWindow::StartExperiment(QWidget *paramsWdg, const QUuid &existingId) {
	auto &currentHandler(hardware.currentInstrument.handler);
	auto currentChannel = hardware.currentInstrument.channel;

	if (currentHandler == hardware.handlers.end()) {
		LOG() << "No instruments selected";
		return;
	}

	if (currentHandler->experiment[currentChannel].busy) {
		LOG() << "Current channel is busy now";
		return;
	}

	auto expPtr	= prebuiltExperiments.selectedExp;

	if (0 == expPtr) {
		LOG() << "No experiment selected";
		return;
	}
	
	InstrumentInfo &instrumentInfo(hardware.currentInstrument.handler->info);
	auto nodesData = expPtr->GetNodesData(paramsWdg, instrumentInfo.calData[currentChannel], instrumentInfo.hwVer);
	if (nodesData.isEmpty()) {
		LOG() << "Error while getting user input";
		return;
	}

	ExperimentNotes notes;

	if (!MainWindowUI::GetExperimentNotes(this, notes)) {
		return;
	}

	QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);
	QString dirName = settings.value(DATA_SAVE_PATH, "").toString();

	auto types = expPtr->GetTypes();

	QList<StartExperimentParameters> startParams;

	QUuid newId = existingId.isNull() ? QUuid::createUuid() : existingId;
	
	hardware.currentInstrument.handler->trigger->SetUuid(newId);
	bool ok = true;
	foreach(auto type, types) {
		StartExperimentParameters curParam;
		curParam.id = newId;
		curParam.type = type;
		curParam.exp = expPtr;
		curParam.cal = instrumentInfo.calData[currentChannel];
		curParam.hwVer = instrumentInfo.hwVer;
		curParam.notes = notes;

		QString tabName = expPtr->GetShortName() + " (" + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ")";
		if (types.count() > 1) {
			switch (type) {
				case ET_DC:
					tabName = "DC - " + tabName;
					break;
				case ET_AC:
					tabName = "AC - " + tabName;
					break;
			}
		}
		curParam.name = tabName;
		tabName.replace(QRegExp("[\\\\/\\*\\?:\"<>|\\.]"), "_");

		auto dialogRet = QFileDialog::getSaveFileName(this, "Save experiment data", dirName + "/" + tabName, "Data files (*.csv)");

		if (dialogRet.isEmpty()) {
			ok = false;
			break;
		}

		dirName = QFileInfo(dialogRet).absolutePath();
		settings.setValue(DATA_SAVE_PATH, dirName);
	
		auto saveFile = new QFile(this);
		saveFile->setFileName(QFileInfo(dialogRet).absoluteFilePath());
		if (!saveFile->open(QIODevice::WriteOnly)) {
			saveFile->deleteLater();
			ok = false;
			break;
		}
		curParam.file = saveFile;
		curParam.filePath = QFileInfo(dialogRet).absoluteFilePath();

		startParams << curParam;
	}

	if (!ok) {
		foreach(auto &param, startParams) {
			param.file->close();
			param.file->deleteLater();
		}
		return;
	}

	hardware.currentInstrument.handler->experiment[currentChannel].busy = true;
	hardware.currentInstrument.handler->experiment[currentChannel].paused = false;
	hardware.currentInstrument.handler->experiment[currentChannel].isManual = false;
	hardware.currentInstrument.handler->experiment[currentChannel].id = newId;
	//hardware.currentInstrument.handler->experiment.channel = hardware.currentInstrument.channel;

	foreach(auto &param, startParams) {
		switch (param.type) {
			case ET_DC:
				expPtr->SaveDcDataHeader(*param.file, param.notes);
				break;
			case ET_AC:
				expPtr->SaveAcDataHeader(*param.file, param.notes);
				break;
		}

		if (existingId.isNull()) {
			emit CreateNewDataWindow(param);
		}
		else {
			//emit UpdateDataWindow(param);
		}
	}

	LOG() << "Start experiment";
	hardware.currentInstrument.handler->oper->StartExperiment(nodesData, hardware.currentInstrument.channel);
}
QList<MainWindow::InstrumentHandler>::iterator MainWindow::SearchForHandler(const QString &name/*, quint8 channel*/) {
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
		bool found = false;
		for (int i = 0; i < it->experiment.size(); ++i) {
			if (it->experiment[i].id == id) {
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}
	return it;
}
quint8 MainWindow::SearchForChannel(QList<MainWindow::InstrumentHandler>::iterator it, const QUuid &id) {
	for (quint8 i = 0; i < it->experiment.size(); ++i) {
		if (it->experiment[i].id == id) {
			return i;
		}
	}

	return -1;
}
void MainWindow::StopExperiment(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name/*, channel*/);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	StopExperiment(hwIt->experiment[channel].id);
}
void MainWindow::PauseExperiment(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name/*, channel*/);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	PauseExperiment(hwIt->experiment[channel].id);

}
void MainWindow::ResumeExperiment(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name/*, channel*/);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	ResumeExperiment(hwIt->experiment[channel].id);
}
void MainWindow::PauseExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);

	if (!it->experiment[channel].busy) {
		return;
	}

	it->oper->PauseExperiment(channel);
}
void MainWindow::ResumeExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);

	if (!it->experiment[channel].busy) {
		return;
	}

	it->oper->ResumeExperiment(channel);
}
void MainWindow::StopExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);

	if (!it->experiment[channel].busy) {
		return;
	}

	it->oper->StopExperiment(channel);
}
void MainWindow::StartManualExperiment(const QUuid &id) {
	auto it(hardware.currentInstrument.handler);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = hardware.currentInstrument.channel;

	if (it->experiment[channel].busy) {
		return;
	}
	
	InstrumentInfo &instrumentInfo(it->info);

	ExperimentNotes notes;

	if (!MainWindowUI::GetExperimentNotes(this, notes)) {
		return;
	}

	QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);
	QString dirName = settings.value(DATA_SAVE_PATH, "").toString();

	auto expPtr = ManualExperimentRunner::Instance();
	auto types = expPtr->GetTypes();

	QList<StartExperimentParameters> startParams;

	hardware.currentInstrument.handler->trigger->SetUuid(id);
	bool ok = true;
	foreach(auto type, types) {
		StartExperimentParameters curParam;
		curParam.id = id;
		curParam.type = type;
		curParam.exp = expPtr;
		curParam.cal = instrumentInfo.calData[channel];
		curParam.hwVer = instrumentInfo.hwVer;
		curParam.notes = notes;

		QString tabName = expPtr->GetShortName() + " (" + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ")";
		if (types.count() > 1) {
			switch (type) {
			case ET_DC:
				tabName = "DC - " + tabName;
				break;
			case ET_AC:
				tabName = "AC - " + tabName;
				break;
			}
		}
		curParam.name = tabName;
		tabName.replace(QRegExp("[\\\\/\\*\\?:\"<>|\\.]"), "_");

		auto dialogRet = QFileDialog::getSaveFileName(this, "Save experiment data", dirName + "/" + tabName, "Data files (*.csv)");

		if (dialogRet.isEmpty()) {
			ok = false;
			break;
		}

		dirName = QFileInfo(dialogRet).absolutePath();
		settings.setValue(DATA_SAVE_PATH, dirName);

		auto saveFile = new QFile(this);
		saveFile->setFileName(QFileInfo(dialogRet).absoluteFilePath());
		if (!saveFile->open(QIODevice::WriteOnly)) {
			saveFile->deleteLater();
			ok = false;
			break;
		}
		curParam.file = saveFile;
		curParam.filePath = QFileInfo(dialogRet).absoluteFilePath();

		startParams << curParam;
	}

	if (!ok) {
		foreach(auto &param, startParams) {
			param.file->close();
			param.file->deleteLater();
		}
		return;
	}

	if (startParams.empty()) {
		return;
	}

	emit SetManualStartParams(startParams.first());

	it->experiment[channel].busy = true;
	it->experiment[channel].paused = false;
	it->experiment[channel].isManual = true;
	it->experiment[channel].id = id;

	LOG() << "Manual experiment started";
	it->oper->StartManualExperiment(channel);
}
void MainWindow::SetManualSamplingParams(const QUuid &id, double value) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);

	if (!it->experiment[channel].busy) {
		return;
	}

	Manual::SamplingParams params;
	ExperimentNode_t node;

  value = value <= 0.0001 ? 1 : value;

	ExperimentCalcHelperClass::GetSamplingParams_staticDAC(it->info.hwVer.hwModel, &node, value);
	params.timerDiv = node.samplingParams.ADCTimerDiv;
	params.timerPeriod = node.samplingParams.ADCTimerPeriod;
	params.ADCbufsize = node.samplingParams.ADCBufferSizeOdd;

	it->oper->SetManualSamplingParams(channel, params);
}
void MainWindow::SetManualGalvanoSetpoint(const QUuid &id, double setpoint, QString units_str) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);


	if (!it->experiment[channel].busy) {
		return;
	}

	Manual::GalvanoSetpoint params;
  
  setpoint *= ExperimentCalcHelperClass::GetUnitsMultiplier(units_str);
	params.range = ExperimentCalcHelperClass::GetMinCurrentRange(it->info.hwVer.hwModel, &it->info.calData[channel], setpoint);
  params.g_setpoint = ExperimentCalcHelperClass::GetBINCurrent(&it->info.calData[channel], (currentRange_t)params.range, setpoint);

	/////////////////////////////////////////
	//params.g_setpoint = 1024;
	//params.range = 0;
	/////////////////////////////////////////

	it->oper->SetManualGalvanoSetpoint(channel, params);
}
void MainWindow::SetManualPotentioSetpoint(const QUuid &id, double setpoint) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);


	if (!it->experiment[channel].busy) {
		return;
	}

	Manual::PotentioSetpoint params;
	params.setpoint = ExperimentCalcHelperClass::GetBINVoltageForDAC(&it->info.calData[channel], setpoint);

	/////////////////////////////////////////
	//params.setpoint = -1024;
	/////////////////////////////////////////


	it->oper->SetManualPotentioSetpoint(channel, params);
}
void MainWindow::SetManualOcp(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);


	if (!it->experiment[channel].busy) {
		return;
	}

	it->oper->SetManualOcp(channel);
}
void MainWindow::SetCurrentRangingMode(const QUuid &id, quint8 range) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);


	if (!it->experiment[channel].busy) {
		return;
	}

	Manual::RangingMode params;
	params.currentRangeMode = range;

	/////////////////////////////////////////
	params.currentRangeMode = 0;
	/////////////////////////////////////////

	it->oper->SetCurrentRangingMode(channel, params);
}
void MainWindow::StopManualExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);

	if (!it->experiment[channel].busy) {
		return;
	}

	/////////////////////////////////////////
	Manual::SamplingParams params;
	params.timerDiv = 0;
	params.timerPeriod = 390625;
	params.ADCbufsize = 128;
	/////////////////////////////////////////

	SetManualOcp(id);
	it->oper->SetManualSamplingParams(channel, params);

	it->experiment[channel].busy = false;

	it->experiment[channel].busy = false;
	it->experiment[channel].paused = false;


	LOG() << "Manual experiment completed";
	emit ExperimentCompleted(id);

	it->experiment[channel].id = QUuid();
}
void MainWindow::PauseManualExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);

	if (!it->experiment[channel].busy) {
		return;
	}
	it->experiment[channel].paused = true;

	/////////////////////////////////////////
	Manual::SamplingParams params;
	params.timerDiv = 0;
	params.timerPeriod = 390625;
	params.ADCbufsize = 128;
	/////////////////////////////////////////

	//params = ExperimentCalcHelperClass::DoSomeMagic(value);

	SetManualOcp(id);
	it->oper->SetManualSamplingParams(channel, params);

	LOG() << "Manual experiment paused";
	emit ExperimentPaused(id);
}
void MainWindow::ResumeManualExperiment(const QUuid &id) {
	auto it = SearchForHandler(id);

	if (it == hardware.handlers.end()) {
		return;
	}

	auto channel = SearchForChannel(it, id);

	if (!it->experiment[channel].busy) {
		return;
	}
	it->experiment[channel].paused = false;

	LOG() << "Manual experiment resumed";
	emit ExperimentResumed(id);
}

void MainWindow::FillElementPointers(BuilderContainer &bc, const QMap<QString, AbstractBuilderElement*> &elemMap) {
	if (bc.type == BuilderContainer::ELEMENT) {
		bc.elem.ptr = elemMap.value(bc.elem.name, 0);

		if (!bc.elem.ptr) {
			throw QString("Can not find the element named \"%1\"").arg(bc.elem.name);
		}

		return;
	}

	for (auto it = bc.elements.begin(); it != bc.elements.end(); ++it) {
		FillElementPointers(*it, elemMap);
	}
}
void MainWindow::UpdateCustomExperimentList() {
	LOG() << "Loading custom experiments";

	QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	auto expFileInfos = QDir(appDataPath + "/" CUSTOM_EXP_DIR).entryInfoList(QStringList() << "*.json", QDir::Files | QDir::Readable);
	
	QList<CustomExperiment> cExpList;

	foreach(const QFileInfo &expFileInfo, expFileInfos) {
		auto filePath = expFileInfo.absoluteFilePath();

		QFile file(filePath);

		if (!file.open(QIODevice::ReadOnly)) {
			LOG() << "Can not read JSON file.";
			continue;
		}

		QByteArray data = file.readAll();
		file.close();

		try {
			auto ce = ExperimentReader::GenerateExperimentContainer(data);
			ce.fileName = expFileInfo.fileName();
			cExpList << ce;
		}
		catch (const QString &err) {
			LOG() << "Error in the file" << expFileInfo.fileName() << "-" << err;
			continue;
		}
	}

	QMap<QString, AbstractBuilderElement*> elemPtrMap;
	for (auto it = builderElements.elements.begin(); it != builderElements.elements.end(); ++it) {
		elemPtrMap[(*it)->GetFullName()] = *it;
	}

	for (auto it = cExpList.begin(); it != cExpList.end(); ++it) {
		try {
			FillElementPointers(it->bc, elemPtrMap);
		}
		catch (const QString &err) {
			LOG() << "Error in the file" << it->fileName << "-" << err;
			continue;
		}
	}

	QList<QUuid> toDeleteIds;
	QList<CustomExperiment> toAddCe;
	
	auto existingIds = prebuiltExperiments.customExpMap.keys();
	for (auto it = cExpList.begin(); it != cExpList.end(); ++it) {
		if (existingIds.contains(it->id)) {
			existingIds.removeOne(it->id);
			continue;
		}

		toAddCe << *it;
	}

	toDeleteIds = existingIds;

	foreach(auto &id, toDeleteIds) {
		emit RemoveCustomExperiment(prebuiltExperiments.customExpMap.value(id));
		((CustomExperimentRunner*)prebuiltExperiments.customExpMap.value(id))->deleteLater();
		prebuiltExperiments.customExpMap.remove(id);
	}

	QList<AbstractExperiment*> toAddExp;

	foreach(auto &ce, toAddCe) {
		auto newExp = new CustomExperimentRunner(ce);
		prebuiltExperiments.customExpMap[ce.id] = newExp;
		toAddExp << newExp;

		connect(newExp, &CustomExperimentRunner::EditButtonClicked, this, &MainWindow::EditCustomExperiment);
	}

	emit AddNewCustomExperiments(toAddExp);
}
void MainWindow::SaveCustomExperiment(const QString &name, const BuilderContainer &bc, const QString &fileName) {
	CustomExperiment ce;
	ce.name = name;
	ce.bc = bc;
	ce.fileName = fileName;
	ce.id = QUuid::createUuid();

	auto data = ExperimentWriter::GenerateJsonObject(ce);

	QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(appDataPath + "/" CUSTOM_EXP_DIR);
	if (!dir.exists()) {
		if (!QDir("./").mkpath(appDataPath + "/" CUSTOM_EXP_DIR)) {
			LOG() << "Unable to create path (" << appDataPath + "/" CUSTOM_EXP_DIR << ").";
		}
	}

	QFile file(appDataPath + "/" CUSTOM_EXP_DIR + ce.fileName);

	if (!file.open(QIODevice::WriteOnly)) {
		LOG() << "Unable to write experiment" << ce.name << "into file.";
		return;
	}

	file.write(data);
	file.flush();
	file.close();

	UpdateCustomExperimentList();
}
void MainWindow::RequestCurrentHardwareList() {
	InstrumentList list;

	for (auto it = hardware.handlers.begin(); it != hardware.handlers.end(); ++it) {
		list << it->info;
	}

	emit CurrentHardwareList(list);
}
void MainWindow::UpdateFirmware(const QString &instName, const HexRecords &fw) {
	auto it = hardware.handlers.begin();

	for (; it != hardware.handlers.end(); ++it) {
		if (it->info.name == instName) {
			break;
		}
	}

	if (it == hardware.handlers.end()) {
		return;
	}

	auto oper = it->oper;
	if (!oper) {
		oper = new InstrumentOperator(it->info);
	}
	oper->SoftReset();
	if (!it->oper) {
		oper->deleteLater();
	}

	static QEventLoop loop;
	QString hidPath;

	int searchAttempts = 20;
	while (searchAttempts--) {
		hidPath = HidCommunicator::SearchForBootloaderHidPath();

		if (!hidPath.isEmpty()) {
			break;
		}

		QTimer::singleShot(100, &loop, &QEventLoop::quit);
		loop.exec();
	}

	//QThread::msleep(100);

	//auto hidPath = HidCommunicator::SearchForBootloaderHidPath();

	if (hidPath.isEmpty()) {
		LOG() << "No bootloader found";
		return;
	}

	auto bootOp = new BootloaderOperator(hidPath, this);

	static bool infoReceivedFlag;
	static bool crcReceivedFlag;
	static bool flashErasedFlag;
	static bool flashProgramedFlag;
	static uint16_t flashCrc;
	auto disconnector = new Disconnector(bootOp);

	#define BREAK_LOOP(a)			\
			a = true;		\
			loop.quit();

	#define WAIT_LOOP(a)										\
		a = false;									\
		QTimer::singleShot(5000, &loop, &QEventLoop::quit); \
		loop.exec();

	#define PERFORM_REQUEST(a, b)			\
		attempts = 3;					\
		while (attempts--) {			\
			bootOp->a;					\
			WAIT_LOOP(b);				\
			if (b) {			\
				break;					\
			}							\
		}								\
		if (!b) {				\
			LOG() << "No response to the command"; \
			break;						\
			}


	*disconnector << connect(bootOp, &BootloaderOperator::BootloaderInfoReceived, [=](const BootloaderInfo &info) {
		LOG() << "Bootloader version: maj -" << info.major << ", min -" << info.minor;
		BREAK_LOOP(infoReceivedFlag);
	});

	*disconnector << connect(bootOp, &BootloaderOperator::FirmwareCrcReceived, [=](uint16_t crc) {
		flashCrc = crc;
		BREAK_LOOP(crcReceivedFlag);
	});

	*disconnector << connect(bootOp, &BootloaderOperator::FlashErased, [=]() {
		LOG() << "Flash erased";
		BREAK_LOOP(flashErasedFlag);
	});

	*disconnector << connect(bootOp, &BootloaderOperator::FlashProgramed, [=]() {
		BREAK_LOOP(flashProgramedFlag);
	});

	flashCrc = 0;

	int attempts;
	do {
		LOG() << "Requesting bootloader info";
		PERFORM_REQUEST(RequestBootloaderInfo(), infoReceivedFlag);

		LOG() << "Erasing flash";
		PERFORM_REQUEST(EraseFlash(), flashErasedFlag);

		LOG() << "Programming flash...";
		auto it = fw.begin();
		while (it != fw.end()) {
			uint8_t maxPack = 10;

			QByteArray toSend;
			while ((it != fw.end()) && (maxPack--)) {
				toSend += *it++;
			}

			PERFORM_REQUEST(ProgramFlash(toSend), flashProgramedFlag);
		}
		if (it != fw.end()) {
			break;
		}
		LOG() << "...done!";

		auto hexCrc = HexLoader::CalculateCrc(fw);

		LOG() << "Requesting firmware CRC";
		PERFORM_REQUEST(RequestFirmwareCrc(hexCrc), crcReceivedFlag);

		if (flashCrc != hexCrc.crc) {
			LOG() << "Loaded flash is invalid";
			break;
		}
		LOG() << "Loaded flash is valid";

		LOG() << "Jump to application";
		bootOp->JumpToApplication();
	} while (0);

	bootOp->deleteLater();
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
	/*
	QFile f("FontList.txt");
	if (f.open(QIODevice::WriteOnly)) {
		f.write(msg.toLatin1());
		f.flush();
		f.close();
	}
	//*/
}
#include <QCommonStyle>
void MainWindow::ApplyStyle() {
#ifndef QT_NO_DEBUG
    QFile f("./GUI.css");
#else
	QFile f(":/GUI/GUI.css");
#endif

    if(!f.open(QIODevice::ReadOnly))
        return;

    qobject_cast<QApplication *>(QApplication::instance())->setStyleSheet(f.readAll());

    f.close();
}
