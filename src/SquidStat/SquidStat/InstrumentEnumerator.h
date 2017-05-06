#pragma once

#include "InstrumentStructures.h"

class InstrumentEnumerator {
public:
	InstrumentEnumerator();

	InstrumentList FindInstruments();
	InstrumentList FindInstrumentsActive();

	void GetCalibrationData(const InstrumentInfo&);

private:
};