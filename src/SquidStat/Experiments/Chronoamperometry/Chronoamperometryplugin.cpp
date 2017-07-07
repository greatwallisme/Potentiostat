#include "Chronoamperometry.h"

#include <QtCore/QtPlugin>
#include "Chronoamperometryplugin.h"


ChronoamperometryPlugin::ChronoamperometryPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void ChronoamperometryPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool ChronoamperometryPlugin::isInitialized() const
{
	return initialized;
}

QWidget *ChronoamperometryPlugin::createWidget(QWidget *parent)
{
	return new Chronoamperometry(parent);
}

QString ChronoamperometryPlugin::name() const
{
	return "Chronoamperometry";
}

QString ChronoamperometryPlugin::group() const
{
	return "My Plugins";
}

QIcon ChronoamperometryPlugin::icon() const
{
	return QIcon();
}

QString ChronoamperometryPlugin::toolTip() const
{
	return QString();
}

QString ChronoamperometryPlugin::whatsThis() const
{
	return QString();
}

bool ChronoamperometryPlugin::isContainer() const
{
	return false;
}

QString ChronoamperometryPlugin::domXml() const
{
	return "<widget class=\"Chronoamperometry\" name=\"Chronoamperometry\">\n"
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

QString ChronoamperometryPlugin::includeFile() const
{
	return "Chronoamperometry.h";
}

