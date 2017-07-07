#include "Factory.h"

#include "Chronopotentiometry.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new Chronopotentiometry;
}