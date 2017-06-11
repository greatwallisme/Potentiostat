#pragma once

class MainWindow;

class QWidget;
class QLabel;
class QComboBox;
class QPushButton;
class QEvent;
class QVBoxLayout;
class QGridLayout;
class QStackedLayout;
class QTabWidget;

class QwtPlot;
class QwtPlotCurve;

#include <QObject>
#include <QMap>
#include <QVector>

#include "ExternalStructures.h"
#include "InstrumentStructures.h"
#include "AbstractExperiment.h"

class MainWindowUI {
public:
	MainWindowUI(MainWindow *mainWindow);
	~MainWindowUI();

	void CreateUI();

private:
	void CreateCentralWidget();

	QWidget* GetApplyStyleButton();

	QWidget* GetMainTabWidget();

	QWidget* GetOldSearchHardwareTab();
	QWidget* GetRunExperimentTab();

	QWidget* GetNewDataWindowTab();
	QWidget* CreateNewDataTabWidget(const QUuid&, const QString&, const AbstractExperiment*, QFile*, const CalibrationData &);

	QWidget* GetSearchHardwareWidget();
	QWidget* GetLogWidget();
	QWidget* GetPlotWidget();
	QWidget* GetControlButtonsWidget();

	/*
	QWidget* PrebuiltExpCreateGroupHeader(const ExperimentNode_t*);
	QWidget* PrebuiltExpCreateParamsInput(ExperimentNode_t*);
	void FillNodeParameters();
	//*/

	struct {
		struct {
			struct {
			} descr;
		} runExperiment;
		struct {
			QPushButton *newDataTabButton;
			QTabWidget *docTabs;
		} newDataTab;
	} ui;
	
	struct {
		InstrumentInfo instrumentInfo;
		quint8 channel;
	} currentInstrument;
	
	struct {
		QWidget *userInputs;
	} prebuiltExperimentData;

	struct PlotHandler {
		PlotHandler() { data.xData = 0; data.y1Data = 0; data.y2Data = 0; data.saveFile = 0; }
		QwtPlot* plot;
		QwtPlotCurve *curve1;
		QwtPlotCurve *curve2;
		QComboBox *xVarCombo;
		QComboBox *yVarCombo;
		QList<QMetaObject::Connection> varComboConnection;
		const AbstractExperiment *exp;
		struct {
			DataMap container;
			QFile *saveFile;
			CalibrationData cal;
			DataVector *xData;
			DataVector *y1Data;
			DataVector *y2Data;
		} data;
	};

	struct {
		QMap<QUuid, PlotHandler> plots;
	} dataTabs;

	MainWindow *mw;
};