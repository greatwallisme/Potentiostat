#include "ExperimentCalcHelper.h"

void ExperimentCalcHelperClass::GetSamplingParams_staticDAC(HardwareModel_t HWversion, ExperimentNode_t * pNode, double t_sample_period)
{
	//TODO:
	//	-make sure that this doesn't calculate an ADCMult or DACMult too big for the hardware buffers
	//	-make sure that DAC doesn't repeat because of USB holdup (manipulate VStep)
	//	-make sure that overall sampling frequency isn't too high?
	//	-compare with other algorithms already written

	int dt_min = 1;
	switch (HWversion)
	{
	case PRIME:
	case EDGE:
	case PICO:
	case SOLO:
		dt_min = SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD;
		break;
	case PLUS_2_0:
	case SOLO_2_0:
	case PRIME_2_0:
		dt_min = SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD;
		break;
	default:
		break;
	}

	pNode->samplingParams.ADCTimerDiv = 0;
	pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.ADCBufferSizeOdd = 1;
	pNode->DCSweep_pot.VStep = 1;

	/* Minimize dt */		//todo: account for dt overflows
	uint32_t dt;
	do
	{
		dt = (uint32_t)(t_sample_period * 1.0e8 / pNode->samplingParams.ADCBufferSizeEven);
		if (dt / dt_min > 1)
		{
			pNode->samplingParams.ADCBufferSizeEven <<= 1;
		}
	} while (dt / dt_min > 1);
	pNode->samplingParams.ADCTimerPeriod = dt;
	pNode->samplingParams.ADCBufferSizeOdd = pNode->samplingParams.ADCBufferSizeEven;
	pNode->samplingParams.PointsIgnored = pNode->samplingParams.ADCBufferSizeEven / 2;
}

//void ChargeDischargeDC::getSamplingParameters(double sampling_interval, ExperimentNode_t * pNode) const
//{
//
//	//TODO: make sure that ADCMult and DACMult aren't too big for hardware buffers
//
//
//	/* This switch-case is a placeholder for calculating dt_min, which needs to be defined elsewhere*/
//	int dt_min = 1;
//	int HardwareVersion = 0;
//	switch (HardwareVersion)
//	{
//	case 0:
//		dt_min = 50000; //500 microseconds * 100 ticks/microsecond
//		break;
//	case 1:
//		dt_min = 500;	//5 microseconds * 100 ticks/microsecond
//		break;
//	default:
//		break;
//	}
//	pNode->samplingParams.PointsIgnored = 0;
//	pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd = 1;
//	pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.ADCBufferSizeOdd = 1;
//
//	/* 1) Minimize dt, maximize DACMult*/
//	uint64_t dt;
//	do
//	{
//		dt = (uint64_t)(sampling_interval * 1e8 / pNode->samplingParams.ADCBufferSizeEven);
//		if (dt / dt_min > 1)
//		{
//			if (pNode->samplingParams.ADCBufferSizeEven << 1 < DACdcBUF_SIZE)
//			{
//				pNode->samplingParams.ADCBufferSizeEven <<= 1;
//				pNode->samplingParams.ADCBufferSizeOdd <<= 1;
//			}
//			else
//			{
//				break;
//			}
//		}
//	} while (dt / dt_min > 1);
//
//	/* 3) Calculate ADCMult */
//	pNode->samplingParams.ADCTimerDiv = 0;
//	int timerDiv = 1;
//
//	while (dt / timerDiv > 2147483648)
//	{
//		pNode->samplingParams.ADCTimerDiv++;
//		timerDiv <<= 1;
//	}
//	pNode->samplingParams.ADCTimerPeriod = (uint32_t)dt;
//}

currentRange_t ExperimentCalcHelperClass::GetCurrentRange(HardwareModel_t HWversion, const cal_t * calData, double targetCurrent)
{
	int MaxCurrentRange;
	switch (HWversion)
	{
	case PRIME:
	case EDGE:
	case PICO:
	case SOLO:
		MaxCurrentRange = 3;
		break;
	case PLUS_2_0:
	case SOLO_2_0:
	case PRIME_2_0:
		MaxCurrentRange = 7;
		break;
	default:
		break;
	}

	int range = 0;
	int32_t currentBinary;

	while (true)
	{
		currentBinary = targetCurrent > 0 ? targetCurrent * calData->m_DACdcP_I[range] + calData->b_DACdc_I[range] : targetCurrent * calData->m_DACdcN_I[range] + calData->b_DACdc_I[range];
		if (ABS(currentBinary) < UNDERCURRENT_LIMIT)
		{
			if (range == MaxCurrentRange)
				break;
			else
			{
				range++;
				continue;
			}
		}
		else
			break;
	}

	return (currentRange_t)range;
}

int16_t ExperimentCalcHelperClass::GetCurrent(const cal_t * calData, currentRange_t currentRange, double targetCurrent)
{
	float slope = targetCurrent > 0 ? calData->m_DACdcP_I[(int)currentRange] : calData->m_DACdcN_I[(int)currentRange];
	int32_t binCurrent = (int32_t)targetCurrent * slope + calData->b_DACdc_I[(int)currentRange];
	binCurrent = MIN(MAX(binCurrent, -32768), 32767);
	return binCurrent;
}

int16_t ExperimentCalcHelperClass::GetVoltage(const cal_t * calData, double targetVoltage)
{
	float slope = targetVoltage > 0 ? calData->m_DACdcP_V : calData->m_DACdcN_V;
	int32_t binVolt = (int32_t)(targetVoltage * slope + calData->b_DACdc_V);
	binVolt = MIN(MAX(binVolt, -32768), 32767);
	return (int16_t)binVolt;
}

ComplexDataPoint_t ExperimentCalcHelperClass::AnalyzeFRA(double frequency, int16_t * bufEWE, int16_t * bufCurrent, double gainEWE, double gainI, uint16_t len)
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
		WE_RealSum += bufEWE[i] * cos(arg);
		WE_ImagSum += bufEWE[i] * sin(arg);
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