#include "Factory.h"

#include "ConstantPowerElement.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new ConstantPowerElement;
}