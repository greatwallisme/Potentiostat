#include "Factory.h"

#include "ChargeDischargeDC2.h"

AbstractExperiment* Factory::CreateExperiment(const QVariant&) {
	return new ChargeDischargeDC2;
}