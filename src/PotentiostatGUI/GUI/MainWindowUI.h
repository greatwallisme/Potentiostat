#pragma once

class MainWindow;

class QWidget;

class MainWindowUI {
public:
	MainWindowUI(MainWindow *mainWindow);
	~MainWindowUI();

	void CreateUI();

private:
	QWidget* CreateButton();
	QWidget* CreatePlot();

	MainWindow *mw;
};