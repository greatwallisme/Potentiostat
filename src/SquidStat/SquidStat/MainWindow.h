#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "InstrumentStructures.h"
#include "ExternalStructures.h"

#include <AbstractExperiment.h>

#include <QList>
#include <QUuid>
#include <QFile>

class InstrumentEnumerator;
class MainWindowUI;
class InstrumentOperator;
class ExperimentContainer;
class QPluginLoader;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	void LoadFonts();
    void ApplyStyle();
	
	void LoadPrebuildExperiments();
	void PrebuiltExperimentSelected(const AbstractExperiment*);

	/*
	void SearchHwVendor();
	void SearchHwHandshake();
	//*/

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

signals:
	void HardwareFound(const InstrumentList&);
	void DataArrived(const QUuid&, quint8 channel, const ExperimentalData &expData, bool paused);

	void PrebuiltExperimentsFound(const QList<AbstractExperiment*>&);

	void CreateNewDataWindow(const QUuid&, const AbstractExperiment*, QFile*, const CalibrationData &, const HardwareVersion&);

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
	//void FillHardware(const InstrumentList &);

	MainWindowUI *ui;
	InstrumentEnumerator *instrumentEnumerator;

	//InstrumentOperator *instrumentOperator;
	

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
		QList<QPluginLoader*> expLoaders;
	} prebuiltExperiments;

	QList<InstrumentHandler>::iterator SearchForHandler(InstrumentOperator*);
	QList<InstrumentHandler>::iterator SearchForHandler(const QString &name, quint8 channel);
	QList<InstrumentHandler>::iterator SearchForHandler(const QUuid&);
};

#endif // MAINWINDOW_H
