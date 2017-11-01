#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "HexLoader.h"
#include "InstrumentStructures.h"
#include "ExternalStructures.h"
#include "ExperimentReader.h"
#include "ExperimentTrigger.hpp"

#include <AbstractExperiment.h>
#include <AbstractBuilderElement.h>
#include <BuilderWidget.h>

#include <QList>
#include <QUuid>
#include <QFile>
#include <QPair>

class InstrumentEnumerator;
class MainWindowUI;
class InstrumentOperator;
class ExperimentContainer;
class QPluginLoader;

struct StartExperimentParameters {
	QString name;
	QUuid id;
	ExperimentType type;
	QFile *file;
	const AbstractExperiment *exp;
	CalibrationData cal;
	HardwareVersion hwVer;
	ExperimentNotes notes;
	QString filePath;
};

struct HardwareUiDescription {
	QString name;
	quint8 channelAmount;
	HardwareModel_t hwModel;
};

//typedef QPair<QString, quint8> HardwareUiDescription;
Q_DECLARE_METATYPE(HardwareUiDescription)

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	static void FillElementPointers(BuilderContainer &bc, const QMap<QString, AbstractBuilderElement*>&);

public slots:
	void LoadFonts();
    void ApplyStyle();

	void UpdateCustomExperimentList();
	void LoadBuilderElements();
	void LoadPrebuildExperiments();
	void PrebuiltExperimentSelected(const AbstractExperiment*);
	
	void SelectHardware(const QString&, quint8 channel);

	void StartExperiment(QWidget*, const QUuid& = QUuid());
	void StopExperiment(const QUuid&);
	void PauseExperiment(const QUuid&);
	void ResumeExperiment(const QUuid&);
	void StopExperiment(const QString&, quint8 channel);
	void PauseExperiment(const QString&, quint8 channel);
	void ResumeExperiment(const QString&, quint8 channel);

	void RemoveInstruments(InstrumentList);
	void AddInstruments(InstrumentList);

	void UpdateCurrentExperimentState();

	void SaveCustomExperiment(const QString&, const BuilderContainer&, const QString &fileName);

	void RequestCurrentHardwareList();
	void UpdateFirmware(const QString&, const HexRecords&);

	void StartManualExperiment(const QUuid&);
	void SetManualSamplingParams(const QUuid&, double value);
	void SetManualGalvanoSetpoint(const QUuid&, double setpoint, QString units_str);
	void SetManualPotentioSetpoint(const QUuid&, double setpoint);
	void SetManualOcp(const QUuid&);
	void SetCurrentRangingMode(const QUuid&, quint8 range);
	void StopManualExperiment(const QUuid&);
	void PauseManualExperiment(const QUuid&);
	void ResumeManualExperiment(const QUuid&);
  void SetCompRange(const QString name, quint8 channelNum, quint8 range);
    void SendPhaseAngleCalibrationData(const QUuid &id /* ... */);


signals:
	void HardwareFound(const InstrumentList&);
	void DcDataArrived(const QUuid&, const ExperimentalDcData &expData, ExperimentTrigger *, bool paused);
	void AcDataArrived(const QUuid&, const QByteArray &expData, ExperimentTrigger *, bool paused);
	void ExperimentNodeBeginning(const QUuid&, const ExperimentNode_t&);

	void PrebuiltExperimentsFound(const QList<AbstractExperiment*>&);
	void BuilderElementsFound(const QList<AbstractBuilderElement*>&);
	void AddNewCustomExperiments(const QList<AbstractExperiment*>&);
	void RemoveCustomExperiment(const AbstractExperiment*);

	void CreateNewDataWindow(const StartExperimentParameters&);

	void RemoveDisconnectedInstruments(const QStringList&);
	void AddNewInstruments(const QList<HardwareUiDescription>&);

	void CurrentExperimentCompleted();
	void CurrentExperimentResumed();
	void CurrentExperimentPaused();
	void CurrentHardwareBusy();
	void ExperimentCompleted(const QUuid&);
	void ExperimentResumed(const QUuid&);
	void ExperimentPaused(const QUuid&);
	void ExperimentStarted(const QUuid&, const QString&, quint8);
	void EditCustomExperiment(const CustomExperiment&);
	void ExperimentNotification(const QUuid&, const QString&);
	void ExperimentError(const QUuid&);

	void CurrentHardwareList(const InstrumentList&);

	void SetManualStartParams(const StartExperimentParameters&);

	void CurrentExperimentIsNotManual();

private:
	void CleanupCurrentHardware();
	void CleanupExperiments();
	void CleanupBuilderElements();

	MainWindowUI *ui;
	InstrumentEnumerator *instrumentEnumerator;

	struct InstrumentHandler {
		struct ExpDescriptor {
			bool busy;
			bool paused;
			bool isManual;
			QUuid id;
			//quint8 channel;
		};

		InstrumentInfo info;
		InstrumentOperator *oper;
		ExperimentTrigger *trigger;
		QVector<ExpDescriptor> experiment;
		QList<QMetaObject::Connection> connections;
	};

	struct {
		QList<InstrumentHandler> handlers;
		struct {
			QList<InstrumentHandler>::iterator handler;
			quint8 channel;
		} currentInstrument;
	} hardware;

	struct {
		const AbstractExperiment* selectedExp;
		QList<AbstractExperiment*> expList;
		QMap<QUuid, AbstractExperiment*> customExpMap;
		QList<QPluginLoader*> expLoaders;
	} prebuiltExperiments;

	struct {
		QList<AbstractBuilderElement*> elements;
		QList<QPluginLoader*> loaders;
	} builderElements;

	QList<InstrumentHandler>::iterator SearchForHandler(InstrumentOperator*);
	QList<InstrumentHandler>::iterator SearchForHandler(const QString &name/*, quint8 channel*/);
	QList<InstrumentHandler>::iterator SearchForHandler(const QUuid&);
	quint8 SearchForChannel(QList<InstrumentHandler>::iterator, const QUuid&);
	
	void CreateLogicForInstrument(InstrumentHandler&);
};

#endif // MAINWINDOW_H
