#include "Factory.h"

#include "NormalPulseVoltammetry.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new NormalPulseVoltammetry;
}