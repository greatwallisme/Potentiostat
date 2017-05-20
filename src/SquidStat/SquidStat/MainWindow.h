#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "InstrumentStructures.h"
#include "ExternalStructures.h"

#include <QList>

class MainWindowUI;
class InstrumentOperator;
class ExperimentContainer;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	void LoadFonts();
    void ApplyStyle();
	
	void LoadPrebuildExperiments();
	void PrebuiltExperimentSelected(int);

	void SearchHwVendor();
	void SearchHwHandshake();

	void SelectHardware(const InstrumentInfo&, quint8 channel);
	void RequestCalibration();

	void StartExperiment();
	void StopExperiment();

signals:
	void HardwareFound(const InstrumentList&);
	void DataArrived(quint8 channel, const ExperimentalData &expData);

	void PrebuiltExperimentsFound(const QList<ExperimentContainer>&);
	void PrebuiltExperimentSetDescription(const ExperimentContainer&);
	void PrebuiltExperimentSetParameters(const QList<ExperimentNode_t*>&);

private:
	MainWindowUI *ui;

	InstrumentOperator *instrumentOperator;
	struct {
		InstrumentInfo instrumentInfo;
		quint8 channel;
	} currentInstrument;

	struct {
		QList<ExperimentContainer> ecList;
		int selectedEcIndex;
	} prebuiltExperiments;
};

#endif // MAINWINDOW_H
