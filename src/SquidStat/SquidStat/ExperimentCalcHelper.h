#ifndef _EXPERIMENT_CALC_HELPER
#define _EXPERIMENT_CALC_HELPER

#include <ExperimentNode.h>
#include <ExternalStructures.h>
#include <global_typedefs.h>
#include <cal.h>
#include <math.h>
#include <qlist.h>

#define SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD (10 * MICROSECONDS)
#define SQUIDSTAT_PIC_MAX_ADCDC_BUF_SIZE 512
#define SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD (500 * MICROSECONDS)
#define SQUIDSTAT_TEENSY_MAX_ADCDC_BUF_SIZE 512
#define SQUIDSTAT_PIC_TIMER_CLK_SPEED 1e8
#define SQUIDSTAT_MAX_ADC_AC_BUF_SIZE 1024.0
#define HF_CUTOFF_VALUE 500
#define SIGNAL_GEN_RESOLUTION 1024
#define MIN_TICKS_FOR_USB_TRANSMISSION (80 * MILLISECONDS)
#define MAX_CURRENT 1.0e10

class ExperimentCalcHelperClass
{
public:
	static void GetSamplingParams_staticDAC(HardwareModel_t HWversion, ExperimentNode_t * pNode, double t_sample_period);
  static uint32_t GetSamplingParams_potSweep(HardwareModel_t HWversion, const cal_t * calData, ExperimentNode_t * pNode,
    double dEdt, double samplingInterval = 0);
  static uint32_t GetSamplingParams_galvSweep(HardwareModel_t HWversion, const cal_t * calData, ExperimentNode_t * pNode,
    double dIdt, currentRange_t currentRange, double samplingInterval = 0);
  static void GetSamplingParameters_pulse(HardwareModel_t HWversion, quint32 t_period, quint32 t_pulsewidth, ExperimentNode_t * pNode);
	static currentRange_t GetMinCurrentRange(HardwareModel_t HWversion, const cal_t * calData, double targetCurrent);
	static int16_t GetBINCurrent(const cal_t * calData, currentRange_t currentRange, double targetCurrent);
	static int16_t GetBINVoltage(const cal_t * calData, double targetVoltage);
  static ProcessedDCData ProcessDCDataPoint(const cal_t * calData, ExperimentalDcData rawData);
  static double GetUnitsMultiplier(QString units_str);

	/* AC methods */
  static currentRange_t GetMinCurrentRange_DACac(const cal_t * calData, double targetCurrentAmp);
	static ComplexDataPoint_t AnalyzeFRA(double frequency, int16_t * bufEWE, int16_t * bufCurrent, double gainEWE, double gainI, uint16_t len, double numCycles);
  static QList<double> calculateFrequencyList(double lowerFreq, double upperFreq, double pointsPerDecade);
  static void calcACSamplingParams(const cal_t * calData, ExperimentNode_t * pNode, double amplitude);
  static double calcNumberOfCycles(const ExperimentalAcData);
};


#endif	//_EXPERIMENT_CALC_HELPER