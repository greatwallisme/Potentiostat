#pragma once

#include "InstrumentStructures.h"
#include "ExternalStructures.h"

class InstrumentEnumerator {
public:
	InstrumentEnumerator();

	InstrumentList FindInstruments();
	InstrumentList FindInstrumentsActive();

	CalibrationData GetCalibrationData(const InstrumentInfo&, bool *ok = 0);

private:
};