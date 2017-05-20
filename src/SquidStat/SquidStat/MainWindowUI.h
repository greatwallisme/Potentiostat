#pragma once

class MainWindow;

class QWidget;
class QLabel;
class QComboBox;
class QPushButton;
class QEvent;
class QVBoxLayout;
class QGridLayout;

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

	QWidget* GetMainTabWidget();

	QWidget* GetOldSearchHardwareTab();
	QWidget* GetRunExperimentTab();

	QWidget* GetSearchHardwareWidget();
	QWidget* GetLogWidget();
	QWidget* GetPlotWidget();
	QWidget* GetControlButtonsWidget();
	
	struct {
		struct {
			struct {
				QLabel *icon;
				QLabel *fullName;
				QLabel *text;
			} descr;
			QGridLayout *paramsLay;
		} runExperiment;
	} ui;

	struct {
		InstrumentInfo instrumentInfo;
		quint8 channel;
	} currentInstrument;

	MainWindow *mw;
};