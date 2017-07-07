#include "Factory.h"

#include "Chronoamperometry.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new Chronoamperometry;
}