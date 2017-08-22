#include "MainWindow.h"

#include <MainWindowUI.h>

#include "HidCommunicator.h"
#include "BootloaderOperator.h"
#include "InstrumentEnumerator.h"
#include "InstrumentOperator.h"
#include "ExperimentReader.h"
#include "CustomExperimentRunner.h"

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

#define PREBUILT_EXP_DIR		"./prebuilt/"
#define ELEMENTS_DIR			"./elements/"
#define CUSTOM_EXP_DIR			"./custom/"

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
	LoadBuilderElements();
	UpdateCustomExperimentList();

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
	CleanupBuilderElements();
}
void MainWindow::CleanupExperiments() {
	foreach(auto exp, prebuiltExperiments.customExpMap) {
		//delete exp;
		((CustomExperimentRunner*)exp)->deleteLater();
	}

	foreach(auto exp, prebuiltExperiments.expList) {
		delete exp;
	}

	foreach(auto loader, prebuiltExperiments.expLoaders) {
		loader->unload();
		loader->deleteLater();
	}
}
void MainWindow::CleanupBuilderElements() {
	foreach(auto elem, builderElements.elements) {
		delete elem;
	}

	foreach(auto loader, builderElements.loaders) {
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
				if ((hardware.currentInstrument.handler == hardware.handlers.end()) || (hardware.currentInstrument.handler->info == currentInfo)) {
					needToSetEnd = true;
				}

				InstrumentHandler &currentHandler(hardware.handlers[i]);

				if (currentHandler.experiment.busy) {
					emit ExperimentCompleted(currentHandler.experiment.id);
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
		handler.experiment.busy = false;
		handler.experiment.paused = false;

		hardware.handlers << handler;

		addingInstrumentsNames << HardwareUiDescription(instrumentToAdd.name, instrumentToAdd.calData.size());
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
void MainWindow::LoadBuilderElements() {
	LOG() << "Loading builder elements";

	auto expFileInfos = QDir(ELEMENTS_DIR).entryInfoList(QStringList() << "*.dll", QDir::Files | QDir::Readable);

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
}
void MainWindow::PrebuiltExperimentSelected(const AbstractExperiment *exp) {
	prebuiltExperiments.selectedExp = exp;
}
void MainWindow::UpdateCurrentExperimentState() {
	auto &currentHandler(hardware.currentInstrument.handler);
	
	if (currentHandler == hardware.handlers.end()) {
		return;
	}

	if (currentHandler->experiment.busy) {
		emit CurrentHardwareBusy();

		if (currentHandler->experiment.paused) {
			emit CurrentExperimentPaused();
		}
		else {
			emit CurrentExperimentResumed();
		}
	}
	else {
		emit CurrentExperimentCompleted();
	}
}
void MainWindow::SelectHardware(const QString &name, quint8 channel) {
	auto hwIt = SearchForHandler(name, channel);
	if (hwIt == hardware.handlers.end()) {
		return;
	}

	hardware.currentInstrument.handler = hwIt;
	hardware.currentInstrument.channel = channel;

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
		connect(instrumentOperator, &InstrumentOperator::ExperimentPaused, this, [=]() {
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

			handler->experiment.paused = true;

			LOG() << "Experiment paused";

			emit ExperimentPaused(handler->experiment.id);
			if (handler == hardware.currentInstrument.handler) {
				emit CurrentExperimentPaused();
			}
		});

		hardware.currentInstrument.handler->connections <<
		connect(instrumentOperator, &InstrumentOperator::ExperimentResumed, this, [=]() {
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

			handler->experiment.paused = false;

			LOG() << "Experiment resumed";

			emit ExperimentResumed(handler->experiment.id);
			if (handler == hardware.currentInstrument.handler) {
				emit CurrentExperimentResumed();
			}
		});

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

			foreach(auto conn, handler->connections) {
				QObject::disconnect(conn);
			}

			handler->experiment.busy = false;
			handler->experiment.paused = false;

			LOG() << "Experiment completed";
			
			emit ExperimentCompleted(handler->experiment.id);
			if (handler == hardware.currentInstrument.handler) {
				emit CurrentExperimentCompleted();
			}
			
			handler->experiment.id = QUuid();
			handler->oper->deleteLater();
			handler->oper = 0;
		});

		hardware.currentInstrument.handler->connections <<
		QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentNodeBeginning, this, [=](quint8 channel, const ExperimentNode_t &node) {
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
			emit ExperimentNodeBeginning(handler->experiment.id, channel, node);
		});

		hardware.currentInstrument.handler->connections <<
		QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentalDcDataReceived, this, [=](quint8 channel, const ExperimentalDcData &expData) {
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

			emit DcDataArrived(handler->experiment.id, channel, expData, handler->experiment.paused);
		});
		hardware.currentInstrument.handler->connections <<
		QObject::connect(instrumentOperator, &InstrumentOperator::ExperimentalAcDataReceived, this, [=](quint8 channel, const QByteArray &expData) {
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
			
			emit AcDataArrived(handler->experiment.id, channel, expData, handler->experiment.paused);
		});
	}

	quint8 curChan = hardware.currentInstrument.channel;
	InstrumentInfo &instrumentInfo(hardware.currentInstrument.handler->info);
	auto nodesData = prebuiltExperiments.selectedExp->GetNodesData(paramsWdg, instrumentInfo.calData[curChan], instrumentInfo.hwVer);
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

	auto types = prebuiltExperiments.selectedExp->GetTypes();

	QList<StartExperimentParameters> startParams;

	QUuid newId = QUuid::createUuid();
	bool ok = true;
	foreach(auto type, types) {
		StartExperimentParameters curParam;
		curParam.id = newId;
		curParam.type = type;
		curParam.exp = prebuiltExperiments.selectedExp;
		curParam.cal = instrumentInfo.calData[curChan];
		curParam.hwVer = instrumentInfo.hwVer;
		curParam.notes = notes;

		QString tabName = prebuiltExperiments.selectedExp->GetShortName() + " (" + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ")";
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
	
	hardware.currentInstrument.handler->experiment.busy = true;
	hardware.currentInstrument.handler->experiment.id = newId;
	hardware.currentInstrument.handler->experiment.channel = hardware.currentInstrument.channel;

	foreach(auto &param, startParams) {
		switch (param.type) {
			case ET_DC:
				prebuiltExperiments.selectedExp->SaveDcDataHeader(*param.file, param.notes);
				break;
			case ET_AC:
				prebuiltExperiments.selectedExp->SaveAcDataHeader(*param.file, param.notes);
				break;
		}

		emit CreateNewDataWindow(param);
	}

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

	auto expFileInfos = QDir(CUSTOM_EXP_DIR).entryInfoList(QStringList() << "*.json", QDir::Files | QDir::Readable);
	
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

	QDir dir(CUSTOM_EXP_DIR);
	if (!dir.exists()) {
		if (!QDir("./").mkdir(CUSTOM_EXP_DIR)) {
			LOG() << "Unable to create path (" << CUSTOM_EXP_DIR << ").";
		}
	}

	QFile file(CUSTOM_EXP_DIR + ce.fileName);

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

	QThread::msleep(100);

	auto hidPath = HidCommunicator::SearchForBootloaderHidPath();

	if (hidPath.isEmpty()) {
		LOG() << "No bootloader found";
		return;
	}

	auto bootOp = new BootloaderOperator(hidPath, this);

	static QEventLoop loop;
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
void MainWindow::ApplyStyle() {
    QFile f("./GUI.css");
    if(!f.open(QIODevice::ReadOnly))
        return;

    qobject_cast<QApplication *>(QApplication::instance())->setStyleSheet(f.readAll());

    f.close();
}
