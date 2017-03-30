#pragma once

class MainWindow;

class QWidget;
class QLabel;
class QComboBox;

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

	QWidget* CreateCockpitSettingsWidget();
	QWidget* CreateCockpitTopWidget();
	QWidget* CreateCockpitBottomWidget();
	QWidget* CreateCockpitPlot();

	void CreateCockpitLogic();

	struct {
		struct {
			struct {
				QLabel *channel;
			} top;
			struct {
				QComboBox *selectChannel;
			} settings;
		} cockpit;
	} ui;

	MainWindow *mw;
};