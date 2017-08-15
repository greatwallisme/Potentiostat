#include "Factory.h"

#include "DiffPulseVoltammetry_galv.h"

AbstractBuilderElement* Factory::CreateElement(const QVariant&) {
	return new DiffPulseVoltammetry_galv;
}