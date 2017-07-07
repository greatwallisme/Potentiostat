#include "Chronopotentiometry.h"

#include <QtCore/QtPlugin>
#include "Chronopotentiometryplugin.h"


ChronopotentiometryPlugin::ChronopotentiometryPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void ChronopotentiometryPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool ChronopotentiometryPlugin::isInitialized() const
{
	return initialized;
}

QWidget *ChronopotentiometryPlugin::createWidget(QWidget *parent)
{
	return new Chronopotentiometry(parent);
}

QString ChronopotentiometryPlugin::name() const
{
	return "Chronopotentiometry";
}

QString ChronopotentiometryPlugin::group() const
{
	return "My Plugins";
}

QIcon ChronopotentiometryPlugin::icon() const
{
	return QIcon();
}

QString ChronopotentiometryPlugin::toolTip() const
{
	return QString();
}

QString ChronopotentiometryPlugin::whatsThis() const
{
	return QString();
}

bool ChronopotentiometryPlugin::isContainer() const
{
	return false;
}

QString ChronopotentiometryPlugin::domXml() const
{
	return "<widget class=\"Chronopotentiometry\" name=\"Chronopotentiometry\">\n"
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

QString ChronopotentiometryPlugin::includeFile() const
{
	return "Chronopotentiometry.h";
}

