#include "cyclicvoltammetry.h"

#include <QtCore/QtPlugin>
#include "diffpulsevoltammetryplugin.h"


DiffPulseVoltammetryPlugin::DiffPulseVoltammetryPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void DiffPulseVoltammetryPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool DiffPulseVoltammetryPlugin::isInitialized() const
{
	return initialized;
}

QWidget *DiffPulseVoltammetryPlugin::createWidget(QWidget *parent)
{
	return new CyclicVoltammetry(parent);
}

QString DiffPulseVoltammetryPlugin::name() const
{
	return "DiffPulseVoltammetry";
}

QString DiffPulseVoltammetryPlugin::group() const
{
	return "My Plugins";
}

QIcon DiffPulseVoltammetryPlugin::icon() const
{
	return QIcon();
}

QString DiffPulseVoltammetryPlugin::toolTip() const
{
	return QString();
}

QString DiffPulseVoltammetryPlugin::whatsThis() const
{
	return QString();
}

bool DiffPulseVoltammetryPlugin::isContainer() const
{
	return false;
}

QString DiffPulseVoltammetryPlugin::domXml() const
{
	return "<widget class=\"DiffPulseVoltammetry\" name=\"diffPulseVoltammetry\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>100</width>\n"
		"   <height>100</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QString DiffPulseVoltammetryPlugin::includeFile() const
{
	return "diffpulsevoltammetry.h";
}

