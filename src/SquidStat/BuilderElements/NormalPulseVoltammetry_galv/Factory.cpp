#include "Factory.h"

#include "NormalPulseVoltammetry_galv.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new NormalPulseVoltammetry_galv;
}