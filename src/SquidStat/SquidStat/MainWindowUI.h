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

#include <qwt_plot.h>
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
	static bool GetColor(QWidget *parent, QColor&);

	struct CurveParameters {
		enum {
			PRIMARY = 0,
			SECONDARY,

			TOTAL_CURVES
		};
		struct {
			QColor color;
			qreal width;
			Qt::PenStyle style;
		} pen[TOTAL_CURVES];
	};

	struct AxisParameters {
		struct Val {
			Val() : autoScale(true) {}
			bool autoScale;
			double val;
		};
		Val min;
		Val max;
		Val step;
	};

	struct PlotHandler;

	static bool GetNewPen(QWidget *parent, QMap<QString, CurveParameters>&);
	static bool GetNewAxisParams(QWidget *parent, AxisParameters &);
	static bool ApplyNewAxisParams(QwtPlot::Axis, PlotHandler &handler);

	static QwtPlotCurve* CreateCurve(int yAxisId, const QColor&);

	void CreateCentralWidget();

	QWidget* GetApplyStyleButton();

	QWidget* GetMainTabWidget();

	QWidget* GetOldSearchHardwareTab();
	QWidget* GetRunExperimentTab();

	QWidget* GetNewDataWindowTab();
	QWidget* CreateNewDataTabWidget(const QUuid&, const QString&, const QStringList &xAxis, const QStringList &yAxis, const DataMap* = 0, bool showControlButtons = true);

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
	
	/*
	struct {
		InstrumentInfo instrumentInfo;
		quint8 channel;
	} currentInstrument;
	//*/
	
	struct {
		QWidget *userInputs;
	} prebuiltExperimentData;
	
	struct DataMapVisualization {
		DataMap container;
		QFile *saveFile;
		CalibrationData cal;
		HardwareVersion hwVer;
		QMap<QwtPlot::Axis, DataVector*> data;
		QwtPlotCurve *curve1;
		QwtPlotCurve *curve2;
		QString name;
	};
	struct PlotHandler {
		QwtPlot* plot;
		QMap<QwtPlot::Axis, QComboBox*> varCombo;
		QMap<QwtPlot::Axis, AxisParameters> axisParams;
		QList<QMetaObject::Connection> plotTabConnections;
		const AbstractExperiment *exp;
		QList<DataMapVisualization> data;
	};

	struct {
		QMap<QUuid, PlotHandler> plots;
	} dataTabs;

	MainWindow *mw;
};