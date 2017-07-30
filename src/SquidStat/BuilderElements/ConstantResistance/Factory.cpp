#include "Factory.h"

#include "ConstantResistanceElement.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new ConstantResistanceElement;
}