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
	QWidget* CreateNewDataTabWidget(const QUuid&, const QString&, const AbstractExperiment*);

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

	/*
	struct SavedInputs {
		ExperimentNode_t *node;
		QMap<QString, QWidget*> input;
	};
	//*/

	struct {
		//QList<QWidget*> paramWidgets;
		//QList<SavedInputs> inputsList;

		QWidget *userInputs;
	} prebuiltExperimentData;

	struct PlotHandler {
		PlotHandler() { data.xData = 0; data.yData = 0; }
		QwtPlot* plot;
		QwtPlotCurve *curve;
		QComboBox *xVarCombo;
		QComboBox *yVarCombo;
		QMetaObject::Connection xVarComboConnection;
		QMetaObject::Connection yVarComboConnection;
		const AbstractExperiment *exp;
		struct {
			DataMap container;
			/*
			QVector<qreal> timestamp;
			QVector<qreal> ewe;
			QVector<qreal> ece;
			QVector<qreal> current;
			QVector<qreal> currentIntegral;
			//*/
			DataVector *xData;
			DataVector *yData;
		} data;
	};

	struct {
		QMap<QUuid, PlotHandler> plots;
	} dataTabs;

	MainWindow *mw;
};