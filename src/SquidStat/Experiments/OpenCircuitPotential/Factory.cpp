#include "Factory.h"

#include "OpenCircuit.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new OpenCircuit;
}