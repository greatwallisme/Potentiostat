#include "Factory.h"

#include "LinearSweepVoltammetry.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new LinearSweepVoltammetry;
}