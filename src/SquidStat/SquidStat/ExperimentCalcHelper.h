#ifndef _EXPERIMENT_CALC_HELPER
#define _EXPERIMENT_CALC_HELPER

#include <ExperimentNode.h>
#include <ExternalStructures.h>
#include <global_typedefs.h>
#include <cal.h>
#include <math.h>
#include <qlist.h>

#define SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD 1000
#define SQUIDSTAT_PIC_MAX_ADCDC_BUF_SIZE 512
#define SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD 50000
#define SQUIDSTAT_TEENSY_MAX_ADCDC_BUF_SIZE 512
#define HF_CUTOFF_VALUE 500
#define SIGNAL_GEN_RESOLUTION 1024
#define MIN_TICKS_FOR_USB_TRANSMISSION (80 * 1e5)

class ExperimentCalcHelperClass
{
public:
	static void GetSamplingParams_staticDAC(HardwareModel_t HWversion, ExperimentNode_t * pNode, double t_sample_period);
  static void GetSamplingParams_potSweep(HardwareModel_t HWversion, const cal_t * calData, ExperimentNode_t * pNode, double dEdt);
	static currentRange_t GetCurrentRange(HardwareModel_t HWversion, const cal_t * calData, double targetCurrent);
	static int16_t GetBINCurrent(const cal_t * calData, currentRange_t currentRange, double targetCurrent);
	static int16_t GetBINVoltage(const cal_t * calData, double targetVoltage);
  static ProcessedDCData ProcessDCDataPoint(const cal_t * calData, ExperimentalDcData rawData);

	/* AC methods */
	static ComplexDataPoint_t AnalyzeFRA(double frequency, int16_t * bufEWE, int16_t * bufCurrent, double gainEWE, double gainI, uint16_t len);
  static QList<double> calculateFrequencyList(double lowerFreq, double upperFreq, double pointsPerDecade);
  static void calcACSamplingParams(const cal_t * calData, ExperimentNode_t * pNode, double amplitude);
};


#endif	//_EXPERIMENT_CALC_HELPER