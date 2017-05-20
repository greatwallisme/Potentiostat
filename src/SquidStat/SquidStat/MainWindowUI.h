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
#include <QMap>

#include "ExternalStructures.h"
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

	QWidget* PrebuiltExpCreateGroupHeader(const ExperimentNode_t*);
	QWidget* PrebuiltExpCreateParamsInput(ExperimentNode_t*);
	void FillNodeParameters();

	struct {
		struct {
			struct {
			} descr;
		} runExperiment;
	} ui;

	struct {
		InstrumentInfo instrumentInfo;
		quint8 channel;
	} currentInstrument;

	struct SavedInputs {
		ExperimentNode_t *node;
		QMap<QString, QWidget*> input;
	};

	struct {
		QList<QWidget*> paramWidgets;
		QList<SavedInputs> inputsList;
	} prebuiltExperimentData;

	MainWindow *mw;
};