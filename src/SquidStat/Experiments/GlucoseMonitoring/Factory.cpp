#include "Factory.h"

#include "GlucoseMonitoring.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new GlucoseMonitoring;
}