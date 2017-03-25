#pragma once

class MainWindow;

class QWidget;

class MainWindowUI {
public:
	MainWindowUI(MainWindow *mainWindow);
	~MainWindowUI();

	void CreateUI();

private:
	void CreateCentralWidget();

	QWidget* CreateCockpitModeWidget();
	QWidget* CreateExperimentBuilderWidget();
	QWidget* CreateOpenDataFileWidget();

	QWidget* CreateButton();
	QWidget* CreatePlot();

	MainWindow *mw;
};