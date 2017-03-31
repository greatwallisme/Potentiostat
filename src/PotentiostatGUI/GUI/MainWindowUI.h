#pragma once

class MainWindow;

class QWidget;
class QLabel;
class QComboBox;
class QPushButton;
class QEvent;

#include <QObject>

class SettingsResizeEventFilter : public QObject {
	Q_OBJECT
public:
	SettingsResizeEventFilter(QObject *p) :
		QObject(p) {}

signals :
	void ResizeSettings();

protected:
	bool eventFilter(QObject *obj, QEvent *e);
};

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
	void InstallEventFilter();
	void ResizeSettings();

	struct {
		struct {
			struct {
				QLabel *channel;
				QPushButton *runControl;
				QPushButton *settings;
				QWidget *owner;
			} top;
			
			struct {
				QComboBox *selectChannel;
				QWidget *owner;
			} settings;
			struct {
				QWidget *owner;
			} plot;
			struct {
				QWidget *owner;
			} bottom;
			
			QWidget *owner;
		} cockpit;
	} ui;

	MainWindow *mw;
};