#include "Factory.h"

#include "ExampleExperiment.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new ExampleExperiment;
}