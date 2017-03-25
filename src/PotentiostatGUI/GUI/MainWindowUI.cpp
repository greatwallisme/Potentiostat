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

#include <qwt_plot.h>

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

	auto *pbt = OBJ_PROP(OBJ_NAME(PBT("Cockpit mode"), "bar-button"), "order", "first");
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
	
	centralLayout->addWidget(CreateButton());
	centralLayout->addLayout(tabLayout);
	//centralLayout->addWidget(CreatePlot());
}
QWidget* MainWindowUI::CreateCockpitModeWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = new QWidget();

	QVBoxLayout *cockpitLayout = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	QWidget *topPanelWidget = OBJ_NAME(WDG(), "topPanelWidget");
	QHBoxLayout *topPanelLayout = NO_SPACING(NO_MARGIN(new QHBoxLayout(topPanelWidget)));
	topPanelLayout->addWidget(OBJ_NAME(LBL("channel 1"), "channelLabel"));
	topPanelLayout->addWidget(OBJ_NAME(LBL("To modify settings click the button"), "settingsCommentLabel"));
	topPanelLayout->addWidget(OBJ_NAME(PBT("expand"), "hideExpandSettingsButton"));

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

	QGroupBox *runControl = OBJ_NAME(new QGroupBox(tr("RUN CONTROL")), "runControl");
	QVBoxLayout *runControlLayout = new QVBoxLayout;
	runControl->setLayout(runControlLayout);
	runControlLayout->addWidget(OBJ_NAME(LBL("To control run click the button"), "runControlLabel"));
	runControlLayout->addWidget(OBJ_NAME(PBT("start"), "startStopButton"));

	bottomPanelLayout->addWidget(realTimeValues);
	bottomPanelLayout->addWidget(runControl);

	cockpitLayout->addWidget(topPanelWidget);
	cockpitLayout->addWidget(CreatePlot());
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
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = new QwtPlot();

	return w;
}