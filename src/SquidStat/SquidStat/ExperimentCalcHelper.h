#ifndef _EXPERIMENT_CALC_HELPER
#define _EXPERIMENT_CALC_HELPER

#include <ExperimentNode.h>
#include <global_typedefs.h>
#include <cal.h>
#include <math.h>

#define SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD 1000
#define SQUIDSTAT_PIC_MAX_ADCDC_BUF_SIZE 512
#define SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD 50000
#define SQUIDSTAT_TEENSY_MAX_ADCDC_BUF_SIZE 512

class ExperimentCalcHelperClass
{
public:
	static void GetSamplingParams_staticDAC(HardwareModel_t HWversion, ExperimentNode_t * pNode, double t_sample_period);
	static currentRange_t GetCurrentRange(HardwareModel_t HWversion, const cal_t * calData, double targetCurrent);
	static int16_t GetCurrent(const cal_t * calData, currentRange_t currentRange, double targetCurrent);
	static int16_t GetVoltage(const cal_t * calData, double targetVoltage);

	/* AC methods */
	static ComplexDataPoint_t AnalyzeFRA(double frequency, int16_t * bufEWE, int16_t * bufCurrent, double gainEWE, double gainI, uint16_t len);
};


#endif	//_EXPERIMENT_CALC_HELPER