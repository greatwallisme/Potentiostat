#include "Factory.h"

#include "MaxPowerElement.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new MaxPowerElement;
}