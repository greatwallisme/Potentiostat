#include "Factory.h"

#include "DCCurrentSweep.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new DCCurrentSweep;
}