#include "Factory.h"
#include "CyclicVoltammetryElement.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new CyclicVoltammetryElement;
}