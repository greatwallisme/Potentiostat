#include "MainWindowUI.h"
#include "MainWindow.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_scale_widget.h>
#include <qwt_text_label.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>

#include "UIHelper.hpp"

#include "Log.h"

#include "ListSeriesData.h"

#include <QButtonGroup>

#include <QEvent>
#include <QKeyEvent>
#include <QMimeData>

#include <QIntValidator>
#include <QListView>
#include <QTabWidget>
#include <QSortFilterProxyModel>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QScrollArea>
#include <QCheckBox>

#include <QStandardItemModel>

#include <QPixmap>
#include <QLine>

#include <QTime>
#include <QDateTime>
#include <QFileDialog>
#include <QStringList>
#include <QSettings>
#include <QColorDialog>
#include <QMessageBox>

#include <QXmlStreamReader>

#include <QEventLoop>

#include <qtcsv/reader.h>

#include <QDrag>
#include <QMimeData>

#include "ExperimentReader.h"
#include "HexLoader.h"
#include "UIEventFilters.hpp"
#include "ManualExperimentRunner.h"
#include "HwListButton.h"

#include <functional>
#include <QScrollBar>
#include <QGraphicsDropShadowEffect> 
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QToolTip>
#include <QTimer>

#include <QDesktopServices>
#include <QSplitter>
#include <QMenu>
#include <QMenuBar>
#include <QGroupBox>
#include <QStandardPaths>
#include <QDesktopWidget>
#include <QSignalMapper>
#include <QTreeView>
#include <QHeaderView>
#include <QMdiArea>
#include <QMdiSubWindow>

#define FW_HEX_OPEN_PATH				"fw-hex-open-path"

#define EXPERIMENT_VIEW_ALL_CATEGORY	"View All"
#define NONE_Y_AXIS_VARIABLE			"None"

#define CUSTOM_EXP_DIR			"./custom/"

#define DEFAULT_MAJOR_CURVE_COLOR		QColor(42, 127, 220)
#define DEFAULT_MINOR_CURVE_COLOR		QColor(208, 35, 39)

#define CURVE_PARAMS_PRI_PEN_COLOR		"curve-params-pri-pen-color"
#define CURVE_PARAMS_PRI_PEN_WIDTH		"curve-params-pri-pen-width"
#define CURVE_PARAMS_PRI_PEN_STYLE		"curve-params-pri-pen-style"
#define CURVE_PARAMS_PRI_STYLE			"curve-params-pri-style"
#define CURVE_PARAMS_PRI_SYMBOL_WIDTH	"curve-params-pri-symbol-width"
#define CURVE_PARAMS_PRI_SYMBOL_STYLE	"curve-params-pri-symbol-style"

#define CURVE_PARAMS_SEC_PEN_COLOR		"curve-params-sec-pen-color"
#define CURVE_PARAMS_SEC_PEN_WIDTH		"curve-params-sec-pen-width"
#define CURVE_PARAMS_SEC_PEN_STYLE		"curve-params-sec-pen-style"
#define CURVE_PARAMS_SEC_STYLE			"curve-params-sec-style"
#define CURVE_PARAMS_SEC_SYMBOL_WIDTH	"curve-params-sec-symbol-width"
#define CURVE_PARAMS_SEC_SYMBOL_STYLE	"curve-params-sec-symbol-style"

#define PAUSE_EXP_BUTTON_TEXT		"Pause Experiment"
#define RESUME_EXP_BUTTON_TEXT		"Resume Experiment"

#define ACTIVE_STATUS	"Active"
#define STOPPED_STATUS	"Stopped"
#define ERROR_STATUS	"Error"
#define PAUSED_STATUS	"Paused"

#define SUB_WINDOWS_FLAGS			(Qt::Window | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::CustomizeWindowHint)

/*
#define STOPPED_DOT		QIcon(":/GUI/Resources/red-dot.png")
#define MIDDLE_DOT		QIcon(":/GUI/Resources/yellow-dot.png")
#define ACTIVE_DOT		QIcon(":/GUI/Resources/green-dot.png")
//*/

enum : quint8 {
	NAME_COL = 0,
	STATUS_COL,
	EXPERIMENT_COL,
	STEP_COL,
	LAST_NOTIF_COL
};

MainWindowUI::MainWindowUI(MainWindow *mainWindow) :
	mw(mainWindow)
{
	prebuiltExperimentData.userInputs = 0;
	builderTabs.userInputs = 0;
	selectedHardware.prebuilt.exp = 0;
	selectedHardware.manual.exp = ManualExperimentRunner::Instance();
	selectedHardware.prebuilt.channel = 0;

	mw->setObjectName("mainUI");
	mw->ApplyStyle();
}
MainWindowUI::~MainWindowUI() {
}
void MainWindowUI::DisconnectAll() {
	for (auto it = connections.begin(); it < connections.end(); ++it) {
		QObject::disconnect(*it);
	}
}
void MainWindowUI::CreateUI() {
	SetLogSignalEmitterParent(mw);
	CreateCentralWidget();
	CreateMenu();
}
void MainWindowUI::CreateMenu() {
	auto menuBar = new QMenuBar;

	auto moreOptionsMenu = new QMenu("More Options");

	auto updateHardware = moreOptionsMenu->addAction("Update Hardware");
 
  /*auto selectCompRangeMenu = moreOptionsMenu->addMenu("Potentiostat stability");
  auto compRange0Select = selectCompRangeMenu->addAction("Stability setting 1");
  auto compRange1Select = selectCompRangeMenu->addAction("Stability setting 2");
  compRange0Select->setCheckable(true);
  compRange1Select->setCheckable(true);*/

	menuBar->addMenu(moreOptionsMenu);

#ifndef QT_NO_DEBUG
    auto debugMenu = new QMenu("Debug");
    auto applyStyleSheet = debugMenu->addAction("Apply stylesheet");
    menuBar->addMenu(debugMenu);

	connections << CONNECT(applyStyleSheet, &QAction::triggered, mw, &MainWindow::ApplyStyle);
#endif

	connections << CONNECT(updateHardware, &QAction::triggered, [=]() {
		GetUpdateFirmwareDialog(mw);
	});

  //connections << CONNECT(compRange0Select, &QAction::triggered, [=]() {
  //    mw->SetCompRange(0);
  //});

  //connections << CONNECT(compRange1Select, &QAction::triggered, [=]() {
  //    mw->SetCompRange(1);
  //});
	mw->setMenuBar(menuBar);
}
void MainWindowUI::GetUpdateFirmwareDialog(QWidget *parent) {
	QDialog* dialog = new QDialog(parent, Qt::SplashScreen);
	QList<QMetaObject::Connection> dialogConn;
	auto lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(dialog)));

	auto frame = OBJ_NAME(new QFrame, "update-firmware-dialog");
	lay->addWidget(frame);

	auto globalLay = NO_MARGIN(new QVBoxLayout(frame));

	QListView *instrumentList;

	globalLay->addWidget(OBJ_NAME(new QLabel("Update Firmware"), "heading-label"));
	globalLay->addWidget(OBJ_PROP(OBJ_NAME(new QLabel("1) Choose which device you want to update:"), "experiment-params-comment"), "comment-placement", "right"));
	globalLay->addWidget(instrumentList = OBJ_NAME(new QListView, "curve-params-data-set-list"));
	globalLay->addWidget(OBJ_PROP(OBJ_NAME(new QLabel("2) Select the .hex file to program into the hardware:"), "experiment-params-comment"), "comment-placement", "right"));

	instrumentList->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QLineEdit *hexFileLed;
	QPushButton *hexFilePbt;

	auto hexLay = new QHBoxLayout;
	hexLay->addWidget(hexFileLed = LED());
	hexLay->addWidget(hexFilePbt = OBJ_NAME(PBT("Browse"), "secondary-button"));

	hexFileLed->setReadOnly(true);
	hexFileLed->setPlaceholderText("(No file selected)");

	globalLay->addLayout(hexLay);

	QPushButton *nextBut;
	QPushButton *cancelBut;

	auto buttonLay = new QHBoxLayout;
	buttonLay->addStretch(1);
	buttonLay->addWidget(nextBut = OBJ_NAME(PBT("Next"), "secondary-button"));
	buttonLay->addWidget(cancelBut = OBJ_NAME(PBT("Cancel"), "secondary-button"));
	buttonLay->addStretch(1);

	globalLay->addLayout(buttonLay);

	dialogConn << CONNECT(cancelBut, &QPushButton::clicked, dialog, &QDialog::reject);

	dialogConn << CONNECT(hexFilePbt, &QPushButton::clicked, [=]() {
		QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);
		QString dirName = settings.value(FW_HEX_OPEN_PATH, "").toString();

		QString filePath = QFileDialog::getOpenFileName(dialog, "Select firmware file", dirName, "Intel HEX (*.hex)");

		if (filePath.isEmpty()) {
			return;
		}

		settings.setValue(FW_HEX_OPEN_PATH, QFileInfo(filePath).absolutePath());
		hexFileLed->setText(filePath);
	});

	dialogConn << CONNECT(mw, &MainWindow::CurrentHardwareList, [=](const InstrumentList &_list) {
		InstrumentList list(_list);
		for (int i = 0; i < list.size();) {
			if (list.at(i).hwVer.hwModel < PLUS_2_0) {
				list.removeAt(i);
			}
			else {
				++i;
			}
		}

		QStandardItemModel *model = new QStandardItemModel(list.size(), 1);
		int row = 0;
		for (auto it = list.begin(); it != list.end(); ++it) {
			auto *item = new QStandardItem(it->name);
			model->setItem(row++, item);
		}
		instrumentList->setModel(model);
		
        if (list.size()) {
            instrumentList->setCurrentIndex(instrumentList->model()->index(0, 0));

		}
	});
	
	dialogConn << CONNECT(nextBut, &QPushButton::clicked, [=]() {
		auto index = instrumentList->currentIndex();

		if (!index.isValid()) {
			return;
		}

		auto fw = HexLoader::ReadFile(hexFileLed->text());

		if (fw.isEmpty()) {
			return;
		}

		auto instName = index.data(Qt::DisplayRole).toString();

		mw->UpdateFirmware(instName, fw);
	});

	mw->RequestCurrentHardwareList();

	dialog->exec();

	foreach(auto conn, dialogConn) {
		QObject::disconnect(conn);
	}

	dialog->deleteLater();
}
void MainWindowUI::CreateCentralWidget() {
	QWidget *centralWidget = OBJ_NAME(WDG(), "central-widget");
	QGridLayout *centralLayout = NO_SPACING(NO_MARGIN(new QGridLayout(centralWidget)));
	mw->setCentralWidget(centralWidget);

	//centralLayout->addWidget(GetApplyStyleButton(),		0, 0);
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

	connections << CONNECT(pbt, &QPushButton::clicked, mw, &MainWindow::ApplyStyle);

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

	widgetsLayout->addWidget(GetRunExperimentTab());
	widgetsLayout->addWidget(GetBuildExperimentTab());
	widgetsLayout->addWidget(GetManualControlTab());
	widgetsLayout->addWidget(GetNewDataWindowTab());
	widgetsLayout->addWidget(GetStatisticsTab());
#ifndef QT_NO_DEBUG
	widgetsLayout->addWidget(GetOldSearchHardwareTab());
#endif

	QButtonGroup *buttonGroup = new QButtonGroup(mw);
	
	QPushButton *pbt;
	
	pbt = OBJ_NAME(PBT("Run an Experiment"), "bar-button");
	pbt->setCheckable(true);
	pbt->setChecked(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	connections << CONNECT(pbt, &QPushButton::toggled, [=](bool checked) {
		if (!checked) {
			return;
		}

		widgetsLayout->setCurrentWidget(GetRunExperimentTab());

		mw->SelectHardware(selectedHardware.prebuilt.hwName, selectedHardware.prebuilt.channel);
		mw->PrebuiltExperimentSelected(selectedHardware.prebuilt.exp);
	});

	pbt = OBJ_NAME(PBT("Build an Experiment"), "bar-button");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	ui.newDataTab.buildExperimentButton = pbt;

	connections << CONNECT(pbt, &QPushButton::toggled, [=](bool checked) {
		if (!checked) {
			return;
		}

		widgetsLayout->setCurrentWidget(GetBuildExperimentTab());
	});

	pbt = OBJ_NAME(PBT("Manual Control"), "bar-button");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	ui.manualExperiment.tabButton = pbt;

	connections << CONNECT(pbt, &QPushButton::toggled, [=](bool checked) {
		if (!checked) {
			return;
		}

		widgetsLayout->setCurrentWidget(GetManualControlTab());

		quint8 channel = selectedHardware.manual.channel.value(selectedHardware.manual.hwName, 0);
		mw->SelectHardware(selectedHardware.manual.hwName, channel);
		mw->PrebuiltExperimentSelected(selectedHardware.manual.exp);
	});

	pbt = OBJ_PROP(OBJ_NAME(PBT("View Data"), "bar-button"), "order", "last");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	ui.newDataTab.newDataTabButton = pbt;

	connections << CONNECT(pbt, &QPushButton::toggled, [=](bool checked) {
		if (!checked) {
			return;
		}

		widgetsLayout->setCurrentWidget(GetNewDataWindowTab());
	});

	pbt = OBJ_PROP(OBJ_NAME(PBT("Channel Status"), "bar-button"), "order", "last");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	connections << CONNECT(pbt, &QPushButton::toggled, [=](bool checked) {
		if (!checked) {
			return;
		}

		widgetsLayout->setCurrentWidget(GetStatisticsTab());
	});

#ifndef QT_NO_DEBUG
	pbt = OBJ_PROP(OBJ_NAME(PBT("Search the Hardware"), "bar-button"), "order", "first");
	pbt->setCheckable(true);
	buttonGroup->addButton(pbt);
	barLayout->addWidget(pbt);

	connections << CONNECT(pbt, &QPushButton::toggled, [=](bool checked) {
		if (!checked) {
			return;
		}

		widgetsLayout->setCurrentWidget(GetOldSearchHardwareTab());
	});
#endif

	barLayout->addStretch(1);

	auto *bellFrame = OBJ_NAME(new QFrame, "notification-bell-frame");
	auto *bellFrameLayout = NO_SPACING(NO_MARGIN(new QVBoxLayout(bellFrame)));

	auto *digitLabel = OBJ_NAME(new QLabel("0"), "notification-new-messages");
	digitLabel->hide();
	bellFrameLayout->addWidget(digitLabel);

	barLayout->addWidget(bellFrame);
	
	ui.notificationDependencies.owner = w;
	ui.notificationDependencies.mainTabBar = barWidget;

	ShowNotificationDialog(false);

	bellFrame->installEventFilter(new UniversalEventFilter(bellFrame, [=](QObject *obj, QEvent *e) -> bool {
		if (e->type() == QEvent::MouseButtonPress) {
			auto me = (QMouseEvent*)e;

			auto marg = bellFrame->contentsMargins();
			auto pos = me->pos();


			QPoint bottomRight = QPoint(bellFrame->width(), bellFrame->height());
			bottomRight -= QPoint(marg.right(), 0);

			QPoint topLeft = QPoint(0, 0);

			if (!QRect(topLeft, bottomRight).contains(me->pos())) {
				return false;
			}
			
			digitLabel->setText("0");
			digitLabel->hide();
			ShowNotificationDialog();
		}
		return false;
	}));
	connections << CONNECT(GetLogSignalEmitter(), &LogSignalEmitter::SendLogExtended, [=]() {
		digitLabel->show();
		digitLabel->setText(QString("%1").arg(digitLabel->text().toUInt() + 1));
	});

	return w;
}
void MainWindowUI::ShowNotificationDialog(bool needToExec) {
	static QWidget* dialog = 0;
	static std::function<void(void)> handler = 0;

	if (dialog) {
		if (needToExec) {
			handler();
			dialog->show();
		}
		return;
	}

	dialog = OBJ_NAME(new QWidget(ui.notificationDependencies.owner, Qt::FramelessWindowHint | Qt::Popup), "notification-dialog");
	dialog->setAttribute(Qt::WA_TranslucentBackground);

	auto dialogLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(dialog)));

	auto dialogMainWidget = OBJ_NAME(new QScrollArea(), "notification-dialog-main-widget");
	dialogLay->addWidget(dialogMainWidget);

	auto itemsOwner = OBJ_NAME(WDG(), "notification-dialog-items-owner");

	dialogMainWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	dialogMainWidget->setWidgetResizable(true);
	dialogMainWidget->setWidget(itemsOwner);

	auto itemsLayout = NO_SPACING(NO_MARGIN(new QVBoxLayout(itemsOwner)));

	itemsLayout->addStretch(1);

	#define LEFT_MARGIN		15
	auto updateDialogGeometry = [=]() {
		auto mwGeom = mw->geometry();
		int xPos = ui.notificationDependencies.owner->width() - dialog->width() - LEFT_MARGIN;
		int yPos = mw->menuWidget()->height() + ui.notificationDependencies.mainTabBar->height() + 1;

		dialog->setGeometry(mwGeom.x() + xPos, mwGeom.y() + yPos, dialog->width(), dialog->height());
	};
	handler = updateDialogGeometry;

	dialog->installEventFilter(new UniversalEventFilter(dialog, [=](QObject*, QEvent *e) -> bool {
		if (e->type() == QEvent::Resize) {
			updateDialogGeometry();
			
			return true;
		}
		if (e->type() == QEvent::Hide) {
			for (int i = 0; i < itemsLayout->count(); ++i) {
				auto item = itemsLayout->itemAt(i);
				auto w = item->widget();

				if (!w) {
					continue;
				}

				w->setStyleSheet("border-left: 2px solid white;");
			}
		}
		return false;
	}));
	ui.notificationDependencies.owner->installEventFilter(new UniversalEventFilter(dialog, [=](QObject*, QEvent *e) -> bool {
		if (e->type() == QEvent::Resize) {
			updateDialogGeometry();

			return true;
		}
		return false;
	}));
	ui.notificationDependencies.mainTabBar->installEventFilter(new UniversalEventFilter(dialog, [=](QObject*, QEvent *e) -> bool {
		if (e->type() == QEvent::Resize) {
			updateDialogGeometry();

			return true;
		}
		return false;
	}));

	connections << CONNECT(GetLogSignalEmitter(), &LogSignalEmitter::SendLogExtended, [=](const QString &header, const QString &text) {
		auto item = OBJ_NAME(new QFrame, "notification-dialog-item");
		OBJ_PROP(item, "new-notification", "true");
		item->setAttribute(Qt::WA_Hover);

		auto itemLay = NO_SPACING(NO_MARGIN(new QGridLayout(item)));

		QPushButton *deleteItem;
		itemLay->addWidget(OBJ_NAME(new QLabel(header), "notification-dialog-item-header"), 1, 0);
		itemLay->addWidget(OBJ_NAME(new QLabel(text), "notification-dialog-item-text"), 2, 0, 1, 2);
		itemLay->addWidget(deleteItem = OBJ_NAME(PBT(""), "notification-dialog-item-delete-pbt"), 0, 1, 2, 1);
		itemLay->setRowStretch(2, 1);

		CONNECT(deleteItem, &QPushButton::clicked, item, &QObject::deleteLater);

		deleteItem->hide();
		item->installEventFilter(new UniversalEventFilter(item, [=](QObject *obj, QEvent *e) -> bool {
			if (e->type() == QEvent::HoverEnter) {
				deleteItem->show();
				return true;
			}
			if (e->type() == QEvent::HoverLeave) {
				deleteItem->hide();
				return true;
			}

			return false;
		}));

		itemsLayout->insertWidget(0, item);
	});

	if (needToExec) {
		updateDialogGeometry();
		dialog->show();
	}
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
	lay->setColumnStretch(0, 1);
	lay->setColumnStretch(1, 1);

	return w;
}
QWidget* MainWindowUI::GetSearchHardwareWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}
	
	w = WDG();
	QGridLayout *lay = NO_SPACING(NO_MARGIN(new QGridLayout(w)));
	
	QGridLayout *buttonLay = NO_SPACING(NO_MARGIN(new QGridLayout));

	buttonLay->setColumnStretch(0, 1);
	buttonLay->setColumnStretch(3, 1);

	lay->addLayout(buttonLay, 3, 0, 1, 2);

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
	
	connections << CONNECT(GetLogSignalEmitter(), &LogSignalEmitter::SendLog, log, &QTextEdit::append);
	
	return w;
}
QWidget* MainWindowUI::GetPlotWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = WDG();

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
	w->setMinimumHeight(50);
	QHBoxLayout *buttonLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(w)));

	return w;
}
QWidget* MainWindowUI::CreateBuildExpHolderWidget(const QUuid &id) {
	QWidget *w = 0;

	w = WDG();
	auto lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	MyScrollArea *buildExpHolder;
	QSpinBox *mult;
	lay->addWidget(buildExpHolder = OBJ_NAME(new MyScrollArea, "exp-builder-scroll-area"));
	lay->addWidget(mult = OBJ_NAME(new QSpinBox(), "exp-builder-global-multiplier"));

	mult->setMinimum(1);
	mult->setMaximum(99999);
	mult->setValue(1);
	mult->setSuffix("x");
	mult->setToolTip("Number of the experiment repeats");

	auto buildExpHolderOwner = new BuilderWidget(mw);

	builderTabs.builders[id].builder = buildExpHolderOwner;
	builderTabs.builders[id].globalMult = mult;

	buildExpHolder->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	buildExpHolder->setWidgetResizable(true);
	buildExpHolder->setWidget(buildExpHolderOwner);

	connections << CONNECT(mult, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		buildExpHolderOwner, &BuilderWidget::SetTotalRepeats);

	connections << CONNECT(buildExpHolderOwner, &BuilderWidget::EnsureVisible, 
		buildExpHolder, &MyScrollArea::EnsureVisible);

	return w;
}
QWidget* MainWindowUI::CreateElementsListWidget() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}
	w = OBJ_PROP(OBJ_NAME(WDG(), "node-list-owner"), "widget-type", "left-grey");
	auto nodeListOwnerLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	QLabel *commentLabel;

	auto elementsListHolder = OBJ_NAME(new QFrame(), "elements-list-holder");
	auto elementsListHolderLay = new QGridLayout(elementsListHolder);
	elementsListHolderLay->addWidget(commentLabel = OBJ_NAME(LBL("Drag and drop elements on right window"), "elements-list-descr-label"), 0, 0, 1, 2);
	elementsListHolderLay->setSpacing(15);

	QScrollArea *elementsListArea = OBJ_NAME(new QScrollArea(), "node-list-scroll-area");
	elementsListArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	elementsListArea->setWidgetResizable(true);
	elementsListArea->setWidget(elementsListHolder);

	auto searchLay = NO_SPACING(NO_MARGIN(new QHBoxLayout));
	QLabel *searchLabel;
	QPushButton *searchClearPbt;
	QLineEdit *searchExpLed;
	searchLay->addWidget(searchLabel = OBJ_NAME(LBL(""), "search-experiments-label"));
	searchLay->addWidget(searchExpLed = OBJ_NAME(LED(), "search-experiments"));
	searchLay->addWidget(searchClearPbt = OBJ_NAME(PBT(""), "search-experiments-clear"));
	searchLay->addWidget(OBJ_NAME(WDG(), "search-experiments-spacing"));
	searchLabel->setPixmap(QPixmap(":/GUI/Resources/search-icon.png"));
	searchClearPbt->setIcon(QIcon(":/GUI/Resources/search-clear-button.png"));
	searchClearPbt->setIconSize(QPixmap(":/GUI/Resources/search-clear-button.png").size());
	searchClearPbt->hide();

	connections << CONNECT(searchExpLed, &QLineEdit::textChanged, [=](const QString &text) {
		if (text.isEmpty()) {
			searchClearPbt->hide();
		}
		else {
			searchClearPbt->show();
		}
	});

	connections << CONNECT(searchClearPbt, &QPushButton::clicked, [=]() {
		searchExpLed->clear();
	});

	auto selectCategoryLay = NO_SPACING(NO_MARGIN(new QHBoxLayout));
	auto selectCategory = OBJ_NAME(CMB(), "select-category");
	selectCategory->setView(OBJ_NAME(new QListView, "combo-list"));

	selectCategoryLay->addWidget(OBJ_NAME(WDG(), "search-experiments-spacing"));
	selectCategoryLay->addWidget(selectCategory);
	selectCategoryLay->addWidget(OBJ_NAME(WDG(), "search-experiments-spacing"));
	
	nodeListOwnerLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Categories"), "heading-label"), "widget-type", "left-grey"));
	nodeListOwnerLay->addLayout(selectCategoryLay);
	nodeListOwnerLay->addLayout(searchLay);
	nodeListOwnerLay->addWidget(elementsListArea);
	
	auto ListFilter = [=]() {
		for (int i = 0; i < elementsListHolderLay->count();) {
			auto item = elementsListHolderLay->itemAt(i);
			auto w = item->widget();
			if (!w) {
				++i;
				continue;
			}

			if (w == commentLabel) {
				++i;
				continue;
			}

			elementsListHolderLay->removeItem(item);
			w->deleteLater();
			delete item;
		}

		QList<AbstractBuilderElement*> showElements;
		QString category = selectCategory->currentText();
		QString search = searchExpLed->text();

		foreach(auto elem, elementsPtrMap.values()) {
			if (category != EXPERIMENT_VIEW_ALL_CATEGORY) {
				if (!elem->GetCategory().contains(category)) {
					continue;
				}
			}

			if (!search.isEmpty()) {
				if (!elem->GetFullName().contains(search)) {
					continue;
				}
			}

			showElements << elem;
		}

		ui.buildExperiment.listItemOverlay = 0;
		ui.buildExperiment.listItemHolder = elementsListHolder;
		int i = 0;
		foreach(auto elem, showElements) {
			auto label = OBJ_NAME(new QLabel, "element-builder");
			label->setPixmap(elem->GetImage());

			//label->installEventFilter(new ElementListEventFiler(label, elem));
			label->installEventFilter(new UniversalEventFilter(label, [=](QObject *obj, QEvent *e) {
				auto &_elem(elem);

				if (e->type() == QEvent::Enter) {
					if (ui.buildExperiment.listItemOverlay) {
						ui.buildExperiment.listItemOverlay->deleteLater();
						ui.buildExperiment.listItemOverlay = 0;
						ui.buildExperiment.listItemHolder->update();
					}

					QWidget *w = qobject_cast<QWidget*>(obj);

					if (!w) {
						return false;
					}

					auto marg = w->contentsMargins();

					auto additionalHeight = (w->height() - marg.bottom() - marg.top()) * 0.25;

					QPoint topLeft = QPoint(marg.left() - 1, marg.top() - 3);

					QPoint bottomRight = QPoint(w->width(), w->height() + additionalHeight);
					bottomRight -= QPoint(marg.right(), marg.bottom());

					auto parentWdg = w->parentWidget();
					auto overlay = OBJ_NAME(new QFrame(w->parentWidget()), "hover-element-overlay");
					ui.buildExperiment.listItemOverlay = overlay;
					overlay->setGeometry(QRect(topLeft + w->pos(), bottomRight + w->pos()));
					//overlay->installEventFilter(new OverlayEventFilter(overlay, _elem));
					
					static bool dragInAction;
					dragInAction = false;
					overlay->installEventFilter(new UniversalEventFilter(overlay, [=](QObject *obj, QEvent *e) {
						if (e->type() == QEvent::MouseButtonPress) {
							QWidget *w = qobject_cast<QWidget*>(obj);
							if (!w) {
								return false;
							}

							QMouseEvent *me = (QMouseEvent*)e;

							if (me->button() == Qt::LeftButton) {
								auto margins = w->contentsMargins();
								auto rect = w->rect();

								QPoint startPoint;
								startPoint.setX(margins.left() - 1);
								startPoint.setY(margins.top() - 1);
								QPoint endPoint;
								endPoint.setX(rect.width() - margins.right());
								endPoint.setY(rect.height() - margins.bottom());

								ElementMimeData emd;
								emd.elem = _elem;

								auto mime = new QMimeData;
								mime->setData(ELEMENT_MIME_TYPE, QByteArray((char*)&emd, sizeof(ElementMimeData)));

								auto pixmap = w->grab(QRect(startPoint, endPoint));
								pixmap = pixmap.scaledToHeight((endPoint.y() - startPoint.y()) / 2, Qt::SmoothTransformation);

								QDrag *drag = new QDrag(obj);
								drag->setMimeData(mime);
								drag->setPixmap(pixmap);
								drag->setHotSpot((me->pos() - QPoint(margins.left(), margins.top())) / 2);

								dragInAction = true;
								Qt::DropAction dropAction = drag->exec(Qt::CopyAction, Qt::CopyAction);
								dragInAction = false;
								if (ui.buildExperiment.listItemOverlay) {
									ui.buildExperiment.listItemOverlay->deleteLater();
									ui.buildExperiment.listItemOverlay = 0;
									ui.buildExperiment.listItemHolder->update();
								}
								return true;
							}

							return false;
						}
						if (e->type() == QEvent::Enter) {
							return true;
						}
						if (e->type() == QEvent::Leave) {
							if (dragInAction) {
								return false;
							}
							if (ui.buildExperiment.listItemOverlay) {
								auto objToDelete = ui.buildExperiment.listItemOverlay;
								ui.buildExperiment.listItemOverlay = 0;
								QTimer::singleShot(50, [=]() {
									objToDelete->deleteLater();
									ui.buildExperiment.listItemHolder->update();
								});
							}
							return true;
						}
						return false;
					}));

					auto effect = new QGraphicsDropShadowEffect(overlay);
					effect->setOffset(3, 3);
					effect->setColor(QColor("#7f7f7f7f"));
					effect->setBlurRadius(5);
					overlay->setGraphicsEffect(effect);

					QLabel *iconLbl;
					QLabel *textLbl;

					auto lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(overlay)));
					lay->addWidget(iconLbl = OBJ_NAME(new QLabel(), "hover-element-overlay-icon"));
					lay->addWidget(textLbl = OBJ_NAME(new QLabel(_elem->GetFullName()), "hover-element-overlay-name"));

					textLbl->setWordWrap(true);
					iconLbl->setPixmap(_elem->GetImage());

					overlay->show();
					overlay->raise();

					return true;
				}
				if (e->type() == QEvent::Leave) {
					return false;
				}
				return false;
			}));

			elementsListHolderLay->addWidget(label, 1 + i / 2, i % 2);
			++i;
		}
		elementsListHolderLay->setRowStretch(1 + i / 2 + i % 2, 1);
	};

	connections << CONNECT(selectCategory, &QComboBox::currentTextChanged, ListFilter);
	connections << CONNECT(searchExpLed, &QLineEdit::textChanged, ListFilter);
	connections << CONNECT(mw, &MainWindow::BuilderElementsFound, [=](const QList<AbstractBuilderElement*> &elements) {
		elementsPtrMap.clear();
		for (auto it = elements.begin(); it != elements.end(); ++it) {
			elementsPtrMap[(*it)->GetFullName()] = *it;
		}

		QStringList categoryStrList;
		//int i = 0;
		foreach(auto elem, elementsPtrMap.values()) {
			categoryStrList << elem->GetCategory();

			//auto label = OBJ_NAME(new QLabel, "element-builder");
			//label->setPixmap(elem->GetImage());

			//label->installEventFilter(new ElementListEventFiler(label, elem));

			//elementsListHolderLay->addWidget(label, 1 + i / 2, i % 2);
			//++i;
		}
		//elementsListHolderLay->setRowStretch(1 + i / 2 + i % 2, 1);

		categoryStrList << EXPERIMENT_VIEW_ALL_CATEGORY;
		categoryStrList.removeDuplicates();

		foreach(auto str, categoryStrList) {
			selectCategory->addItem(str);
		}
		selectCategory->setCurrentIndex(selectCategory->count() - 1);
	});

	return w;
}
QString MainWindowUI::GetCustomExperimentName(QWidget *parent, const QString &name) {
	static bool dialogCanceled;
	dialogCanceled = true;
	
	QDialog* dialog = OBJ_NAME(new QDialog(parent, Qt::SplashScreen), "custom-exp-name-dialog");
	QList<QMetaObject::Connection> dialogConn;
	auto globalLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(dialog)));

	auto lay = new QVBoxLayout();

	lay->addWidget(OBJ_NAME(LBL("Save Experiment"), "heading-label"));

	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));
	globalLay->addLayout(lay);
	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));

	QLineEdit *titleLed;

	auto paramsLay = new QHBoxLayout;
	paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Experiment Name: "), "experiment-params-comment"), "comment-placement", "left"));
	paramsLay->addWidget(titleLed = new QLineEdit(name));
	titleLed->setPlaceholderText("Enter new name here");


	auto buttonLay = new QHBoxLayout;
	QPushButton *okBut;
	QPushButton *cancelBut;
	buttonLay->addStretch(1);
	buttonLay->addWidget(okBut = OBJ_NAME(PBT("Save"), "secondary-button"));
	buttonLay->addWidget(cancelBut = OBJ_NAME(PBT("Cancel"), "secondary-button"));
	buttonLay->addStretch(1);

	lay->addLayout(paramsLay);
	lay->addSpacing(40);
	lay->addLayout(buttonLay);
	lay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-vertical-spacing"));

	dialogConn << CONNECT(okBut, &QPushButton::clicked, [=]() {
		dialogCanceled = false;
	});

	CONNECT(okBut, &QPushButton::clicked, dialog, &QDialog::accept);
	CONNECT(cancelBut, &QPushButton::clicked, dialog, &QDialog::reject);

	dialog->exec();

	QString ret;
	if (!dialogCanceled) {
		ret = titleLed->text();
	}

	foreach(auto conn, dialogConn) {
		QObject::disconnect(conn);
	}

	dialog->deleteLater();

	return ret;
}
bool MainWindowUI::GetUserAgreement(QWidget *parent, const QString &title, const QString &text, const QString &okText, const QString &cancelText) {
	static bool dialogCanceled;
	dialogCanceled = true;

	QDialog* dialog = OBJ_NAME(new QDialog(parent, Qt::SplashScreen), "custom-exp-agreement-dialog");
	QList<QMetaObject::Connection> dialogConn;
	auto globalLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(dialog)));

	auto lay = new QVBoxLayout();

	lay->addWidget(OBJ_NAME(new QLabel(title), "heading-label"));
	//lay->addWidget(OBJ_NAME(LBL("Open Experiment"), "heading-label"));

	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));
	globalLay->addLayout(lay);
	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));

	auto paramsLay = new QHBoxLayout;
	//paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Make sure that you <b>saved</b> the experiment.<br>All unsaved data will be <b>lost</b>."), "experiment-params-comment"), "comment-placement", "right"));
	QLabel *textLabel;
	paramsLay->addWidget(textLabel = OBJ_PROP(OBJ_NAME(new QLabel(text), "experiment-params-comment"), "comment-placement", "right"));
	textLabel->setWordWrap(true);

	auto buttonLay = new QHBoxLayout;
	QPushButton *okBut;
	QPushButton *cancelBut;
	buttonLay->addStretch(1);
	buttonLay->addWidget(okBut = OBJ_NAME(new QPushButton(okText), "secondary-button"));
	buttonLay->addWidget(cancelBut = OBJ_NAME(new QPushButton(cancelText), "secondary-button"));
	buttonLay->addStretch(1);

	lay->addLayout(paramsLay);
	lay->addSpacing(40);
	lay->addLayout(buttonLay);
	lay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-vertical-spacing"));

	dialogConn << CONNECT(okBut, &QPushButton::clicked, [=]() {
		dialogCanceled = false;
	});

	CONNECT(okBut, &QPushButton::clicked, dialog, &QDialog::accept);
	CONNECT(cancelBut, &QPushButton::clicked, dialog, &QDialog::reject);

	dialog->exec();

	foreach(auto conn, dialogConn) {
		QObject::disconnect(conn);
	}

	dialog->deleteLater();

	return !dialogCanceled;
}
bool MainWindowUI::GetOpenCustomExperiment(QWidget *parent, CustomExperiment &cExp) {
	static bool dialogCanceled;
	dialogCanceled = true;

	QList<CustomExperiment> cExpList;
	{
		QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
		auto expFileInfos = QDir(appDataPath +  "/" CUSTOM_EXP_DIR).entryInfoList(QStringList() << "*.json", QDir::Files | QDir::Readable);
		foreach(const QFileInfo &expFileInfo, expFileInfos) {
			auto filePath = expFileInfo.absoluteFilePath();

			QFile file(filePath);

			if (!file.open(QIODevice::ReadOnly)) {
				LOG() << "Can not read JSON file.";
				continue;
			}

			QByteArray data = file.readAll();
			file.close();

			try {
				auto ce = ExperimentReader::GenerateExperimentContainer(data);
				ce.fileName = expFileInfo.fileName();
				cExpList << ce;
			}
			catch (const QString &err) {
				LOG() << "Error in the file" << expFileInfo.fileName() << "-" << err;
				continue;
			}
		}
	}

	if (!cExpList.count()) {
		auto title = "Open Experiment";
		auto text = "No custom experiments were created.";
		auto okText = "Ok";
		auto cancelText = "Cancel";

		GetUserAgreement(parent, title, text, okText, cancelText);
		
		return false;
	}
	else {
		auto title = "Open Experiment";
		auto text = "Make sure that you <b>saved</b> the experiment.<br>All unsaved data will be <b>lost</b>.";
		auto okText = "Continue";
		auto cancelText = "Cancel";

		if (!GetUserAgreement(parent, title, text, okText, cancelText)) {
			return false;
		}
	}

	QDialog* dialog = OBJ_NAME(new QDialog(parent, Qt::SplashScreen), "custom-exp-name-dialog");
	QList<QMetaObject::Connection> dialogConn;
	auto globalLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(dialog)));

	auto lay = new QVBoxLayout();
	
	QListView *fileList;
	
	lay->addWidget(OBJ_NAME(LBL("Open Experiment"), "heading-label"));
	lay->addWidget(fileList = OBJ_NAME(new QListView, "curve-params-data-set-list"));

	fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QStandardItemModel *model = new QStandardItemModel(cExpList.size(), 1);
	int row = 0;
	foreach(auto &ce, cExpList) {
		auto *item = new QStandardItem(ce.name);
		item->setData(ce.id, Qt::UserRole);
		model->setItem(row++, item);
	}
	fileList->setModel(model);
    fileList->setCurrentIndex(fileList->model()->index(0, 0));
	
	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));
	globalLay->addLayout(lay);
	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));

	auto buttonLay = NO_SPACING(NO_MARGIN(new QHBoxLayout));
	QPushButton *okBut;
	QPushButton *cancelBut;
	QPushButton *deleteBut;
	buttonLay->addWidget(okBut = OBJ_NAME(PBT("Open"), "secondary-button"));
	buttonLay->addWidget(cancelBut = OBJ_NAME(PBT("Cancel"), "secondary-button"));
	buttonLay->addSpacing(20);
	buttonLay->addWidget(deleteBut = OBJ_NAME(PBT("Delete"), "secondary-button"));

	lay->addSpacing(40);
	lay->addLayout(buttonLay);
	lay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-vertical-spacing"));

	dialogConn << CONNECT(okBut, &QPushButton::clicked, [=]() {
		dialogCanceled = false;
	});

	CONNECT(okBut, &QPushButton::clicked, dialog, &QDialog::accept);
	CONNECT(cancelBut, &QPushButton::clicked, dialog, &QDialog::reject);

	dialogConn << CONNECT(deleteBut, &QPushButton::clicked, [=]() {
		auto index = fileList->selectionModel()->currentIndex();
		if (!index.isValid()) {
			return;
		}

		auto title = "Delete Experiment";
		QString text = "You are about to delete the experiment <b>\"" + index.data(Qt::DisplayRole).toString() + "\"</b>.<br>Are you sure?";
		auto okText = "Delete";
		auto cancelText = "Cancel";

		if (GetUserAgreement(parent, title, text, okText, cancelText)) {
			auto curId = index.data(Qt::UserRole).toUuid();
			
			QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
			foreach(auto &ce, cExpList) {
				if (curId == ce.id) {
					fileList->model()->removeRow(index.row());

					if (QFile(appDataPath + "/" CUSTOM_EXP_DIR + ce.fileName).remove()) {
						mw->UpdateCustomExperimentList();
					}
					break;
				}
			}
		}
	});

	dialog->exec();

	if (!dialogCanceled) {
		auto index = fileList->selectionModel()->currentIndex();
		if (index.isValid()) {
			auto curId = index.data(Qt::UserRole).toUuid();

			foreach(auto &ce, cExpList) {
				if (curId == ce.id) {
					cExp = ce;
					break;
				}
			}
		}
		else {
			dialogCanceled = true;
		}
	}

	foreach(auto conn, dialogConn) {
		QObject::disconnect(conn);
	}

	dialog->deleteLater();

	return !dialogCanceled;
}
QWidget* MainWindowUI::GetStatisticsTab() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = WDG();
	auto lay = new QHBoxLayout(w);

	auto view = OBJ_NAME(new QTreeView(w), "status-tab-view");
	lay->addWidget(view);

	auto model = new QStandardItemModel(view);
	view->setModel(model);

	QStringList headerNames = QStringList() << "" << "Status" << "Experiment" << "Step" << "Last notification";

	model->setHorizontalHeaderLabels(headerNames);
	view->header()->setSectionsMovable(false);
	view->header()->setSectionsClickable(false);
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	view->setItemsExpandable(false);
	view->setAlternatingRowColors(true);

	view->installEventFilter(new UniversalEventFilter(view, [=](QObject *obj, QEvent *e) -> bool {
		bool ret = false;

		if (e->type() != QEvent::Resize) {
			return false;
		}

		if (view->header()->count() != headerNames.size()) {
			return false;
		}

		QList<int> columnWeights{3, 2, 4, 2, 4};
		int weightSum = 0;
		for (auto i : columnWeights) {
			weightSum += i;
		}

		auto width = view->width();

		for (int i = 0; i < columnWeights.size(); ++i) {
			view->setColumnWidth(i, (float)width * columnWeights.at(i) / weightSum);
		}

		return true;
	}));


	auto rootItem = model->invisibleRootItem();
	static QMap<QUuid, ExperimentType> activeNodeType;

	auto searchForTabIndex = [=](QwtPlot *plot) -> int {
		int index = -1;

		//auto slay = ui.newDataTab.stackedLay;
		for (int i = 0; i < dataTabs.dataWindowOrder.count(); ++i) {
			auto wdg = dataTabs.dataWindowOrder.at(i)->widget();
			auto curPlot = wdg->findChild<QWidget*>("qwt-plot");
			if (curPlot == plot) {
				index = i;
				break;
			}
		}

		return index;
	};
	auto searchById = [rootItem](const QUuid &id, QStandardItem* &instItem, quint8 &channel) -> bool {
		bool ret = false;

		for (int i = 0; i < rootItem->rowCount(); ++i) {
			auto item = rootItem->child(i);

			for (int j = 0; j < item->rowCount(); ++j) {
				auto chNameItem = item->child(j, NAME_COL);
				auto curId = chNameItem->data(Qt::UserRole).toUuid();
				if (curId == id) {
					instItem = item;
					channel = j;
					ret = true;
					break;
				}
			}
			if (ret) {
				break;
			}
		}

		return ret;
	};
	auto createCachedWindow = [=](const QUuid &id, const QString &name) {
		auto axisStrList = dataTabs.lastData[id].keys();
		axisStrList.removeAll(NONE_Y_AXIS_VARIABLE);

		auto dataTabWidget = CreateNewDataTabWidget(id,
			ET_SAVED,
			name,
			axisStrList,
			axisStrList,
			"",
			&(dataTabs.lastData[id]));

		auto &handler(dataTabs.plots[id][ET_SAVED]);

		dataTabs.dataWindowOrder << ui.newDataTab.mdiArea->addSubWindow(dataTabWidget, SUB_WINDOWS_FLAGS);
		ui.newDataTab.tabBar->insertTab(ui.newDataTab.tabBar->count(), name);
		//ui.newDataTab.stackedLay->insertWidget(ui.newDataTab.tabBar->count() - 1, dataTabWidget);
		ui.newDataTab.mdiArea->setActiveSubWindow(dataTabs.dataWindowOrder.at(ui.newDataTab.tabBar->count() - 1));

		if (ui.newDataTab.tabBar->isHidden()) {
			ui.newDataTab.tabBar->show();
		}

		DataTabPtr tabPtr;
		tabPtr.type = DataTabPtr::DT_REGULAR;
		tabPtr.regular.tabBar = ui.newDataTab.tabBar;
		tabPtr.regular.plot = handler.plot;

		dataTabs.dataTabPtrs[id] = tabPtr;
	};
	auto triggerIndex = [=](const QModelIndex &index) {
		auto id = index.data(Qt::UserRole).toUuid();
		if (id == QUuid()) {
			return;
		}

		if (!dataTabs.dataTabPtrs.contains(id)) {
			if(dataTabs.lastData.contains(id)) {
				auto name = index.parent().child(index.row(), EXPERIMENT_COL).data(Qt::DisplayRole).toString();

				createCachedWindow(id, name);

				dataTabs.lastData.remove(id);
				ui.newDataTab.newDataTabButton->click();
				ui.newDataTab.tabBar->setCurrentIndex(ui.newDataTab.tabBar->count()-1);
			}
			return;
		}
		
		DataTabPtr &tabPtr(dataTabs.dataTabPtrs[id]);

		switch (tabPtr.type) {
			case DataTabPtr::DT_MANUAL:
				ui.manualExperiment.tabButton->click();
				tabPtr.manual.hwTabBar->setCurrentIndex(tabPtr.manual.hwIndex);
				tabPtr.manual.channelButton->click();
				break;

			case DataTabPtr::DT_REGULAR: {
				ui.newDataTab.newDataTabButton->click();
				int index = searchForTabIndex(tabPtr.regular.plot);

				if (-1 != index) {
					tabPtr.regular.tabBar->setCurrentIndex(index);
				}
			} break;
		}
	};

	CONNECT(mw, &MainWindow::RemoveDisconnectedInstruments, [rootItem](const QStringList &instNames) {
		for (int i = 0; i < rootItem->rowCount();) {
			auto curName = rootItem->child(i)->data(Qt::DisplayRole).toString();

			if (instNames.contains(curName)) {
				rootItem->removeRow(i);
			}
			else {
				++i;
			}
		}
	});
	CONNECT(mw, &MainWindow::AddNewInstruments, [rootItem, view, triggerIndex](const QList<HardwareUiDescription> &hwList) {
		for (auto &hw : hwList) {
			auto instItem = new QStandardItem(hw.name);
			rootItem->setChild(rootItem->rowCount(), 0, instItem);

			for (int i = 0; i < hw.channelAmount; ++i) {
				auto item = new QStandardItem();
				item->setData(QUuid(), Qt::UserRole);
				instItem->setChild(i, NAME_COL, item);

				auto wdg = new HwListButton(instItem->index().child(i, NAME_COL), QString("Channel %1").arg(i + 1));
				CONNECT(wdg, &HwListButton::Clicked, triggerIndex);
				view->setIndexWidget(instItem->index().child(i, NAME_COL), wdg);

				item = new QStandardItem(STOPPED_STATUS);
				instItem->setChild(i, STATUS_COL, item);

				instItem->setChild(i, EXPERIMENT_COL, new QStandardItem(""));
				instItem->setChild(i, STEP_COL, new QStandardItem(""));
				instItem->setChild(i, LAST_NOTIF_COL, new QStandardItem(""));
			}
		}

		view->expandAll();
	});

	CONNECT(view, &QTreeView::doubleClicked, triggerIndex);

	CONNECT(mw, &MainWindow::ExperimentCompleted, [view, searchById](const QUuid &id) {
		QStandardItem *instItem = 0;
		quint8 channel = 0;
		
		if (!searchById(id, instItem, channel)) {
			return;
		}

		QStandardItem *chItem;
		//chItem = instItem->child(channel, NAME_COL);
		//chItem->setData(QUuid(), Qt::UserRole);
		
		if (auto w = view->indexWidget(instItem->index().child(channel, NAME_COL))) {
			auto hw = qobject_cast<HwListButton*>(w);
			if (hw) {
				hw->SetColorState(HwListButton::STOPPED);
			}
		}
		
		chItem = instItem->child(channel, STATUS_COL);
		chItem->setText(STOPPED_STATUS);

		//chItem = instItem->child(channel, EXPERIMENT_COL);
		//chItem->setText("");
		
		//chItem = instItem->child(channel, STEP_COL);
		//chItem->setText("");
	});
	CONNECT(mw, &MainWindow::ExperimentResumed, [view, searchById](const QUuid &id) {
		QStandardItem *instItem = 0;
		quint8 channel = 0;

		if (!searchById(id, instItem, channel)) {
			return;
		}

		auto chItem = instItem->child(channel, NAME_COL);
		if (auto w = view->indexWidget(instItem->index().child(channel, NAME_COL))) {
			auto hw = qobject_cast<HwListButton*>(w);
			if (hw) {
				hw->SetColorState(HwListButton::ACTIVE);
			}
		}
		
		chItem = instItem->child(channel, STATUS_COL);
		chItem->setText(ACTIVE_STATUS);
	});
	CONNECT(mw, &MainWindow::ExperimentPaused, [view, searchById](const QUuid &id) {
		QStandardItem *instItem = 0;
		quint8 channel = 0;

		if (!searchById(id, instItem, channel)) {
			return;
		}

		auto chItem = instItem->child(channel, NAME_COL);
		if (auto w = view->indexWidget(instItem->index().child(channel, NAME_COL))) {
			auto hw = qobject_cast<HwListButton*>(w);
			if (hw) {
				hw->SetColorState(HwListButton::MIDDLE);
			}
		}
		
		chItem = instItem->child(channel, STATUS_COL);
		chItem->setText(PAUSED_STATUS);
	});
	auto expStartedHandler = [=](const QUuid &id, const QString &name, quint8 channel) {
		for (int i = 0; i < rootItem->rowCount(); ++i) {
			auto instItem = rootItem->child(i);
			auto curName = instItem->data(Qt::DisplayRole).toString();

			if (curName != name) {
				continue;
			}

			if (!dataTabs.plots.contains(id)) {
				return;
			}

			activeNodeType[id] = dataTabs.plots[id].firstKey();

			auto chItem = instItem->child(channel, NAME_COL);
			chItem->setData(id, Qt::UserRole);
			if (auto w = view->indexWidget(instItem->index().child(channel, NAME_COL))) {
				auto hw = qobject_cast<HwListButton*>(w);
				if (hw) {
					hw->SetColorState(HwListButton::ACTIVE);
				}
			}

			chItem = instItem->child(channel, STATUS_COL);
			chItem->setText(ACTIVE_STATUS);

			if (dataTabs.plots[id].contains(activeNodeType[id])) {
				chItem = instItem->child(channel, EXPERIMENT_COL);
				auto filePath = dataTabs.plots[id][activeNodeType[id]].data.first().filePath;
				chItem->setText(QFileInfo(filePath).fileName());
			}
		}
	};
	CONNECT(mw, &MainWindow::ExperimentStarted, expStartedHandler);
	#define NODE_TYPE_STR_FILL(a, b) case a: nodeTypeStr = b; break;

	CONNECT(mw, &MainWindow::ExperimentNodeBeginning, [=](const QUuid &id, const ExperimentNode_t &node) {
		if (!dataTabs.plots.contains(id)) {
			return;
		}

		switch (node.nodeType) {
			case FRA_NODE_POT:
			case FRA_NODE_GALV:
			case FRA_NODE_PSEUDOGALV:
				activeNodeType[id] = ET_AC;
				break;

			case DCNODE_OCP:
			case DCNODE_SWEEP_POT:
			case DCNODE_SWEEP_GALV:
			case DCNODE_POINT_POT:
			case DCNODE_POINT_GALV:
			case DCNODE_NORMALPULSE_POT:
			case DCNODE_NORMALPULSE_GALV:
			case DCNODE_DIFFPULSE_POT:
			case DCNODE_DIFFPULSE_GALV:
			case DCNODE_SQRWAVE_POT:
			case DCNODE_SQRWAVE_GALV:
			case DCNODE_SINEWAVE:
			case DCNODE_CONST_RESISTANCE:
			case DCNODE_CONST_POWER:
			case DCNODE_MAX_POWER:
				activeNodeType[id] = ET_DC;
				break;

			default:
				return;
				break;
		}

		QString nodeTypeStr = "";
		switch (node.nodeType) {
			NODE_TYPE_STR_FILL(END_EXPERIMENT_NODE, "Experiment complete")
			NODE_TYPE_STR_FILL(DCNODE_OCP, "Open circuit")
			NODE_TYPE_STR_FILL(DCNODE_SWEEP_POT, "Potential sweep")
			NODE_TYPE_STR_FILL(DCNODE_SWEEP_GALV, "Current sweep")
			NODE_TYPE_STR_FILL(DCNODE_POINT_POT, "Fixed potential")
			NODE_TYPE_STR_FILL(DCNODE_POINT_GALV, "Fixed current")
			NODE_TYPE_STR_FILL(DCNODE_NORMALPULSE_POT, "Potential pulse (normal)")
			NODE_TYPE_STR_FILL(DCNODE_NORMALPULSE_GALV, "Current pulse (normal)")
			NODE_TYPE_STR_FILL(DCNODE_DIFFPULSE_POT, "Potential pulse (differential)")
			NODE_TYPE_STR_FILL(DCNODE_DIFFPULSE_GALV, "Current pulse (differential)")
			NODE_TYPE_STR_FILL(DCNODE_SQRWAVE_POT, "Potential square wave")
			NODE_TYPE_STR_FILL(DCNODE_SQRWAVE_GALV, "Current square wave")
			NODE_TYPE_STR_FILL(DCNODE_SINEWAVE, "Potential sine wave")
			NODE_TYPE_STR_FILL(DCNODE_CONST_RESISTANCE, "Constant resistance")
			NODE_TYPE_STR_FILL(DCNODE_CONST_POWER, "Constant power")
			NODE_TYPE_STR_FILL(DCNODE_MAX_POWER, "Maximum power point")
			NODE_TYPE_STR_FILL(FRA_NODE_POT, "Potentiostatic impedance")
			NODE_TYPE_STR_FILL(FRA_NODE_GALV, "Galvanostatic impedance")
			NODE_TYPE_STR_FILL(FRA_NODE_PSEUDOGALV, "Pseudo-galvanostatic impedance")
			NODE_TYPE_STR_FILL(DUMMY_NODE, " ")

			default: break;
		}
		
		QStandardItem *instItem = 0;
		quint8 channel = 0;

		if (!searchById(id, instItem, channel)) {
			return;
		}

		if (dataTabs.plots[id].contains(activeNodeType[id])) {
			auto chItem = instItem->child(channel, EXPERIMENT_COL);
			auto filePath = dataTabs.plots[id][activeNodeType[id]].data.first().filePath;
			chItem->setText(QFileInfo(filePath).fileName());
		}

		auto chItem = instItem->child(channel, STEP_COL);
		chItem->setText(nodeTypeStr);
	});
	CONNECT(mw, &MainWindow::ExperimentNotification, [searchById](const QUuid &id, const QString &msg) {
		QStandardItem *instItem = 0;
		quint8 channel = 0;

		if (!searchById(id, instItem, channel)) {
			return;
		}

		auto chItem = instItem->child(channel, LAST_NOTIF_COL);
		chItem->setText(msg);
	});
	CONNECT(mw, &MainWindow::ExperimentError, [view, searchById](const QUuid &id) {
		QStandardItem *instItem = 0;
		quint8 channel = 0;

		if (!searchById(id, instItem, channel)) {
			return;
		}

		auto chItem = instItem->child(channel, NAME_COL);
		if (auto w = view->indexWidget(instItem->index().child(channel, NAME_COL))) {
			auto hw = qobject_cast<HwListButton*>(w);
			if (hw) {
				hw->SetColorState(HwListButton::MIDDLE);
			}
		}

		chItem = instItem->child(channel, STATUS_COL);
		chItem->setText(ERROR_STATUS);
	});
	
	view->expandAll();

	return  w;
}
QWidget* MainWindowUI::CreateBuildExperimentTabWidget(const QUuid &id) {
	QWidget *w = 0;
	
	QPushButton *deletePbt;
	QPushButton *duplicatePbt;
	QPushButton *openPbt;
	QPushButton *savePbt;
	QPushButton *newPbt;

	auto topButtonOwner = OBJ_NAME(new QFrame, "builder-top-button-owner");
	auto topButtonOwnerLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(topButtonOwner)));
	//topButtonOwnerLay->addWidget(OBJ_NAME(WDG(), "exp-builder-top-buttons-replacement"));
	topButtonOwnerLay->addStretch(1);
	topButtonOwnerLay->addWidget(newPbt = OBJ_NAME(PBT("New"), "builder-new-file-button"));
	topButtonOwnerLay->addWidget(openPbt = OBJ_NAME(PBT("Load"), "builder-open-file-button"));
	topButtonOwnerLay->addWidget(savePbt = OBJ_NAME(PBT("Save"), "builder-save-file-button"));
	topButtonOwnerLay->addWidget(duplicatePbt = OBJ_NAME(PBT("Duplicate"), "builder-duplicate-button"));
	topButtonOwnerLay->addWidget(deletePbt = OBJ_NAME(PBT("Delete"), "builder-delete-button"));
	//topButtonOwnerLay->addWidget(OBJ_NAME(PBT("Select All"), "builder-select-all-button"));
	topButtonOwnerLay->addStretch(1);

	auto *expBuilderOwner = OBJ_NAME(WDG(), "experiment-builder-owner");
	auto expBuilderOwnerLay = NO_SPACING(NO_MARGIN(new QGridLayout(expBuilderOwner)));

	expBuilderOwnerLay->addWidget(topButtonOwner, 0, 0, 1, 3);
	expBuilderOwnerLay->addWidget(OBJ_NAME(WDG(), "exp-builder-right-buttons-replacement"), 1, 2, 2, 1);
	expBuilderOwnerLay->addWidget(OBJ_NAME(WDG(), "exp-builder-left-spacer"), 1, 0, 2, 1);
	expBuilderOwnerLay->addWidget(OBJ_NAME(WDG(), "exp-builder-bottom-spacer"), 3, 0, 1, 3);
	expBuilderOwnerLay->addWidget(CreateBuildExpHolderWidget(id), 1, 1);

	builderTabs.builders[id].connections <<
	CONNECT(newPbt, &QPushButton::clicked, ui.buildExperiment.addNewTabButton, &QPushButton::click);

	builderTabs.builders[id].connections <<
	CONNECT(openPbt, &QPushButton::clicked, [=]() {
		CustomExperiment newCe;

		if (!GetOpenCustomExperiment(mw, newCe)) {
			return;
		}

		builderTabs.builders[id].fileName = newCe.fileName;
		builderTabs.builders[id].name = newCe.name;
		builderTabs.builders[id].globalMult->setValue(newCe.bc.repeats);
		
		builderTabs.tabBar->setTabText(builderTabs.tabBar->currentIndex(), newCe.name);

		MainWindow::FillElementPointers(newCe.bc, elementsPtrMap);

		builderTabs.builders[id].builder->SetupNewContainer(newCe.bc);
	});

	builderTabs.builders[id].connections <<
	CONNECT(savePbt, &QPushButton::clicked, [=]() {
		auto &container(builderTabs.builders[id].builder->GetContainer());

		if (!container.elements.count()) {
			return;
		}

		QString name = GetCustomExperimentName(mw, builderTabs.builders[id].name);

		if (name.isEmpty()) {
			return;
		}

		builderTabs.builders[id].name = name;

		builderTabs.tabBar->setTabText(builderTabs.tabBar->currentIndex(), name);

		//if (builderTabs.builders[id].fileName.isEmpty()) {
			builderTabs.builders[id].fileName = name + ".json";
		//}

		mw->SaveCustomExperiment(name, container, builderTabs.builders[id].fileName);
	});

	builderTabs.builders[id].connections <<
	CONNECT(builderTabs.builders[id].builder, &BuilderWidget::BuilderContainerSelected, [=](BuilderContainer *bc) {
		if (builderTabs.userInputs) {
			builderTabs.paramsLay->removeWidget(builderTabs.userInputs);
			builderTabs.userInputs->deleteLater();
			builderTabs.userInputs = 0;
		}

		if (bc && (bc->type == BuilderContainer::ELEMENT)) {
			auto elem = bc->elem.ptr;

			builderTabs.userInputs = elem->CreateUserInput(bc->elem.input);
			builderTabs.paramsLay->addWidget(builderTabs.userInputs);

			builderTabs.paramsHeadWidget->show();
			auto lbl = builderTabs.paramsHeadWidget->findChild<QLabel*>("heading-label");
			lbl->setText(bc->elem.name + "<br>Parameters");
			lbl->setWordWrap(true);
		}
		else {
			builderTabs.paramsHeadWidget->hide();
		}
	});

	CONNECT(deletePbt, &QPushButton::clicked, builderTabs.builders[id].builder, &BuilderWidget::DeleteSelected);
	CONNECT(duplicatePbt, &QPushButton::clicked, builderTabs.builders[id].builder, &BuilderWidget::DuplicateSelected);

	w = expBuilderOwner;

	return w;
}
QWidget* MainWindowUI::GetBuildExperimentTab() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = OBJ_NAME(WDG(), "build-experiment-owner");
	auto *tabBarLayout = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	QHBoxLayout *lay = NO_SPACING(NO_MARGIN(new QHBoxLayout));

	QPushButton *addNewButton;
	QTabBar *tabBar;

	auto tabHeaderLay = NO_SPACING(NO_MARGIN(new QHBoxLayout()));
	
	QFrame *tabFrame = OBJ_NAME(new QFrame, "builder-tab-frame");
	auto *tabFrameLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(tabFrame)));
	tabFrameLay->addWidget(builderTabs.tabBar = tabBar = OBJ_NAME(new QTabBar, "builder-tab"));
	
	tabHeaderLay->addWidget(tabFrame);
	tabHeaderLay->addWidget(addNewButton = OBJ_NAME(PBT("+"), "builder-tab-add-new"));
	tabHeaderLay->addStretch(1);
	ui.buildExperiment.addNewTabButton = addNewButton;

	tabBarLayout->addLayout(tabHeaderLay);
	tabBarLayout->addLayout(lay);

	tabBar->setExpanding(false);
	tabBar->setMovable(true);
	//tabBar->addTab(QIcon(":/GUI/Resources/new-tab.png"), "");

	auto nodeListOwner = CreateElementsListWidget();

	auto *nodeParamsOwner = OBJ_NAME(WDG(), "node-params-owner");
	auto *nodeParamsOwnerLay = NO_SPACING(NO_MARGIN(new QGridLayout(nodeParamsOwner)));

	auto paramsHeadWidget = WDG();
	auto paramsHeadWidgetLay = new QGridLayout(paramsHeadWidget);
	paramsHeadWidgetLay->addWidget(OBJ_NAME(LBL("Parameters"), "heading-label"), 2, 0, 1, 3);
	paramsHeadWidgetLay->setColumnStretch(2, 1);
	
	paramsHeadWidget->hide();

	auto *scrollAreaWidget = OBJ_NAME(new QFrame, "buider-parameters-owner");
	QVBoxLayout *paramsLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(scrollAreaWidget)));

	QScrollArea *scrollArea = OBJ_NAME(new QScrollArea(), "experiment-params-scroll-area");
	scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(scrollAreaWidget);

	auto scrollAreaOverlay = OBJ_NAME(new QWidget(scrollArea), "run-exp-params-overlay");
	scrollArea->installEventFilter(new PlotOverlayEventFilter(scrollArea, scrollAreaOverlay));
	scrollAreaOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
	scrollAreaOverlay->hide();

	auto vertBar = scrollArea->verticalScrollBar();
	connections << CONNECT(vertBar, &QScrollBar::rangeChanged, [=]() {
		if (vertBar->value() == vertBar->maximum()) {
			scrollAreaOverlay->hide();
		}
		else {
			scrollAreaOverlay->show();
			scrollAreaOverlay->raise();
		}
	});
	connections << CONNECT(vertBar, &QScrollBar::valueChanged, [=]() {
		if (vertBar->value() == vertBar->maximum()) {
			scrollAreaOverlay->hide();
		}
		else {
			scrollAreaOverlay->show();
			scrollAreaOverlay->raise();
		}
	});
	vertBar->installEventFilter(new UniversalEventFilter(vertBar, [=](QObject *obj, QEvent *e) -> bool {
		switch (e->type()) {
		case QEvent::Hide:
			scrollAreaOverlay->hide();
			break;

		case QEvent::Show:
			if (vertBar->value() == vertBar->maximum()) {
				scrollAreaOverlay->hide();
			}
			else {
				scrollAreaOverlay->show();
				scrollAreaOverlay->raise();
			}
			break;
		}
		return false;
	}));
	
	nodeParamsOwnerLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-top"), 0, 0, 1, 3);
	nodeParamsOwnerLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-bottom"), 4, 0, 1, 3);
	//nodeParamsOwnerLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-left"), 1, 0, 2, 1);
	//nodeParamsOwnerLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-right"), 1, 3, 2, 1);
	nodeParamsOwnerLay->addWidget(paramsHeadWidget, 1, 1);
	nodeParamsOwnerLay->addWidget(scrollArea, 2, 1);
	//nodeParamsOwnerLay->addLayout(buttonLay, 3, 1);
	nodeParamsOwnerLay->setRowStretch(2, 1);

	builderTabs.paramsHeadWidget = paramsHeadWidget;
	builderTabs.paramsLay = paramsLay;

	//CreateBuildExperimentTabWidget()

	auto builderTabsPlacerholder = OBJ_NAME(WDG(), "experiment-builder-placeholder");
	auto builderTabsLay = NO_SPACING(NO_MARGIN(new QStackedLayout(builderTabsPlacerholder)));


	auto splitter = new QSplitter(Qt::Horizontal);
	splitter->addWidget(builderTabsPlacerholder);
	splitter->addWidget(nodeParamsOwner);
	splitter->setChildrenCollapsible(false);

	lay->addWidget(nodeListOwner);
	lay->addWidget(splitter);

	static QPushButton *closeTabButton = 0;
	static QMetaObject::Connection closeTabButtonConnection;
	static int prevCloseTabButtonPos = -1;

	auto RemoveSelection = [=]() {
		auto wdg = builderTabsLay->widget(builderTabsLay->currentIndex());
		if (!wdg) {
			return;
		}

		auto builder = wdg->findChild<BuilderWidget*>("build-exp-holder");

		if (!builder) {
			return;
		}

		builder->RemoveSelection();
	};

	connections << CONNECT(mw, &MainWindow::EditCustomExperiment, [=](const CustomExperiment &_ce) {
		CustomExperiment ce = _ce;
		tabBar->insertTab(tabBar->count(), ce.name);

		const QUuid id = QUuid::createUuid();
		auto builderTabWidget = CreateBuildExperimentTabWidget(id);

		builderTabsLay->insertWidget(tabBar->count() - 1, builderTabWidget);
		tabBar->setCurrentIndex(tabBar->count() - 1);

		builderTabs.builders[id].fileName = ce.fileName;
		builderTabs.builders[id].name = ce.name;
		builderTabs.builders[id].globalMult->setValue(ce.bc.repeats);

		MainWindow::FillElementPointers(ce.bc, elementsPtrMap);

		builderTabs.builders[id].builder->SetupNewContainer(ce.bc);

		ui.newDataTab.buildExperimentButton->click();
	});

	connections << CONNECT(addNewButton, &QPushButton::clicked, [=]() {
		auto tabName = QString("New experiment");
		tabBar->insertTab(tabBar->count(), tabName);

		const QUuid id = QUuid::createUuid();

		builderTabs.builders[id].name = "New experiment";

		auto builderTabWidget = CreateBuildExperimentTabWidget(id);

		builderTabsLay->insertWidget(tabBar->count() - 1, builderTabWidget);
		tabBar->setCurrentIndex(tabBar->count() - 1);
	});

	connections << CONNECT(tabBar, &QTabBar::tabMoved, [=](int from, int to) {
		auto wdg = builderTabsLay->widget(from);
		builderTabsLay->removeWidget(wdg);
		builderTabsLay->insertWidget(to, wdg);
	});

	connections << CONNECT(tabBar, &QTabBar::currentChanged, [=](int index) {
		if (index < 0) {
			return;
		}
		if (index >= tabBar->count()) {
			tabBar->setCurrentIndex(prevCloseTabButtonPos);
			return;
		}

		if (closeTabButton) {
			tabBar->setTabButton(prevCloseTabButtonPos, QTabBar::RightSide, 0);
			QObject::disconnect(closeTabButtonConnection);
			closeTabButton->deleteLater();
		}

		tabBar->setTabButton(index, QTabBar::RightSide, closeTabButton = OBJ_NAME(PBT("x"), "close-document-pbt"));
		RemoveSelection();
		builderTabsLay->setCurrentIndex(index);
		prevCloseTabButtonPos = index;

		closeTabButtonConnection =
		CONNECT(closeTabButton, &QPushButton::clicked, [=]() {
			int currentIndex = tabBar->currentIndex();

			if ((-1 == currentIndex) || (currentIndex >= tabBar->count())) {
				return;
			}

			auto wdg = builderTabsLay->widget(currentIndex);
			auto builder = wdg->findChild<QWidget*>("build-exp-holder");
			
			if (0 != builder) {
				for (auto itId = builderTabs.builders.begin(); itId != builderTabs.builders.end(); ++itId) {
					if (itId.value().builder != builder) {
						continue;
					}

					foreach(auto conn, itId.value().connections) {
						QObject::disconnect(conn);
					}
					
					break;
				}
			}

			if ((prevCloseTabButtonPos == (tabBar->count() - 1)) && (tabBar->count() > 1)) {
				--prevCloseTabButtonPos;
			}

			RemoveSelection();

			tabBar->setTabButton(prevCloseTabButtonPos, QTabBar::RightSide, 0);
			QObject::disconnect(closeTabButtonConnection);
			closeTabButton->deleteLater();
			closeTabButton = 0;
			tabBar->removeTab(currentIndex);
			builderTabsLay->removeWidget(wdg);
			wdg->deleteLater();

			if (tabBar->count() == 0) {
				addNewButton->click();
			}
		});
	});

	addNewButton->click();

	return w;
}
bool MainWindowUI::GetExperimentNotes(QWidget *parent, ExperimentNotes &ret) {
	static bool dialogCanceled;
	dialogCanceled = true;

	ret.other.currentDensityWorkingElectrode.first = "Electrode area (working electrode)";
	ret.other.currentDensityCounterElectrode.first = "Electrode area (counter electrode)";
	ret.other.solvent.first = "Solvent";
	ret.other.electrolyte.first = "Electrolyte";
	ret.other.electrolyteConcentration.first = "Electrolyte concentration (moles per liter)";
	ret.other.atmosphere.first = "Atmosphere";

	static QMap<QString, qreal> references;
	references["Ag/AgCl in 0.1M KCl"] = 0.2894;
	references["Ag/AgCl in 1.0M KCl"] = 0.2368;
	references["Ag/AgCl in saturated KCl"] = 0.1976;
	references["Calomel in 0.1M KCl"] = 0.3337;
	references["Calomel in 1.0M KCl"] = 0.2807;
	references["Calomel in saturated KCl"] = 0.2415;
	references["Lead sulphate"] = -0.2760;
	references["Mercury sulphate in 0.5M H2SO4"] = 0.6820;
	references["Mercury sulphate in saturated K2SO4"] = 0.6500;
	references["Mercury oxide in 0.1M NaOH"] = 0.1650;
	references["Mercury oxide in 1.0M NaOH"] = 0.1400;

	QDialog* dialog = OBJ_NAME(new QDialog(parent, Qt::SplashScreen), "notes-dialog");

	QRect screenSize = QDesktopWidget().availableGeometry(parent);
	dialog->setFixedHeight(screenSize.height() < 800 ? screenSize.height() * 0.80 : 800);
	dialog->setFixedWidth(660);

	auto electrodeCombo = CMB();
	QRadioButton *commRefRadio;
	QRadioButton *otherRefRadio;
	QLineEdit *otherRefLed;
	QLineEdit *potVsSheLed;
	QTextEdit *notesTed;

	QLineEdit* currentDensityWorkingElectrode;
	QLineEdit* currentDensityCounterElectrode;
	QLineEdit* solvent;
	QLineEdit* electrolyte;
	QLineEdit* electrolyteConcentration;
	QLineEdit* atmosphere;

	QVBoxLayout *dialogLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(dialog)));

	/*
	auto scrolledWidget = OBJ_NAME(WDG(), "experimental-notes-scroll-area");
	/*/
	auto scrolledWidget = OBJ_NAME(WDG(), "experimental-notes-scrolled-widget");
	auto scrollArea = OBJ_NAME(new QScrollArea, "experimental-notes-scroll-area");
	scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(scrolledWidget);
	//*/

	auto lay = new QGridLayout(scrolledWidget);

	lay->addWidget(OBJ_NAME(LBL("Experimental Notes"), "heading-label"), 0, 0, 1, 2);
	lay->addWidget(notesTed = OBJ_NAME(TED(), "experimental-notes-text-edit"), 1, 0, 1, 2);

	lay->addWidget(OBJ_NAME(LBL("Reference Electrode"), "heading-label"), 2, 0, 1, 2);
	lay->addWidget(commRefRadio = new QRadioButton("Common reference electrode"), 3, 0);
	lay->addWidget(otherRefRadio = new QRadioButton("Other reference electrode"), 4, 0);
	lay->addWidget(electrodeCombo, 3, 1);
	lay->addWidget(otherRefLed = LED(), 4, 1);
	lay->addWidget(OBJ_NAME(LBL("Potential vs SHE (V)"), "notes-dialog-right-comment"), 5, 0);
	lay->addWidget(potVsSheLed = LED(), 5, 1);
	lay->addWidget(OBJ_NAME(WDG(), "notes-dialog-right-spacing"), 0, 2, 6, 1);
	lay->addWidget(OBJ_NAME(LBL("Other parameters"), "heading-label"), 6, 0, 1, 2);
	int row = 7;
	lay->addWidget(OBJ_NAME(LBL("Current density (working electrode)"), "notes-dialog-right-comment"), row, 0);
	lay->addWidget(currentDensityWorkingElectrode = LED(), row++, 1);
	lay->addWidget(OBJ_NAME(LBL("Current density (counter electrode)"), "notes-dialog-right-comment"), row, 0);
	lay->addWidget(currentDensityCounterElectrode = LED(), row++, 1);
	lay->addWidget(OBJ_NAME(LBL("Solvent"), "notes-dialog-right-comment"), row, 0);
	lay->addWidget(solvent = LED(), row++, 1);
	lay->addWidget(OBJ_NAME(LBL("Electrolyte"), "notes-dialog-right-comment"), row, 0);
	lay->addWidget(electrolyte = LED(), row++, 1);
	lay->addWidget(OBJ_NAME(LBL("Electrolyte concentration (moles per liter)"), "notes-dialog-right-comment"), row, 0);
	lay->addWidget(electrolyteConcentration = LED(), row++, 1);
	lay->addWidget(OBJ_NAME(LBL("Atmosphere"), "notes-dialog-right-comment"), row, 0);
	lay->addWidget(atmosphere = LED(), row++, 1);

	QPushButton *okBut;
	QPushButton *cancelBut;

	auto buttonLay = new QHBoxLayout;
	buttonLay->addStretch(1);
	buttonLay->addWidget(okBut = OBJ_NAME(PBT("OK"), "secondary-button"));
	buttonLay->addWidget(cancelBut = OBJ_NAME(PBT("Cancel"), "secondary-button"));
	buttonLay->addStretch(1);

	dialogLay->addWidget(scrollArea);
	//dialogLay->addWidget(scrolledWidget);
	dialogLay->addWidget(OBJ_NAME(WDG(), "notes-dialog-bottom-spacing"));// , 6, 0, 1, -1);
	dialogLay->addLayout(buttonLay);// , 7, 0, 1, -1);
	dialogLay->addWidget(OBJ_NAME(WDG(), "notes-dialog-bottom-spacing"));// , 8, 0, 1, -1);

	QListView *electrodeComboList = OBJ_NAME(new QListView, "combo-list");
	electrodeCombo->setView(electrodeComboList);

	QList<QMetaObject::Connection> dialogConn;
	
	#define COMMON_REFERENCE_ELECTRODE_NAME	"common-reference-electrode-name"
	#define OTHER_REFERENCE_ELECTRODE_NAME	"other-reference-electrode-name"
	#define OTHER_REFERENCE_ELECTRODE_VALUE	"other-reference-electrode-value"

	dialogConn << CONNECT(commRefRadio, &QRadioButton::clicked, [=]() {
		QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);
		settings.setValue(OTHER_REFERENCE_ELECTRODE_NAME, otherRefLed->text());
		settings.setValue(OTHER_REFERENCE_ELECTRODE_VALUE, potVsSheLed->text());
		
		otherRefLed->setDisabled(true);
		otherRefLed->setPlaceholderText("");
		otherRefLed->setText("");

		potVsSheLed->setPlaceholderText("");
		potVsSheLed->setReadOnly(true);
		potVsSheLed->setText("");

		electrodeCombo->addItems(references.keys());
		electrodeCombo->setEnabled(true);

		QString currentText = settings.value(COMMON_REFERENCE_ELECTRODE_NAME, "").toString();
		if (!currentText.isEmpty()) {
			electrodeCombo->setCurrentText(currentText);
		}
	});

	dialogConn << CONNECT(otherRefRadio, &QRadioButton::clicked, [=]() {
		QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);
		settings.setValue(COMMON_REFERENCE_ELECTRODE_NAME, electrodeCombo->currentText());

		electrodeCombo->setDisabled(true);
		electrodeCombo->clear();

		otherRefLed->setEnabled(true);
		otherRefLed->setPlaceholderText("Type here electrode name");
		otherRefLed->setText(settings.value(OTHER_REFERENCE_ELECTRODE_NAME, "").toString());

		potVsSheLed->setPlaceholderText("Type here the value");
		potVsSheLed->setReadOnly(false);
		potVsSheLed->setText(settings.value(OTHER_REFERENCE_ELECTRODE_VALUE, "").toString());
	});

	dialogConn << CONNECT(electrodeCombo, &QComboBox::currentTextChanged, [=](const QString &key) {
		if (key.isEmpty()) {
			return;
		}
		QString text = QString("%1").arg(references[key]).replace(QChar('.'), QLocale().decimalPoint());
		potVsSheLed->setText(text);
	});

	dialogConn << CONNECT(okBut, &QPushButton::clicked, [=]() {
		dialogCanceled = false;
	});

	CONNECT(okBut, &QPushButton::clicked, dialog, &QDialog::accept);
	CONNECT(cancelBut, &QPushButton::clicked, dialog, &QDialog::reject);

	commRefRadio->click();

	dialog->exec();

	foreach(auto conn, dialogConn) {
		QObject::disconnect(conn);
	}

	if (!dialogCanceled) {
		ret.description = notesTed->toPlainText();
		ret.refElectrode.first = commRefRadio->isChecked() ? electrodeCombo->currentText() : otherRefLed->text();
		ret.refElectrode.second = potVsSheLed->text();

		#define COPY_NOTE_VALUE(a) ret.other.a.second = a->text();

		COPY_NOTE_VALUE(currentDensityWorkingElectrode);
		COPY_NOTE_VALUE(currentDensityCounterElectrode);
		COPY_NOTE_VALUE(solvent);
		COPY_NOTE_VALUE(electrolyte);
		COPY_NOTE_VALUE(electrolyteConcentration);
		COPY_NOTE_VALUE(atmosphere);
	}

	dialog->deleteLater();

	return !dialogCanceled;
}
QWidget* MainWindowUI::GetRunExperimentTab() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	QLabel *descrIcon;
	QLabel *descrName;
	QLabel *descrText;

	w = WDG();
	QHBoxLayout *lay = NO_SPACING(NO_MARGIN(new QHBoxLayout(w)));

	auto *experimentListOwner = OBJ_PROP(OBJ_NAME(WDG(), "experiment-list-owner"), "widget-type", "left-grey");
	auto *experimentListLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(experimentListOwner)));
	auto *experimentList = OBJ_PROP(OBJ_NAME(new QListView, "experiment-list"), "widget-type", "left-grey");
	experimentList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	auto proxyModel = new ExperimentFilterModel;
	proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	proxyModel->setSourceModel(new QStandardItemModel(0, 0));
	experimentList->setModel(proxyModel);

	auto searchLay = NO_SPACING(NO_MARGIN(new QHBoxLayout));
	QLabel *searchLabel;
	QPushButton *searchClearPbt;
	QLineEdit *searchExpLed;
	searchLay->addWidget(searchLabel = OBJ_NAME(LBL(""), "search-experiments-label"));
	searchLay->addWidget(searchExpLed = OBJ_NAME(LED(), "search-experiments"));
	searchLay->addWidget(searchClearPbt = OBJ_NAME(PBT(""), "search-experiments-clear"));
	searchLay->addWidget(OBJ_NAME(WDG(), "search-experiments-spacing"));
	searchLabel->setPixmap(QPixmap(":/GUI/Resources/search-icon.png"));
	searchClearPbt->setIcon(QIcon(":/GUI/Resources/search-clear-button.png"));
	searchClearPbt->setIconSize(QPixmap(":/GUI/Resources/search-clear-button.png").size());
	searchClearPbt->hide();

	connections << CONNECT(searchExpLed, &QLineEdit::textChanged, [=](const QString &text) {
		if (text.isEmpty()) {
			searchClearPbt->hide();
		}
		else {
			searchClearPbt->show();
		}
	});

	connections << CONNECT(searchClearPbt, &QPushButton::clicked, [=]() {
		searchExpLed->clear();
	});

	connections << CONNECT(searchExpLed, &QLineEdit::textChanged, proxyModel, &QSortFilterProxyModel::setFilterFixedString);

	auto selectCategoryLay = NO_SPACING(NO_MARGIN(new QHBoxLayout));
	auto selectCategory = OBJ_NAME(CMB(), "select-category");
	selectCategory->setView(OBJ_NAME(new QListView, "combo-list"));

	selectCategoryLay->addWidget(OBJ_NAME(WDG(), "search-experiments-spacing"));
	selectCategoryLay->addWidget(selectCategory);
	selectCategoryLay->addWidget(OBJ_NAME(WDG(), "search-experiments-spacing"));

	connections << CONNECT(selectCategory, &QComboBox::currentTextChanged, [=](const QString &category) {
		proxyModel->SetCurrentCategory(category);
	});

	experimentListLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Categories"), "heading-label"), "widget-type", "left-grey"));
	experimentListLay->addLayout(selectCategoryLay);
	experimentListLay->addLayout(searchLay);
	experimentListLay->addWidget(experimentList);

	auto *descriptionHelpLay = NO_SPACING(NO_MARGIN(new QVBoxLayout()));

	auto *descriptionWidget = OBJ_NAME(WDG(), "experiment-description-owner");
	auto *descriptionWidgetLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(descriptionWidget)));

	auto descrTextHolder = WDG();
	auto descrTextHolderLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(descrTextHolder)));
	descrTextHolderLay->addWidget(descrText = OBJ_NAME(LBL(""), "experiment-description-text"));
	descrTextHolderLay->addStretch(1);

	QScrollArea *descrTextArea = OBJ_NAME(new QScrollArea, "experiment-description-text-scroll");
	descrTextArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	descrTextArea->setWidgetResizable(true);
	descrTextArea->setWidget(descrTextHolder);

	descriptionWidgetLay->addWidget(descrIcon = OBJ_NAME(LBL(""), "experiment-description-icon"));
	descriptionWidgetLay->addWidget(descrName = OBJ_NAME(LBL(""), "experiment-description-name"));
	descriptionWidgetLay->addWidget(descrTextArea, 1);
	//descriptionWidgetLay->addStretch(1);

	descriptionHelpLay->addWidget(OBJ_NAME(WDG(), "experiment-description-spacing-top"));
	descriptionHelpLay->addWidget(descriptionWidget);
	descriptionHelpLay->addWidget(OBJ_NAME(WDG(), "experiment-description-spacing-bottom"));

	auto *paramsWidget = OBJ_NAME(WDG(), "experiment-parameters-owner");
	auto *paramsWidgetLay = NO_SPACING(NO_MARGIN(new QGridLayout(paramsWidget)));

	auto *startExpPbt = OBJ_PROP(OBJ_NAME(PBT("Start Experiment"), "primary-button"), "button-type", "experiment-start-pbt");
	startExpPbt->setIcon(QIcon(":/GUI/Resources/start.png"));
	startExpPbt->setIconSize(QPixmap(":/GUI/Resources/start.png").size());
	startExpPbt->hide();

	auto *pauseExpPbt = OBJ_PROP(OBJ_NAME(PBT("Pause Experiment"), "primary-button"), "button-type", "experiment-start-pbt");
	auto *stopExpPbt = OBJ_PROP(OBJ_NAME(PBT("Stop Experiment"), "primary-button"), "button-type", "experiment-start-pbt");

	pauseExpPbt->hide();
	stopExpPbt->hide();

	auto *buttonLay = NO_SPACING(NO_MARGIN(new QHBoxLayout()));
	//auto *buttonLay = NO_SPACING(NO_MARGIN(new QGridLayout()));

	//buttonLay->addStretch(1);
	buttonLay->addWidget(startExpPbt);
	buttonLay->addWidget(pauseExpPbt);
	buttonLay->addWidget(stopExpPbt);
	buttonLay->addStretch(1);

	auto paramsHeadWidget = WDG();
	paramsHeadWidget->hide();

	auto paramsHeadWidgetLay = new QGridLayout(paramsHeadWidget);

	auto channelEdit = CMB();
	channelEdit->setView(OBJ_NAME(new QListView, "combo-list"));
	//channelEdit->addItem("Channel 1", 0);
	//channelEdit->addItem("Channel 2", 1);

	auto hwList = OBJ_NAME(CMB(), "hw-list-combo");
	hwList->setView(OBJ_NAME(new QListView, "combo-list"));

	/*
	paramsHeadWidgetLay->addWidget(OBJ_NAME(LBL("Select Channel"), "heading-label"), 0, 0, 1, 3);
	paramsHeadWidgetLay->addWidget(hwList, 1, 0);
	paramsHeadWidgetLay->addWidget(channelEdit, 1, 1);
	//*/
	paramsHeadWidgetLay->addWidget(OBJ_NAME(LBL("Parameters"), "heading-label"), 2, 0, 1, 3);
	paramsHeadWidgetLay->setColumnStretch(2, 1);


	auto paramsFooterWidget = WDG();
	paramsFooterWidget->hide();

	auto paramsFooterWidgetLay = new QGridLayout(paramsFooterWidget);

	paramsFooterWidgetLay->addWidget(OBJ_NAME(LBL("Select Device/Channel"), "heading-label"), 0, 0, 1, 3);
	paramsFooterWidgetLay->addWidget(hwList, 1, 0);
	paramsFooterWidgetLay->addWidget(channelEdit, 1, 1);
	paramsFooterWidgetLay->addLayout(buttonLay, 2, 0, 1, 3);

	paramsWidgetLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-top"), 0, 0, 1, 3);
	paramsWidgetLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-bottom"), 4, 0, 1, 3);
	paramsWidgetLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-left"), 1, 0, 2, 1);
	paramsWidgetLay->addWidget(OBJ_NAME(WDG(), "experiment-params-spacing-right"), 1, 3, 2, 1);
	paramsWidgetLay->addWidget(paramsHeadWidget, 1, 1);
	paramsWidgetLay->addWidget(paramsFooterWidget, 3, 1);
	paramsWidgetLay->setRowStretch(2, 1);

	auto *scrollAreaWidget = WDG();
	QVBoxLayout *paramsLay = NO_SPACING(NO_MARGIN(new QVBoxLayout(scrollAreaWidget)));

	QScrollArea *scrollArea = OBJ_NAME(new QScrollArea(), "experiment-params-scroll-area");
	paramsWidgetLay->addWidget(scrollArea, 2, 1);

	lay->addWidget(experimentListOwner);
	lay->addLayout(descriptionHelpLay);
	lay->addWidget(paramsWidget);


	scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(scrollAreaWidget);

	auto scrollAreaOverlay = OBJ_NAME(new QWidget(scrollArea), "run-exp-params-overlay");
	scrollArea->installEventFilter(new PlotOverlayEventFilter(scrollArea, scrollAreaOverlay));
	scrollAreaOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
	scrollAreaOverlay->hide();

	auto hwLambda = [=](const QString &hwName) {
		channelEdit->clear();
		/*
		if (!experimentList->selectionModel()->currentIndex().isValid()) {
			return;
		}
		//*/
		if (hwName.isEmpty()) {
			return;
		}

		for (int i = 0; i < hwList->currentData().toInt(); ++i) {
			channelEdit->addItem(QString("Channel %1").arg(i + 1), i);
		}

		mw->SelectHardware(hwList->currentText(), channelEdit->currentData().toInt());
		mw->UpdateCurrentExperimentState();

		selectedHardware.prebuilt.hwName = hwList->currentText();
		selectedHardware.prebuilt.channel = channelEdit->currentData().toInt();
	};

	connections << CONNECT(hwList, &QComboBox::currentTextChanged, hwLambda);

	connections << CONNECT(channelEdit, &QComboBox::currentTextChanged, [=](const QString &channelName) {
		if (!experimentList->selectionModel()->currentIndex().isValid()) {
			return;
		}

		mw->SelectHardware(hwList->currentText(), channelEdit->currentData().toInt());
		mw->UpdateCurrentExperimentState();

		selectedHardware.prebuilt.hwName = hwList->currentText();
		selectedHardware.prebuilt.channel = channelEdit->currentData().toInt();
	});

	auto vertBar = scrollArea->verticalScrollBar();
	connections << CONNECT(vertBar, &QScrollBar::rangeChanged, [=]() {
		if (vertBar->value() == vertBar->maximum()) {
			scrollAreaOverlay->hide();
		}
		else {
			scrollAreaOverlay->show();
			scrollAreaOverlay->raise();
		}
	});
	connections << CONNECT(vertBar, &QScrollBar::valueChanged, [=]() {
		if (vertBar->value() == vertBar->maximum()) {
			scrollAreaOverlay->hide();
		}
		else {
			scrollAreaOverlay->show();
			scrollAreaOverlay->raise();
		}
	});
	vertBar->installEventFilter(new UniversalEventFilter(vertBar, [=](QObject *obj, QEvent *e) -> bool {
		switch(e->type()) {
		case QEvent::Hide:
			scrollAreaOverlay->hide();
			break;

		case QEvent::Show:
			if (vertBar->value() == vertBar->maximum()) {
				scrollAreaOverlay->hide();
			}
			else {
				scrollAreaOverlay->show();
				scrollAreaOverlay->raise();
			}
			break;
		}
		return false;
	}));

	connections << CONNECT(mw, &MainWindow::AddNewInstruments, [=](const QList<HardwareUiDescription> &newLines) {
		for (auto it = newLines.begin(); it != newLines.end(); ++it) {
			hwList->addItem(it->name, it->channelAmount);
		}
		//hwList->addItems(newLines);
	});

	auto removeDisconnectedInstrumentsHandler = [=](const QStringList &linesToDelete) {
		for (int i = 0; i < hwList->count();) {
			if (linesToDelete.contains(hwList->itemText(i))) {
				hwList->removeItem(i);
			}
			else {
				++i;
			}
		}
	};
	connections << CONNECT(mw, &MainWindow::RemoveDisconnectedInstruments, removeDisconnectedInstrumentsHandler);

	connections << CONNECT(mw, &MainWindow::PrebuiltExperimentsFound, [=](const QList<AbstractExperiment*> &expList) {
		QStandardItemModel *model = new QStandardItemModel(expList.size(), 1);

		int row = 0;

		QStringList categoryStrList;
		foreach(const AbstractExperiment* exp, expList) {
			auto *item = new QStandardItem(exp->GetShortName());
			item->setData(QVariant::fromValue(exp), Qt::UserRole);
			
			model->setItem(row++, item);

			categoryStrList << exp->GetCategory();
		}
		categoryStrList << EXPERIMENT_VIEW_ALL_CATEGORY;
		categoryStrList.removeDuplicates();

		foreach(auto str, categoryStrList) {
			selectCategory->addItem(str);
		}
		selectCategory->setCurrentIndex(selectCategory->count() - 1);

		auto oldModel = proxyModel->sourceModel();
		proxyModel->setSourceModel(model);
		oldModel->deleteLater();
	});

	connections << CONNECT(mw, &MainWindow::AddNewCustomExperiments, [=](const QList<AbstractExperiment*> &expList) {
		auto originModel = proxyModel->sourceModel();
		QStandardItemModel *model = qobject_cast<QStandardItemModel*>(originModel);

		if (!model) {
			LOG() << "Unable to get model pointer";
		}

		QStringList newCategoryStrList;
		int row = model->rowCount();
		foreach(const AbstractExperiment *exp, expList) {
			auto *item = new QStandardItem(exp->GetShortName());
			item->setData(QVariant::fromValue(exp), Qt::UserRole);

			model->insertRow(row++, item);

			newCategoryStrList << exp->GetCategory();
		}

		newCategoryStrList.removeDuplicates();

		QStringList currentCategories;
		for (int i = 0; i < selectCategory->count(); ++i) {
			currentCategories << selectCategory->itemText(i);
		}

		QStringList categotiesToPrepend;
		foreach(auto &c, newCategoryStrList) {
			if (!currentCategories.contains(c)) {
				categotiesToPrepend << c;
			}
		}

		selectCategory->insertItems(0, categotiesToPrepend);
	});

	connections << CONNECT(mw, &MainWindow::RemoveCustomExperiment, [=](const AbstractExperiment *exp) {
		auto srcModel = proxyModel->sourceModel();

		for (int i = 0; i < srcModel->rowCount(); ++i) {
			auto curExp = srcModel->index(i, 0).data(Qt::UserRole).value<const AbstractExperiment*>();

			if (curExp == exp) {
				srcModel->removeRow(i);
				break;
			}
		}

		QList<QUuid> toDeleteIds;

		for (auto it = dataTabs.plots.begin(); it != dataTabs.plots.end(); ++it) {
			for (auto subIt = it.value().begin(); subIt != it.value().end(); ++subIt) {
				if (subIt.value().exp == exp) {
					if (!toDeleteIds.contains(it.key())) {
						toDeleteIds << it.key();
					}
					subIt.value().exp = 0;
				}
			}
		}

		foreach(auto &id, toDeleteIds) {
			mw->StopExperiment(id);
		}
	});

	connections << CONNECT(experimentList->selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &index, const QModelIndex &) {
		if (prebuiltExperimentData.userInputs) {
			paramsLay->removeWidget(prebuiltExperimentData.userInputs);
			prebuiltExperimentData.userInputs->deleteLater();
			prebuiltExperimentData.userInputs = 0;

			descrName->setText("");
			descrText->setText("");
			descrIcon->setPixmap(QPixmap());
		}

		if (index.isValid()) {
			auto exp = index.data(Qt::UserRole).value<const AbstractExperiment*>();

			descrName->setText(exp->GetFullName());
			descrText->setText(exp->GetDescription());
			descrIcon->setPixmap(exp->GetImage());

			prebuiltExperimentData.userInputs = exp->CreateUserInput();
			paramsLay->addWidget(prebuiltExperimentData.userInputs);
				
			mw->PrebuiltExperimentSelected(exp);
			selectedHardware.prebuilt.exp = exp;

			paramsHeadWidget->show();
			paramsFooterWidget->show();

			if (hwList->count()) {
				mw->SelectHardware(hwList->currentText(), channelEdit->currentData().toInt());
				mw->UpdateCurrentExperimentState();
			}
		}
		else {
			startExpPbt->hide();
			pauseExpPbt->hide();
			stopExpPbt->hide();
			paramsHeadWidget->hide();
			paramsFooterWidget->hide();
		}
	});
		
	connections << CONNECT(pauseExpPbt, &QPushButton::clicked, [=]() {
		if (pauseExpPbt->text() == PAUSE_EXP_BUTTON_TEXT) {
			mw->PauseExperiment(hwList->currentText(), channelEdit->currentData().toInt());
		}
		else {
			mw->ResumeExperiment(hwList->currentText(), channelEdit->currentData().toInt());
		}
	});

	connections << CONNECT(stopExpPbt, &QPushButton::clicked, [=]() {
		mw->StopExperiment(hwList->currentText(), channelEdit->currentData().toInt());
	});

	connections << CONNECT(startExpPbt, &QPushButton::clicked, [=]() {
		mw->StartExperiment(prebuiltExperimentData.userInputs);
		mw->UpdateCurrentExperimentState();
	});

	connections << CONNECT(mw, &MainWindow::CurrentHardwareBusy, [=]() {
		startExpPbt->hide();
		pauseExpPbt->show();
		stopExpPbt->show();
	});

	connections << CONNECT(mw, &MainWindow::CurrentExperimentCompleted, [=]() {
		startExpPbt->show();
		pauseExpPbt->hide();
		stopExpPbt->hide();
	});

	connections << CONNECT(mw, &MainWindow::CurrentExperimentPaused, [=]() {
		pauseExpPbt->setText(RESUME_EXP_BUTTON_TEXT);
	});

	connections << CONNECT(mw, &MainWindow::CurrentExperimentResumed, [=]() {
		pauseExpPbt->setText(PAUSE_EXP_BUTTON_TEXT);
	});

	return w;
}
QWidget* MainWindowUI::GetManualControlTab() {
	static QWidget *w = 0;

	if (w != 0) {
		return w;
	}
	
	w = OBJ_NAME(WDG(), "manual-control-tab-owner");

	static QMap<QString, qint8> channelAmountMap;
	static QList<QPushButton*> channelSelectButtons;
	//static QMap<QString, QWidget*> userInputs;
	static QMap<QString, QList<QUuid>> ids;

	auto channelSelectMapper = new QSignalMapper(w);

	auto *lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	auto tabHeaderLay = NO_SPACING(NO_MARGIN(new QHBoxLayout()));

	QTabBar *tabBar;

	auto channelSelectWdg = OBJ_NAME(new QFrame(), "channel-selecting-placeholder");
	auto channelSelectWdgLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(channelSelectWdg)));

	auto buttonGroup = new QButtonGroup(channelSelectWdg);
	for(int i = 0; i < MAX_CHANNEL_VALUE; ++i) {
		auto pbt = OBJ_NAME(new QPushButton(QString("Channel %1").arg(i + 1)), "select-channel-button");
		pbt->setCheckable(true);
		pbt->hide();

		buttonGroup->addButton(pbt);
		channelSelectMapper->setMapping(pbt, i);
		connections << CONNECT(pbt, &QPushButton::clicked, channelSelectMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
		
		channelSelectButtons << pbt;
		
		channelSelectWdgLay->addWidget(pbt);
	}
	if (channelSelectButtons.size()) {
		channelSelectButtons.at(0)->setChecked(true);
	}
	channelSelectWdgLay->addStretch(1);
	channelSelectWdg->hide();

	//auto startButtonWdg = OBJ_NAME(WDG(), "start-button-placeholder");
	//auto startButtonWdgLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(startButtonWdg)));

	//QPushButton *startExpPbt;
	//startButtonWdgLay->addWidget(startExpPbt = OBJ_NAME(PBT("Start Experiment"), "control-button-blue"));

	auto stackedLayWdg = OBJ_NAME(WDG(), "manual-control-tab-placeholder");
	auto stackedLay = NO_SPACING(NO_MARGIN(new QStackedLayout(stackedLayWdg)));

	QFrame *tabFrame = OBJ_NAME(new QFrame, "builder-tab-frame");
	auto *tabFrameLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(tabFrame)));
	tabFrameLay->addWidget(tabBar = OBJ_NAME(new QTabBar, "new-data-window-tab"));

	tabHeaderLay->addWidget(tabFrame);
	tabHeaderLay->addStretch(1);

	lay->addLayout(tabHeaderLay);
	lay->addWidget(channelSelectWdg);
	lay->addWidget(stackedLayWdg);
	//lay->addWidget(startButtonWdg);

	tabBar->setExpanding(false);
	tabBar->setMovable(true);

	tabBar->hide();

	/*
	connections << CONNECT(startExpPbt, &QPushButton::clicked, [=]() {
		auto curInstrName = tabBar->tabText(tabBar->currentIndex());
		mw->StartExperiment(userInputs[curInstrName], ids[curInstrName]);
		mw->UpdateCurrentExperimentState();
	});
	//*/

	connections << CONNECT(channelSelectMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), [=] (int channel) {
		auto name = tabBar->tabText(tabBar->currentIndex());

		selectedHardware.manual.channel[name] = channel;
		mw->SelectHardware(name, channel);
		mw->UpdateCurrentExperimentState();
	});

	connections << CONNECT(tabBar, &QTabBar::currentChanged, [=](int index) {
		if (index < 0) {
			return;
		}

		selectedHardware.manual.hwName = tabBar->tabText(index);
		stackedLay->setCurrentIndex(index);

		auto channelCount = channelAmountMap.value(selectedHardware.manual.hwName, 0);
		for (int i = 0; i < channelCount; ++i) {
			channelSelectButtons.at(i)->show();
		}
		for (int i = channelCount; i < MAX_CHANNEL_VALUE; ++i) {
			channelSelectButtons.at(i)->hide();
		}

		quint8 channel = selectedHardware.manual.channel.value(selectedHardware.manual.hwName, 0);

		channelSelectButtons.at(channel)->click();
		mw->SelectHardware(selectedHardware.manual.hwName, channel);
	});

	connections << CONNECT(tabBar, &QTabBar::tabMoved, [=](int from, int to) {
		auto wdg = stackedLay->widget(from);
		stackedLay->removeWidget(wdg);
		stackedLay->insertWidget(to, wdg);
	});

	connections << CONNECT(mw, &MainWindow::ExperimentCompleted, [=](const QUuid &id) {
		/*
		for (auto it = dataTabs.plots[id].begin(); it != dataTabs.plots[id].end(); ++it) {
			PlotHandler &handler(it.value());
			DataMapVisualization &majorData(handler.data.first());

			if (majorData.saveFile) {
				majorData.saveFile->close();
				majorData.saveFile->deleteLater();
				majorData.saveFile = 0;
			}

			handler.plot->replot();

			for (int i = 0; i < tabBar->count(); ++i) {
				auto wdg = stackedLay->widget(i);
				auto plot = wdg->findChild<QWidget*>("qwt-plot");

				if (0 == plot) {
					continue;
				}

				if (handler.plot != plot) {
					continue;
				}

				tabBar->setTabIcon(i, QIcon());
			}
		}
		//*/
	});

	connections << CONNECT(mw, &MainWindow::DcDataArrived, [=](const QUuid &id, const ExperimentalDcData &expData, ExperimentTrigger *trigger, bool paused) {
		/*
		if (!dataTabs.plots.keys().contains(id)) {
			return;
		}

		if (!dataTabs.plots[id].contains(ET_DC)) {
			return;
		}

		if (paused) {
			return;
		}

		PlotHandler &handler(dataTabs.plots[id][ET_DC]);
		DataMapVisualization &majorData(handler.data.first());

		if (handler.exp) {
			handler.exp->PushNewDcData(expData, majorData.container, majorData.cal, majorData.hwVer, majorData.notes, trigger);
			if (majorData.saveFile && ((majorData.container[majorData.container.lastKey()].data.size() - 1) % expData.decimation_num == 0)) {
				//if (majorData.saveFile) {
				handler.exp->SaveDcData(*majorData.saveFile, majorData.container);
			}
		}

		if (majorData.data[QwtPlot::xBottom] && majorData.data[QwtPlot::yLeft]) {
			majorData.curve1->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yLeft]));

			ApplyNewAxisParams(QwtPlot::yLeft, handler);
		}

		if (majorData.data[QwtPlot::xBottom] && majorData.data[QwtPlot::yRight]) {
			majorData.curve2->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yRight]));

			ApplyNewAxisParams(QwtPlot::yRight, handler);
		}

		ApplyNewAxisParams(QwtPlot::xBottom, handler);

		auto curStamp = QDateTime::currentMSecsSinceEpoch();
		if (curStamp > handler.plotCounter.stamp) {
			handler.plot->replot();
			handler.plotCounter.stamp = curStamp + 50;
		}
		//*/
	});

	connections << CONNECT(mw, &MainWindow::AcDataArrived, [=](const QUuid &id, const QByteArray &expData, ExperimentTrigger *trigger, bool paused) {
		/*
		if (!dataTabs.plots.keys().contains(id)) {
			return;
		}

		if (!dataTabs.plots[id].contains(ET_AC)) {
			return;
		}

		if (paused) {
			return;
		}

		PlotHandler &handler(dataTabs.plots[id][ET_AC]);
		DataMapVisualization &majorData(handler.data.first());

		if (handler.exp) {
			handler.exp->PushNewAcData(expData, majorData.container, majorData.cal, majorData.hwVer, majorData.notes, trigger);
			if (majorData.saveFile) {
				handler.exp->SaveAcData(*majorData.saveFile, majorData.container);
			}
		}

		if (majorData.data[QwtPlot::xBottom] && majorData.data[QwtPlot::yLeft]) {
			majorData.curve1->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yLeft]));

			ApplyNewAxisParams(QwtPlot::yLeft, handler);
		}

		if (majorData.data[QwtPlot::xBottom] && majorData.data[QwtPlot::yRight]) {
			majorData.curve2->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yRight]));

			ApplyNewAxisParams(QwtPlot::yRight, handler);
		}

		ApplyNewAxisParams(QwtPlot::xBottom, handler);

		auto curStamp = QDateTime::currentMSecsSinceEpoch();
		if (curStamp > handler.plotCounter.stamp) {
			handler.plot->replot();
			handler.plotCounter.stamp = curStamp + 50;
		}
		//*/
	});

	connections << CONNECT(mw, &MainWindow::AddNewInstruments, [=](const QList<HardwareUiDescription> &hwList) {
		foreach(auto &hwDescr, hwList) {
			channelAmountMap[hwDescr.name] = hwDescr.channelAmount;

			auto wdg = WDG();
			auto dataLayout = NO_MARGIN(NO_SPACING(new QStackedLayout(wdg)));

			auto mapper = new QSignalMapper(wdg);

			for (int i = 0; i < hwDescr.channelAmount; ++i) {
				auto curId = QUuid::createUuid();
				ids[hwDescr.name] << curId;

				auto dataWidget = CreateNewDataTabWidget(ids[hwDescr.name].at(i),
					ET_DC,
					hwDescr.name + QString(" - channel #%1").arg(i+1),
					ManualExperimentRunner::Instance()->GetXAxisParameters(ET_DC),
					ManualExperimentRunner::Instance()->GetYAxisParameters(ET_DC),
					"",
					0,
					true,
					hwDescr.hwModel);

				mapper->setMapping(channelSelectButtons.at(i), i);
				connections << CONNECT(channelSelectButtons.at(i), &QPushButton::clicked,
					mapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));

				dataLayout->addWidget(dataWidget);

				DataTabPtr dataTabPtr;
				dataTabPtr.type = DataTabPtr::DT_MANUAL;
				dataTabPtr.manual.hwTabBar = tabBar;
				dataTabPtr.manual.channelButton = channelSelectButtons.at(i);
				dataTabPtr.manual.hwIndex = tabBar->count();

				dataTabs.dataTabPtrs[curId] = dataTabPtr;
			}
			connections << CONNECT(mapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped),
				dataLayout, &QStackedLayout::setCurrentIndex);

			tabBar->insertTab(tabBar->count(), hwDescr.name);
			stackedLay->insertWidget(tabBar->count()-1, wdg);
		}

		if (tabBar->isHidden()) {
			tabBar->show();
			channelSelectWdg->show();
			//startButtonWdg->show();
		}
	});

	auto removeDisconnectedInstrumentsHandler = [=](const QStringList &names) {
		foreach(auto &name, names) {
			foreach(auto curId, ids[name]) {
				auto &handlers(dataTabs.plots[curId]);

				for (auto it = handlers.begin(); it != handlers.end(); ++it) {
					foreach(auto conn, it->plotTabConnections) {
						QObject::disconnect(conn);
					}
				}
			}
			channelAmountMap.remove(name);

			int currentIndex = -1;
			for (int i = 0; i < tabBar->count(); ++i) {
				if (tabBar->tabText(i) == name) {
					currentIndex = i;
					break;
				}
			}

			if (-1 == currentIndex) {
				continue;
			}

			auto wdg = stackedLay->widget(currentIndex);
			tabBar->removeTab(currentIndex);
			stackedLay->removeWidget(wdg);
			wdg->deleteLater();
		}

		if (tabBar->count() == 0) {
			tabBar->hide();
			channelSelectWdg->hide();
			//startButtonWdg->hide();
		}
	};
	connections << CONNECT(mw, &MainWindow::RemoveDisconnectedInstruments, removeDisconnectedInstrumentsHandler);

	return w;
}
bool MainWindowUI::ReadCsvFile(QWidget *parent, QList<MainWindowUI::CsvFileData> &dataList) {
	bool ret = false;

	QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);
	QString dirName = settings.value(DATA_SAVE_PATH, "").toString();

	auto dialogRetList = QFileDialog::getOpenFileNames(parent, "Open experiment data", dirName, "Data files (*.csv)");

	if (dialogRetList.isEmpty()) {
		return ret;
	}

	settings.setValue(DATA_SAVE_PATH, QFileInfo(dialogRetList.first()).absolutePath());

	foreach(auto dialogRet, dialogRetList) {
		if (dialogRet.isEmpty()) {
			return ret;
		}

		if (!QFileInfo(dialogRet).isReadable()) {
			return ret;
		}

		CsvFileData data;
		ret = ReadCsvFile(dialogRet, data);

		if (!ret) {
			return ret;
		}

		dataList << data;
	}


	return ret;
}
bool MainWindowUI::ReadCsvFile(const QString &dialogRet, MainWindowUI::CsvFileData &data) {
	bool ret = false;
	QChar listSeparator = (QLocale().decimalPoint() == QChar(',')) ? ';' : ',';
	QList<QStringList> readData = QtCSV::Reader::readToList(dialogRet, listSeparator);

	if (readData.size() < 13) {
		return ret;
	}

	data.fileName = QFileInfo(dialogRet).fileName();
	data.filePath = QFileInfo(dialogRet).absoluteFilePath();


	for(int i = 0; i < (COUNT_OF_EXPERIMENT_NOTES_LINES + 1); ++i) {
		readData.pop_front();
	}

	QStringList hdrList = readData.front();
	readData.pop_front();
	QStringList axisList = readData.front();
	readData.pop_front();

	int hdrListSize = hdrList.size();

	if (hdrListSize != axisList.size()) {
		return ret;
	}

	for (int i = 0; i < hdrListSize; ++i) {
		const QString &varName(hdrList.at(i));

		if (axisList.at(i).contains('X')) {
			data.xAxisList << varName;
		}
		if (axisList.at(i).contains('Y')) {
			data.yAxisList << varName;
		}
	}

	QChar systemDecimalPoint = QLocale().decimalPoint();
	QChar cDecimalPoint = QLocale::c().decimalPoint();

	for (auto it = readData.begin(); it != readData.end(); ++it) {
		QStringList &list(*it);

		if (hdrListSize != list.size()) {
			return ret;
		}

		for (int i = 0; i < hdrListSize; ++i) {
			bool ok;
			qreal val = list[i].replace(systemDecimalPoint, cDecimalPoint).toFloat(&ok);
			if (!ok) {
				return ret;
			}

			if (data.container[hdrList.at(i)].data.isEmpty()) {
				data.container[hdrList.at(i)].max = val;
				data.container[hdrList.at(i)].min = val;
			}
			else {
				if (val > data.container[hdrList.at(i)].max) {
					data.container[hdrList.at(i)].max = val;
				}
				if (val < data.container[hdrList.at(i)].min) {
					data.container[hdrList.at(i)].min = val;
				}
			}

			data.container[hdrList.at(i)].data.push_back(val);
		}
	}
	
	ret = true;

	return ret;
}
bool MainWindowUI::ReadCsvFile(QWidget *parent, MainWindowUI::CsvFileData &data) {
	bool ret = false;
	QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);
	QString dirName = settings.value(DATA_SAVE_PATH, "").toString();

	auto dialogRet = QFileDialog::getOpenFileName(parent, "Open experiment data", dirName, "Data files (*.csv)");

	if (dialogRet.isEmpty()) {
		return ret;
	}

	settings.setValue(DATA_SAVE_PATH, QFileInfo(dialogRet).absolutePath());

	if (!QFileInfo(dialogRet).isReadable()) {
		return ret;
	}


	ret = ReadCsvFile(dialogRet, data);

	return ret;
}
QWidget* MainWindowUI::GetNewDataWindowTab() {
	static QWidget *w = 0;

	if (w) {
		return w;
	}

	w = OBJ_NAME(WDG(), "new-data-window-owner");

	auto *lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(w)));

	auto tabHeaderLay = NO_SPACING(NO_MARGIN(new QHBoxLayout()));

	QPushButton *addNewButton;
	QPushButton *cascadeButton;
	QPushButton *tileButton;

	QTabBar *tabBar;

	auto stackedLayWdg = OBJ_NAME(WDG(), "new-data-window-placeholder");
	auto stackedLayWdgLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(stackedLayWdg)));
	auto mdiArea = new QMdiArea;
	mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.newDataTab.mdiArea = mdiArea;
	stackedLayWdgLay->addWidget(mdiArea);

	//auto stackedLay = NO_SPACING(NO_MARGIN(new QStackedLayout(stackedLayWdg)));
	//ui.newDataTab.stackedLay = stackedLay;

	QFrame *tabFrame = OBJ_NAME(new QFrame, "builder-tab-frame");
	auto *tabFrameLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(tabFrame)));
	tabFrameLay->addWidget(tabBar = OBJ_NAME(new QTabBar, "new-data-window-tab"));
	ui.newDataTab.tabBar = tabBar;

	tabHeaderLay->addWidget(tabFrame);
	tabHeaderLay->addWidget(addNewButton = OBJ_NAME(PBT("+"), "builder-tab-add-new"));
	tabHeaderLay->addStretch(1);
	tabHeaderLay->addWidget(cascadeButton = OBJ_NAME(PBT("Cascade"), "builder-tab-mdi"));
	tabHeaderLay->addWidget(tileButton = OBJ_NAME(PBT("Tile"), "builder-tab-mdi"));

	lay->addLayout(tabHeaderLay);
	lay->addWidget(stackedLayWdg);

	tabBar->setExpanding(false);
	tabBar->setMovable(true);

	tabBar->installEventFilter(new UniversalEventFilter(tabBar, [=](QObject *obj, QEvent *e) -> bool {
		switch (e->type()) {
		case QEvent::Hide:
			cascadeButton->hide();
			tileButton->hide();
			break;
		case QEvent::Show:
			cascadeButton->show();
			tileButton->show();
			break;
		}
		return false;
	}));

	tabBar->hide();
	cascadeButton->hide();
	tileButton->hide();
	
	static QPushButton *closeTabButton = 0;
	static QMetaObject::Connection closeTabButtonConnection;
	static int prevCloseTabButtonPos = -1;

	/*
	tabBar->installEventFilter(new UniversalEventFilter(tabBar, [=](QObject *obj, QEvent *e) -> bool {
		bool ret = false;
		auto w = qobject_cast<QTabBar*>(obj);

		if (0 == w) {
			return ret;
		}

		#define ME(a) ((QMouseEvent*)a)

		static Qt::MouseButton pressedButton = Qt::MouseButton::NoButton;
		static QPoint startPoint;

		switch (e->type()) {
			case QEvent::MouseButtonPress:
				pressedButton = ME(e)->button();
				startPoint = ME(e)->pos();
				break;

			case QEvent::MouseMove:
				if (pressedButton == Qt::LeftButton) {
					QPoint pos = ME(e)->pos();
					QLine moveVector(startPoint, pos);

					auto vlength = qAbs(moveVector.dy());
					if (vlength > w->height()) {
						auto curIndex = w->currentIndex();
						auto curTabRect = w->tabRect(curIndex);
						auto tabText = w->tabText(curIndex);

						auto mime = new QMimeData;
						mime->setData("name", tabText.toLocal8Bit());
						
						auto tempFrame = OBJ_NAME(new QFrame, "tab-drag-frame");
						auto tempFrameLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(tempFrame)));
						tempFrameLay->addWidget(new QLabel(tabText));
						tempFrameLay->addWidget(OBJ_NAME(PBT("x"), "close-document-pbt"));
						tempFrame->setFixedSize(curTabRect.width(), curTabRect.height() - 10);

						QDrag *drag = new QDrag(w);
						drag->setMimeData(mime);
						drag->setPixmap(tempFrame->grab());
						drag->setHotSpot(QPoint(startPoint.x() - curTabRect.x(), startPoint.y() - curTabRect.y() - 10));
						drag->setDragCursor(drag->dragCursor(Qt::MoveAction), Qt::IgnoreAction);

						auto dropAction = drag->exec(Qt::ActionMask, Qt::MoveAction);
						tempFrame->deleteLater();

						if (closeTabButton) {
							tabBar->setTabButton(prevCloseTabButtonPos, QTabBar::RightSide, 0);
							QObject::disconnect(closeTabButtonConnection);
							closeTabButton->deleteLater();
							closeTabButton = 0;
						}

						static QMap<QWidget*, QIcon> iconHolder;
						auto plotWdg = stackedLay->widget(curIndex);
						iconHolder[plotWdg] = w->tabIcon(curIndex);
						w->removeTab(curIndex);
						stackedLay->removeWidget(plotWdg);
						
						if (0 == w->count()) {
							w->hide();
						}

						QRect screenSize = QDesktopWidget().availableGeometry(mw);

						auto window = new QMainWindow(nullptr);
						window->setMinimumHeight(screenSize.height() < 768 ? screenSize.height() * 0.95 : 768);
						window->setMinimumWidth(screenSize.width() < 1200 ? screenSize.width() * 0.95 : 1200);
						window->setWindowTitle(tabText);
						
						auto menuBar = new QMenuBar;
						window->setMenuBar(menuBar);

						auto menu = new QMenu("Options");
						menuBar->addMenu(menu);

						auto action = menu->addAction("Move back");

						connections << CONNECT(action, &QAction::triggered, [=]() {
							if (0 == tabBar->count()) {
								tabBar->show();
							}
							tabBar->insertTab(tabBar->count(), iconHolder[plotWdg], tabText);
							stackedLay->insertWidget(tabBar->count()-1, plotWdg);
							window->deleteLater();
						});
						
						connections << CONNECT(mw, &MainWindow::ExperimentCompleted, [=](const QUuid &id) {
							auto plot = plotWdg->findChild<QWidget*>("qwt-plot");
							if (0 == plot) {
								return;
							}
							for (auto it = dataTabs.plots[id].begin(); it != dataTabs.plots[id].end(); ++it) {
								PlotHandler &handler(it.value());
								if (handler.plot != plot) {
									continue;
								}
								
								iconHolder[plotWdg] = QIcon();
							}
						});

						window->setCentralWidget(plotWdg);
						plotWdg->hide();

						window->show();
						plotWdg->show();

						ret = true;
					}
				}
				break;
			
			case QEvent::MouseButtonRelease:
				pressedButton = Qt::MouseButton::NoButton;
				break;
		}

		return ret;
	}));
	//*/
	auto actionOnAddNewPbt = [=]() {
		CsvFileData csvData;
		if (!ReadCsvFile(mw, csvData)) {
			return;
		}

		if (tabBar->isHidden()) {
			tabBar->show();
		}

		QString tabName = csvData.fileName;
		const QUuid id = QUuid::createUuid();

		auto dataTabWidget = CreateNewDataTabWidget(id,
			ET_SAVED,
			tabName,
			csvData.xAxisList,
			csvData.yAxisList,
			csvData.filePath,
			&csvData.container);

		dataTabs.dataWindowOrder << mdiArea->addSubWindow(dataTabWidget, SUB_WINDOWS_FLAGS);
		tabBar->insertTab(tabBar->count(), tabName);
		//stackedLay->insertWidget(tabBar->count() - 1, dataTabWidget);
		tabBar->setCurrentIndex(tabBar->count() - 1);
		mdiArea->setActiveSubWindow(dataTabs.dataWindowOrder.at(tabBar->count() - 1));
		dataTabs.dataWindowOrder.last()->show();

		if (dataTabs.firstDataTab) {
			dataTabs.firstDataTab = false;
			dataTabs.dataWindowOrder.last()->showMaximized();
		}
	};
	connections << CONNECT(addNewButton, &QPushButton::clicked, actionOnAddNewPbt);
	connections << CONNECT(cascadeButton, &QPushButton::clicked, mdiArea, &QMdiArea::cascadeSubWindows);
	connections << CONNECT(tileButton, &QPushButton::clicked, mdiArea, &QMdiArea::tileSubWindows);

	auto actionOnClosePbt = [=]() {
		int currentIndex = tabBar->currentIndex();

		if ((-1 == currentIndex) || (currentIndex >= tabBar->count())) {
			return;
		}

		auto wdg = dataTabs.dataWindowOrder.at(currentIndex)->widget();
		//auto wdg = stackedLay->widget(currentIndex);
		auto plot = wdg->findChild<QWidget*>("qwt-plot");

		if (0 != plot) {
			for (auto itId = dataTabs.plots.begin(); itId != dataTabs.plots.end(); ++itId) {
				bool found = false;
				for (auto it = itId.value().begin(); it != itId.value().end(); ++it) {
					if (it.value().plot != plot) {
						break;
					}

					foreach(auto conn, it.value().plotTabConnections) {
						QObject::disconnect(conn);
					}

					if (it.value().data.first().saveFile) {
						it.value().data.first().saveFile->close();
						it.value().data.first().saveFile->deleteLater();
						it.value().data.first().saveFile = 0;
					}

					mw->StopExperiment(itId.key());

					dataTabs.lastData[itId.key()] = it->data.first().container;

					itId.value().remove(it.key());

					dataTabs.dataTabPtrs.remove(itId.key());

					if (0 == itId.value().count()) {
						dataTabs.plots.remove(itId.key());
					}

					found = true;
					break;
				}

				if (found) {
					break;
				}
			}
		}

		if ((prevCloseTabButtonPos == (tabBar->count() - 1)) && (tabBar->count() > 1)) {
			--prevCloseTabButtonPos;
		}

		tabBar->setTabButton(prevCloseTabButtonPos, QTabBar::RightSide, 0);
		QObject::disconnect(closeTabButtonConnection);
		closeTabButton->deleteLater();
		closeTabButton = 0;
		tabBar->removeTab(currentIndex);
		mdiArea->removeSubWindow(dataTabs.dataWindowOrder.at(currentIndex));
		dataTabs.dataWindowOrder.at(currentIndex)->deleteLater();
		dataTabs.dataWindowOrder.removeAt(currentIndex);
		//stackedLay->removeWidget(wdg);
		//wdg->deleteLater();

		if (0 == tabBar->count()) {
			tabBar->hide();
		}
	};

	connections << CONNECT(tabBar, &QTabBar::currentChanged, [=](int index) {
		if (index < 0) {
			return;
		}
		if (index >= tabBar->count()) {
			tabBar->setCurrentIndex(prevCloseTabButtonPos);
			//stackedLay->setCurrentIndex(prevCloseTabButtonPos);
			mdiArea->setActiveSubWindow(mdiArea->subWindowList().at(prevCloseTabButtonPos));
			return;
		}

		if (closeTabButton) {
			tabBar->setTabButton(prevCloseTabButtonPos, QTabBar::RightSide, 0);
			QObject::disconnect(closeTabButtonConnection);
			closeTabButton->deleteLater();
		}

		mdiArea->setActiveSubWindow(dataTabs.dataWindowOrder.at(index));
		//stackedLay->setCurrentIndex(index);
		tabBar->setTabButton(index, QTabBar::RightSide, closeTabButton = OBJ_NAME(PBT("x"), "close-document-pbt"));
		prevCloseTabButtonPos = index;

		closeTabButtonConnection = 
			CONNECT(closeTabButton, &QPushButton::clicked, actionOnClosePbt);
	});

	connections << CONNECT(tabBar, &QTabBar::tabMoved, [=](int from, int to) {
		auto wdg = dataTabs.dataWindowOrder.at(from);
		dataTabs.dataWindowOrder.removeAt(from);
		dataTabs.dataWindowOrder.insert(to, wdg);
	});

	connections << CONNECT(mw, &MainWindow::CreateNewDataWindow, [=](const StartExperimentParameters &startParams) {
		//QString expName = exp->GetShortName();

		auto dataTabWidget = CreateNewDataTabWidget(startParams.id,
			startParams.type,
			startParams.exp->GetShortName(),
			startParams.exp->GetXAxisParameters(startParams.type),
			startParams.exp->GetYAxisParameters(startParams.type),
			startParams.filePath);

		auto &handler(dataTabs.plots[startParams.id][startParams.type]);

		handler.exp = startParams.exp;
		handler.data.first().saveFile = startParams.file;
		handler.data.first().cal = startParams.cal;
		handler.data.first().hwVer = startParams.hwVer;
		handler.data.first().notes = startParams.notes;

		dataTabs.dataWindowOrder << mdiArea->addSubWindow(dataTabWidget, SUB_WINDOWS_FLAGS);
		tabBar->insertTab(tabBar->count(), startParams.name);
		//stackedLay->insertWidget(tabBar->count() - 1, dataTabWidget);

		if (tabBar->isHidden()) {
			tabBar->show();
		}

		ui.newDataTab.newDataTabButton->click();
		tabBar->setCurrentIndex(tabBar->count() - 1);
		mdiArea->setActiveSubWindow(dataTabs.dataWindowOrder.at(tabBar->count() - 1));
		//stackedLay->setCurrentIndex(tabBar->count() - 1);
		tabBar->setTabIcon(tabBar->count() - 1, QIcon(":/GUI/Resources/green-dot.png"));

		if (dataTabs.firstDataTab) {
			dataTabs.firstDataTab = false;
			dataTabs.dataWindowOrder.last()->showMaximized();
		}

		DataTabPtr tabPtr;
		tabPtr.type = DataTabPtr::DT_REGULAR;
		tabPtr.regular.tabBar = tabBar;
		tabPtr.regular.plot = handler.plot;

		dataTabs.dataTabPtrs[startParams.id] = tabPtr;
	});

	auto expCompleteHandler = [=](const QUuid &id) {
		int i = 0;
		for (auto it = dataTabs.plots[id].begin(); it != dataTabs.plots[id].end(); ++it, ++i) {

			PlotHandler &handler(it.value());
			DataMapVisualization &majorData(handler.data.first());

			if (majorData.saveFile) {
				majorData.saveFile->close();
				majorData.saveFile->deleteLater();
				majorData.saveFile = 0;
			}

			handler.plot->replot();

      /*Matt*/
        //todo: add command to start manual sampling whenever the channel stops, and when the instrument becomes connected

			for (int i = 0; i < tabBar->count(); ++i) {
				auto wdg = dataTabs.dataWindowOrder.at(i)->widget();
				auto plot = wdg->findChild<QWidget*>("qwt-plot");

				if (0 == plot) {
					continue;
				}

				if (handler.plot != plot) {
					continue;
				}

				tabBar->setTabIcon(i, QIcon());
			}
		}
	};
	connections << CONNECT(mw, &MainWindow::ExperimentCompleted, expCompleteHandler);

	auto dcDataArrivedLambda = [=](const QUuid &id, const ExperimentalDcData &expData, ExperimentTrigger *trigger, bool paused) {
		if (!dataTabs.plots.keys().contains(id)) {
			return;
		}

		if (!dataTabs.plots[id].contains(ET_DC)) {
			return;
		}

		if (paused) {
			return;
		}

		PlotHandler &handler(dataTabs.plots[id][ET_DC]);
		DataMapVisualization &majorData(handler.data.first());

		if (handler.exp) {
			--handler.helpers.dc.counter;
			if (0 == handler.helpers.dc.counter) {
				handler.exp->PushNewDcData(expData, majorData.container, majorData.cal, majorData.hwVer, majorData.notes, trigger);

				if (majorData.saveFile) {
					handler.exp->SaveDcData(*majorData.saveFile, majorData.container);
				}
				
				handler.helpers.dc.counter = handler.helpers.dc.amount;
			}

			/*
			if (majorData.saveFile)
			{
				QString key = majorData.container.firstKey();
				uint32_t n = MAX(majorData.currentNode.DCsamplingParams.PointsSkippedPC, 1);
				if ((majorData.container[key].data.size() - 1) % n == 0)
					handler.exp->SaveDcData(*majorData.saveFile, majorData.container);
			}
			//*/
		}

		if (majorData.data[QwtPlot::xBottom] && majorData.data[QwtPlot::yLeft]) {
			majorData.curve1->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yLeft]));
			
			ApplyNewAxisParams(QwtPlot::yLeft, handler);
		}

		if (majorData.data[QwtPlot::xBottom] && majorData.data[QwtPlot::yRight]) {
			majorData.curve2->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yRight]));
			
			ApplyNewAxisParams(QwtPlot::yRight, handler);
		}

		ApplyNewAxisParams(QwtPlot::xBottom, handler);

		auto curStamp = QDateTime::currentMSecsSinceEpoch();
		if (curStamp > handler.plotCounter.stamp) {
			handler.plot->replot();
			handler.plotCounter.stamp = curStamp + 50;
		}
	};

	auto acDataArrivedLambda = [=](const QUuid &id, const QByteArray &expData, ExperimentTrigger *trigger, bool paused) {
		if (!dataTabs.plots.keys().contains(id)) {
			return;
		}

		if (!dataTabs.plots[id].contains(ET_AC)) {
			return;
		}

		if (paused) {
			return;
		}

		PlotHandler &handler(dataTabs.plots[id][ET_AC]);
		DataMapVisualization &majorData(handler.data.first());

		if (handler.exp) {
			/* <Matt/> */
			/*
			ExperimentalAcData * data = (ExperimentalAcData *)expData.data();
			for (int i = 0; i < ADCacBUF_SIZE * 2; i++)
			{
				majorData.accumulatingACdata.append(data->data[i]);
			}
			uint32_t dataCount = majorData.accumulatingACdata.count() / ADCacBUF_SIZE;
			if (dataCount >= majorData.currentNode.ACsamplingParams.numBufs * 2)
			{
				handler.exp->PushNewAcData(data, majorData.accumulatingACdata.data(), majorData.currentNode.ACsamplingParams.numBufs,
					majorData.container, majorData.cal, majorData.hwVer, majorData.notes, trigger);
				majorData.accumulatingACdata.clear();
			}
			//*/
			/* </Matt> */

			//*
			//handler.exp->AddNewData(...);
      
			ExperimentalAcData ACdataHeader = *((ExperimentalAcData *)expData.data());
			QByteArray ACDataOnly((const char *)(expData.data() + sizeof(ExperimentalAcData)), expData.count() - sizeof(ExperimentalAcData));
			handler.helpers.ac.accumData += ACDataOnly;
			--handler.helpers.ac.counter;
			if (0 == handler.helpers.ac.counter) {
				handler.exp->PushNewAcData(
					ACdataHeader,
					handler.helpers.ac.accumData,
					handler.helpers.ac.amount,
					majorData.container,
					majorData.cal,
					majorData.hwVer,
					majorData.notes,
					trigger);

				if (majorData.saveFile) {
					handler.exp->SaveAcData(*majorData.saveFile, majorData.container);
				}

				handler.helpers.ac.accumData.clear();
				handler.helpers.ac.counter = handler.helpers.ac.amount;
			}
			/*/
			handler.exp->PushNewAcData(expData, majorData.container, majorData.cal, majorData.hwVer, majorData.notes, trigger);
			//*/
        
		}

		if (majorData.data[QwtPlot::xBottom] && majorData.data[QwtPlot::yLeft]) {
			majorData.curve1->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yLeft]));

			ApplyNewAxisParams(QwtPlot::yLeft, handler);
		}

		if (majorData.data[QwtPlot::xBottom] && majorData.data[QwtPlot::yRight]) {
			majorData.curve2->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yRight]));

			ApplyNewAxisParams(QwtPlot::yRight, handler);
		}

		ApplyNewAxisParams(QwtPlot::xBottom, handler);

		auto curStamp = QDateTime::currentMSecsSinceEpoch();
		if (curStamp > handler.plotCounter.stamp) {
			handler.plot->replot();
			handler.plotCounter.stamp = curStamp + 50;
		}
	};

	connections << CONNECT(mw, &MainWindow::DcDataArrived, dcDataArrivedLambda);
	connections << CONNECT(mw, &MainWindow::AcDataArrived, acDataArrivedLambda);

	auto expNodeBegining = [=](const QUuid &curId, const ExperimentNode_t &node) {
		if (!dataTabs.plots.contains(curId)) {
			return;
		}

		PlotHandler *handler = 0;
		ExperimentType etype;

		switch (node.nodeType) {
			case FRA_NODE_POT:
			case FRA_NODE_GALV:
			case FRA_NODE_PSEUDOGALV:
				etype = ET_AC;
				break;

			case DCNODE_OCP:
			case DCNODE_SWEEP_POT:
			case DCNODE_SWEEP_GALV:
			case DCNODE_POINT_POT:
			case DCNODE_POINT_GALV:
			case DCNODE_NORMALPULSE_POT:
			case DCNODE_NORMALPULSE_GALV:
			case DCNODE_DIFFPULSE_POT:
			case DCNODE_DIFFPULSE_GALV:
			case DCNODE_SQRWAVE_POT:
			case DCNODE_SQRWAVE_GALV:
			case DCNODE_SINEWAVE:
			case DCNODE_CONST_RESISTANCE:
			case DCNODE_CONST_POWER:
			case DCNODE_MAX_POWER:
				etype = ET_DC;
				break;

			default:
				return;
				break;
		}

		if (!dataTabs.plots[curId].contains(etype)) {
			return;
		}
		handler = &(dataTabs.plots[curId][etype]);

		if (0 == handler) {
			return;
		}

		switch (etype) {
			case ET_AC:
				handler->helpers.ac.amount = node.ACsamplingParams.numBufs;
				handler->helpers.ac.counter = handler->helpers.ac.amount;
				handler->helpers.ac.accumData.clear();
				break;
			case ET_DC:
				handler->helpers.dc.amount = node.DCsamplingParams.PointsSkippedPC;
				if (0 == handler->helpers.dc.amount) {
					handler->helpers.dc.amount = 1;
				}
				handler->helpers.dc.counter = handler->helpers.dc.amount;
				break;
			default:
				return;
				break;
		}


		//TODO: if(node.newFileTrigger) {
		if (0) {
			auto &majorData(handler->data.first());

			QString path = majorData.filePath;
			QRegExp rx("((_[0-9]{3})?\\.csv)$");
			if (-1 == rx.indexIn(path)) {
				return;
			}
			auto postfixStr = rx.cap(1);
			auto counterStr = rx.cap(2);
			if (counterStr.size()) {
				counterStr.remove(0, 1);
			}
			int counter = counterStr.toInt() + 1;
			
			path.remove(QRegExp(postfixStr + "$"));

			path += "_" + QString("%1").arg(counter, 3, 10, QChar('0')) + ".csv";

			majorData.filePath = path;
			majorData.saveFile->flush();
			majorData.saveFile->close();
			majorData.saveFile->deleteLater();
			majorData.saveFile = new QFile(majorData.filePath);
			if (!majorData.saveFile->open(QIODevice::WriteOnly)) {
				majorData.saveFile->deleteLater();
				return;
			}
			
			switch (etype) {
				case ET_DC:
					handler->exp->SaveDcDataHeader(*majorData.saveFile, majorData.notes);
					break;
				case ET_AC:
					handler->exp->SaveAcDataHeader(*majorData.saveFile, majorData.notes);
					break;
			}
		}
	};
	connections << CONNECT(mw, &MainWindow::ExperimentNodeBeginning, expNodeBegining);

	return w;
}
template<typename T>
void ModifyObject(QObject *parent, std::function<void(T*)> lambda) {
    foreach(QObject* obj, parent->children()) {
        T* objT = qobject_cast<T*>(obj);
        if (objT) {
            lambda(objT);
        }

        ModifyObject<T>(obj, lambda);
    }
}
bool MainWindowUI::GetColor(QWidget *parent, QColor &color) {
	static bool ret;
	ret = false;

	QColorDialog colorDialog(DEFAULT_MAJOR_CURVE_COLOR, parent);
	colorDialog.setWindowFlags(Qt::SplashScreen);
	colorDialog.setOptions(QColorDialog::NoButtons | QColorDialog::DontUseNativeDialog);
	colorDialog.setCurrentColor(color);
	colorDialog.setCustomColor(0, DEFAULT_MAJOR_CURVE_COLOR);
	colorDialog.setCustomColor(1, DEFAULT_MINOR_CURVE_COLOR);

	QObject *dialogPtr = &colorDialog;

	QPushButton *ok;
	QPushButton *cancel;

	auto buttonLay = new QHBoxLayout;
	buttonLay->addStretch(1);
	buttonLay->addWidget(ok = PBT("Select"));
	buttonLay->addWidget(cancel = PBT("Cancel"));
	buttonLay->addStretch(1);

	CONNECT(ok, &QPushButton::clicked, &colorDialog, &QColorDialog::accept);
	CONNECT(cancel, &QPushButton::clicked, &colorDialog, &QColorDialog::reject);

	auto okConnection = CONNECT(ok, &QPushButton::clicked, [=]() {
		ret = true;
	});

	ModifyObject<QVBoxLayout>(dialogPtr, [=](QVBoxLayout *obj) {
		if (obj->parent() != dialogPtr) {
			return;
		}

		obj->addStretch(1);
		obj->addLayout(buttonLay);
	});

	ModifyObject<QAbstractButton>(dialogPtr, [](QAbstractButton *obj) {
		OBJ_NAME(obj, "secondary-button");
	});

	#define BASIC_HEADER "&Basic colors"
	#define CUSTOM_HEADER "&Custom colors"

	ModifyObject<QLabel>(dialogPtr, [](QLabel *obj) {
		if ((obj->text() == BASIC_HEADER) || (obj->text() == CUSTOM_HEADER)) {
			OBJ_NAME(obj, "heading-label");
		}
		else {
			OBJ_PROP(OBJ_NAME(obj, "experiment-params-comment"), "comment-placement", "left");
		}
	});


	ModifyObject<QFrame>(dialogPtr, [](QFrame *obj) {
		auto objParent = qobject_cast<QWidget*>(obj->parent());
	});

	colorDialog.exec();

	QObject::disconnect(okConnection);

	color = colorDialog.selectedColor();

	return ret;
}
bool MainWindowUI::GetNewPen(QWidget *parent, QMap<QString, MainWindowUI::CurveParameters> &curveParams, const QString &primaryLineName, const QString &secondaryLineName) {
	static bool dialogCanceled;
	dialogCanceled = true;
	
	QDialog* dialog = OBJ_NAME(new QDialog(parent, Qt::SplashScreen), "curve-params-dialog");
	QList<QMetaObject::Connection> dialogConn;
	auto globalLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(dialog)));

	auto lay = new QVBoxLayout();

	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));
	globalLay->addLayout(lay);
	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));

	QListView *fileList;

	lay->addWidget(OBJ_NAME(LBL("Dataset List"), "heading-label"));
	lay->addWidget(fileList = OBJ_NAME(new QListView, "curve-params-data-set-list"));

	fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);

	auto optLay = new QHBoxLayout;
	QRadioButton *primBut;
	QRadioButton *secBut;
	//optLay->addWidget(primBut = RBT("Primary curve"));
	//optLay->addWidget(secBut = RBT("Secondary curve"));
	optLay->addWidget(primBut = new QRadioButton(primaryLineName));
	optLay->addWidget(secBut = new QRadioButton(secondaryLineName));
	lay->addLayout(optLay);

	if (secondaryLineName == NONE_Y_AXIS_VARIABLE) {
		primBut->hide();
		secBut->hide();
	}

	auto stackLay = NO_SPACING(NO_MARGIN(new QStackedLayout));

	QLabel *curveSettingLabel;
	//lay->addWidget(curveSettingLabel = OBJ_NAME(LBL("Primary Curve"), "heading-label"));
	lay->addWidget(curveSettingLabel = OBJ_NAME(new QLabel(primaryLineName), "heading-label"));

	static CurveParameters currentParams;

	QSettings squidSettings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);

	currentParams.curve[CurveParameters::PRIMARY].pen.color = QColor(squidSettings.value(CURVE_PARAMS_PRI_PEN_COLOR, DEFAULT_MAJOR_CURVE_COLOR.name()).toString());
	currentParams.curve[CurveParameters::PRIMARY].pen.width = squidSettings.value(CURVE_PARAMS_PRI_PEN_WIDTH, 1.0).toDouble();
	currentParams.curve[CurveParameters::PRIMARY].pen.style = (Qt::PenStyle)squidSettings.value(CURVE_PARAMS_PRI_PEN_STYLE, (int)Qt::SolidLine).toInt();
	currentParams.curve[CurveParameters::PRIMARY].style = (QwtPlotCurve::CurveStyle)squidSettings.value(CURVE_PARAMS_PRI_STYLE, (int)QwtPlotCurve::Lines).toInt();
	currentParams.curve[CurveParameters::PRIMARY].symbol.width = squidSettings.value(CURVE_PARAMS_PRI_SYMBOL_WIDTH, 8.0).toDouble();
	currentParams.curve[CurveParameters::PRIMARY].symbol.style = (QwtSymbol::Style)squidSettings.value(CURVE_PARAMS_PRI_SYMBOL_STYLE, (int)QwtSymbol::NoSymbol).toInt();

	currentParams.curve[CurveParameters::SECONDARY].pen.color = QColor(squidSettings.value(CURVE_PARAMS_SEC_PEN_COLOR, DEFAULT_MINOR_CURVE_COLOR.name()).toString());
	currentParams.curve[CurveParameters::SECONDARY].pen.width = squidSettings.value(CURVE_PARAMS_SEC_PEN_WIDTH, 1.0).toDouble();
	currentParams.curve[CurveParameters::SECONDARY].pen.style = (Qt::PenStyle)squidSettings.value(CURVE_PARAMS_SEC_PEN_STYLE, (int)Qt::SolidLine).toInt();
	currentParams.curve[CurveParameters::SECONDARY].style = (QwtPlotCurve::CurveStyle)squidSettings.value(CURVE_PARAMS_SEC_STYLE, (int)QwtPlotCurve::Lines).toInt();
	currentParams.curve[CurveParameters::SECONDARY].symbol.width = squidSettings.value(CURVE_PARAMS_SEC_SYMBOL_WIDTH, 8.0).toDouble();
	currentParams.curve[CurveParameters::SECONDARY].symbol.style = (QwtSymbol::Style)squidSettings.value(CURVE_PARAMS_SEC_SYMBOL_STYLE, (int)QwtSymbol::NoSymbol).toInt();

	for(int what = CurveParameters::START; what < CurveParameters::TOTAL_CURVES; ++what) {
		auto paramsOwner = OBJ_NAME(WDG(), "curve-params-adjusting-owner");

		QwtPlot *smallPlot;
		QComboBox *penStyleCmb;
		QComboBox *curveSymbolCmb;
		QDoubleSpinBox *spin;
		QPushButton *colorPbt;
		QLabel *widthLbl;
		QSpinBox *widthSpin;
		QLineEdit *legendLed;
		auto paramsLay = NO_SPACING(NO_MARGIN(new QGridLayout(paramsOwner)));

		paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Legend: "), "experiment-params-comment"), "comment-placement", "left"), 0, 0);
		paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Color (click): "), "experiment-params-comment"), "comment-placement", "left"), 1, 0);
		paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Width: "), "experiment-params-comment"), "comment-placement", "left"), 2, 0);
		paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Line Style: "), "experiment-params-comment"), "comment-placement", "left"), 3, 0);
		paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Dot Style: "), "experiment-params-comment"), "comment-placement", "left"), 4, 0);
		paramsLay->addWidget(widthLbl = OBJ_PROP(OBJ_NAME(LBL("Width: "), "experiment-params-comment"), "comment-placement", "left"), 4, 2);
		paramsLay->addWidget(legendLed = LED(), 0, 1);
		paramsLay->addWidget(colorPbt = PBT(""), 1, 1);
		paramsLay->addWidget(spin = new QDoubleSpinBox, 2, 1);
		paramsLay->addWidget(penStyleCmb = CMB(), 3, 1);
		paramsLay->addWidget(curveSymbolCmb = CMB(), 4, 1);
		paramsLay->addWidget(widthSpin = new QSpinBox, 4, 3);
		paramsLay->addWidget(smallPlot = new QwtPlot, 1, 2, 3, 2);

		legendLed->setPlaceholderText("Auto");
		legendLed->setText(currentParams.curve[what].title);

		smallPlot->enableAxis(QwtPlot::yLeft, false);
		smallPlot->enableAxis(QwtPlot::xBottom, false);

		auto curve = CreateCurve(QwtPlot::yLeft, DEFAULT_MAJOR_CURVE_COLOR);
		curve->setSamples(QVector<qreal>() << 0.0 << 0.25 << 0.5 << 0.75 << 1.0, QVector<qreal>() << 0.0 << 0.25 << 0.5 << 0.75 << 1.0);
		curve->attach(smallPlot);

		curve->setPen(currentParams.curve[what].pen.color,
			currentParams.curve[what].pen.width,
			currentParams.curve[what].pen.style);

		curve->setStyle(currentParams.curve[what].style);

		auto curSymbol = new QwtSymbol(currentParams.curve[what].symbol.style);
		curSymbol->setPen(currentParams.curve[what].pen.color, 1, Qt::SolidLine);
		curSymbol->setColor(currentParams.curve[what].pen.color);
		curSymbol->setSize(currentParams.curve[what].symbol.width);
		
		curve->setSymbol(curSymbol);

		smallPlot->replot();

		colorPbt->setStyleSheet("background-color: " + currentParams.curve[what].pen.color.name() + ";");

		spin->setDecimals(1);
		spin->setSingleStep(0.1);
		spin->setValue(currentParams.curve[what].pen.width);

		widthSpin->setValue(currentParams.curve[what].symbol.width);

		penStyleCmb->setView(OBJ_NAME(new QListView, "combo-list"));
		{
			penStyleCmb->addItem("No Line", QVariant::fromValue<Qt::PenStyle>(Qt::NoPen));
			penStyleCmb->addItem("Solid", QVariant::fromValue<Qt::PenStyle>(Qt::SolidLine));
			penStyleCmb->addItem("Dash", QVariant::fromValue<Qt::PenStyle>(Qt::DashLine));
			penStyleCmb->addItem("Dot", QVariant::fromValue<Qt::PenStyle>(Qt::DotLine));
			penStyleCmb->addItem("Dash Dot", QVariant::fromValue<Qt::PenStyle>(Qt::DashDotLine));
			penStyleCmb->addItem("Dash Dot Dot", QVariant::fromValue<Qt::PenStyle>(Qt::DashDotDotLine));
		}

		if (QwtPlotCurve::Dots == currentParams.curve[what].style) {
			penStyleCmb->setCurrentIndex(0);
		}
		else {
			for (int i = 0; i < penStyleCmb->count(); ++i) {
				if (penStyleCmb->itemData(i).value<Qt::PenStyle>() == currentParams.curve[what].pen.style) {
					penStyleCmb->setCurrentIndex(i);
					break;
				}
			}
		}

		curveSymbolCmb->setView(OBJ_NAME(new QListView, "combo-list"));
		{
			curveSymbolCmb->addItem("NoSymbol", QVariant::fromValue<int>((int)QwtSymbol::NoSymbol));
			curveSymbolCmb->addItem("Ellipse", QVariant::fromValue<int>((int)QwtSymbol::Ellipse));
			curveSymbolCmb->addItem("Rect", QVariant::fromValue<int>((int)QwtSymbol::Rect));
			curveSymbolCmb->addItem("Diamond", QVariant::fromValue<int>((int)QwtSymbol::Diamond));
			curveSymbolCmb->addItem("Triangle", QVariant::fromValue<int>((int)QwtSymbol::Triangle));
			curveSymbolCmb->addItem("DTriangle", QVariant::fromValue<int>((int)QwtSymbol::DTriangle));
			curveSymbolCmb->addItem("UTriangle", QVariant::fromValue<int>((int)QwtSymbol::UTriangle));
			curveSymbolCmb->addItem("LTriangle", QVariant::fromValue<int>((int)QwtSymbol::LTriangle));
			curveSymbolCmb->addItem("RTriangle", QVariant::fromValue<int>((int)QwtSymbol::RTriangle));
			curveSymbolCmb->addItem("Cross", QVariant::fromValue<int>((int)QwtSymbol::Cross));
			curveSymbolCmb->addItem("XCross", QVariant::fromValue<int>((int)QwtSymbol::XCross));
			curveSymbolCmb->addItem("HLine", QVariant::fromValue<int>((int)QwtSymbol::HLine));
			curveSymbolCmb->addItem("VLine", QVariant::fromValue<int>((int)QwtSymbol::VLine));
			curveSymbolCmb->addItem("Star1", QVariant::fromValue<int>((int)QwtSymbol::Star1));
			curveSymbolCmb->addItem("Star2", QVariant::fromValue<int>((int)QwtSymbol::Star2));
			curveSymbolCmb->addItem("Hexagon", QVariant::fromValue<int>((int)QwtSymbol::Hexagon));
		}
		
		for (int i = 0; i < curveSymbolCmb->count(); ++i) {
			if ((QwtSymbol::Style)curveSymbolCmb->itemData(i).value<int>() == currentParams.curve[what].symbol.style) {
				curveSymbolCmb->setCurrentIndex(i);
				break;
			}
		}
				
		stackLay->addWidget(paramsOwner);

		dialogConn << CONNECT(legendLed, &QLineEdit::textChanged, [=]() {
			currentParams.curve[what].title = legendLed->text();
		});

		dialogConn << CONNECT(curveSymbolCmb, &QComboBox::currentTextChanged, [=]() {
			auto symbolStyle = (QwtSymbol::Style)curveSymbolCmb->currentData().value<int>();
			auto symbol = new QwtSymbol(symbolStyle);

			widthSpin->setHidden(QwtSymbol::NoSymbol == symbolStyle);
			widthLbl->setHidden(QwtSymbol::NoSymbol == symbolStyle);

			currentParams.curve[what].symbol.style = symbolStyle;

			symbol->setPen(currentParams.curve[what].pen.color, 1, Qt::SolidLine);
			symbol->setColor(currentParams.curve[what].pen.color);
			symbol->setSize(currentParams.curve[what].symbol.width);

			curve->setSymbol(symbol);
			smallPlot->replot();
		});
		
		dialogConn << CONNECT(penStyleCmb, &QComboBox::currentTextChanged, [=]() {
			auto penStyle = penStyleCmb->currentData().value<Qt::PenStyle>();
			if (Qt::NoPen == penStyle) {
				currentParams.curve[what].style = QwtPlotCurve::Dots;
			}
			else {
				currentParams.curve[what].style = QwtPlotCurve::Lines;
				currentParams.curve[what].pen.style = penStyle;
				curve->setPen(curve->pen().color(), curve->pen().width(), penStyleCmb->currentData().value<Qt::PenStyle>());
			}
			curve->setStyle(currentParams.curve[what].style);
			smallPlot->replot();
		});

		dialogConn << CONNECT(spin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double val) {
			currentParams.curve[what].pen.width = val;
			curve->setPen(curve->pen().color(), val, curve->pen().style());
			smallPlot->replot();
		});

		dialogConn << CONNECT(widthSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int val) {
			currentParams.curve[what].symbol.width = val;

			auto symbol = new QwtSymbol(currentParams.curve[what].symbol.style);
			symbol->setPen(currentParams.curve[what].pen.color, 1, Qt::SolidLine);
			symbol->setColor(currentParams.curve[what].pen.color);
			symbol->setSize(currentParams.curve[what].symbol.width);

			curve->setSymbol(symbol);

			smallPlot->replot();
		});

		dialogConn << CONNECT(colorPbt, &QPushButton::clicked, [=]() {
			QColor color = curve->pen().color();
			if (GetColor(dialog, color)) {
				colorPbt->setStyleSheet("background-color: " + color.name() + ";");

				currentParams.curve[what].pen.color = color;

				curve->setPen(color, curve->pen().width(), curve->pen().style());

				auto symbol = new QwtSymbol(currentParams.curve[what].symbol.style);
				symbol->setPen(currentParams.curve[what].pen.color, 1, Qt::SolidLine);
				symbol->setColor(currentParams.curve[what].pen.color);
				symbol->setSize(currentParams.curve[what].symbol.width);

				curve->setSymbol(symbol);

				smallPlot->replot();
			}
		});
	}

	primBut->setChecked(true);

	dialogConn << CONNECT(primBut, &QRadioButton::clicked, [=]() {
		//curveSettingLabel->setText("Primary Curve");
		curveSettingLabel->setText(primaryLineName);
		stackLay->setCurrentIndex(0);
	});
	dialogConn << CONNECT(secBut, &QRadioButton::clicked, [=]() {
		//curveSettingLabel->setText("Secondary Curve");
		curveSettingLabel->setText(secondaryLineName);
		stackLay->setCurrentIndex(1);
	});

	lay->addLayout(stackLay);
	lay->addSpacing(40);
	
	auto buttonLay = new QHBoxLayout;
	QPushButton *okBut;
	QPushButton *cancelBut;
	buttonLay->addStretch(1);
	buttonLay->addWidget(okBut = OBJ_NAME(PBT("Apply"), "secondary-button"));
	buttonLay->addWidget(cancelBut = OBJ_NAME(PBT("Cancel"), "secondary-button"));
	buttonLay->addStretch(1);

	lay->addLayout(buttonLay);
	lay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-vertical-spacing"));

	QStandardItemModel *model = new QStandardItemModel(curveParams.size(), 1);
	int row = 0;
	foreach(auto key, curveParams.keys()) {
		auto *item = new QStandardItem(key);
		model->setItem(row++, item);
	}
	fileList->setModel(model);
    fileList->setCurrentIndex(fileList->model()->index(0, 0));
	
	dialogConn << CONNECT(okBut, &QPushButton::clicked, [=]() {
		dialogCanceled = false;
	});

	CONNECT(okBut, &QPushButton::clicked, dialog, &QDialog::accept);
	CONNECT(cancelBut, &QPushButton::clicked, dialog, &QDialog::reject);

	dialog->exec();

	if (!dialogCanceled) {
		foreach(const QModelIndex &index, fileList->selectionModel()->selectedIndexes()) {
			QString curName = index.data(Qt::DisplayRole).toString();
			curveParams[curName].curve[CurveParameters::PRIMARY] = currentParams.curve[CurveParameters::PRIMARY];
			curveParams[curName].curve[CurveParameters::SECONDARY] = currentParams.curve[CurveParameters::SECONDARY];
		}
	}

	squidSettings.setValue(CURVE_PARAMS_PRI_PEN_COLOR, currentParams.curve[CurveParameters::PRIMARY].pen.color.name());
	squidSettings.setValue(CURVE_PARAMS_PRI_PEN_WIDTH, currentParams.curve[CurveParameters::PRIMARY].pen.width);
	squidSettings.setValue(CURVE_PARAMS_PRI_PEN_STYLE, (int)currentParams.curve[CurveParameters::PRIMARY].pen.style);
	squidSettings.setValue(CURVE_PARAMS_PRI_STYLE, (int)currentParams.curve[CurveParameters::PRIMARY].style);
	squidSettings.setValue(CURVE_PARAMS_PRI_SYMBOL_WIDTH, currentParams.curve[CurveParameters::PRIMARY].symbol.width);
	squidSettings.setValue(CURVE_PARAMS_PRI_SYMBOL_STYLE, (int)currentParams.curve[CurveParameters::PRIMARY].symbol.style);

	squidSettings.setValue(CURVE_PARAMS_SEC_PEN_COLOR, currentParams.curve[CurveParameters::SECONDARY].pen.color.name());
	squidSettings.setValue(CURVE_PARAMS_SEC_PEN_WIDTH, currentParams.curve[CurveParameters::SECONDARY].pen.width);
	squidSettings.setValue(CURVE_PARAMS_SEC_PEN_STYLE, (int)currentParams.curve[CurveParameters::SECONDARY].pen.style);
	squidSettings.setValue(CURVE_PARAMS_SEC_STYLE, (int)currentParams.curve[CurveParameters::SECONDARY].style);
	squidSettings.setValue(CURVE_PARAMS_SEC_SYMBOL_WIDTH, currentParams.curve[CurveParameters::SECONDARY].symbol.width);
	squidSettings.setValue(CURVE_PARAMS_SEC_SYMBOL_STYLE, (int)currentParams.curve[CurveParameters::SECONDARY].symbol.style);

	foreach(auto conn, dialogConn) {
		QObject::disconnect(conn);
	}

	dialog->deleteLater();

	return !dialogCanceled;
}
QwtPlotCurve* MainWindowUI::CreateCurve(int yAxisId, const QColor &color) {
	QwtPlotCurve *curve = new QwtPlotCurve("");

	curve->setLegendAttribute(QwtPlotCurve::LegendShowLine);
	curve->setPen(color, 1);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	curve->setYAxis(yAxisId);

	return curve;
}

QString MainWindowUI::GetNewTitle(QWidget *parent, const QString &oldText) {
	static bool dialogCanceled;
	dialogCanceled = false;

	QDialog* dialog = OBJ_NAME(new QDialog(parent, Qt::FramelessWindowHint | Qt::Popup), "plot-title-dialog");
	QList<QMetaObject::Connection> dialogConn;
	auto globalLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(dialog)));

	auto lay = new QVBoxLayout();

	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));
	globalLay->addLayout(lay);
	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));

	QLineEdit *titleLed;

	auto paramsLay = new QHBoxLayout;
	paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Plot title: "), "experiment-params-comment"), "comment-placement", "left"));
	paramsLay->addWidget(titleLed = new QLineEdit(oldText));

	lay->addLayout(paramsLay);
	lay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-vertical-spacing"));

	dialog->installEventFilter(new PopupDialogEventFilter(dialog, [=](QEvent *e, bool &ret) {
		QKeyEvent *ke = (QKeyEvent*)e;

		ret = false;
		int key = ke->key();

		if (ke->matches(QKeySequence::Cancel)) {
			dialog->reject();
			dialogCanceled = true;
			ret = true;
		}
		if ((Qt::Key_Enter == key) || (Qt::Key_Return == key)) {
			dialog->accept();
			ret = true;
		}
	}));

	dialog->exec();

	QString ret;
	if (!dialogCanceled) {
		ret = titleLed->text();
	}

	foreach(auto conn, dialogConn) {
		QObject::disconnect(conn);
	}

	dialog->deleteLater();

	return ret;
}
bool MainWindowUI::GetNewAxisParams(QWidget *parent, MainWindowUI::AxisParameters &axisParams) {
	static bool dialogCanceled;
	dialogCanceled = false;

	QDialog* dialog = OBJ_NAME(new QDialog(parent, Qt::FramelessWindowHint | Qt::Popup), "axis-params-dialog");
	QList<QMetaObject::Connection> dialogConn;
	auto globalLay = NO_SPACING(NO_MARGIN(new QHBoxLayout(dialog)));

	auto lay = new QVBoxLayout();

	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));
	globalLay->addLayout(lay);
	globalLay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-horizontal-spacing"));

	QLineEdit *minLed;
	QLineEdit *maxLed;
	QLineEdit *stepLed;

	auto paramsLay = new QGridLayout;
	paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Min value: "), "experiment-params-comment"), "comment-placement", "left"), 1, 0);
	paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Max value: "), "experiment-params-comment"), "comment-placement", "left"), 2, 0);
	paramsLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Step size: "), "experiment-params-comment"), "comment-placement", "left"), 3, 0);
	paramsLay->addWidget(minLed = LED(), 1, 1);
	paramsLay->addWidget(maxLed = LED(), 2, 1);
	paramsLay->addWidget(stepLed = LED(), 3, 1);

	minLed->setPlaceholderText("Auto");
	maxLed->setPlaceholderText("Auto");
	stepLed->setPlaceholderText("Auto");

	lay->addLayout(paramsLay);
	lay->addWidget(OBJ_NAME(WDG(), "curve-params-dialog-vertical-spacing"));
	
	if (!axisParams.min.autoScale) {
		minLed->setText(QString::number(axisParams.min.val).replace(QChar('.'), QLocale().decimalPoint()));
	}
	if (!axisParams.max.autoScale) {
		maxLed->setText(QString::number(axisParams.max.val).replace(QChar('.'), QLocale().decimalPoint()));
	}
	if (!axisParams.step.autoScale) {
		stepLed->setText(QString::number(axisParams.step.val).replace(QChar('.'), QLocale().decimalPoint()));
	}

	dialog->installEventFilter(new PopupDialogEventFilter(dialog, [=](QEvent *e, bool &ret) {
		QKeyEvent *ke = (QKeyEvent*)e;

		ret = false;
		int key = ke->key();

		if (ke->matches(QKeySequence::Cancel)) {
			dialog->reject();
			dialogCanceled = true;
			ret = true;
		}
		if( (Qt::Key_Enter == key) || (Qt::Key_Return == key) ) {
			dialog->accept();
			ret = true;
		}
	}));

	dialog->exec();

	QString resStr;
	#define GET_VAL(led, var)					\
		resStr = led->text();					\
		if (resStr.isEmpty()) {					\
			axisParams.var.autoScale = true;	\
		}										\
		else {									\
			axisParams.var.val = resStr.replace(QLocale().decimalPoint(), QChar('.')).toDouble(); \
			axisParams.var.autoScale = false;	\
		}

	if (!dialogCanceled) {
		GET_VAL(minLed, min);
		GET_VAL(maxLed, max);
		GET_VAL(stepLed, step);
	}

	foreach(auto conn, dialogConn) {
		QObject::disconnect(conn);
	}

	dialog->deleteLater();

	return !dialogCanceled;
}
void MainWindowUI::ResetAxis(MainWindowUI::PlotHandler &handler, QwtPlot::Axis axis) {
	AxisParameters &axisParams(handler.axisParams[axis]);

	axisParams.max.autoScale = true;
	axisParams.min.autoScale = true;
	axisParams.step.autoScale = true;
}
bool MainWindowUI::ApplyNewAxisParams(QwtPlot::Axis axis, MainWindowUI::PlotHandler &handler) {
	bool ret = false;
	if ((axis == QwtPlot::yRight) && !handler.plot->axisEnabled(axis)) {
		return ret;
	}

	AxisParameters &axisParams(handler.axisParams[axis]);
	QwtPlot *plot = handler.plot;

	if (axisParams.min.autoScale && axisParams.max.autoScale && axisParams.step.autoScale) {
		if (!plot->axisAutoScale(axis)) {
			plot->setAxisAutoScale(axis);
			ret = true;
		}
	}
	else {
		double step = axisParams.step.autoScale ? (double)0.0 : axisParams.step.val;
		double min;
		double max;

		if (!axisParams.min.autoScale) {
			min = axisParams.min.val;
		}
		if (!axisParams.max.autoScale) {
			max = axisParams.max.val;
		}

		for (auto it = handler.data.begin(); it != handler.data.end(); ++it) {
			double curMin;
			double curMax;

			switch (axis) {
				case QwtPlot::xBottom:
				case QwtPlot::xTop:
					if (handler.plot->axisEnabled(QwtPlot::yRight)) {
						curMin = qMin(it->curve1->minXValue(), it->curve2->minXValue());
						curMax = qMax(it->curve1->maxXValue(), it->curve2->maxXValue());
					}
					else {
						curMin = it->curve1->minXValue();
						curMax = it->curve1->maxXValue();
					}
					break;
				case QwtPlot::yLeft:
					curMin = it->curve1->minYValue();
					curMax = it->curve1->maxYValue();
					break;
				case QwtPlot::yRight:
					curMin = it->curve2->minYValue();
					curMax = it->curve2->maxYValue();
					break;
			}

			if (it == handler.data.begin()) {
				if (axisParams.min.autoScale) {
					min = curMin;
				}
				if (axisParams.max.autoScale) {
					max = curMax;
				}
				continue;
			}
			if (axisParams.min.autoScale) {
				if (curMin < min) {
					min = curMin;
				}
			}

			if (axisParams.max.autoScale) {
				if (curMax > max) {
					max = curMax;
				}
			}
		}

		if (plot->axisAutoScale(axis) ||
			(min != plot->axisInterval(axis).minValue()) ||
			(max != plot->axisInterval(axis).maxValue()) ||
			(step != plot->axisStepSize(axis))) {
			plot->setAxisScale(axis, min, max, step);
			ret = true;
		}
	}

	return ret;
}
void MainWindowUI::MoveAxis(MainWindowUI::PlotHandler &handler, QwtPlot::Axis axis, int dVal) {
	if (!handler.plot->axisEnabled(axis)) {
		return;
	}

	handler.axisParams[axis].min.autoScale = false;
	handler.axisParams[axis].max.autoScale = false;
	handler.axisParams[axis].min.val = handler.plot->axisInterval(axis).minValue();
	handler.axisParams[axis].max.val = handler.plot->axisInterval(axis).maxValue();

	double canvasWidth = handler.plot->canvas()->width();
	double axisWidth = handler.axisParams[axis].max.val - handler.axisParams[axis].min.val;
	double axisdVal = (axisWidth * dVal) / canvasWidth;

	handler.axisParams[axis].min.val -= axisdVal;
	handler.axisParams[axis].max.val -= axisdVal;
}
void MainWindowUI::ZoomAxis(MainWindowUI::PlotHandler &handler, QwtPlot::Axis axis, double percents) {
	ZoomAxis(handler, axis, percents, percents);
}
void MainWindowUI::ZoomAxis(PlotHandler &handler, QwtPlot::Axis axis, double percentsMin, double percentsMax) {
	if (!handler.plot->axisEnabled(axis)) {
		return;
	}

	handler.axisParams[axis].min.autoScale = false;
	handler.axisParams[axis].max.autoScale = false;
	handler.axisParams[axis].min.val = handler.plot->axisInterval(axis).minValue();
	handler.axisParams[axis].max.val = handler.plot->axisInterval(axis).maxValue();

	double axisWidth = handler.axisParams[axis].max.val - handler.axisParams[axis].min.val;

	handler.axisParams[axis].min.val += (axisWidth * percentsMin);
	handler.axisParams[axis].max.val -= (axisWidth * percentsMax);

}


QMap<QString, std::function<QString(qreal)>> valueDisplayHandler = { 
    { QString(REAL_TIME_ELAPSED_TIME), [](qreal val) -> QString {
		QString ret("%1:%2:%3");

		int intVal = val;

		int s = intVal % 60;
		int m = (intVal / 60) % 60;
		int h = (intVal / 60) / 60;

		return ret.arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
	} },
    { QString(REAL_TIME_WORKING_ELECTRODE), [](qreal val) -> QString {
		QString ret("%1");
		return ret.arg(val, 0, 'f', 3);
	} },
    { QString(REAL_TIME_COUNTER_ELECTRODE), [](qreal val) -> QString {
		QString ret("%1");
		return ret.arg(val, 0, 'f', 3);
	} },
    { QString(REAL_TIME_CURRENT), [](qreal val) -> QString {
		QString ret("%1");
		return ret.arg(val, 0, 'e', 3);
	} },
    /* AC experimental values */
  { QString(REAL_TIME_FREQUENCY), [](qreal val) -> QString {
    QString ret("%1");
    return ret.arg(val, 0, 'f', 3);
  } },
  { QString(REAL_TIME_IMPEDANCE_MAG), [](qreal val) -> QString {
    QString ret("%1");
    return ret.arg(val, 0, 'f', 3);
  } },
  { QString(REAL_TIME_IMPEDANCE_PHASE), [](qreal val) -> QString {
    QString ret("%1");
    return ret.arg(val, 0, 'f', 3);
  } },
  { QString(REAL_TIME_IMPEDANCE_REAL), [](qreal val) -> QString {
    QString ret("%1");
    return ret.arg(val, 0, 'f', 3);
  } },
  { QString(REAL_TIME_IMPEDANCE_IMAG), [](qreal val) -> QString {
    QString ret("%1");
    return ret.arg(val, 0, 'f', 3);
  } }
};
auto *valueDisplayHandlerPtr = &valueDisplayHandler;

QStringList valueHideList = {
	QString(REAL_TIME_ELAPSED_TIME_HR),
	QString(REAL_TIME_CURRENT_INTEGRAL),
  QString(REAL_TIME_NEG_IMP_IMAG),
  QString(REAL_TIME_ERROR)
};

QWidget* MainWindowUI::CreateNewDataTabWidget(const QUuid &id, ExperimentType type, const QString &expName, const QStringList &xAxisList, const QStringList &yAxisList, const QString &filePath, const DataMap *loadedContainerPtr, bool isManualMode, HardwareModel_t hwModel) {
	QFont axisTitleFont("Segoe UI");
	axisTitleFont.setPixelSize(22);
	axisTitleFont.setBold(false);

	auto w = WDG();

	auto lay = NO_SPACING(NO_MARGIN(new QGridLayout(w)));

	QwtPlot *plot = OBJ_NAME(new QwtPlot(), "qwt-plot");
	plot->insertLegend(new QwtLegend(), QwtPlot::TopLegend);


	QwtText plotTitle;
	plotTitle.setFont(axisTitleFont);
	plotTitle.setText(expName);
	plot->setTitle(plotTitle);

	QwtPlotCurve *curve1 = CreateCurve(QwtPlot::yLeft, DEFAULT_MAJOR_CURVE_COLOR);
	QwtPlotCurve *curve2 = CreateCurve(QwtPlot::yRight, DEFAULT_MINOR_CURVE_COLOR);
	curve1->attach(plot);

	auto settingsLay = NO_SPACING(NO_MARGIN(new QGridLayout));

	auto realTimeGroup = OBJ_NAME(new QGroupBox("Real time value section"), "collapsible-group-box");
	auto realTimeGroupLay = NO_SPACING(NO_MARGIN(new QGridLayout(realTimeGroup)));
	auto realTimeGroupFrame = OBJ_NAME(new QFrame, "collapsible-group-box-frame");
	auto realTimeGroupFrameLay = NO_SPACING(NO_MARGIN(new QGridLayout(realTimeGroupFrame)));
	realTimeGroupLay->addWidget(realTimeGroupFrame);
	realTimeGroup->setCheckable(true);

	QStringList realTimeValueNames;
	realTimeValueNames << xAxisList << yAxisList;
	realTimeValueNames.removeDuplicates();
	
	int row = 0;
	foreach(const QString &valueName, realTimeValueNames) {
		if (valueHideList.contains(valueName)) {
			continue;
		}
		realTimeGroupFrameLay->addWidget(OBJ_PROP(OBJ_PROP(OBJ_NAME(new QLabel(valueName + " = "), "experiment-params-comment"), "comment-placement", "left"), "add-name", "real-time-values"), row, 0);
		realTimeGroupFrameLay->addWidget(dataTabs.realTimeLabels[id][valueName] = OBJ_PROP(OBJ_PROP(OBJ_NAME(LBL(""), "experiment-params-comment"), "comment-placement", "right"), "add-name", "real-time-values"), row, 1);
		++row;
	}
	#define STEP_VALUE_LBL_NAME			"Step"
	realTimeGroupFrameLay->addWidget(OBJ_PROP(OBJ_PROP(OBJ_NAME(new QLabel(STEP_VALUE_LBL_NAME " = "), "experiment-params-comment"), "comment-placement", "left"), "add-name", "real-time-values"), row, 0);
	realTimeGroupFrameLay->addWidget(dataTabs.realTimeLabels[id][STEP_VALUE_LBL_NAME] = OBJ_PROP(OBJ_PROP(OBJ_NAME(LBL(""), "experiment-params-comment"), "comment-placement", "right"), "add-name", "real-time-values"), row, 1);

	auto settingsGroup = OBJ_NAME(new QGroupBox("Graph options"), "collapsible-group-box");
	auto settingsGroupLay = NO_SPACING(NO_MARGIN(new QGridLayout(settingsGroup)));
	auto settingsGroupFrame = OBJ_NAME(new QFrame, "collapsible-group-box-frame");
	auto settingsGroupFrameLay = NO_SPACING(NO_MARGIN(new QGridLayout(settingsGroupFrame)));
	settingsGroupLay->addWidget(settingsGroupFrame);
	settingsGroup->setCheckable(true);


	settingsGroupFrameLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("X axis = "), "experiment-params-comment"), "comment-placement", "left"), 1, 0);
	settingsGroupFrameLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Y<sub>1</sub> axis = "), "experiment-params-comment"), "comment-placement", "left"), 2, 0);
	settingsGroupFrameLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Y<sub>2</sub> axis = "), "experiment-params-comment"), "comment-placement", "left"), 3, 0);

	auto xCombo = CMB();
	auto *xComboList = OBJ_NAME(new QListView, "combo-list");
	xCombo->setView(xComboList);
	xCombo->addItems(xAxisList);

	auto y1Combo = CMB();
	QListView *y1ComboList = OBJ_NAME(new QListView, "combo-list");
	y1Combo->setView(y1ComboList);
	y1Combo->addItems(yAxisList);

	auto y2Combo = CMB();
	auto *y2ComboList = OBJ_NAME(new QListView, "combo-list");
	y2Combo->setView(y2ComboList);
	y2Combo->addItems(QStringList() << NONE_Y_AXIS_VARIABLE << yAxisList);

	QCheckBox *xChkBox;
	QCheckBox *y1ChkBox;
	QCheckBox *y2ChkBox;

	QPushButton *addDataPbt;
	QPushButton *editLinesPbt;
	QPushButton *savePlotPbt;
	QPushButton *openFilePbt;

	auto buttonLay = new QHBoxLayout;
	buttonLay->addWidget(addDataPbt = OBJ_PROP(OBJ_NAME(PBT("Add Data\nFile(s)"), "secondary-button"), "add-name", "new-data-controls"));
	buttonLay->addWidget(editLinesPbt = OBJ_PROP(OBJ_NAME(PBT("Edit Line\nAppearance"), "secondary-button"), "add-name", "new-data-controls"));
	buttonLay->addWidget(savePlotPbt = OBJ_PROP(OBJ_NAME(PBT("Save Plot\nas Image"), "secondary-button"), "add-name", "new-data-controls"));
	buttonLay->addWidget(openFilePbt = OBJ_PROP(OBJ_NAME(PBT("Open data\nin Excel"), "secondary-button"), "add-name", "new-data-controls"));

	settingsGroupFrameLay->addWidget(xCombo, 1, 1);
	settingsGroupFrameLay->addWidget(y1Combo, 2, 1);
	settingsGroupFrameLay->addWidget(y2Combo, 3, 1);
	#define LINEAR_TEXT				"Linear"
	#define LOGARITHMIC_TEXT		"Logarithmic"
	settingsGroupFrameLay->addWidget(xChkBox = OBJ_NAME(new QCheckBox(LINEAR_TEXT), "log-linear-check-box"), 1, 2);
	settingsGroupFrameLay->addWidget(y1ChkBox = OBJ_NAME(new QCheckBox(LINEAR_TEXT), "log-linear-check-box"), 2, 2);
	settingsGroupFrameLay->addWidget(y2ChkBox = OBJ_NAME(new QCheckBox(LINEAR_TEXT), "log-linear-check-box"), 3, 2);
	settingsGroupFrameLay->setColumnStretch(1, 1);
	settingsGroupFrameLay->addLayout(buttonLay, 4, 0, 1, -1);

	QGroupBox *advOptionsGroup;
	QWidget *advOptionsGroupFrame;
	QCheckBox *potGalvModeChk;
	QCheckBox *openCircuitModeChk;
	QComboBox *rangeCombo;
	QPushButton *startManualExpPbt;
	QPushButton *pauseManualExpPbt;
	QPushButton *stopManualExpPbt;
	QLineEdit *appliedPotLed;
	QLineEdit *appliedCurLed;
	QLineEdit *samplingIntLed;
	QLabel *appliedPotLblLeft;
	QLabel *appliedPotLblRight;
	QLabel *appliedCurLblLeft;
	QComboBox *appliedCurLblRight;

	#define POTENTIOSTATIC_TEXT "Potentiostatic"
	#define GALVANOSTATIC_TEXT	"Galvanostatic"
	#define OCP_MODE_TEXT		"Cell off (open circuit)"
	#define CELL_ON_TEXT		"Cell On"

	if (isManualMode) {
		advOptionsGroup = OBJ_NAME(new QGroupBox("Operating conditions"), "collapsible-group-box");
		auto advOptionsGroupLay = NO_SPACING(NO_MARGIN(new QGridLayout(advOptionsGroup)));
		advOptionsGroupFrame = OBJ_NAME(new QFrame, "collapsible-group-box-frame");
		auto advOptionsGroupFrameLay = NO_SPACING(NO_MARGIN(new QGridLayout(advOptionsGroupFrame)));
		advOptionsGroupLay->addWidget(advOptionsGroupFrame);
		advOptionsGroup->setCheckable(true);

		advOptionsGroupFrameLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Operating mode: "), "experiment-params-comment"), "comment-placement", "left"), 0, 0);
		advOptionsGroupFrameLay->addWidget(appliedPotLblLeft = OBJ_PROP(OBJ_NAME(LBL("Applied potential: "), "experiment-params-comment"), "comment-placement", "left"), 3, 0);
		advOptionsGroupFrameLay->addWidget(appliedCurLblLeft = OBJ_PROP(OBJ_NAME(LBL("Applied current: "), "experiment-params-comment"), "comment-placement", "left"), 4, 0);
		advOptionsGroupFrameLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Sampling interval: "), "experiment-params-comment"), "comment-placement", "left"), 5, 0);
		advOptionsGroupFrameLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("Current range: "), "experiment-params-comment"), "comment-placement", "left"), 6, 0);

		advOptionsGroupFrameLay->addWidget(potGalvModeChk = OBJ_NAME(new QCheckBox(POTENTIOSTATIC_TEXT), "log-linear-check-box"), 1, 0, 1, -1);
		advOptionsGroupFrameLay->addWidget(openCircuitModeChk = OBJ_NAME(new QCheckBox(OCP_MODE_TEXT), "log-linear-check-box"), 2, 0, 1, -1);

		rangeCombo = CMB();
		auto *rangeComboList = OBJ_NAME(new QListView, "combo-list");
		rangeCombo->setView(rangeComboList);
		rangeCombo->addItem("Autorange", currentRange_t::AUTORANGE);
		switch (hwModel) {
		case HardwareModel_t::PRIME:
			rangeCombo->addItem("5mA - 100mA", currentRange_t::RANGE0);
			rangeCombo->addItem("200uA - 5mA", currentRange_t::RANGE1);
			rangeCombo->addItem("10uA - 200uA", currentRange_t::RANGE2);
			rangeCombo->addItem("0 - 10uA", currentRange_t::RANGE3);
			break;
		case HardwareModel_t::SOLO_2_0:
		case HardwareModel_t::PLUS_2_0:
			rangeCombo->addItem("1A", currentRange_t::RANGE0);
			rangeCombo->addItem("100mA", currentRange_t::RANGE1);
			rangeCombo->addItem("10mA", currentRange_t::RANGE2);
			rangeCombo->addItem("1mA", currentRange_t::RANGE3);
			rangeCombo->addItem("100uA", currentRange_t::RANGE4);
			rangeCombo->addItem("10uA", currentRange_t::RANGE5);
			rangeCombo->addItem("1uA", currentRange_t::RANGE6);
			rangeCombo->addItem("100nA", currentRange_t::RANGE7);
			break;
		case HardwareModel_t::PRIME_2_0:
			rangeCombo->addItem("100mA", currentRange_t::RANGE0);
			rangeCombo->addItem("10mA", currentRange_t::RANGE1);
			rangeCombo->addItem("1mA", currentRange_t::RANGE2);
			rangeCombo->addItem("100uA", currentRange_t::RANGE3);
			rangeCombo->addItem("10uA", currentRange_t::RANGE4);
			rangeCombo->addItem("1uA", currentRange_t::RANGE5);
			rangeCombo->addItem("100nA", currentRange_t::RANGE6);
			rangeCombo->addItem("10nA", currentRange_t::RANGE7);
			break;
		}

		advOptionsGroupFrameLay->addWidget(appliedPotLed = new QLineEdit("0"), 3, 1);
		advOptionsGroupFrameLay->addWidget(appliedCurLed = new QLineEdit("0"), 4, 1);
		advOptionsGroupFrameLay->addWidget(samplingIntLed = new QLineEdit("1"), 5, 1);
		advOptionsGroupFrameLay->addWidget(rangeCombo, 6, 1);

		appliedCurLblRight = CMB();
		auto *appliedCurLblRightList = OBJ_NAME(new QListView, "combo-list");
		appliedCurLblRight->setView(appliedCurLblRightList);
		appliedCurLblRight->addItem("mA", currentRange_t::RANGE0);
		appliedCurLblRight->addItem("uA", currentRange_t::RANGE1);
		appliedCurLblRight->addItem("nA", currentRange_t::RANGE2);

		advOptionsGroupFrameLay->addWidget(appliedPotLblRight = OBJ_PROP(OBJ_NAME(LBL("V"), "experiment-params-comment"), "comment-placement", "right"), 3, 2);
		advOptionsGroupFrameLay->addWidget(appliedCurLblRight, 4, 2);
		advOptionsGroupFrameLay->addWidget(OBJ_PROP(OBJ_NAME(LBL("s"), "experiment-params-comment"), "comment-placement", "right"), 5, 2);
		
		appliedCurLblLeft->hide();
		appliedCurLblRight->hide();
		appliedCurLed->hide();

		startManualExpPbt = OBJ_PROP(OBJ_NAME(PBT("Start recording"), "primary-button"), "button-type", "experiment-start-pbt");
		startManualExpPbt->setIcon(QIcon(":/GUI/Resources/start.png"));
		startManualExpPbt->setIconSize(QPixmap(":/GUI/Resources/start.png").size());

		pauseManualExpPbt = OBJ_PROP(OBJ_NAME(PBT("Pause experiment"), "primary-button"), "button-type", "experiment-start-pbt");
		stopManualExpPbt = OBJ_PROP(OBJ_NAME(PBT("Stop experiment"), "primary-button"), "button-type", "experiment-start-pbt");

		pauseManualExpPbt->hide();
		stopManualExpPbt->hide();

		auto *startButtonLay = NO_SPACING(NO_MARGIN(new QHBoxLayout()));
		startButtonLay->addWidget(startManualExpPbt);
		startButtonLay->addWidget(pauseManualExpPbt);
		startButtonLay->addWidget(stopManualExpPbt);
		startButtonLay->addStretch(1);

		advOptionsGroupFrameLay->addLayout(startButtonLay, 7, 0, 1, -1);

		advOptionsGroupFrameLay->setColumnStretch(1, 1);

		settingsLay->addWidget(advOptionsGroup, 0, 0, 1, -1);
	}
	settingsLay->addWidget(realTimeGroup, 2, 0, 1, -1);
	settingsLay->addWidget(settingsGroup, 3, 0, 1, -1);
	//settingsLay->addWidget(OBJ_NAME(WDG(), "settings-vertical-spacing"), 7, 0, 1, -1);
	settingsLay->setRowStretch(8, 1);
	//settingsLay->addLayout(buttonLay, 8, 0, -1, -1);

	auto controlButtonLay = new QHBoxLayout;
	QPushButton *pauseExperiment;
	QPushButton *stopExperiment;

	controlButtonLay->addWidget(pauseExperiment = OBJ_NAME(PBT("Pause experiment"), "control-button-blue"));
	controlButtonLay->addWidget(stopExperiment = OBJ_NAME(PBT("Stop experiment"), "control-button-red"));

	if ( (type == ET_SAVED) || (isManualMode) ) {
		pauseExperiment->hide();
		stopExperiment->hide();
	}

	auto plotCanvas = plot->canvas();

	auto plotOverlay = OBJ_NAME(new QWidget(plotCanvas), "plot-canvas-overlay");
	plotCanvas->installEventFilter(new PlotOverlayEventFilter(plotCanvas, plotOverlay));

	auto plotButtonsLay = NO_SPACING(NO_MARGIN(new QVBoxLayout));
	
	QPushButton *zoomInPbt;
	QPushButton *zoomOutPbt;
	QPushButton *showGridlinesPbt;
	QPushButton *zoomToSelectionPbt;
	QPushButton *panViewPbt;
	QPushButton *resetZoomPbt;

	plotButtonsLay->addStretch(1);
	plotButtonsLay->addWidget(showGridlinesPbt = OBJ_NAME(PBT(""), "plot-gridlines-button"));
	plotButtonsLay->addWidget(zoomToSelectionPbt = OBJ_NAME(PBT(""), "plot-zoom-to-selection-button"));
	plotButtonsLay->addWidget(panViewPbt = OBJ_NAME(PBT(""), "plot-pan-view-button"));
	plotButtonsLay->addWidget(resetZoomPbt = OBJ_NAME(PBT(""), "plot-reset-zoom-button"));
	plotButtonsLay->addWidget(zoomInPbt = OBJ_NAME(PBT(""), "plot-zoom-in-button"));
	plotButtonsLay->addWidget(zoomOutPbt = OBJ_NAME(PBT(""), "plot-zoom-out-button"));

	showGridlinesPbt->setToolTip("Show/hide gridlines");
	zoomToSelectionPbt->setToolTip("Zoom to selection");
	panViewPbt->setToolTip("Pan view");
	resetZoomPbt->setToolTip("Reset zoom");
	zoomInPbt->setToolTip("Zoom in");
	zoomOutPbt->setToolTip("Zoom out");

	zoomToSelectionPbt->setCheckable(true);
	panViewPbt->setCheckable(true);

	auto *cursorModeGroup = new QButtonGroup(w);
	cursorModeGroup->addButton(zoomToSelectionPbt);
	cursorModeGroup->addButton(panViewPbt);

	auto settingsOwner = OBJ_NAME(new QFrame, "collapsible-settings-owner");
	settingsOwner->setLayout(settingsLay);
	
	QScrollArea *settingsArea = OBJ_NAME(new QScrollArea(), "collapsible-settings-scroll-area");
	settingsArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	settingsArea->setWidgetResizable(true);
	settingsArea->setWidget(settingsOwner);

	if (!isManualMode) {
		lay->addWidget(OBJ_NAME(new QLabel(expName), "heading-label"), 0, 0, 1, -1);
	}
	lay->addWidget(OBJ_NAME(WDG(), "new-data-tab-left-spacing"), 1, 0, -1, 1);
	lay->addWidget(settingsArea, 1, 1);
	lay->addWidget(plot, 1, 2);
	lay->addLayout(plotButtonsLay, 1, 3);
	lay->addLayout(controlButtonLay, 2, 0, 1, -1);
	//lay->setColumnStretch(1, 1);
	lay->setColumnStretch(2, 1);

	PlotHandler plotHandler;
	plotHandler.plot = plot;
	plotHandler.varCombo[QwtPlot::xBottom] = xCombo;
	plotHandler.varCombo[QwtPlot::yLeft] = y1Combo;
	plotHandler.varCombo[QwtPlot::yRight] = y2Combo;
	plotHandler.exp = 0;
	plotHandler.data << DataMapVisualization();
	plotHandler.data.first().saveFile = 0;
	plotHandler.data.first().filePath = filePath;
	plotHandler.data.first().curve1 = curve1;
	plotHandler.data.first().curve2 = curve2;
	plotHandler.plotCounter.stamp = 0;
	plotHandler.plotCounter.realTimeValueStamp = 0;

	/*
	if (isManualMode) {
		plotHandler.exp = ManualExperimentRunner::Instance();
	}
	//*/

	#define OPEN_COLOR_TAG "<font color=#1d1d1d>"
	#define CLOSE_COLOR_TAG "</font>"

	#define NODE_TYPE_STR_FILL(a, b) case a: nodeTypeStr = b; break;

	auto y1ComboHandler = [=](const QString &curText) {
		PlotHandler &handler(dataTabs.plots[id][type]);

		QwtText title;
		title.setFont(axisTitleFont);
		title.setText(curText);
		handler.plot->setAxisTitle(QwtPlot::yLeft, title);

		for (auto it = handler.data.begin(); it != handler.data.end(); ++it) {
			it->data[QwtPlot::yLeft] = &it->container[curText];
			it->curve1->setSamples(new ListSeriesData(*it->data[QwtPlot::xBottom], *it->data[QwtPlot::yLeft]));
			if (-1 != y1Combo->findText(it->curve1->title().text())) {
				it->curve1->setTitle(curText);
			}
		}
		handler.plot->replot();
	};
	auto y2ComboHandler = [=](const QString &curText) {
		PlotHandler &handler(dataTabs.plots[id][type]);

		QwtText title;
		title.setFont(axisTitleFont);
		title.setText(curText);
		handler.plot->setAxisTitle(QwtPlot::yRight, title);

		if (curText == NONE_Y_AXIS_VARIABLE) {
			handler.plot->enableAxis(QwtPlot::yRight, false);
			for (auto it = handler.data.begin(); it != handler.data.end(); ++it) {
				it->curve2->detach();
			}
		}
		else {
			handler.plot->enableAxis(QwtPlot::yRight);
			for (auto it = handler.data.begin(); it != handler.data.end(); ++it) {
				it->curve2->attach(handler.plot);

				it->data[QwtPlot::yRight] = &it->container[curText];

				it->curve2->setSamples(new ListSeriesData(*it->data[QwtPlot::xBottom], *it->data[QwtPlot::yRight]));
				if (-1 != y2Combo->findText(it->curve2->title().text())) {
					it->curve2->setTitle(curText);
				}
			}
		}

		handler.plot->replot();
	};

	if (isManualMode) {
		plotHandler.plotTabConnections << CONNECT(advOptionsGroup, &QGroupBox::toggled, [=](bool on) {
			advOptionsGroupFrame->setVisible(on);
		});
		plotHandler.plotTabConnections << CONNECT(potGalvModeChk, &QCheckBox::stateChanged, [=](int state) {
			switch (state) {
			case Qt::Unchecked:
				potGalvModeChk->setText(POTENTIOSTATIC_TEXT);
				appliedCurLblLeft->hide();
				appliedCurLblRight->hide();
				appliedCurLed->hide();
				appliedPotLblLeft->show();
				appliedPotLblRight->show();
				appliedPotLed->show();
				mw->SetManualPotentioSetpoint(id, appliedPotLed->text().toDouble());
				break;

			case Qt::Checked:
				potGalvModeChk->setText(GALVANOSTATIC_TEXT);
				appliedCurLblLeft->show();
				appliedCurLblRight->show();
				appliedCurLed->show();
				appliedPotLblLeft->hide();
				appliedPotLblRight->hide();
				appliedPotLed->hide();
				mw->SetManualGalvanoSetpoint(id, appliedCurLed->text().toDouble(), appliedCurLblRight->currentText());
				break;
			}
		});
		plotHandler.plotTabConnections << CONNECT(openCircuitModeChk, &QCheckBox::stateChanged, [=](int state) {
			switch (state) {
			case Qt::Unchecked:
				openCircuitModeChk->setText(OCP_MODE_TEXT);
				mw->SetManualOcp(id);
				break;

			case Qt::Checked:
				openCircuitModeChk->setText(CELL_ON_TEXT);
				if (potGalvModeChk->isChecked()) {
					mw->SetManualGalvanoSetpoint(id, appliedCurLed->text().toDouble(), appliedCurLblRight->currentText());
				}
				else {
					mw->SetManualPotentioSetpoint(id, appliedPotLed->text().toDouble());
				}
				break;
			}
		});
		plotHandler.plotTabConnections << CONNECT(rangeCombo, &QComboBox::currentTextChanged, [=]() {
			mw->SetCurrentRangingMode(id, rangeCombo->currentData().toUInt());
		});
		plotHandler.plotTabConnections << CONNECT(appliedPotLed, &QLineEdit::editingFinished, [=]() {
			mw->SetManualPotentioSetpoint(id, appliedPotLed->text().toDouble());
		});
		plotHandler.plotTabConnections << CONNECT(appliedCurLed, &QLineEdit::editingFinished, [=]() {
			mw->SetManualGalvanoSetpoint(id, appliedCurLed->text().toDouble(), appliedCurLblRight->currentText());
		});
		plotHandler.plotTabConnections << CONNECT(appliedCurLblRight, &QComboBox::currentTextChanged, [=]() {
			mw->SetManualGalvanoSetpoint(id, appliedCurLed->text().toDouble(), appliedCurLblRight->currentText());
		});
		plotHandler.plotTabConnections << CONNECT(samplingIntLed, &QLineEdit::editingFinished, [=]() {
			mw->SetManualSamplingParams(id, samplingIntLed->text().toDouble());
		});
		plotHandler.plotTabConnections << CONNECT(startManualExpPbt, &QPushButton::clicked, [=]() {
			mw->StartManualExperiment(id);
			mw->UpdateCurrentExperimentState();

			mw->SetManualSamplingParams(id, samplingIntLed->text().toDouble());

			if (potGalvModeChk->isChecked()) {
				mw->SetManualGalvanoSetpoint(id, appliedCurLed->text().toDouble(), appliedCurLblRight->currentText());
			}
			else {
				mw->SetManualPotentioSetpoint(id, appliedPotLed->text().toDouble());
			}
		});
		plotHandler.plotTabConnections << CONNECT(pauseManualExpPbt, &QPushButton::clicked, [=]() {
			if (pauseManualExpPbt->text() == PAUSE_EXP_BUTTON_TEXT) {
				mw->PauseManualExperiment(id);
				mw->UpdateCurrentExperimentState();
			}
			else {
				mw->ResumeManualExperiment(id);
				mw->UpdateCurrentExperimentState();

				mw->SetManualSamplingParams(id, samplingIntLed->text().toDouble());

				if (potGalvModeChk->isChecked()) {
					mw->SetManualGalvanoSetpoint(id, appliedCurLed->text().toDouble(), appliedCurLblRight->currentText());
				}
				else {
					mw->SetManualPotentioSetpoint(id, appliedPotLed->text().toDouble());
				}
			}
		});
		plotHandler.plotTabConnections << CONNECT(stopManualExpPbt, &QPushButton::clicked, [=]() {
			mw->StopManualExperiment(id);
			mw->UpdateCurrentExperimentState();
		});

		plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::CurrentHardwareBusy, [=]() {
			pauseManualExpPbt->show();
			stopManualExpPbt->show();
			startManualExpPbt->hide();

			advOptionsGroupFrame->setEnabled(true);
		});
		plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::CurrentExperimentResumed, [=]() {
			pauseManualExpPbt->setText(PAUSE_EXP_BUTTON_TEXT);
		});

		plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::CurrentExperimentPaused, [=]() {
			pauseManualExpPbt->setText(RESUME_EXP_BUTTON_TEXT);
		});

		plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::CurrentExperimentCompleted, [=]() {
			pauseManualExpPbt->hide();
			stopManualExpPbt->hide();
			startManualExpPbt->show();

			advOptionsGroupFrame->setEnabled(true);
		});

		plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::CurrentExperimentIsNotManual, [=]() {
			pauseManualExpPbt->hide();
			stopManualExpPbt->hide();
			startManualExpPbt->hide();

			advOptionsGroupFrame->setEnabled(false);
		});

		auto setManualStartParams = [=](const StartExperimentParameters &startParams) {
			PlotHandler &handler(dataTabs.plots[startParams.id][startParams.type]);

			for (auto it = handler.data.begin(); it != handler.data.end(); ++it) {
				it->curve1->detach();
				delete it->curve1;
				it->curve2->detach();
				delete it->curve2;
			}
			handler.data.clear();
			
			handler.exp = startParams.exp;
			handler.plotCounter.stamp = 0;
			handler.plotCounter.realTimeValueStamp = 0;
			handler.data << DataMapVisualization();

			DataMapVisualization &currentData(handler.data.first());

			currentData.saveFile = startParams.file;
			currentData.filePath = QFileInfo(startParams.file->fileName()).absoluteFilePath();
			currentData.curve1 = CreateCurve(QwtPlot::yLeft, DEFAULT_MAJOR_CURVE_COLOR);;
			currentData.curve2 = CreateCurve(QwtPlot::yRight, DEFAULT_MINOR_CURVE_COLOR);;
			currentData.cal = startParams.cal;
			currentData.hwVer = startParams.hwVer;
			currentData.notes = startParams.notes;
			
			currentData.data[QwtPlot::xBottom] = &currentData.container[handler.varCombo[QwtPlot::xBottom]->currentText()];
			currentData.data[QwtPlot::yLeft] = &currentData.container[handler.varCombo[QwtPlot::yLeft]->currentText()];
			currentData.data[QwtPlot::yRight] = &currentData.container[handler.varCombo[QwtPlot::yRight]->currentText()];

			currentData.curve1->setSamples(new ListSeriesData(*currentData.data[QwtPlot::xBottom], *currentData.data[QwtPlot::yLeft]));
			currentData.curve2->setSamples(new ListSeriesData(*currentData.data[QwtPlot::xBottom], *currentData.data[QwtPlot::yRight]));

			currentData.curve1->setTitle(handler.varCombo[QwtPlot::yLeft]->currentText());
			currentData.curve2->setTitle(handler.varCombo[QwtPlot::yRight]->currentText());

			currentData.curve1->attach(handler.plot);

			if (handler.varCombo[QwtPlot::yRight]->currentText() != NONE_Y_AXIS_VARIABLE) {
				currentData.curve2->attach(handler.plot);
			}
		};
		plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::SetManualStartParams, setManualStartParams);
	}

	plotHandler.plotTabConnections <<
	CONNECT(mw, &MainWindow::ExperimentNodeBeginning, [=](const QUuid &curId, const ExperimentNode_t &node) {
		if (curId != id) {
			return;
		}

		if (!dataTabs.plots.keys().contains(id)) {
			return;
		}

		//Question: how do I know which item in the "data" QList is the right one?
		/* <Matt/> */
		//dataTabs.plots[id][ET_AC].data.first().currentNode = node;
		/* </Matt> */

		QString nodeTypeStr = "";

		switch (node.nodeType) {
			NODE_TYPE_STR_FILL(END_EXPERIMENT_NODE, "Experiment complete")
			NODE_TYPE_STR_FILL(DCNODE_OCP, "Open circuit")
			NODE_TYPE_STR_FILL(DCNODE_SWEEP_POT, "Potential sweep")
			NODE_TYPE_STR_FILL(DCNODE_SWEEP_GALV, "Current sweep")
			NODE_TYPE_STR_FILL(DCNODE_POINT_POT, "Fixed potential")
			NODE_TYPE_STR_FILL(DCNODE_POINT_GALV, "Fixed current")
			NODE_TYPE_STR_FILL(DCNODE_NORMALPULSE_POT, "Potential pulse (normal)")
			NODE_TYPE_STR_FILL(DCNODE_NORMALPULSE_GALV, "Current pulse (normal)")
			NODE_TYPE_STR_FILL(DCNODE_DIFFPULSE_POT, "Potential pulse (differential)")
			NODE_TYPE_STR_FILL(DCNODE_DIFFPULSE_GALV, "Current pulse (differential)")
			NODE_TYPE_STR_FILL(DCNODE_SQRWAVE_POT, "Potential square wave")
			NODE_TYPE_STR_FILL(DCNODE_SQRWAVE_GALV, "Current square wave")
			NODE_TYPE_STR_FILL(DCNODE_SINEWAVE, "Potential sine wave")
			NODE_TYPE_STR_FILL(DCNODE_CONST_RESISTANCE, "Constant resistance")
			NODE_TYPE_STR_FILL(DCNODE_CONST_POWER, "Constant power")
			NODE_TYPE_STR_FILL(DCNODE_MAX_POWER, "Maximum power point")
			NODE_TYPE_STR_FILL(FRA_NODE_POT, "Potentiostatic impedance")
			NODE_TYPE_STR_FILL(FRA_NODE_GALV, "Galvanostatic impedance")
			NODE_TYPE_STR_FILL(FRA_NODE_PSEUDOGALV, "Pseudo-galvanostatic impedance")
			NODE_TYPE_STR_FILL(DUMMY_NODE, " ")

			default: break;
		}

		QString text = QString("%1%2%3").
			arg(OPEN_COLOR_TAG).
			arg(nodeTypeStr).
			arg(CLOSE_COLOR_TAG);
		dataTabs.realTimeLabels[id][QString(STEP_VALUE_LBL_NAME)]->setText(text);
	});

	auto dcDataArrivedHandler = [=](const QUuid &curId, const ExperimentalDcData &expData, ExperimentTrigger *trigger, bool paused) {
		if (curId != id) {
			return;
		}

		if (!dataTabs.plots.keys().contains(id)) {
			return;
		}

		if (!dataTabs.plots[id].contains(ET_DC)) {
			return;
		}

		PlotHandler &handler(dataTabs.plots[id][ET_DC]);
		DataMapVisualization &majorData(handler.data.first());

		auto curStamp = QDateTime::currentMSecsSinceEpoch();
		if (curStamp > handler.plotCounter.realTimeValueStamp) {
			DataMap container;
			if (!handler.exp) {
				return;
			}
			
			if (dataTabs.realTimeElapsedTime.keys().contains(curId)) {
				container[REAL_TIME_ELAPSED_TIME].data.push_back(dataTabs.realTimeElapsedTime[curId]);
			}
			
			handler.exp->PushNewDcData(expData, container, majorData.cal, majorData.hwVer, majorData.notes, trigger);
			
			if (!dataTabs.realTimeElapsedTime.keys().contains(curId) && container.contains(REAL_TIME_ELAPSED_TIME)) {
				dataTabs.realTimeElapsedTime[curId] = container[REAL_TIME_ELAPSED_TIME].data.last();
			}

			foreach(const QString &curVal, dataTabs.realTimeLabels[id].keys()) {
				if (container.keys().contains(curVal)) {
					QString text;

					if (valueDisplayHandlerPtr->keys().contains(curVal)) {
						text = (*valueDisplayHandlerPtr)[curVal](container[curVal].data.last());
					}
					else {
						text = QString("%1").arg(container[curVal].data.last(), 0, 'f', 6);
					}

					dataTabs.realTimeLabels[id][curVal]->setText(QString(OPEN_COLOR_TAG) + text + CLOSE_COLOR_TAG);
				}
			}
			handler.plotCounter.realTimeValueStamp = curStamp + 300;
		}
	};
	
	auto acDataArrivedHandler = [=](const QUuid &curId, const QByteArray &expData, ExperimentTrigger *trigger, bool paused) {
		if (curId != id) {
			return;
		}

		if (!dataTabs.plots.keys().contains(id)) {
			return;
		}

		if (!dataTabs.plots[id].contains(ET_AC)) {
			return;
		}

		PlotHandler &handler(dataTabs.plots[id][ET_AC]);
		DataMapVisualization &majorData(handler.data.first());

		auto curStamp = QDateTime::currentMSecsSinceEpoch();
		if (curStamp > handler.plotCounter.realTimeValueStamp) {
			DataMap container;
			if (!handler.exp) {
				return;
			}

			/* <Matt/> */
			/*
			ExperimentalAcData * data = (ExperimentalAcData *)expData.data();
			for (int i = 0; i < ADCacBUF_SIZE; i++) {
				majorData.accumulatingACdata.append(data->data[i]);
			}
			uint32_t dataCount = majorData.accumulatingACdata.count() / ADCacBUF_SIZE;
			if (dataCount >= majorData.currentNode.ACsamplingParams.numBufs) {
				handler.exp->PushNewAcData(data, majorData.accumulatingACdata.data(), majorData.currentNode.ACsamplingParams.numBufs,
					container, majorData.cal, majorData.hwVer, majorData.notes, trigger);
			}
			//*/
      ExperimentalAcData ACdataHeader = *((ExperimentalAcData *)expData.data());
      QByteArray ACDataOnly((const char *)(expData.data() + sizeof(ExperimentalAcData)), expData.count() - sizeof(ExperimentalAcData));
			handler.exp->PushNewAcData(ACdataHeader, ACDataOnly, 1, container, majorData.cal, majorData.hwVer, majorData.notes, trigger);
			/* </Matt> */
			
			foreach(const QString &curVal, dataTabs.realTimeLabels[id].keys()) {
				if (container.keys().contains(curVal)) {
					QString text;

					if (valueDisplayHandlerPtr->keys().contains(curVal)) {
						text = (*valueDisplayHandlerPtr)[curVal](container[curVal].data.last());
					}
					else {
						text = QString("%1").arg(container[curVal].data.last(), 0, 'f', 6);
					}

					dataTabs.realTimeLabels[id][curVal]->setText(QString(OPEN_COLOR_TAG) + text + CLOSE_COLOR_TAG);
				}
			}
			handler.plotCounter.realTimeValueStamp = curStamp + 300;
		}
	};
  
	plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::AcDataArrived, acDataArrivedHandler);
	plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::DcDataArrived, dcDataArrivedHandler);

	plotHandler.plotTabConnections << CONNECT(realTimeGroup, &QGroupBox::toggled, [=](bool on) {
		realTimeGroupFrame->setVisible(on);
	});

	plotHandler.plotTabConnections << CONNECT(settingsGroup, &QGroupBox::toggled, [=](bool on) {
		settingsGroupFrame->setVisible(on);
	});

	plotHandler.plotTabConnections << CONNECT(openFilePbt, &QPushButton::clicked, [=]() {
		PlotHandler &handler(dataTabs.plots[id][type]);
		
		if (handler.data.size()) {
			QDesktopServices::openUrl(QUrl("file:///" + handler.data.first().filePath));
		}
	});

	plotHandler.plotTabConnections << CONNECT(zoomInPbt, &QPushButton::clicked, [=]() {
		PlotHandler &handler(dataTabs.plots[id][type]);
		ZoomAxis(handler, QwtPlot::xBottom, 0.1);
		ZoomAxis(handler, QwtPlot::yLeft, 0.1);
		ZoomAxis(handler, QwtPlot::yRight, 0.1);

		bool needReplot = false;
		needReplot |= ApplyNewAxisParams(QwtPlot::xBottom, handler);
		needReplot |= ApplyNewAxisParams(QwtPlot::yLeft, handler);
		needReplot |= ApplyNewAxisParams(QwtPlot::yRight, handler);

		if (needReplot) {
			plot->replot();
		}
	});

	plotHandler.plotTabConnections << CONNECT(zoomOutPbt, &QPushButton::clicked, [=]() {
		PlotHandler &handler(dataTabs.plots[id][type]);
		ZoomAxis(handler, QwtPlot::xBottom, -0.1);
		ZoomAxis(handler, QwtPlot::yLeft, -0.1);
		ZoomAxis(handler, QwtPlot::yRight, -0.1);

		bool needReplot = false;
		needReplot |= ApplyNewAxisParams(QwtPlot::xBottom, handler);
		needReplot |= ApplyNewAxisParams(QwtPlot::yLeft, handler);
		needReplot |= ApplyNewAxisParams(QwtPlot::yRight, handler);

		if (needReplot) {
			plot->replot();
		}
	});

	#define PROPERTY_PAN_VIEW_PRESSED			"pan-view-pressed"
	#define PROPERTY_ZOOM_TO_SELECTION_PRESSED	"zoom-to-selection-pressed"
	plotHandler.plotTabConnections << CONNECT(panViewPbt, &QPushButton::clicked, [=]() {
		plotOverlay->setProperty(PROPERTY_PAN_VIEW_PRESSED, true);
		plotOverlay->setProperty(PROPERTY_ZOOM_TO_SELECTION_PRESSED, false);
	});
	plotHandler.plotTabConnections << CONNECT(zoomToSelectionPbt, &QPushButton::clicked, [=]() {
		plotOverlay->setProperty(PROPERTY_PAN_VIEW_PRESSED, false);
		plotOverlay->setProperty(PROPERTY_ZOOM_TO_SELECTION_PRESSED, true);
	});

	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->setPen(QColor("#7f4d565f"), 1, Qt::DashLine);
	grid->attach(plot);
	grid->hide();

	auto titleLabel = plot->titleLabel();
	QLineEdit *titleLed = new QLineEdit(titleLabel);
	titleLed->setGeometry(0, 0, titleLabel->width(), titleLabel->height());
	titleLed->setText(plot->title().text());
	titleLed->hide();

	plotHandler.plotTabConnections << CONNECT(showGridlinesPbt, &QPushButton::clicked, [=]() {
		if (grid->isVisible()) {
			grid->hide();
		}
		else {
			grid->show();
		}
		plot->replot();
	});

	plotHandler.plotTabConnections << CONNECT(resetZoomPbt, &QPushButton::clicked, [=]() {
		PlotHandler &handler(dataTabs.plots[id][type]);
		ResetAxis(handler, QwtPlot::xBottom);
		ResetAxis(handler, QwtPlot::yLeft);
		ResetAxis(handler, QwtPlot::yRight);

		ApplyNewAxisParams(QwtPlot::xBottom, handler);
		ApplyNewAxisParams(QwtPlot::yLeft, handler);
		ApplyNewAxisParams(QwtPlot::yRight, handler);

		plot->replot();
	});

	#define PROPERTY_MOUSE_MOVE_PRESSED			"mouse-move-pressed"
	#define PROPERTY_MOUSE_MOVE_PRESSED_POINT	"mouse-move-pressed-point"
	#define PROPERTY_MOUSE_GRAB_PRESSED			"mouse-grab-pressed"
	#define PROPERTY_MOUSE_GRAB_PRESSED_POINT	"mouse-grab-pressed-point"
	#define PROPERTY_MOUSE_GRAB_END_POINT		"mouse-grab-end-point"
	#define PROPERTY_MOUSE_CURVE_POINT			"mouse-curve-point"
	#define PROPERTY_MOUSE_CURVE_POINT_VALID	"mouse-curve-point-valid"
	#define PROPERTY_MOUSE_CURVE_POINT_COLOR	"mouse-curve-point-color"
	#define PROPERTY_MOUSE_CURVE_POINT_WIDTH	"mouse-curve-point-width"

	plotOverlay->setAttribute(Qt::WA_Hover, true);
	plotOverlay->setProperty(PROPERTY_PAN_VIEW_PRESSED, false);
	plotOverlay->setProperty(PROPERTY_ZOOM_TO_SELECTION_PRESSED, false);
	plotOverlay->setProperty(PROPERTY_MOUSE_MOVE_PRESSED, false);
	plotOverlay->setProperty(PROPERTY_MOUSE_MOVE_PRESSED_POINT, QPoint(0, 0));
	plotOverlay->setProperty(PROPERTY_MOUSE_GRAB_PRESSED, false);
	plotOverlay->setProperty(PROPERTY_MOUSE_GRAB_PRESSED_POINT, QPoint(0, 0));
	plotOverlay->setProperty(PROPERTY_MOUSE_GRAB_END_POINT, QPoint(0, 0));
	plotOverlay->setProperty(PROPERTY_MOUSE_CURVE_POINT, QPointF(0, 0));
	plotOverlay->setProperty(PROPERTY_MOUSE_CURVE_POINT_VALID, false);
	plotOverlay->setProperty(PROPERTY_MOUSE_CURVE_POINT_COLOR, QString(""));
	plotOverlay->setProperty(PROPERTY_MOUSE_CURVE_POINT_WIDTH, 5.0);

	plotOverlay->installEventFilter(new PlotDragOverlayEventFilter(plotOverlay, [=](QObject *obj, QEvent *e) -> bool {
		bool ret = false;

		auto pressed = obj->property(PROPERTY_MOUSE_MOVE_PRESSED).toBool();
		auto startPoint = obj->property(PROPERTY_MOUSE_MOVE_PRESSED_POINT).toPoint();
		auto grabbed = obj->property(PROPERTY_MOUSE_GRAB_PRESSED).toBool();
		auto grabbedPoint = obj->property(PROPERTY_MOUSE_GRAB_PRESSED_POINT).toPoint();
		auto grabbedEndPoint = obj->property(PROPERTY_MOUSE_GRAB_END_POINT).toPoint();
		auto curvePointValid = obj->property(PROPERTY_MOUSE_CURVE_POINT_VALID).toBool();
		auto curvePoint = obj->property(PROPERTY_MOUSE_CURVE_POINT).toPointF();
		auto pointColor = QColor(obj->property(PROPERTY_MOUSE_CURVE_POINT_COLOR).toString());
		auto penWidth = obj->property(PROPERTY_MOUSE_CURVE_POINT_WIDTH).toDouble();
		auto panViewPressed = obj->property(PROPERTY_PAN_VIEW_PRESSED).toBool();
		auto zoomToSelectionPressed = obj->property(PROPERTY_ZOOM_TO_SELECTION_PRESSED).toBool();

		QWidget *w = qobject_cast<QWidget*>(obj);

		switch (e->type()) {
		case QEvent::HoverEnter:
			ret = true;
			break;
		case QEvent::HoverLeave:
			ret = true;
			break;
		case QEvent::HoverMove:
			if (!pressed && !grabbed) {
				double dist = 100000;
				auto pos = ((QMouseEvent*)e)->pos();
				QwtPlot::Axis yAxis;

				PlotHandler &handler(dataTabs.plots[id][type]);


				QPointF curvePointSample;
				curvePointValid = false;
				foreach(auto &curData, handler.data) {
					double curDist;
					auto index = curData.curve1->closestPoint(pos, &curDist);

					if ( (-1 != index) && (curDist <= 10.) && (curDist < dist) ) {
						dist = curDist;
						yAxis = QwtPlot::yLeft;

						auto sym = curData.curve1->symbol();
						if (sym) {
							penWidth = sym->size().width();
							if (penWidth < 5.0) {
								penWidth = 5.0;
							}
						}
						else {
							penWidth = 5.0;
						}

						curvePointSample = curData.curve1->sample(index);
						pointColor = curData.curve1->pen().color();
						curvePoint = QPoint(handler.plot->transform(QwtPlot::xBottom, curvePointSample.x()) + 1,
							handler.plot->transform(yAxis, curvePointSample.y()) + 1);

						curvePointValid = true;
					}
					
					index = curData.curve2->closestPoint(pos, &curDist);

					if ((-1 != index) && (curDist <= 10.) && (curDist < dist)) {
						dist = curDist;
						yAxis = QwtPlot::yRight;

						auto sym = curData.curve2->symbol();
						if (sym) {
							penWidth = sym->size().width();
							if (penWidth < 5.0) {
								penWidth = 5.0;
							}
						}
						else {
							penWidth = 5.0;
						}

						curvePointSample = curData.curve2->sample(index);
						pointColor = curData.curve2->pen().color();
						curvePoint = QPoint(handler.plot->transform(QwtPlot::xBottom, curvePointSample.x()) + 1,
							handler.plot->transform(yAxis, curvePointSample.y()) + 1);

						curvePointValid = true;
					}
				}
				
				if (curvePointValid) {
					auto tooltipText = QString("(%1; %2)").arg(curvePointSample.x(), 0, 'e', 3).arg(curvePointSample.y(), 0, 'e', 3);
					//plotOverlay->setToolTip();
					QToolTip::showText(plotOverlay->mapToGlobal(pos), tooltipText, plotOverlay, QRect(), 0);
				}
				else {
					plotOverlay->setToolTip(QString());
				}

				if (w) {
					w->update();
				}
			}
			else {
				auto prevCurvePointValid = curvePointValid;
				curvePointValid = false;

				if ((prevCurvePointValid != curvePointValid) && w) {
					w->update();
				}
			}

			break;

		case QEvent::MouseButtonPress:
			titleLed->hide();

			if( !grabbed && panViewPressed ) {
				pressed = true;
				startPoint = ((QMouseEvent*)e)->pos();
				ret = true;
			}
			if( !pressed && zoomToSelectionPressed ) {
				grabbed = true;
				grabbedPoint = ((QMouseEvent*)e)->pos();
				grabbedEndPoint = grabbedPoint;
				ret = true;
			}
			/*
			if (grabbed && (((QMouseEvent*)e)->button() == Qt::RightButton)) {
				grabbed = false;

				if (w) {
					w->update();
				}

				ret = true;
			}
			//*/
			break;

		case QEvent::MouseMove:
			if (pressed) {
				auto curPos = ((QMouseEvent*)e)->pos();

				QLine line(startPoint, curPos);
				startPoint = curPos;

				PlotHandler &handler(dataTabs.plots[id][type]);
				MoveAxis(handler, QwtPlot::xBottom, line.dx());
				MoveAxis(handler, QwtPlot::yLeft, -line.dy());
				MoveAxis(handler, QwtPlot::yRight, -line.dy());

				bool needReplot = false;
				needReplot |= ApplyNewAxisParams(QwtPlot::xBottom, handler);
				needReplot |= ApplyNewAxisParams(QwtPlot::yLeft, handler);
				needReplot |= ApplyNewAxisParams(QwtPlot::yRight, handler);

				if (needReplot) {
					plot->replot();
				}

				ret = true;
			}
			if (grabbed) {
				grabbedEndPoint = ((QMouseEvent*)e)->pos();
				
				if (w) {
					w->update();
				}

				ret = true;
			}
			break;

		case QEvent::MouseButtonRelease:
			if (pressed && panViewPressed) {
				pressed = false;
				ret = true;
			}
			if (grabbed && zoomToSelectionPressed) {
				grabbed = false;

				if ((grabbedPoint.x() == grabbedEndPoint.x()) && (grabbedPoint.y() == grabbedEndPoint.y()) ) {
					break;
				}
				
				if (w) {
					w->update();
				}
				
				PlotHandler &handler(dataTabs.plots[id][type]);
				auto rect = w->rect();

				auto topLeft = QPoint(qMin(grabbedPoint.x(), grabbedEndPoint.x()), qMin(grabbedPoint.y(), grabbedEndPoint.y()));
				auto bottomRight = QPoint(qMax(grabbedPoint.x(), grabbedEndPoint.x()), qMax(grabbedPoint.y(), grabbedEndPoint.y()));

				double xMinPers = (double)topLeft.x() / rect.width();
				double xMaxPers = (double)(rect.width() - bottomRight.x()) / rect.width();
				double yMinPers = (double)(rect.height() - bottomRight.y()) / rect.height();
				double yMaxPers = (double)topLeft.y() / rect.height();

				ZoomAxis(handler, QwtPlot::xBottom, xMinPers, xMaxPers);
				ZoomAxis(handler, QwtPlot::yLeft, yMinPers, yMaxPers);
				ZoomAxis(handler, QwtPlot::yRight, yMinPers, yMaxPers);

				bool needReplot = false;
				needReplot |= ApplyNewAxisParams(QwtPlot::xBottom, handler);
				needReplot |= ApplyNewAxisParams(QwtPlot::yLeft, handler);
				needReplot |= ApplyNewAxisParams(QwtPlot::yRight, handler);

				if (needReplot) {
					handler.plot->replot();
				}

				ret = true;
			}
			break;

		case QEvent::Paint:
			if (curvePointValid && w) {
				QPainter painter(w);
				painter.setRenderHint(QPainter::Antialiasing, true);

				auto pen = QPen(QColor("#4d565f"), 2, Qt::SolidLine);
				painter.setPen(pen);
				painter.setBrush(pointColor);
				painter.drawEllipse(curvePoint, penWidth, penWidth);

				ret = true;
			}
			if (grabbed && w) {
				QPainter painter(w);

				auto pen = QPen(QColor("#4d565f"), 2, Qt::DashLine);
				painter.setPen(pen);

				painter.drawRect(QRect(grabbedPoint, grabbedEndPoint));

				ret = true;
			}
			break;
		}

		obj->setProperty(PROPERTY_MOUSE_MOVE_PRESSED_POINT, startPoint);
		obj->setProperty(PROPERTY_MOUSE_MOVE_PRESSED, pressed);
		obj->setProperty(PROPERTY_MOUSE_GRAB_PRESSED_POINT, grabbedPoint);
		obj->setProperty(PROPERTY_MOUSE_GRAB_END_POINT, grabbedEndPoint);
		obj->setProperty(PROPERTY_MOUSE_GRAB_PRESSED, grabbed);
		obj->setProperty(PROPERTY_MOUSE_CURVE_POINT, curvePoint);
		obj->setProperty(PROPERTY_MOUSE_CURVE_POINT_VALID, curvePointValid);
		obj->setProperty(PROPERTY_MOUSE_CURVE_POINT_COLOR, pointColor.name());
		obj->setProperty(PROPERTY_MOUSE_CURVE_POINT_WIDTH, penWidth);
		obj->setProperty(PROPERTY_PAN_VIEW_PRESSED, panViewPressed);
		obj->setProperty(PROPERTY_ZOOM_TO_SELECTION_PRESSED, zoomToSelectionPressed);
		return ret;
	}));

	plot->axisWidget(QwtPlot::yLeft)->installEventFilter(new PlotEventFilter(w, [=]() {
		PlotHandler &handler(dataTabs.plots[id][type]);
		
		if (GetNewAxisParams(mw, handler.axisParams[QwtPlot::yLeft])) {
			if (ApplyNewAxisParams(QwtPlot::yLeft, handler)) {
				plot->replot();
			}
		}
	}));

	plot->axisWidget(QwtPlot::yRight)->installEventFilter(new PlotEventFilter(w, [=]() {
		PlotHandler &handler(dataTabs.plots[id][type]);

		if (GetNewAxisParams(mw, handler.axisParams[QwtPlot::yRight])) {
			if (ApplyNewAxisParams(QwtPlot::yRight, handler)) {
				plot->replot();
			}
		}
	}));

	plot->axisWidget(QwtPlot::xBottom)->installEventFilter(new PlotEventFilter(w, [=]() {
		PlotHandler &handler(dataTabs.plots[id][type]);

		if (GetNewAxisParams(mw, handler.axisParams[QwtPlot::xBottom])) {
			if(ApplyNewAxisParams(QwtPlot::xBottom, handler)) {
				plot->replot();
			}
		}
	}));

	plot->legend()->installEventFilter(new LegendEventFilter(w, plot));

	plot->canvas()->installEventFilter(new PlotEventFilter(w, [=]() {
		if (0 == plot->legend()) {
			plot->insertLegend(new QwtLegend(), QwtPlot::TopLegend);
			plot->legend()->installEventFilter(new LegendEventFilter(w, plot));
			plot->replot();
		}
	}));

	titleLabel->installEventFilter(new PlotOverlayEventFilter(titleLabel, titleLed));
	titleLed->installEventFilter(new UniversalEventFilter(titleLed, [=](QObject *obj, QEvent *e) -> bool {
		bool ret = false;

		if (e->type() == QEvent::FocusOut) {
			titleLed->hide();
		}

		return ret;
	}));
	titleLabel->installEventFilter(new UniversalEventFilter(titleLabel, [=](QObject *obj, QEvent *e) -> bool {
		bool ret = false;

		if (e->type() == QEvent::MouseButtonPress) {
			titleLed->show();
			titleLed->raise();
			titleLed->setFocus();
			ret = true;
		}

		return ret;
	}));
	w->installEventFilter(new UniversalEventFilter(w, [=](QObject *obj, QEvent *e) -> bool {
		bool ret = false;

		if (e->type() == QEvent::MouseButtonPress) {
			titleLed->hide();
		}

		return ret;
	}));
	plotHandler.plotTabConnections << CONNECT(titleLed, &QLineEdit::textChanged, [=](const QString &str) {
		auto titleText = plot->title();
		titleText.setText(str);
		plot->setTitle(titleText);
	});

	plotHandler.plotTabConnections << CONNECT(xChkBox, &QCheckBox::stateChanged, [=](int state) {
		switch (state) {
		case Qt::Unchecked:
			xChkBox->setText(LINEAR_TEXT);
			plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine());
			break;

		case Qt::Checked:
			xChkBox->setText(LOGARITHMIC_TEXT);
			plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));
			break;
		}

		plot->replot();
	});
	plotHandler.plotTabConnections << CONNECT(y1ChkBox, &QCheckBox::stateChanged, [=](int state) {
		switch (state) {
		case Qt::Unchecked:
			y1ChkBox->setText(LINEAR_TEXT);
			plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine());
			break;

		case Qt::Checked:
			y1ChkBox->setText(LOGARITHMIC_TEXT);
			plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));
			break;
		}

		plot->replot();
	});
	plotHandler.plotTabConnections << CONNECT(y2ChkBox, &QCheckBox::stateChanged, [=](int state) {
		switch (state) {
		case Qt::Unchecked:
			y2ChkBox->setText(LINEAR_TEXT);
			plot->setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine());
			break;

		case Qt::Checked:
			y2ChkBox->setText(LOGARITHMIC_TEXT);
			plot->setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine(10));
			break;
		}

		plot->replot();
	});

	plotHandler.plotTabConnections << CONNECT(pauseExperiment, &QPushButton::clicked, [=]() {
		if (pauseExperiment->text() == PAUSE_EXP_BUTTON_TEXT) {
			mw->PauseExperiment(id);
		}
		else {
			mw->ResumeExperiment(id);
		}
	});

	plotHandler.plotTabConnections << CONNECT(stopExperiment, &QPushButton::clicked, [=]() {
		mw->StopExperiment(id);
	});

	plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::ExperimentResumed, [=](const QUuid &extId) {
		if (id != extId)
			return;

		pauseExperiment->setText(PAUSE_EXP_BUTTON_TEXT);
	});

	plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::ExperimentPaused, [=](const QUuid &extId) {
		if (id != extId)
			return;

		pauseExperiment->setText(RESUME_EXP_BUTTON_TEXT);
	});

	plotHandler.plotTabConnections << CONNECT(mw, &MainWindow::ExperimentCompleted, [=](const QUuid &extId) {
		if (id != extId)
			return;

		pauseExperiment->hide();
		stopExperiment->hide();
	});

	plotHandler.plotTabConnections << CONNECT(addDataPbt, &QPushButton::clicked, [=]() {
		QList<CsvFileData> csvDataList;

		if (!ReadCsvFile(mw, csvDataList)) {
			return;
		}

		PlotHandler &handler(dataTabs.plots[id][type]);
		QStringList firstDataKeys = handler.data.first().container.keys();
		firstDataKeys.removeAll(NONE_Y_AXIS_VARIABLE);
		firstDataKeys.sort();
		
		foreach(auto csvData, csvDataList) {
			QStringList curDataKeys = csvData.container.keys();
			curDataKeys.sort();

			if (curDataKeys != firstDataKeys) {
				LOG() << "Incompatible data sets were selected!";
				return;
			}
		}

		foreach(auto csvData, csvDataList) {
			DataMapVisualization data;
			data.container = csvData.container;
			data.saveFile = 0;
			data.curve1 = CreateCurve(QwtPlot::yLeft, DEFAULT_MAJOR_CURVE_COLOR);
			data.curve2 = CreateCurve(QwtPlot::yRight, DEFAULT_MINOR_CURVE_COLOR);
			data.name = csvData.fileName;

			handler.data << data;

			DataMapVisualization &currentData(handler.data.last());
			currentData.data[QwtPlot::xBottom] = &currentData.container[handler.varCombo[QwtPlot::xBottom]->currentText()];
			currentData.data[QwtPlot::yLeft] = &currentData.container[handler.varCombo[QwtPlot::yLeft]->currentText()];
			currentData.data[QwtPlot::yRight] = &currentData.container[handler.varCombo[QwtPlot::yRight]->currentText()];

			currentData.curve1->setSamples(new ListSeriesData(*currentData.data[QwtPlot::xBottom], *currentData.data[QwtPlot::yLeft]));
			currentData.curve2->setSamples(new ListSeriesData(*currentData.data[QwtPlot::xBottom], *currentData.data[QwtPlot::yRight]));

			currentData.curve1->setTitle(handler.varCombo[QwtPlot::yLeft]->currentText());
			currentData.curve2->setTitle(handler.varCombo[QwtPlot::yRight]->currentText());

			currentData.curve1->attach(handler.plot);

			if (handler.varCombo[QwtPlot::yRight]->currentText() != NONE_Y_AXIS_VARIABLE) {
				currentData.curve2->attach(handler.plot);
			}

			handler.plot->replot();
		}
	});

	plotHandler.plotTabConnections << CONNECT(editLinesPbt, &QPushButton::clicked, [=]() {
		PlotHandler &handler(dataTabs.plots[id][type]);
		QMap<QString, CurveParameters> currentParams;

		foreach(const DataMapVisualization &data, handler.data) {
			currentParams[data.name].curve[CurveParameters::PRIMARY].pen.color = data.curve1->pen().color();
			currentParams[data.name].curve[CurveParameters::PRIMARY].pen.width = data.curve1->pen().width();
			currentParams[data.name].curve[CurveParameters::PRIMARY].pen.style = data.curve1->pen().style();
			currentParams[data.name].curve[CurveParameters::PRIMARY].style = data.curve1->style();
			if (data.curve1->symbol()) {
				currentParams[data.name].curve[CurveParameters::PRIMARY].symbol.width = data.curve1->symbol()->size().width();
				currentParams[data.name].curve[CurveParameters::PRIMARY].symbol.style = data.curve1->symbol()->style();
			}
			else {
				currentParams[data.name].curve[CurveParameters::PRIMARY].symbol.width = 10;
				currentParams[data.name].curve[CurveParameters::PRIMARY].symbol.style = QwtSymbol::NoSymbol;
			}
			if (data.curve1->title().text() == y1Combo->currentText()) {
				currentParams[data.name].curve[CurveParameters::PRIMARY].title = "";
			}
			else {
				currentParams[data.name].curve[CurveParameters::PRIMARY].title = data.curve1->title().text();
			}

			currentParams[data.name].curve[CurveParameters::SECONDARY].pen.color = data.curve2->pen().color();
			currentParams[data.name].curve[CurveParameters::SECONDARY].pen.width = data.curve2->pen().width();
			currentParams[data.name].curve[CurveParameters::SECONDARY].pen.style = data.curve2->pen().style();
			currentParams[data.name].curve[CurveParameters::SECONDARY].style = data.curve2->style();
			if (data.curve2->symbol()) {
				currentParams[data.name].curve[CurveParameters::SECONDARY].symbol.width = data.curve2->symbol()->size().width();
				currentParams[data.name].curve[CurveParameters::SECONDARY].symbol.style = data.curve2->symbol()->style();
			}
			else {
				currentParams[data.name].curve[CurveParameters::SECONDARY].symbol.width = 10;
				currentParams[data.name].curve[CurveParameters::SECONDARY].symbol.style = QwtSymbol::NoSymbol;
			}
			if (data.curve2->title().text() == y2Combo->currentText()) {
				currentParams[data.name].curve[CurveParameters::SECONDARY].title = "";
			}
			else {
				currentParams[data.name].curve[CurveParameters::SECONDARY].title = data.curve2->title().text();
			}
		}

		if (GetNewPen(mw, currentParams, y1Combo->currentText(), y2Combo->currentText())) {
			foreach(const DataMapVisualization &data, handler.data) {
				data.curve1->setPen(currentParams[data.name].curve[CurveParameters::PRIMARY].pen.color,
					currentParams[data.name].curve[CurveParameters::PRIMARY].pen.width,
					currentParams[data.name].curve[CurveParameters::PRIMARY].pen.style);
				data.curve2->setPen(currentParams[data.name].curve[CurveParameters::SECONDARY].pen.color,
					currentParams[data.name].curve[CurveParameters::SECONDARY].pen.width,
					currentParams[data.name].curve[CurveParameters::SECONDARY].pen.style);
				data.curve1->setStyle(currentParams[data.name].curve[CurveParameters::PRIMARY].style);
				data.curve2->setStyle(currentParams[data.name].curve[CurveParameters::SECONDARY].style);

				if (currentParams[data.name].curve[CurveParameters::PRIMARY].title.isEmpty()) {
					data.curve1->setTitle(y1Combo->currentText());
				}
				else {
					data.curve1->setTitle(currentParams[data.name].curve[CurveParameters::PRIMARY].title);
				}

				if (currentParams[data.name].curve[CurveParameters::SECONDARY].title.isEmpty()) {
					data.curve2->setTitle(y2Combo->currentText());
				}
				else {
					data.curve2->setTitle(currentParams[data.name].curve[CurveParameters::SECONDARY].title);
				}

				auto symbol = new QwtSymbol(currentParams[data.name].curve[CurveParameters::PRIMARY].symbol.style);
				symbol->setPen(currentParams[data.name].curve[CurveParameters::PRIMARY].pen.color, 1, Qt::SolidLine);
				symbol->setColor(currentParams[data.name].curve[CurveParameters::PRIMARY].pen.color);
				symbol->setSize(currentParams[data.name].curve[CurveParameters::PRIMARY].symbol.width);
				data.curve1->setSymbol(symbol);

				symbol = new QwtSymbol(currentParams[data.name].curve[CurveParameters::SECONDARY].symbol.style);
				symbol->setPen(currentParams[data.name].curve[CurveParameters::SECONDARY].pen.color, 1, Qt::SolidLine);
				symbol->setColor(currentParams[data.name].curve[CurveParameters::SECONDARY].pen.color);
				symbol->setSize(currentParams[data.name].curve[CurveParameters::SECONDARY].symbol.width);
				data.curve2->setSymbol(symbol);
			}
			handler.plot->replot();
		}
	});

	plotHandler.plotTabConnections << CONNECT(savePlotPbt, &QPushButton::clicked, [=]() {
		QSettings settings(SQUID_STAT_PARAMETERS_INI, QSettings::IniFormat);
		QString dirName = settings.value(DATA_SAVE_PATH, "").toString();

		QString fileName = QFileDialog::getSaveFileName(mw, "Saving plot", dirName, "Image file (*.png)");

		if (fileName.isEmpty()) {
			return;
		}

		settings.setValue(DATA_SAVE_PATH, QFileInfo(fileName).absolutePath());

		plotOverlay->hide();
		auto marg = plot->contentsMargins();
		auto rect = plot->rect();
		auto topLeft = QPoint(marg.left(), marg.top());
		auto bottomRight = QPoint(rect.width() - marg.right(), rect.height() - marg.bottom());
		if (!plot->grab(QRect(topLeft, bottomRight)).save(fileName, "PNG")) {
			LOG() << "Error during saving plot into \"" << fileName << "\"";
		}
		plotOverlay->show();
	});

	plotHandler.plotTabConnections << CONNECT(xCombo, &QComboBox::currentTextChanged, [=](const QString &curText) {
		PlotHandler &handler(dataTabs.plots[id][type]);

		QwtText title;
		title.setFont(axisTitleFont);
		title.setText(curText);
		handler.plot->setAxisTitle(QwtPlot::xBottom, title);

		for (auto it = handler.data.begin(); it != handler.data.end(); ++it) {
			it->data[QwtPlot::xBottom] = &it->container[curText];
			it->curve1->setSamples(new ListSeriesData(*it->data[QwtPlot::xBottom], *it->data[QwtPlot::yLeft]));
			it->curve2->setSamples(new ListSeriesData(*it->data[QwtPlot::xBottom], *it->data[QwtPlot::yRight]));
		}
		handler.plot->replot();
	});

	plotHandler.plotTabConnections << CONNECT(y1Combo, &QComboBox::currentTextChanged, y1ComboHandler);
	plotHandler.plotTabConnections << CONNECT(y2Combo, &QComboBox::currentTextChanged, y2ComboHandler);

	dataTabs.plots[id][type] = plotHandler;
	DataMapVisualization &majorData(dataTabs.plots[id][type].data.first());

	if (loadedContainerPtr) {
		majorData.container = *loadedContainerPtr;
	}
	majorData.data[QwtPlot::xBottom] = &majorData.container[xCombo->currentText()];
	majorData.data[QwtPlot::yLeft] = &majorData.container[y1Combo->currentText()];
	majorData.data[QwtPlot::yRight] = &majorData.container[NONE_Y_AXIS_VARIABLE];
	majorData.curve1->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yLeft]));
	majorData.curve2->setSamples(new ListSeriesData(*majorData.data[QwtPlot::xBottom], *majorData.data[QwtPlot::yRight]));
	majorData.curve1->setTitle(y1Combo->currentText());
	majorData.curve2->setTitle(NONE_Y_AXIS_VARIABLE);
	majorData.name = expName;

	QwtText title;
	
	title.setFont(axisTitleFont);

	title.setText(xCombo->currentText());
	plot->setAxisTitle(QwtPlot::xBottom, title);

	//title.setText(QString("Impedance (`") + QChar(0x03a9) + QString(")"));
	title.setText(y1Combo->currentText());
	plot->setAxisTitle(QwtPlot::yLeft, title);

	title.setText(NONE_Y_AXIS_VARIABLE);
	plot->setAxisTitle(QwtPlot::yRight, title);

	zoomToSelectionPbt->click();

	return w;
}
