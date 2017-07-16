#pragma once

#include <QVariant>

#include <AbstractBuilderElement.h>

class BuilderElementFactoryInterface {
public:
	virtual ~BuilderElementFactoryInterface() {}
	virtual AbstractBuilderElement* CreateElement(const QVariant& = QVariant()) = 0;
};

Q_DECLARE_INTERFACE(BuilderElementFactoryInterface, "BuilderElementFactoryInterface")
