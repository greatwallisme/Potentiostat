#pragma once

#include <BuilderElementFactoryInterface.h>

class Factory : public QObject, public BuilderElementFactoryInterface {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "BuilderElementFactoryInterface")
	Q_INTERFACES(BuilderElementFactoryInterface)

public:
	AbstractBuilderElement* CreateElement(const QVariant& = QVariant());
};