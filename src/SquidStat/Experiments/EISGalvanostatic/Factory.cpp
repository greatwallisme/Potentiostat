#include "Factory.h"

#include "EISGalvanostatic.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new EISGalvanostatic;
}