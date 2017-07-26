#include "Factory.h"

#include "DiffPulseVoltammetry.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new DiffPulseVoltammetry;
}