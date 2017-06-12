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
#include <QDialog>

#include "ExternalStructures.h"
#include "InstrumentStructures.h"
#include "AbstractExperiment.h"

#define SQUID_STAT_PARAMETERS_INI	"SquidStatParameters.ini"
#define DATA_SAVE_PATH				"data-save-dir-name"

class MainWindowUI {
public:
	MainWindowUI(MainWindow *mainWindow);
	~MainWindowUI();

	void CreateUI();

	struct ExperimentNotes {
		QString notes;
		QString refElectrode;
		QString potential;
		bool dialogCanceled;
	};

	struct CsvFileData {
		ExperimentNotes notes;
		QStringList xAxisList;
		QStringList yAxisList;
		DataMap container;
	};

	static ExperimentNotes GetExperimentNotes(QWidget *parent);
	static bool ReadCsvFile(QWidget *parent, CsvFileData&);

private:
	void CreateCentralWidget();

	QWidget* GetApplyStyleButton();

	QWidget* GetMainTabWidget();

	QWidget* GetOldSearchHardwareTab();
	QWidget* GetRunExperimentTab();

	QWidget* GetNewDataWindowTab();
	QWidget* CreateNewDataTabWidget(const QUuid&, const QString&, const QStringList &xAxis, const QStringList &yAxis);

	QWidget* GetSearchHardwareWidget();
	QWidget* GetLogWidget();
	QWidget* GetPlotWidget();
	QWidget* GetControlButtonsWidget();

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