#include "Factory.h"

#include "EISPotentiostatic.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new EISPotentiostatic;
}