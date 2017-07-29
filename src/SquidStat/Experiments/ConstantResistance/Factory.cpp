#include "Factory.h"

#include "ConstantResistance.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new ConstantResistance;
}