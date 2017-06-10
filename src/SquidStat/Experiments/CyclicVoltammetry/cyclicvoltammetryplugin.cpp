#include "cyclicvoltammetry.h"

#include <QtCore/QtPlugin>
#include "cyclicvoltammetryplugin.h"


CyclicVoltammetryPlugin::CyclicVoltammetryPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void CyclicVoltammetryPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool CyclicVoltammetryPlugin::isInitialized() const
{
	return initialized;
}

QWidget *CyclicVoltammetryPlugin::createWidget(QWidget *parent)
{
	return new CyclicVoltammetry(parent);
}

QString CyclicVoltammetryPlugin::name() const
{
	return "CyclicVoltammetry";
}

QString CyclicVoltammetryPlugin::group() const
{
	return "My Plugins";
}

QIcon CyclicVoltammetryPlugin::icon() const
{
	return QIcon();
}

QString CyclicVoltammetryPlugin::toolTip() const
{
	return QString();
}

QString CyclicVoltammetryPlugin::whatsThis() const
{
	return QString();
}

bool CyclicVoltammetryPlugin::isContainer() const
{
	return false;
}

QString CyclicVoltammetryPlugin::domXml() const
{
	return "<widget class=\"CyclicVoltammetry\" name=\"cyclicVoltammetry\">\n"
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

QString CyclicVoltammetryPlugin::includeFile() const
{
	return "cyclicvoltammetry.h";
}

