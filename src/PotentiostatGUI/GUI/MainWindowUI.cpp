#include <MainWindowUI.h>

#include <QGraphicsOpacityEffect>

#include <MainWindow.h>

#include <QStackedWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QTreeView>
#include <QListView>
#include <QScrollBar>
#include <QToolButton>
#include <QTabWidget>
#include <QGroupBox>
#include <QCheckBox>

#include <QButtonGroup>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>

#include <QStandardItemModel>

#include <QEvent>

#include <QIcon>
#include <QImage>
#include <QPixmap>
#include <QPainter>

#include <QString>

#include <QtMath>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include "UIHelper.hpp"

MainWindowUI::MainWindowUI(MainWindow *mainWindow) :
	mw(mainWindow)
{
	mw->setObjectName("mainUI");
}
MainWindowUI::~MainWindowUI() {
}
void MainWindowUI::CreateUI() {
	CreateCentralWidget();
}
void MainWindowUI::CreateCentralWidget() {
	QWidget *centralWidget = OBJ_NAME(WDG(), "central-widget");
	QVBoxLayout *centralLayout = NO_SPACING(NO_MARGIN(new QVBoxLayout(centralWidget)));
	mw->setCentralWidget(centralWidget);

	QHBoxLayout *tabLayout = NO_MARGIN(new QHBoxLayout);

	QWidget *barWidget = OBJ_NAME(WDG(), "bar-widget");
	QVBoxLayout *barLayout = NO_SPACING(NO_MARGIN(new QVBoxLayout(barWidget)));
	QStackedLayout *widgetsLayout = NO_MARGIN(new QStackedLayout);

	tabLayout->addWidget(barWidget);
	tabLayout->addLayout(widgetsLayout);

	QButtonGroup *buttonGroup = new QButtonGroup(mw);
	
	barLayout->addWidget(OBJ_NAME(LBL("<logo>"), "logo"));

	auto *pbt = OBJ_NAME(PBT(""), "bar-button");
	pbt->setDisabled(true);
	barLayout->addWidget(pbt);
	pbt = OBJ_NAME(PBT(""), "bar-button");
	pbt->setDisabled(true);
	barLayout->addWidget(pbt);

	pbt = OBJ_PROP(OBJ_NAME(PBT("Cockpit mode"), "bar-button"), "order", "first");
	pbt->setCheckable(true);
	pbt->setChecked(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);
	
	pbt = OBJ_NAME(PBT("Experiment builder"), "bar-button");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);
	
	pbt = OBJ_PROP(OBJ_NAME(PBT("Open data file"), "bar-button"), "order", "last");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	barLayout->addStretch(1);

	widgetsLayout->addWidget(OBJ_NAME(CreateCockpitModeWidget(), "cockpit-owner"));
	widgetsLayout->addWidget(CreateExperimentBuilderWidget());
	widgetsLayout->addWidget(CreateOpenDataFileWidget());
	
	centralLayout->addLayout(tabLayout);
}
QWidget* MainWindowUI::CreateCockpitModeWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = new QWidget();

	QVBoxLayout *cockpitLayout = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	QPushButton *hideExpandSettingsButton;
	QLabel *channelLabel;

	QWidget *topPanelWidget = OBJ_NAME(WDG(), "topPanelWidget");
	QHBoxLayout *topPanelLayout = NO_SPACING(NO_MARGIN(new QHBoxLayout(topPanelWidget)));
	topPanelLayout->addWidget(channelLabel = OBJ_NAME(LBL("Channel 1"), "channelLabel"));
	topPanelLayout->addWidget(OBJ_NAME(LBL("To modify settings click the button"), "settingsCommentLabel"));
	topPanelLayout->addWidget(hideExpandSettingsButton = OBJ_NAME(PBT("expand"), "hideExpandSettingsButton"));

	QWidget *bottomPanelWidget = OBJ_NAME(WDG(), "bottomPanelWidget");
	QHBoxLayout *bottomPanelLayout = NO_SPACING(NO_MARGIN(new QHBoxLayout(bottomPanelWidget)));

	QGroupBox *realTimeValues = OBJ_NAME(new QGroupBox(tr("REAL TIME VALUES")), "realTimeValues");
	QGridLayout *realTimeValuesLayout = new QGridLayout;
	realTimeValues->setLayout(realTimeValuesLayout);
	realTimeValuesLayout->addWidget(LBL("Ewe"),	0, 0);
	realTimeValuesLayout->addWidget(TXT_CNTR(LED("-1.345 V")),		0, 1);
	realTimeValuesLayout->addWidget(LBL("Ece"),	1, 0);
	realTimeValuesLayout->addWidget(TXT_CNTR(LED("-2.255 V")),		1, 1);
	realTimeValuesLayout->addWidget(LBL("Ewe - Ece"),				2, 0);
	realTimeValuesLayout->addWidget(TXT_CNTR(LED("0.91 V")),		2, 1);
	realTimeValuesLayout->addWidget(LBL("Current (mA)"),			0, 2);
	realTimeValuesLayout->addWidget(TXT_CNTR(LED("0")),				0, 3);
	realTimeValuesLayout->addWidget(LBL("Redox state"),				1, 2);
	realTimeValuesLayout->addWidget(TXT_CNTR(LED("Open circuit")),	1, 3);

	QPushButton *startStopButton;
	QGroupBox *runControl = OBJ_NAME(new QGroupBox(tr("RUN CONTROL")), "runControl");
	QVBoxLayout *runControlLayout = new QVBoxLayout;
	runControl->setLayout(runControlLayout);
	runControlLayout->addWidget(OBJ_NAME(LBL("To control run click the button"), "runControlLabel"));
	runControlLayout->addWidget(startStopButton = OBJ_NAME(PBT("start"), "startStopButton"));

	connect(startStopButton, &QPushButton::clicked,
		mw, &MainWindow::applyStyle);

	bottomPanelLayout->addWidget(realTimeValues);
	bottomPanelLayout->addWidget(runControl);

	QWidget *settingsPanelWidget = OBJ_NAME(WDG(), "settingsPanelWidget");
	QGridLayout *settingsPanelLayout = NO_SPACING(NO_MARGIN(new QGridLayout(settingsPanelWidget)));

	QGroupBox *selectChannel = OBJ_NAME(new QGroupBox(tr("SELECT CHANNEL")), "selectChannel");
	QVBoxLayout *selectChannelLayout = new QVBoxLayout;
	selectChannel->setLayout(selectChannelLayout);
	QComboBox *selectChannelCombo = new QComboBox;
	QListView *selectChannelComboList = OBJ_NAME(new QListView, "selectChannelComboList");
	selectChannelCombo->setView(selectChannelComboList);
	selectChannelLayout->addWidget(selectChannelCombo);
	selectChannelLayout->addStretch(1);
	selectChannelCombo->addItem("Channel 1");
	selectChannelCombo->addItem("Channel 2");
	selectChannelCombo->addItem("Channel 3");
	selectChannelCombo->addItem("Channel 4");

	connect(selectChannelCombo, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated), [=](const QString &text) {
		SetText(channelLabel, text);
	});

	QGroupBox *operatingConditions = OBJ_NAME(new QGroupBox(tr("OPERATING CONDITIONS")), "operatingConditions");
	QVBoxLayout *operatingConditionsLayout = new QVBoxLayout;
	operatingConditions->setLayout(operatingConditionsLayout);
	QComboBox *operatingConditionsCombo = new QComboBox;
	QListView *operatingConditionsComboList = OBJ_NAME(new QListView, "operatingConditionsComboList");
	operatingConditionsCombo->setView(operatingConditionsComboList);
	QComboBox *currentRangeCombo = new QComboBox;
	QListView *currentRangeComboList = OBJ_NAME(new QListView, "currentRangeComboList");
	currentRangeCombo->setView(currentRangeComboList);
	operatingConditionsLayout->addWidget(operatingConditionsCombo);
	operatingConditionsLayout->addWidget(currentRangeCombo);
	operatingConditionsLayout->addStretch(1);
	operatingConditionsCombo->addItem("Potentiostat");
	operatingConditionsCombo->addItem("Galvanostat");
	currentRangeCombo->addItem("Autorange");

	QGroupBox *commonSettings = OBJ_NAME(new QGroupBox(tr("COMMON SETTINGS")), "commonSettings");
	QGridLayout *commonSettingsLayout = new QGridLayout;
	commonSettings->setLayout(commonSettingsLayout);
	commonSettingsLayout->addWidget(LBL("Sampling period"),			0, 0);
	commonSettingsLayout->addWidget(TXT_CNTR(LED("1.000")),			0, 1);
	commonSettingsLayout->addWidget(LBL("seconds"),					0, 2);
	commonSettingsLayout->addWidget(LBL("W. E. setpoint"),			1, 0);
	commonSettingsLayout->addWidget(TXT_CNTR(LED("0.000")),			1, 1);
	commonSettingsLayout->addWidget(LBL("V"),						1, 2);
	commonSettingsLayout->addWidget(CHEKABLE(PBT("Switch to open circuit")),	2, 0, 1, 3);
	commonSettingsLayout->setRowStretch(3, 1);

	QGroupBox *advancedSettings = OBJ_NAME(new QGroupBox(tr("ADVANCED SETTINGS")), "advancedSettings");
	QVBoxLayout *advancedSettingsLayout = new QVBoxLayout;
	advancedSettings->setLayout(advancedSettingsLayout);
	advancedSettingsLayout->addWidget(CHEKABLE(PBT("Discrete sampling")));
	advancedSettingsLayout->addWidget(CHEKABLE(PBT("IR compensation")));
	advancedSettingsLayout->addWidget(CHEKABLE(PBT("Voltage error correction")));
	advancedSettingsLayout->addStretch(1);


	settingsPanelLayout->addWidget(selectChannel,			0, 0);
	settingsPanelLayout->addWidget(operatingConditions,		0, 1);
	settingsPanelLayout->addWidget(commonSettings,			1, 0);
	settingsPanelLayout->addWidget(advancedSettings,		1, 1);
	settingsPanelLayout->setColumnStretch(0, 1);
	settingsPanelLayout->setColumnStretch(1, 1);


	QWidget *plotWidget = CreatePlot();
	settingsPanelWidget->hide();

	connect(hideExpandSettingsButton, &QPushButton::clicked, [=] {
		if (settingsPanelWidget->isVisible()) {
			SetText(hideExpandSettingsButton, "expand");
			settingsPanelWidget->hide();
			plotWidget->show();
		}
		else {
			SetText(hideExpandSettingsButton, "collapse");
			settingsPanelWidget->show();
			plotWidget->hide();
		}
	});

	cockpitLayout->addWidget(settingsPanelWidget);
	cockpitLayout->addWidget(topPanelWidget);
	cockpitLayout->addWidget(plotWidget);
	cockpitLayout->addWidget(bottomPanelWidget);

	return w;
}
QWidget* MainWindowUI::CreateExperimentBuilderWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = new QWidget();

	return w;
}
QWidget* MainWindowUI::CreateOpenDataFileWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = new QWidget();

	return w;
}
QWidget* MainWindowUI::CreateButton() {
	static QPushButton *w = 0;

	if (w) {
		return w;
	}

	w = PBT("Set Style");

	connect(w, &QPushButton::clicked,
		mw, &MainWindow::applyStyle);

	return w;
}
QWidget* MainWindowUI::CreatePlot() {
	static QWidget *plotWidget = 0;

	if (plotWidget) {
		return plotWidget;
	}

	plotWidget = OBJ_NAME(WDG(), "plotWidget");
	QVBoxLayout *plotWidgetLayout = NO_MARGIN(NO_SPACING(new QVBoxLayout(plotWidget)));

	QwtPlot *w = new QwtPlot();
	plotWidgetLayout->addWidget(w);

	QwtPlotCurve *curve1 = new QwtPlotCurve("Curve 1");

	QVector<QPointF> samples;

	for (float x = 0; x < 10; x += 10 / 1000.) {
		samples << QPointF(x, qSin(x)*1000);
	}

	curve1->setSamples(samples);
	curve1->setPen(QColor(0x3D, 0x41, 0x4A), 1),
	curve1->setRenderHint(QwtPlotItem::RenderAntialiased, true);

	curve1->attach(w);

	w->replot();

	return plotWidget;
}