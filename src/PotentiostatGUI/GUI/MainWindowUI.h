#pragma once

class MainWindow;

class QWidget;
class QLabel;
class QComboBox;
class QPushButton;

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
				QPushButton *runControl;
				QPushButton *settings;
			} top;
			
			struct {
				QComboBox *selectChannel;
				QWidget *owner;
			} settings;

			struct {
				QLabel *ewe;
			} bottom;
		} cockpit;
	} ui;

	MainWindow *mw;
};