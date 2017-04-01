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
#include <QScrollArea>

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

bool SettingsResizeEventFilter::eventFilter(QObject *obj, QEvent *e) {
	if (e->type() == QEvent::Resize) {
		emit ResizeSettings();
		return true;
	}
	else {
		return QObject::eventFilter(obj, e);
	}
}
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
void MainWindowUI::InstallEventFilter() {
	SettingsResizeEventFilter *filter = new SettingsResizeEventFilter(mw);
	
	connect(filter, &SettingsResizeEventFilter::ResizeSettings, [=] () {
		ResizeSettings();
	});

	mw->installEventFilter(filter);
}
void MainWindowUI::ResizeSettings() {
	QSize size = ui.cockpit.settings.owner->sizeHint();
	
	QRect cockpitRect = ui.cockpit.owner->geometry();
	QRect topRect = ui.cockpit.top.owner->geometry();
	QRect plotRect = ui.cockpit.plot.owner->geometry();
	QRect bottomRect = ui.cockpit.bottom.owner->geometry();

	ui.cockpit.settings.owner->setGeometry(cockpitRect.width() - size.width(),
		topRect.height(),
		size.width(),
		plotRect.height() + bottomRect.height());

	ui.cockpit.settings.owner->raise();
}
void MainWindowUI::CreateCockpitLogic() {
	connect(ui.cockpit.settings.selectChannel, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated), [=](const QString &text) {
		SetText(ui.cockpit.top.channel, text);
	});


	connect(ui.cockpit.top.runControl, &QPushButton::clicked,
		mw, &MainWindow::applyStyle);

	connect(ui.cockpit.top.settings, &QPushButton::clicked, [=] {
		if (ui.cockpit.settings.owner->isVisible()) {
			ui.cockpit.settings.owner->hide();
		}
		else {
			ui.cockpit.settings.owner->show();
			ResizeSettings();
		}
	});
}
QWidget* MainWindowUI::CreateCockpitTopWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	QPushButton *hideExpandSettingsButton;

	QWidget *topPanelWidget = OBJ_NAME(WDG(), "topPanelWidget");
	QHBoxLayout *topPanelLayout = NO_SPACING(NO_MARGIN(new QHBoxLayout(topPanelWidget)));

	topPanelLayout->addWidget(ui.cockpit.top.channel = OBJ_NAME(LBL("Channel 1"), "channelLabel"));
	topPanelLayout->addStretch(1);
	topPanelLayout->addWidget(ui.cockpit.top.runControl = OBJ_NAME(PBT(""), "startStopButton"));
	topPanelLayout->addWidget(ui.cockpit.top.settings = OBJ_NAME(PBT(""), "hideExpandSettingsButton"));

	ui.cockpit.top.settings->setCheckable(true);

	w = topPanelWidget;
	ui.cockpit.top.owner = w;

	return w;
}
QWidget* MainWindowUI::CreateCockpitBottomWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	QWidget *bottomPanelWidget = OBJ_NAME(WDG(), "bottomPanelWidget");
	QHBoxLayout *bottomPanelLayout = NO_SPACING(NO_MARGIN(new QHBoxLayout(bottomPanelWidget)));

	bottomPanelLayout->addWidget(OBJ_PROP(OBJ_PROP(LBL("Ewe:"), "type", "label"), "order", "first"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("-1.345 V"), "type", "value"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("Ece:"), "type", "label"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("-2.255 V"), "type", "value"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("Ewe - Ece:"), "type", "label"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("0.91 V"), "type", "value"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("Current (mA):"), "type", "label"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("0"), "type", "value"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("Redox state:"), "type", "label"));
	bottomPanelLayout->addWidget(OBJ_PROP(LBL("Open circuit"), "type", "value"));
	bottomPanelLayout->addStretch(1);

	w = bottomPanelWidget;
	ui.cockpit.bottom.owner = w;

	return w;
}
QWidget* MainWindowUI::CreateCockpitSettingsWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	QWidget *settingsPanelWidget = OBJ_NAME(WDG(), "settingsPanelWidget");
	QVBoxLayout *settingsPanelLayout = NO_SPACING(NO_MARGIN(new QVBoxLayout(settingsPanelWidget)));

	QGroupBox *selectChannel = OBJ_NAME(new QGroupBox(tr("Select channel")), "selectChannel");
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
	ui.cockpit.settings.selectChannel = selectChannelCombo;

	QGroupBox *operatingConditions = OBJ_NAME(new QGroupBox(tr("Operating conditions")), "operatingConditions");
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

	QGroupBox *commonSettings = OBJ_NAME(new QGroupBox(tr("Common settings")), "commonSettings");
	QGridLayout *commonSettingsLayout = new QGridLayout;
	commonSettings->setLayout(commonSettingsLayout);
	commonSettingsLayout->addWidget(LBL("Sampling period"), 0, 0);
	commonSettingsLayout->addWidget(TXT_CNTR(LED("1.000")), 0, 1);
	commonSettingsLayout->addWidget(LBL("seconds"), 0, 2);
	commonSettingsLayout->addWidget(LBL("W. E. setpoint"), 1, 0);
	commonSettingsLayout->addWidget(TXT_CNTR(LED("0.000")), 1, 1);
	commonSettingsLayout->addWidget(LBL("V"), 1, 2);
	commonSettingsLayout->addWidget(CKB("Switch to open circuit"), 2, 0, 1, 3);
	commonSettingsLayout->setRowStretch(3, 1);

	QGroupBox *advancedSettings = OBJ_NAME(new QGroupBox(tr("Advanced settings")), "advancedSettings");
	QVBoxLayout *advancedSettingsLayout = new QVBoxLayout;
	advancedSettings->setLayout(advancedSettingsLayout);
	advancedSettingsLayout->addWidget(CKB("Discrete sampling"));
	advancedSettingsLayout->addWidget(CKB("IR compensation"));
	advancedSettingsLayout->addWidget(CKB("Voltage error correction"));
	advancedSettingsLayout->addStretch(1);


	settingsPanelLayout->addWidget(selectChannel);
	settingsPanelLayout->addWidget(operatingConditions);
	settingsPanelLayout->addWidget(commonSettings);
	settingsPanelLayout->addWidget(advancedSettings);
	settingsPanelLayout->addStretch(1);


	QScrollArea *scrollArea = OBJ_NAME(new QScrollArea(CreateCockpitModeWidget()), "settingsPanelArea");

	scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(settingsPanelWidget);

	w = scrollArea;
	ui.cockpit.settings.owner = w;

	return w;
}
QWidget* MainWindowUI::CreateCockpitPlot() {
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
		samples << QPointF(x, qSin(x) * 1000);
	}

	curve1->setSamples(samples);
	curve1->setPen(QColor(166, 166, 166), 1),
		curve1->setRenderHint(QwtPlotItem::RenderAntialiased, true);

	curve1->attach(w);

	w->replot();

	ui.cockpit.plot.owner = plotWidget;

	return plotWidget;
}
QWidget* MainWindowUI::CreateCockpitModeWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = new QWidget();
	ui.cockpit.owner = w;

	QGridLayout *cockpitLayout = NO_SPACING(NO_MARGIN(new QGridLayout(w)));

	cockpitLayout->addWidget(CreateCockpitTopWidget(),		0, 0, 1, 2);
	cockpitLayout->addWidget(CreateCockpitPlot(),			1, 0);
	cockpitLayout->addWidget(CreateCockpitBottomWidget(),	2, 0);
	//cockpitLayout->addWidget(CreateCockpitSettingsWidget(), 1, 1, 2, 1);

	CreateCockpitSettingsWidget()->hide();

	CreateCockpitLogic();
	InstallEventFilter();

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