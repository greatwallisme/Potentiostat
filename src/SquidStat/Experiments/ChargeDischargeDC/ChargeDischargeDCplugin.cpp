#include "cyclicvoltammetry.h"

#include <QtCore/QtPlugin>
#include "ChargeDischargeDCplugin.h"


ChargeDischargeDCPlugin::ChargeDischargeDCPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void ChargeDischargeDCPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool ChargeDischargeDCPlugin::isInitialized() const
{
	return initialized;
}

QWidget *ChargeDischargeDCPlugin::createWidget(QWidget *parent)
{
	return new ChargeDischargeDC(parent);
}

QString ChargeDischargeDCPlugin::name() const
{
	return "ChargeDischargeDC";
}

QString ChargeDischargeDCPlugin::group() const
{
	return "My Plugins";
}

QIcon ChargeDischargeDCPlugin::icon() const
{
	return QIcon();
}

QString ChargeDischargeDCPlugin::toolTip() const
{
	return QString();
}

QString ChargeDischargeDCPlugin::whatsThis() const
{
	return QString();
}

bool ChargeDischargeDCPlugin::isContainer() const
{
	return false;
}

QString ChargeDischargeDCPlugin::domXml() const
{
	return "<widget class=\"ChargeDischargeDC\" name=\"ChargeDischargeDC\">\n"
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

QString ChargeDischargeDCPlugin::includeFile() const
{
	return "ChargeDischargeDC.h";
}

