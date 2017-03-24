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
	QWidget *centralWidget = WDG();
	QVBoxLayout *lay = new QVBoxLayout(centralWidget);
	mw->setCentralWidget(centralWidget);

	lay->addWidget(CreateButton());
	lay->addWidget(CreatePlot());
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