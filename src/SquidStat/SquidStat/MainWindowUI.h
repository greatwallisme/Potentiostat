#pragma once

class MainWindow;

class QWidget;
class QLabel;
class QComboBox;
class QPushButton;
class QEvent;

#include <QObject>

#include "InstrumentStructures.h"

class MainWindowUI {
public:
	MainWindowUI(MainWindow *mainWindow);
	~MainWindowUI();

	void CreateUI();

private:
	void CreateCentralWidget();

	QWidget* GetApplyStyleButton();
	QWidget* GetSearchHardwareWidget();
	QWidget* GetLogWidget();
	QWidget* GetPlotWidget();
	QWidget* GetControlButtonsWidget();
	
	struct {
		;
	} ui;

	struct {
		InstrumentInfo instrumentInfo;
		quint8 channel;
	} currentInstrument;

	MainWindow *mw;
};