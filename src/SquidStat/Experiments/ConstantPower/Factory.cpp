#include "Factory.h"

#include "ConstantPower.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new ConstantPower;
}