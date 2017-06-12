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
	};

	struct CsvFileData {
		QString fileName;
		ExperimentNotes notes;
		QStringList xAxisList;
		QStringList yAxisList;
		DataMap container;
	};

	static bool GetExperimentNotes(QWidget *parent, ExperimentNotes&);
	static bool ReadCsvFile(QWidget *parent, CsvFileData&);
	static bool ReadCsvFile(QWidget *parent, QList<CsvFileData>&);

private:
	static bool ReadCsvFile(const QString &fileName,  CsvFileData&);
	QwtPlotCurve* CreateCurve(int yAxisId, const QColor&);

	void CreateCentralWidget();

	QWidget* GetApplyStyleButton();

	QWidget* GetMainTabWidget();

	QWidget* GetOldSearchHardwareTab();
	QWidget* GetRunExperimentTab();

	QWidget* GetNewDataWindowTab();
	QWidget* CreateNewDataTabWidget(const QUuid&, const QString&, const QStringList &xAxis, const QStringList &yAxis, const DataMap* = 0);

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
	
	struct DataMapVisualization {
		DataMap container;
		QFile *saveFile;
		CalibrationData cal;
		DataVector *xData;
		DataVector *y1Data;
		DataVector *y2Data;
		QwtPlotCurve *curve1;
		QwtPlotCurve *curve2;
	};
	struct PlotHandler {
		QwtPlot* plot;
		QComboBox *xVarCombo;
		QComboBox *y1VarCombo;
		QComboBox *y2VarCombo;
		QList<QMetaObject::Connection> plotTabConnections;
		const AbstractExperiment *exp;
		QList<DataMapVisualization> data;
	};

	struct {
		QMap<QUuid, PlotHandler> plots;
	} dataTabs;

	MainWindow *mw;
};