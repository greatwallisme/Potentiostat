#include "Factory.h"

#include "MaxPower.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new MaxPower;
}