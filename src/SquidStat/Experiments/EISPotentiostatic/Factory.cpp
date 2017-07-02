#include "Factory.h"

#include "EISPotentiostatic.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new EISPotentiostatic;
}