#include "Factory.h"

#include "cyclicvoltammetry.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new CyclicVoltammetry;
}