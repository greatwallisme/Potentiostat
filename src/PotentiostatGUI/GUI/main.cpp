#include <QApplication>

#include <QFile>
#include <QDir>

#include <QList>
#include <QString>
#include <QStringList>

#include <QStyleFactory>

#include <QMessageBox>

#include <MainWindow.h>

void LoadStyle() {
	QFile f("./GUI.css");
	if (f.open(QIODevice::ReadOnly)) {
		qobject_cast<QApplication*>(QApplication::instance())->setStyleSheet(f.readAll());
		f.close();
	}
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	LoadStyle();

	//*
	MainWindow w;
	/*/
	LoginScreen w;
	//*/

	w.show();

	return a.exec();
}
