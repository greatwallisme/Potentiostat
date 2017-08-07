#include "Factory.h"

#include "OpenCircuitElement.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new OpenCircuitElement;
}