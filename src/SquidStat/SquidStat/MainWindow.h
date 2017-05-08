#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "InstrumentStructures.h"
#include "ExternalStructures.h"

class MainWindowUI;
class InstrumentOperator;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	void LoadFonts();
    void ApplyStyle();

	void SearchHwVendor();
	void SearchHwHandshake();

	void SelectHardware(const InstrumentInfo&, quint8 channel);
	void RequestCalibration();

	void StartExperiment();
	void StopExperiment();

signals:
	void HardwareFound(const InstrumentList&);
	void DataArrived(quint8 channel, const ExperimentalData &expData);

private:
	MainWindowUI *ui;

	InstrumentOperator *instrumentOperator;
	struct {
		InstrumentInfo instrumentInfo;
		quint8 channel;
	} currentInstrument;
};

#endif // MAINWINDOW_H
