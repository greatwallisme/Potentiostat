#include "ACDataAnalyzer.h"

ComplexDataPoint_t ACDataAnalyzer::AnalyzeFRA(double frequency, int16_t * bufEWE, int16_t bufCurrent, double gainEWE, double gainI, uint16_t len)
{
	//TODO: add error statistics, THD

	double I_RealSum = 0;
	double I_ImagSum = 0;
	double WE_RealSum = 0;
	double WE_ImagSum = 0;

	for (int i = 0; i < len; i++)
	{
		double arg = (double)i / (double)len * 2 * PI;
		I_RealSum += bufCurrent[i] * cos(arg);
		I_ImagSum += bufCurrent[i] * sin(arg);
		WE_RealSum += bufEwe[i] * cos(arg);
		WE_ImagSum += bufEwe[i] * sin(arg);
	}

	double I_abs = sqrt(I_RealSum * I_RealSum + I_ImagSum * I_ImagSum);
	double WE_abs = sqrt(WE_RealSum * WE_RealSum + WE_ImagSum * WE_ImagSum);
	double I_phase = atan2(I_ImagSum, I_RealSum);
	double WE_phase = atan2(WE_ImagSum, WE_ImagSum);

	ComplexDataPoint_t dataPoint;
	dataPoint.frequency = frequency;
	dataPoint.ImpedanceMag = WE_abs / I_abs;
	dataPoint.phase = WE_phase - I_phase;
	dataPoint.ImpedanceReal = dataPoint.ImpedanceMag * cos(dataPoint.phase);
	dataPoint.ImpedanceImag = dataPoint.ImpedanceMag * sin(dataPoint.phase);

	return dataPoint;
}