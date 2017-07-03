#ifndef _AC_DATA_ANALYZER_H
#define _AC_DATA_ANALYZER_H

#define PI 3.14159265359

#include "ComplexDataPoint.h"
#include <math.h>
#include <cstdint>

class ACDataAnalyzer
{
public:
	ACDataAnalyzer();
	~ACDataAnalyzer();

	static ComplexDataPoint_t AnalyzeFRA(double frequency, int16_t * bufEWE, int16_t bufCurrent, double gainEWE, double gainI, uint16_t len);
private:

};

ACDataAnalyzer::ACDataAnalyzer()
{
}

ACDataAnalyzer::~ACDataAnalyzer()
{
}

#endif // _AC_DATA_ANALYZER_H
