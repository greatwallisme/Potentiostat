#include "Factory.h"
#include "DCPotentialSweep.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new DCPotentialSweepElement;
}