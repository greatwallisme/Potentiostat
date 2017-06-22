#ifndef DIFFPULSEVOLTAMMETRYPLUGIN_H
#define DIFFPULSEVOLTAMMETRYPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class DiffPulseVoltammetryPlugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface" FILE "diffpulsevoltammetryplugin.json")
	Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
	DiffPulseVoltammetryPlugin(QObject *parent = 0);

	bool isContainer() const;
	bool isInitialized() const;
	QIcon icon() const;
	QString domXml() const;
	QString group() const;
	QString includeFile() const;
	QString name() const;
	QString toolTip() const;
	QString whatsThis() const;
	QWidget *createWidget(QWidget *parent);
	void initialize(QDesignerFormEditorInterface *core);

private:
	bool initialized;
};

#endif // DIFFPULSEVOLTAMMETRYPLUGIN_H
