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
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include <QObject>
#include <QMap>
#include <QVector>
#include <QDialog>
#include <QSpinBox>
#include <QTabBar>

#include "ExternalStructures.h"
#include "ExperimentReader.h"
#include "InstrumentStructures.h"
#include "AbstractExperiment.h"

#include "BuilderWidget.h"

#define SQUID_STAT_PARAMETERS_INI	"SquidStatParameters.ini"
#define DATA_SAVE_PATH				"data-save-dir-name"

class MainWindowUI {
public:
	MainWindowUI(MainWindow *mainWindow);
	~MainWindowUI();

	void CreateUI();

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

			TOTAL_CURVES,
			START = PRIMARY
		};
		struct {
			struct {
				QColor color;
				qreal width;
				Qt::PenStyle style;
			} pen;
			
			struct {
				QwtSymbol::Style style;
				qreal width;
			} symbol;

			QwtPlotCurve::CurveStyle style;
		} curve[TOTAL_CURVES];
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
	static QString GetNewTitle(QWidget*, const QString&);
	static QString GetCustomExperimentName(QWidget*, const QString &);
	static bool GetUserAgreement(QWidget*, const QString &title, const QString &text, const QString &okText, const QString &cancelText);
	bool GetOpenCustomExperiment(QWidget*, CustomExperiment&);

	static QwtPlotCurve* CreateCurve(int yAxisId, const QColor&);

	void CreateCentralWidget();

	QWidget* GetApplyStyleButton();

	QWidget* GetMainTabWidget();

	QWidget* GetOldSearchHardwareTab();
	QWidget* GetRunExperimentTab();

	QWidget* GetBuildExperimentTab();
	QWidget* CreateBuildExperimentTabWidget(const QUuid&);
	QWidget* CreateBuildExpHolderWidget(const QUuid &id);
	QWidget* CreateElementsListWidget();

	QWidget* GetNewDataWindowTab();
	QWidget* CreateNewDataTabWidget(const QUuid&, ExperimentType, const QString&, const QStringList &xAxis, const QStringList &yAxis, const DataMap* = 0);

	QWidget* GetSearchHardwareWidget();
	QWidget* GetLogWidget();
	QWidget* GetPlotWidget();
	QWidget* GetControlButtonsWidget();


	void MoveAxis(PlotHandler &handler, QwtPlot::Axis axis, int dVal);
	void ZoomAxis(PlotHandler &handler, QwtPlot::Axis axis, double percents);
	void ZoomAxis(PlotHandler &handler, QwtPlot::Axis axis, double percentsMin, double percentsMax);

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
		QWidget *userInputs;
	} prebuiltExperimentData;
	
	struct DataMapVisualization {
		DataMap container;
		QFile *saveFile;
		CalibrationData cal;
		HardwareVersion hwVer;
		QMap<QwtPlot::Axis, DataStore*> data;
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
		struct {
			quint64 stamp;
		} plotCounter;
	};

	struct BuilderHandler {
		QList<QMetaObject::Connection> connections;
		BuilderWidget *builder;
		QSpinBox *globalMult;
		QString fileName;
		QString name;
	};

	struct {
		QMap<QUuid, BuilderHandler> builders;
		QWidget *userInputs;
		QWidget *paramsHeadWidget;
		QVBoxLayout *paramsLay;
		QTabBar *tabBar;
	} builderTabs;

	QMap<QString, AbstractBuilderElement*> elementsPtrMap;

	struct {
		QMap<QUuid, QMap<ExperimentType, PlotHandler>> plots;
	} dataTabs;

	MainWindow *mw;
};