#include "MainWindowUI.h"
#include "MainWindow.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>

#include "UIHelper.hpp"

#include "Log.h"

#include "ExperimentReader.h"

#include <QButtonGroup>

#include <QIntValidator>
#include <QListView>
#include <QTabWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QScrollArea>

#include <QStandardItemModel>

#include <QPixmap>

#define PREBUILT_EXP_DIR	"./prebuilt/"

MainWindowUI::MainWindowUI(MainWindow *mainWindow) :
	mw(mainWindow)
{
	mw->setObjectName("mainUI");
	mw->ApplyStyle();
}
MainWindowUI::~MainWindowUI() {
}
void MainWindowUI::CreateUI() {
	CreateCentralWidget();
}
void MainWindowUI::CreateCentralWidget() {
	QWidget *centralWidget = OBJ_NAME(WDG(), "central-widget");
	QGridLayout *centralLayout = NO_SPACING(NO_MARGIN(new QGridLayout(centralWidget)));
	mw->setCentralWidget(centralWidget);

	centralLayout->addWidget(GetApplyStyleButton(),		0, 0);
	centralLayout->addWidget(GetMainTabWidget(),		1, 0);
}
QWidget* MainWindowUI::GetApplyStyleButton() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = OBJ_NAME(WDG(), "apply-button-owner");
	QHBoxLayout *lay = NO_SPACING(NO_MARGIN(new QHBoxLayout(w)));

	auto *pbt = OBJ_NAME(PBT("Apply stylesheet"), "apply-button");

	CONNECT(pbt, &QPushButton::clicked, mw, &MainWindow::ApplyStyle);

	lay->addWidget(pbt);
	lay->addWidget(OBJ_NAME(PBT("SweepVoltammetry.csv"), "apply-button"));
	lay->addStretch(1);

	return w;
}
QWidget* MainWindowUI::GetMainTabWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = WDG();
	auto *lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));
	auto *barWidget = OBJ_NAME(WDG(), "top-tab-bar-owner");
	auto *barLayout = NO_SPACING(NO_MARGIN(new QHBoxLayout(barWidget)));
	auto *widgetsLayout = NO_MARGIN(new QStackedLayout);

	lay->addWidget(barWidget);
	lay->addLayout(widgetsLayout);

	widgetsLayout->addWidget(GetOldSearchHardwareTab());
	widgetsLayout->addWidget(GetRunExperimentTab());

	QButtonGroup *buttonGroup = new QButtonGroup(mw);
	
	auto *pbt = OBJ_PROP(OBJ_NAME(PBT("Search the Hardware"), "bar-button"), "order", "first");
	pbt->setCheckable(true);
	pbt->setChecked(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	CONNECT(pbt, &QPushButton::toggled, [=](bool checked) {
		if (!checked) {
			return;
		}
		
		widgetsLayout->setCurrentWidget(GetOldSearchHardwareTab());
	});

	pbt = OBJ_NAME(PBT("Run an Experiment"), "bar-button");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	CONNECT(pbt, &QPushButton::toggled, [=](bool checked) {
		if (!checked) {
			return;
		}

		widgetsLayout->setCurrentWidget(GetRunExperimentTab());
	});

	pbt = OBJ_PROP(OBJ_NAME(PBT("New Data Window"), "bar-button"), "order", "last");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	barLayout->addStretch(1);

	return w;
}
QWidget* MainWindowUI::GetOldSearchHardwareTab() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = WDG();
	QGridLayout *lay = NO_SPACING(NO_MARGIN(new QGridLayout(w)));

	lay->addWidget(GetSearchHardwareWidget(), 1, 0);
	lay->addWidget(GetLogWidget(), 2, 0);
	lay->addWidget(GetPlotWidget(), 1, 1, 2, 1);
	lay->addWidget(GetControlButtonsWidget(), 3, 0, 1, 2);

	return w;
}
QWidget* MainWindowUI::GetRunExperimentTab() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = WDG();
	QHBoxLayout *lay = NO_SPACING(NO_MARGIN(new QHBoxLayout(w)));

	auto *experimentListOwner = OBJ_PROP(OBJ_NAME(WDG(), "experiment-list-owner"), "widget-type", "left-grey");
	auto *experimentListLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(experimentListOwner)));
	auto *experimentList = OBJ_PROP(OBJ_NAME(new QListView, "experiment-list"), "widget-type", "left-grey");
	experimentList->setEditTriggers(QAbstractItemView::NoEditTriggers);

	experimentListLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Experiments"), "heading-label"), "widget-type", "left-grey"));
	experimentListLay->addWidget(experimentList);

	auto *descriptionHelpLay = NO_SPACING(NO_MARGIN(new QVBoxLayout()));

	auto *descriptionWidget = OBJ_NAME(WDG(), "experiment-description-owner");
	auto *descriptionWidgetLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(descriptionWidget)));

	descriptionWidgetLay->addWidget(ui.runExperiment.descr.icon = OBJ_NAME(LBL(""), "experiment-description-icon"));
	descriptionWidgetLay->addWidget(ui.runExperiment.descr.fullName = OBJ_NAME(LBL(""), "experiment-description-name"));
	descriptionWidgetLay->addWidget(ui.runExperiment.descr.text = OBJ_NAME(LBL(""), "experiment-description-text"));
	descriptionWidgetLay->addStretch(1);

	descriptionHelpLay->addWidget(OBJ_NAME(WDG(), "experiment-description-spacing-top"));
	descriptionHelpLay->addWidget(descriptionWidget);
	descriptionHelpLay->addWidget(OBJ_NAME(WDG(), "experiment-description-spacing-bottom"));

	auto *paramsWidget = WDG();
	auto *paramsWidgetLay = NO_SPACING(NO_MARGIN(new QGridLayout(paramsWidget)));

	auto *startExpPbt = OBJ_PROP(OBJ_NAME(PBT("Start Experiment"), "primary-button"), "button-type", "experiment-start-pbt");
	auto *buttonLay = NO_SPACING(NO_MARGIN(new QHBoxLayout()));

	buttonLay->addStretch(1);
	buttonLay->addWidget(startExpPbt);
	buttonLay->addStretch(1);

	paramsWidgetLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-top"),	0, 0, 1, 3);
	paramsWidgetLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-bottom"), 4, 0, 1, 3);
	paramsWidgetLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-left"),	1, 0, 2, 1);
	paramsWidgetLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-right"),	1, 3, 2, 1);
	paramsWidgetLay->addLayout(buttonLay, 3, 1);
	//paramsWidgetLay->addWidget(OBJ_NAME(LBL("Parameters"), "heading-label"),		1, 1);
	paramsWidgetLay->setRowStretch(2, 1);

	auto *scrollAreaWidget = WDG();
	ui.runExperiment.paramsLay = NO_SPACING(NO_MARGIN(new QGridLayout(scrollAreaWidget)));

	QScrollArea *scrollArea = OBJ_NAME(new QScrollArea(), "experiment-params-scroll-area");
	paramsWidgetLay->addWidget(scrollArea, 2, 1);

	lay->addWidget(experimentListOwner);
	lay->addLayout(descriptionHelpLay);
	lay->addWidget(paramsWidget);


	scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(scrollAreaWidget);
	
	CONNECT(mw, &MainWindow::PrebuiltExperimentsFound, [=](const QList<ExperimentContainer> &expList) {
		QStandardItemModel *model = new QStandardItemModel(4, 1);
		
		int row = 0;

		foreach(const ExperimentContainer &exp, expList) {
			model->setItem(row++, new QStandardItem(exp.shortName));
		}
		
		experimentList->setModel(model);
	});

	CONNECT(experimentList, &QListView::clicked, [=](const QModelIndex &index) {
		mw->PrebuiltExperimentSelected(index.row());
	});

	CONNECT(mw, &MainWindow::PrebuiltExperimentSetParameters, [=](const ExperimentContainer &ec) {
		ui.runExperiment.descr.fullName->setText(ec.name);
		ui.runExperiment.descr.text->setText(ec.description);
		ui.runExperiment.descr.icon->setPixmap(QPixmap(PREBUILT_EXP_DIR + ec.imagePath));
	});

//#define ADD_DUMMY_PARAMETER(row)\
//	ui.runExperiment.paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Parameper " #row " = "), "experiment-params-comment"), "comment-placement", "left"),	row, 0);\
//	ui.runExperiment.paramsLay->addWidget(LED(),					row, 1);\
//	ui.runExperiment.paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("mV"), "experiment-params-comment"), "comment-placement", "right"),				row, 2);
//	
//	ADD_DUMMY_PARAMETER(0);
//	ADD_DUMMY_PARAMETER(1);
//	ADD_DUMMY_PARAMETER(2);
//	ADD_DUMMY_PARAMETER(3);
//	ADD_DUMMY_PARAMETER(4);
//	ADD_DUMMY_PARAMETER(5);
//	ADD_DUMMY_PARAMETER(6);
//	ADD_DUMMY_PARAMETER(7);
//	ADD_DUMMY_PARAMETER(8);
//	ADD_DUMMY_PARAMETER(9);
//	ADD_DUMMY_PARAMETER(10);
//	ADD_DUMMY_PARAMETER(11);
//	ADD_DUMMY_PARAMETER(12);
//	ADD_DUMMY_PARAMETER(13);
//	ADD_DUMMY_PARAMETER(14);
	/*
	ui.runExperiment.paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Extreamly Long Parameper Name Name Name Name Name = "), "experiment-params-comment"), "comment-placement", "left"), 15, 0);
	ui.runExperiment.paramsLay->addWidget(LED(), 15, 1);
	ui.runExperiment.paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("mV"), "experiment-params-comment"), "comment-placement", "right"), 15, 2);
	//*/
	//ui.runExperiment.paramsLay->setRowStretch(15, 1);

	return w;
}
QWidget* MainWindowUI::GetSearchHardwareWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	QPushButton *searchByVendor;
	QPushButton *searchViaHandshake;
	QPushButton *selectHardware;
	QPushButton *requestCalibration;
	QComboBox *hwList;
	QLineEdit *channelEdit;

	w = WDG();
	QGridLayout *lay = NO_SPACING(NO_MARGIN(new QGridLayout(w)));

	lay->addWidget(OBJ_NAME(LBL("Select hardware"), "heading-label"), 0, 0, 1, 2);
	lay->addWidget(OBJ_NAME(LBL("COM port:"), "regular-comment-label"), 1, 0);
	lay->addWidget(OBJ_NAME(LBL("Channel:"), "regular-comment-label"), 2, 0);
	lay->addWidget(hwList = CMB(), 1, 1);
	lay->addWidget(channelEdit = LED(), 2, 1);
	QListView *hwListComboList = OBJ_NAME(new QListView, "combo-list");
	hwList->setView(hwListComboList);

	QGridLayout *buttonLay = NO_SPACING(NO_MARGIN(new QGridLayout));
	buttonLay->addWidget(searchByVendor = OBJ_NAME(PBT("Search by vendor"), "secondary-button"), 0, 1);
	buttonLay->addWidget(searchViaHandshake = OBJ_NAME(PBT("Search via handshake"), "secondary-button"), 0, 2);
	buttonLay->addWidget(selectHardware = OBJ_NAME(PBT("Select this hardware"), "secondary-button"), 1, 1);
	buttonLay->addWidget(requestCalibration = OBJ_NAME(PBT("Request calibration"), "secondary-button"), 1, 2);
	buttonLay->setColumnStretch(0, 1);
	buttonLay->setColumnStretch(3, 1);

	lay->addLayout(buttonLay, 3, 0, 1, 2);


	CONNECT(searchByVendor, &QPushButton::clicked, mw, &MainWindow::SearchHwVendor);
	CONNECT(searchViaHandshake, &QPushButton::clicked, mw, &MainWindow::SearchHwHandshake);

	CONNECT(mw, &MainWindow::HardwareFound, [=](const InstrumentList &instrumentList) {
		hwList->clear();

		foreach(const InstrumentInfo &info, instrumentList) {
			LOG() << info.portName << ": " << info.serial;
			hwList->addItem(info.portName, QVariant::fromValue(info));
		}
	});

	CONNECT(hwList, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		[=](int index) {
			if (-1 != index) {
				currentInstrument.instrumentInfo = hwList->itemData(index).value<InstrumentInfo>();
			}
		}
	);

	CONNECT(channelEdit, &QLineEdit::textChanged, [=](const QString &str) {
		currentInstrument.channel = str.toInt();
	});

	CONNECT(selectHardware, &QPushButton::clicked, [=]() {
		mw->SelectHardware(currentInstrument.instrumentInfo, currentInstrument.channel);
	});
	CONNECT(requestCalibration, &QPushButton::clicked, mw, &MainWindow::RequestCalibration);

	channelEdit->setValidator(new QIntValidator(0, MAX_CHANNEL_VALUE));
	channelEdit->setText("0");

	return w;
}
QWidget* MainWindowUI::GetLogWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	QTextEdit *log;

	w = WDG();
	QVBoxLayout *lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	lay->addWidget(OBJ_NAME(LBL("Log"), "heading-label"));
	lay->addWidget(log = TED());

	log->setReadOnly(true);
	
	SetLogSignalEmitterParent(mw);
	CONNECT(GetLogSignalEmitter(), &LogSignalEmitter::SendLog, log, &QTextEdit::append);
	
	return w;
}
QWidget* MainWindowUI::GetPlotWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = WDG();
	QVBoxLayout *lay = NO_MARGIN(NO_SPACING(new QVBoxLayout(w)));

	QwtPlot *plot = new QwtPlot();
	//plot->setAxisScale(QwtPlot::xBottom, 0, 100000);
	plot->setAxisScale(QwtPlot::yLeft, 0, 1050);
	QwtText title;
	title.setFont(QFont("Segoe UI", 14));
	//title.setText("Frequency (Hz)");
	title.setText("Timestamp (ms)");
	plot->setAxisTitle(QwtPlot::xBottom, title);
	//title.setText(QString("Impedance (`") + QChar(0x03a9) + QString(")"));
	title.setText("Current (ewe)");
	plot->setAxisTitle(QwtPlot::yLeft, title);

	plot->insertLegend(new QwtLegend(), QwtPlot::TopLegend);

	lay->addWidget(plot);

	QwtPlotCurve *curve = new QwtPlotCurve("Impedance 'Filename.csv'");
	curve->setLegendAttribute(QwtPlotCurve::LegendShowLine);
	curve->setPen(QColor(42, 127, 220), 1);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	curve->attach(plot);

	CONNECT(mw, &MainWindow::DataArrived, [=](quint8 channel, const ExperimentalData &expData) {
		static QVector<qreal> xPlotData, yPlotData;
		qreal x = expData.timestamp / 100000UL;
		qreal y = expData.adcData.ewe;
		xPlotData.append(x);
		yPlotData.append(y);

		curve->setSamples(xPlotData, yPlotData);
		plot->replot();
	});

	return w;
}
QWidget* MainWindowUI::GetControlButtonsWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	QPushButton *startExperiment;
	QPushButton *stopExperiment;

	w = WDG();
	QHBoxLayout *buttonLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(w)));
	buttonLay->addWidget(startExperiment = OBJ_NAME(PBT("Start Experiment"), "control-button-blue"));
	buttonLay->addWidget(stopExperiment = OBJ_NAME(PBT("Stop Experiment"), "control-button-red"));

	CONNECT(startExperiment, &QPushButton::clicked, mw, &MainWindow::StartExperiment);
	CONNECT(stopExperiment, &QPushButton::clicked, mw, &MainWindow::StopExperiment);

	return w;
}
