#include <QApplication>

#include "Log.h"

#include "InstrumentEnumerator.h"
#include "InstrumentOperator.h"
#include "ExternalStructures.h"

#include "MainWindow.h"

#include <QTimer>
#include <QtGlobal>


#include <QFile>
#include <ExperimentReader.h>

#include <QLocale>

int main(int argc, char *argv[]) {
	QLocale::setDefault(QLocale::system());
	 
	QApplication a(argc, argv);
	qInstallMessageHandler(LogMessageHandler);

	a.setWindowIcon(QIcon(":/GUI/Resources/Squidstat.png"));

	MainWindow w;

	w.showMaximized();

	return a.exec();
}
