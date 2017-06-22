#include "cyclicvoltammetry.h"

#include <QtCore/QtPlugin>
#include "normalpulsevoltammetryplugin.h"


NormalPulseVoltammetryPlugin::CyclicVoltammetryPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void NormalPulseVoltammetryPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool NormalPulseVoltammetryPlugin::isInitialized() const
{
	return initialized;
}

QWidget *NormalPulseVoltammetryPlugin::createWidget(QWidget *parent)
{
	return new NormalPulseVoltammetry(parent);
}

QString NormalPulseVoltammetryPlugin::name() const
{
	return "NormalPulseVoltammetry";
}

QString NormalPulseVoltammetryPlugin::group() const
{
	return "My Plugins";
}

QIcon NormalPulseVoltammetryPlugin::icon() const
{
	return QIcon();
}

QString NormalPulseVoltammetryPlugin::toolTip() const
{
	return QString();
}

QString NormalPulseVoltammetryPlugin::whatsThis() const
{
	return QString();
}

bool NormalPulseVoltammetryPlugin::isContainer() const
{
	return false;
}

QString NormalPulseVoltammetryPlugin::domXml() const
{
	return "<widget class=\"NoralPulseVoltammetry\" name=\"normalPulseVoltammetry\">\n"
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

QString NormalPulseVoltammetryPlugin::includeFile() const
{
	return "normalpulsevoltammetry.h";
}

