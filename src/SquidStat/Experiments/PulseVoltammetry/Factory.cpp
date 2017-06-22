#include "Factory.h"

#include "normalpulsevoltammetry.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new NormalPulseVoltammetry;
}