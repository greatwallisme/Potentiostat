#include "EISPotentiostatic.h"

#include <QtCore/QtPlugin>
#include "EISPotentiostaticplugin.h"


EISPotentiostaticPlugin::EISPotentiostaticPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void EISPotentiostaticPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool EISPotentiostaticPlugin::isInitialized() const
{
	return initialized;
}

QWidget *EISPotentiostaticPlugin::createWidget(QWidget *parent)
{
	return new EISPotentiostatic(parent);
}

QString EISPotentiostaticPlugin::name() const
{
	return "EISPotentiostatic";
}

QString EISPotentiostaticPlugin::group() const
{
	return "My Plugins";
}

QIcon EISPotentiostaticPlugin::icon() const
{
	return QIcon();
}

QString EISPotentiostaticplugin::toolTip() const
{
	return QString();
}

QString EISPotentiostaticPlugin::whatsThis() const
{
	return QString();
}

bool EISPotentiostaticPlugin::isContainer() const
{
	return false;
}

QString EISPotentiostaticPlugin::domXml() const
{
	return "<widget class=\"EISPotentiostatic\" name=\"EISPotentiostatic\">\n"
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

QString EISPotentiostaticPlugin::includeFile() const
{
	return "EISPotentiostatic.h";
}

