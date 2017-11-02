#include "Factory.h"

#include "PhaseCalibration.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new PhaseCalibration;
}