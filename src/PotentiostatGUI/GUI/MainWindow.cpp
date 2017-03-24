#include "MainWindow.h"

#include <MainWindowUI.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new MainWindowUI(this))
{
    ui->CreateUI();
}
MainWindow::~MainWindow() {
    delete ui;
}

#include <QApplication>
#include <QFile>
#include <QFont>

void MainWindow::applyStyle() {
    QFile f("./GUI.css");
    if(!f.open(QIODevice::ReadOnly))
        return;

    qobject_cast<QApplication *>(QApplication::instance())->setStyleSheet(f.readAll());

    f.close();
}
