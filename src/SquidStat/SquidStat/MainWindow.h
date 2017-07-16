#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "InstrumentStructures.h"
#include "ExternalStructures.h"

#include <AbstractExperiment.h>
#include <AbstractBuilderElement.h>
#include <BuilderWidget.h>

#include <QList>
#include <QUuid>
#include <QFile>

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
};

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

	void StartExperiment(QWidget*);
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

signals:
	void HardwareFound(const InstrumentList&);
	void DcDataArrived(const QUuid&, quint8 channel, const ExperimentalDcData &expData, bool paused);
	void AcDataArrived(const QUuid&, quint8 channel, const QByteArray &expData, bool paused);

	void PrebuiltExperimentsFound(const QList<AbstractExperiment*>&);
	void BuilderElementsFound(const QList<AbstractBuilderElement*>&);
	void AddNewCustomExperiments(const QList<AbstractExperiment*>&);
	void RemoveCustomExperiment(const AbstractExperiment*);

	void CreateNewDataWindow(const StartExperimentParameters&);

	void RemoveDisconnectedInstruments(const QStringList&);
	void AddNewInstruments(const QStringList&);

	void CurrentExperimentCompleted();
	void CurrentExperimentResumed();
	void CurrentExperimentPaused();
	void CurrentHardwareBusy();
	void ExperimentCompleted(const QUuid&);
	void ExperimentResumed(const QUuid&);
	void ExperimentPaused(const QUuid&);

private:
	void CleanupCurrentHardware();
	void CleanupExperiments();
	void CleanupBuilderElements();

	MainWindowUI *ui;
	InstrumentEnumerator *instrumentEnumerator;

	struct InstrumentHandler {
		InstrumentInfo info;
		InstrumentOperator *oper;
		struct ExpDescriptor {
			bool busy;
			bool paused;
			QUuid id;
			quint8 channel;
		} experiment;
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
	QList<InstrumentHandler>::iterator SearchForHandler(const QString &name, quint8 channel);
	QList<InstrumentHandler>::iterator SearchForHandler(const QUuid&);
};

#endif // MAINWINDOW_H
