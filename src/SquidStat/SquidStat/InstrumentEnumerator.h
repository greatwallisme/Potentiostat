#pragma once

#include "InstrumentStructures.h"

class InstrumentEnumerator {
public:
	InstrumentEnumerator();

	InstrumentList FindInstruments();
	InstrumentList FindInstrumentsActive();

private:
};