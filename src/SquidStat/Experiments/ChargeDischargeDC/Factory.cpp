#include "Factory.h"

#include "ChargeDischargeDC.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new ChargeDischargeDC;
}