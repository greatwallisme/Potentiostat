#include "Factory.h"

#include "diffpulsevoltammetry.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new DiffPulseVoltammetry;
}