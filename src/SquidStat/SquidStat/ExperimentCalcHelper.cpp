#define _USE_MATH_DEFINES

#include "ExperimentCalcHelper.h"
#include "Log.h"  //debugging only
#include <qdebug.h>  //debugging only

/* DC methods */

void ExperimentCalcHelperClass::GetSamplingParams_staticDAC(HardwareModel_t HWversion, ExperimentNode_t * pNode, double t_sample_period)
{
	int dt_min = 50000;
	switch (HWversion)
	{
	case PRIME:
	case EDGE:
	case PICO:
	case SOLO:
  case PLUS:
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

	/* Minimize dt */		//todo: account for dt overflows
	uint64_t dt;
  do
  {
    pNode->samplingParams.ADCBufferSizeEven <<= 1;
    pNode->samplingParams.ADCBufferSizeOdd <<= 1;
    dt = (uint64_t)(t_sample_period * 1.0e8 / pNode->samplingParams.ADCBufferSizeEven);
  } while (dt / dt_min > 1 && pNode->samplingParams.ADCBufferSizeEven < ADCdcBUF_SIZE);

  /* Make sure dt isn't too small */
  dt = MAX(dt_min, dt);

  /* Make sure dt isn't too big for uint32_t */
  pNode->samplingParams.ADCTimerDiv = 0;
  while (dt > 4294967295 - 5e5 && pNode->samplingParams.ADCTimerDiv < 7) //(5e5 accounts conservatively for loop time)
  {
    pNode->samplingParams.ADCTimerDiv++;
    dt >>= 1;
  }
  //TODO: insert a software multiplier here for really long periods dt. Make it hw model dependent?
  pNode->samplingParams.ADCTimerPeriod = (uint32_t)dt;
}

uint32_t ExperimentCalcHelperClass::GetSamplingParams_potSweep(HardwareModel_t HWversion, const cal_t * calData, ExperimentNode_t * pNode,
  double dEdt, double samplingInterval)
{
  if (dEdt == 0) return 1;

  uint32_t FilterSize = 1;
  int dt_min = 50000;
  switch (HWversion)
  {
    case PRIME:
    case PICO:
    case EDGE:
    case SOLO:
    case PLUS:
      dt_min = SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD;
    break;
    case PLUS_2_0:
    case PRIME_2_0:
    case SOLO_2_0:
      dt_min = SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD;
    break;
  default:
    break;
  }

  /* 1) Minimize dt, maximize DACMult */
  pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd = 1;
  pNode->DCSweep_pot.VStep = 1;
  uint64_t dt;
  double ticksPerStep = 1 / abs(dEdt) / (calData->m_DACdcP_V / 2 + calData->m_DACdcN_V / 2) * SECONDS * 1000;
  do
  {
    dt = (uint64_t)round(ticksPerStep / pNode->samplingParams.DACMultEven);
    if (dt / dt_min > 1 && pNode->samplingParams.DACMultEven < DACdcBUF_SIZE)
    {
      if (pNode->samplingParams.DACMultEven << 1 < DACdcBUF_SIZE)
      {
        pNode->samplingParams.DACMultEven <<= 1;
        pNode->samplingParams.DACMultOdd <<= 1;
      }
      else
        break;
    }
  } while (dt / dt_min > 1 && pNode->samplingParams.DACMultEven < DACdcBUF_SIZE);

  /* 2) Increase VStep, if necessary (if dt is too small, or if DACbuf is expended too quickly */
  while (dt < dt_min || pNode->samplingParams.DACMultEven * dt * DACdcBUF_SIZE < MIN_TICKS_FOR_USB_TRANSMISSION)
  {
    pNode->DCSweep_pot.VStep++;
    dt = (uint64_t)round(ticksPerStep / pNode->samplingParams.DACMultEven * pNode->DCSweep_pot.VStep);
  }

  /* 3) Calculate ADCMult. If ADC sampling period is greater than maxSamplingInterval, reduce ADCBufSize */
  if (samplingInterval == 0) //if auto-calculate mode is selected, then use dt * DACdcBUF_SIZE
    samplingInterval = dt * pNode->samplingParams.DACMultEven;
  while (round(samplingInterval / dt / FilterSize) > ADCdcBUF_SIZE)
    FilterSize++;
  pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.ADCBufferSizeOdd = (uint16_t) round(samplingInterval / dt / FilterSize);

  /* 4) Calculate Points Ignored (for dynamic sampling) */
  if (pNode->samplingParams.ADCBufferSizeEven == pNode->samplingParams.DACMultEven)
    pNode->samplingParams.PointsIgnored = pNode->samplingParams.ADCBufferSizeEven / 2;
  else
    pNode->samplingParams.PointsIgnored = 0;

  /* 5) Make sure timer period isn't too big for uint32_t */
  pNode->samplingParams.ADCTimerDiv = 0;
  while (dt > 4294967295 - 5 * MILLISECONDS) //5ms accounts conservatively for loop time
  {
    pNode->samplingParams.ADCTimerDiv++;
    dt >>= 1;
  }
  pNode->samplingParams.ADCTimerPeriod = (uint32_t) dt;

  return FilterSize;
}

uint32_t ExperimentCalcHelperClass::GetSamplingParams_galvSweep(HardwareModel_t HWversion, const cal_t * calData, ExperimentNode_t * pNode,
  double dIdt, currentRange_t currentRange, double samplingInterval)
{
  if (dIdt == 0) return 1;

  samplingInterval *= SECONDS;
  uint32_t FilterSize = 1;
  int dt_min = 50000;
  switch (HWversion)
  {
  case PRIME:
  case PICO:
  case EDGE:
  case SOLO:
  case PLUS:
    dt_min = SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD;
    break;
  case PLUS_2_0:
  case PRIME_2_0:
  case SOLO_2_0:
    dt_min = SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD;
    break;
  default:
    break;
  }

  /* 1) Minimize dt, maximize DACMult */
  pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd = 1;
  pNode->DCSweep_galv.IStep = 1;
  uint64_t dt;
  double ticksPerStep = 1 / abs(dIdt) / (calData->m_DACdcP_I[(int)currentRange] / 2 + calData->m_DACdcN_I[currentRange] / 2) * SECONDS;
  do
  {
    dt = (uint64_t)round(ticksPerStep / pNode->samplingParams.DACMultEven);
    if (dt / dt_min > 1 && pNode->samplingParams.DACMultEven < DACdcBUF_SIZE)
    {
      if (pNode->samplingParams.DACMultEven << 1 < DACdcBUF_SIZE)
      {
        pNode->samplingParams.DACMultEven <<= 1;
        pNode->samplingParams.DACMultOdd <<= 1;
      }
      else
        break;
    }
  } while (dt / dt_min > 1 && pNode->samplingParams.DACMultEven < DACdcBUF_SIZE);

  /* 2) Increase VStep, if necessary (if dt is too small, or if DACbuf is expended too quickly */
  while (dt < dt_min || pNode->samplingParams.DACMultEven * dt * DACdcBUF_SIZE < MIN_TICKS_FOR_USB_TRANSMISSION)
  {
    pNode->DCSweep_galv.IStep++;
    dt = (uint64_t)round(ticksPerStep / pNode->samplingParams.DACMultEven * pNode->DCSweep_galv.IStep);
  }

  /* 3) Calculate ADCMult. If ADC sampling period is greater than maxSamplingInterval, reduce ADCBufSize */
  if (samplingInterval == 0) //if auto-calculate mode is selected, then use dt * DACdcBUF_SIZE
    samplingInterval = dt * pNode->samplingParams.DACMultEven;
  while (round(samplingInterval / dt / FilterSize) > ADCdcBUF_SIZE)
    FilterSize++;
  pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.ADCBufferSizeOdd = (uint16_t)round(samplingInterval / dt / FilterSize);

  /* 4) Calculate Points Ignored (for dynamic sampling) */
  if (pNode->samplingParams.ADCBufferSizeEven == pNode->samplingParams.DACMultEven)
    pNode->samplingParams.PointsIgnored = pNode->samplingParams.ADCBufferSizeEven / 2;
  else
    pNode->samplingParams.PointsIgnored = 0;

  /* 5) Make sure timer period isn't too big for uint32_t */
  pNode->samplingParams.ADCTimerDiv = 0;
  while (dt > 4294967295 - 5 * MILLISECONDS) //5ms accounts conservatively for loop time
  {
    pNode->samplingParams.ADCTimerDiv++;
    dt >>= 1;
  }
  pNode->samplingParams.ADCTimerPeriod = (uint32_t)dt;

  return FilterSize;
}

void ExperimentCalcHelperClass::GetSamplingParameters_pulse(HardwareModel_t HWversion, quint32 t_period, quint32 t_pulsewidth, ExperimentNode_t * pNode)
{
  int dt_min = 50000;
  switch (HWversion)
  {
  case PRIME:
  case PICO:
  case EDGE:
  case SOLO:
  case PLUS:
    dt_min = SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD;
    break;
  case PLUS_2_0:
  case PRIME_2_0:
  case SOLO_2_0:
    dt_min = SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD;
    break;
  default:
    break;
  }

  pNode->samplingParams.ADCTimerDiv = 0;
  pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd = 1;

  /* 1) Take the lesser of (period - pulsewidth) and pulsewidth */
  double t_pulse;
  bool isEvenPeriodShorter;
  if (t_period - t_pulsewidth < t_pulsewidth)
  {
    t_pulse = t_period - t_pulsewidth;
    isEvenPeriodShorter = true;
  }
  else
  {
    t_pulse = t_pulsewidth;
    isEvenPeriodShorter = false;
  }
  uint16_t bufMult = 1;

  /* 2) Minimize dt */
  uint32_t dt;
  do
  {
    dt = t_pulse * MILLISECONDS / bufMult;
    if (dt / dt_min > 1 && bufMult < DACdcBUF_SIZE && bufMult < ADCdcBUF_SIZE)
    {
      bufMult <<= 1;
    }
  } while (dt / dt_min > 1);
  pNode->samplingParams.ADCTimerPeriod = dt;

  if (isEvenPeriodShorter)
  {
    pNode->samplingParams.DACMultEven = bufMult;
    pNode->samplingParams.DACMultOdd = pNode->samplingParams.DACMultEven * (t_pulsewidth / (t_period - t_pulsewidth));
  }
  else
  {
    pNode->samplingParams.DACMultOdd = bufMult;
    pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd * ((t_period - t_pulsewidth) / t_pulsewidth);
  }
  pNode->samplingParams.PointsIgnored = bufMult / 2;

  /* 3) Calculate ADCMult */
  pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.DACMultEven;
  pNode->samplingParams.ADCBufferSizeOdd = pNode->samplingParams.DACMultOdd;
}

currentRange_t ExperimentCalcHelperClass::GetMinCurrentRange(HardwareModel_t HWversion, const cal_t * calData, double targetCurrent)
{
	int range;
	switch (HWversion)
	{
	case PRIME:
	case EDGE:
	case PICO:
	case SOLO:
  case PLUS:
    range = 3;
		break;
	case PLUS_2_0:
	case SOLO_2_0:
	case PRIME_2_0:
    range = 7;
		break;
	default:
		break;
	}

  while (1)
  {
    float slope = MAX(calData->m_iN[range], calData->m_iP[range]);
    if (ABS(targetCurrent) > slope * OVERCURRENT_LIMIT + calData->b_i[range] && range > 0)
      range--;
    else
      break;
  }
  return (currentRange_t)range;
}

int16_t ExperimentCalcHelperClass::GetBINCurrent(const cal_t * calData, currentRange_t currentRange, double targetCurrent)
{
	float slope = targetCurrent > 0 ? calData->m_DACdcP_I[(int)currentRange] : calData->m_DACdcN_I[(int)currentRange];
	int32_t binCurrent = (int32_t)(targetCurrent * slope + calData->b_DACdc_I[(int)currentRange]);
	binCurrent = MIN(MAX(binCurrent, -32768), 32767);
	return binCurrent;
}

int16_t ExperimentCalcHelperClass::GetBINVoltageForDAC(const cal_t * calData, double targetVoltage)
{
	float slope = targetVoltage > 0 ? calData->m_DACdcP_V : calData->m_DACdcN_V;
	int32_t binVolt = (int32_t)(targetVoltage * slope + calData->b_DACdc_V);
	binVolt = MIN(MAX(binVolt, -32768), 32767);
	return (int16_t)binVolt;
}

ProcessedDCData ExperimentCalcHelperClass::ProcessDCDataPoint(const cal_t * calData, ExperimentalDcData rawData)
{
  ProcessedDCData processedData;
  double ewe = rawData.ADCrawData.ewe > 0 ? calData->m_eweP * rawData.ADCrawData.ewe + calData->b_ewe : calData->m_eweN * rawData.ADCrawData.ewe + calData->b_ewe;
  double ref = rawData.ADCrawData.ref > 0 ? calData->m_refP * rawData.ADCrawData.ref + calData->b_ref : calData->m_refN * rawData.ADCrawData.ref + calData->b_ref;
  double ece = rawData.ADCrawData.ece > 0 ? calData->m_eceP * rawData.ADCrawData.ece + calData->b_ece : calData->m_eceN * rawData.ADCrawData.ece + calData->b_ece;
  processedData.EWE = ewe - ref;
  processedData.ECE = ece - ref;
  int n = (int)rawData.currentRange;
  if (rawData.currentRange == OFF)

    processedData.current = 0;
  else
    processedData.current = rawData.ADCrawData.current > 0 ? calData->m_iP[(int)rawData.currentRange] * rawData.ADCrawData.current + calData->b_i[(int)rawData.currentRange] :
                                                           calData->m_iN[(int)rawData.currentRange] * rawData.ADCrawData.current + calData->b_i[(int)rawData.currentRange];
  return processedData;
}

double ExperimentCalcHelperClass::GetUnitsMultiplier(QString units_str)
{
  /* current units */
  if (units_str.contains("mA"))
    return 1;
  else if (units_str.contains("uA"))
    return 1e-3;
  else if (units_str.contains("nA"))
    return 1e-6;

  /* frequency units */
  else if (units_str.contains("mHz"))
    return 1e-3;
  else if (units_str.contains("kHz"))
    return 1e3;
  else if (units_str.contains("MHz"))
    return 1e6;
  else if (units_str.contains("Hz"))    //this else-if must come after the other xHz units
    return 1;

  /* resistance units */
  else if (units_str.contains("kOhms"))
    return 1e3;
  else if (units_str.contains("MOhms"))
    return 1e6;
  else if (units_str.contains("Ohms"))
    return 1;

  /* power units */
  else if (units_str.contains("mW"))
    return 1;
  else if (units_str.contains("W"))
    return 1e3;

  /* time units */
  else if (units_str.contains("min"))
    return 60;
  else if (units_str.contains("hr"))
    return 3600;
  else
    return 1;
}

/* AC methods */

currentRange_t ExperimentCalcHelperClass::GetMinCurrentRange_DACac(const cal_t * calData, double targetCurrentAmp)
{
  int range = 7;

  while (1)
  {
    double inv_slope = calData->m_DACac * calData->m_DACdcP_I[range] / calData->m_DACdcP_V;
    if (ABS(targetCurrentAmp) > inv_slope * OVERCURRENT_LIMIT * (3.3 / 10 / 2) && range > 0)
      range--;
    else
      break;
  }
  return (currentRange_t)range;
}

ComplexDataPoint_t ExperimentCalcHelperClass::AnalyzeFRA(double frequency, int16_t * bufEWE, int16_t * bufCurrent, double gainEWE, double gainI, uint16_t len, double numCycles)
{
	//TODO: add error statistics, THD

	double I_RealSum = 0;
	double I_ImagSum = 0;
	double WE_RealSum = 0;
	double WE_ImagSum = 0;

	for (int i = 0; i < len; i++)
	{
		double arg = (double)i / (double)len * numCycles * 2 * M_PI;
		I_RealSum += bufCurrent[i] * cos(arg);
		I_ImagSum += bufCurrent[i] * sin(arg);
		WE_RealSum += bufEWE[i] * cos(arg);
		WE_ImagSum += bufEWE[i] * sin(arg);

    QString debugStr;
    //debugStr.append(QString::number(bufEWE[i])); debugStr.append('\t'); debugStr.append(QString::number(bufCurrent[i]));
    //qDebug().noquote().nospace() << debugStr;
    qDebug().noquote() << QString::number(bufEWE[i]);
	}

	double I_abs = sqrt(I_RealSum * I_RealSum + I_ImagSum * I_ImagSum);
	double WE_abs = sqrt(WE_RealSum * WE_RealSum + WE_ImagSum * WE_ImagSum);
	double I_phase = atan2(I_ImagSum, I_RealSum);
	double WE_phase = atan2(WE_ImagSum, WE_ImagSum);

	ComplexDataPoint_t dataPoint;
	dataPoint.frequency = frequency;
	dataPoint.ImpedanceMag = WE_abs / gainEWE * gainI / I_abs;
  dataPoint.phase = (WE_phase - I_phase) * 180 / M_PI;
	dataPoint.ImpedanceReal = dataPoint.ImpedanceMag * cos(dataPoint.phase);
	dataPoint.ImpedanceImag = dataPoint.ImpedanceMag * sin(dataPoint.phase);

	return dataPoint;
}

QList<double> ExperimentCalcHelperClass::calculateFrequencyList(double lowerFreq, double upperFreq, double pointsPerDecade)
{
  double _pointsPerDecade = ABS(pointsPerDecade);
  double _upperFreq = MAX(upperFreq, lowerFreq);
  double _lowerFreq = MIN(upperFreq, lowerFreq);
  double NumSamples = pointsPerDecade * log10(_upperFreq / _lowerFreq) + 1;
  int NumSamplesInteger = (int)ceil(NumSamples);
  QList<double> frequencies;

  double ratio = pow(10, -1.0 / _pointsPerDecade);

  for (int i = 0; i < NumSamplesInteger - 1; i++)
  {
    double x = _upperFreq * pow(ratio, i);
    frequencies.append(x);
  }
  frequencies.append(_lowerFreq);
  return frequencies;
}

void ExperimentCalcHelperClass::calcACSamplingParams(const cal_t * calData, ExperimentNode_t * pNode, double amplitude)
{
  pNode->FRA_pot_node.freqRange = pNode->FRA_pot_node.frequency > HF_CUTOFF_VALUE ? HF_RANGE : LF_RANGE;

  /* (1) Calculate signal gen clock frequency and signal gen register value */
        //int wavegenClkdiv = 1 << (int) WAVEGENCLK_24KHZ;
        //pNode->FRA_pot_node.wavegenClkSpeed = WAVEGENCLK_24KHZ;
        int wavegenClkdiv = 1 << (int) WAVEGENCLK_12_5MHZ;
        pNode->FRA_pot_node.wavegenClkSpeed = WAVEGENCLK_12_5MHZ;
  double fSignal = pNode->FRA_pot_node.frequency;

  while (fSignal * SIGNAL_GEN_RESOLUTION * wavegenClkdiv >= 25e6)
  {
    wavegenClkdiv >>= 1;
    pNode->FRA_pot_node.wavegenClkSpeed = (WAVEGENCLK_SPEED_t)((int)pNode->FRA_pot_node.wavegenClkSpeed - 1);
    if (pNode->FRA_pot_node.wavegenClkSpeed == WAVEGENCLK_25MHZ)
      break;
  }
  pNode->FRA_pot_node.wavegenFraction = (uint32_t)(fSignal * wavegenClkdiv / 25e6 * 268435456);
  pNode->FRA_pot_node.wavegenFraction = (pNode->FRA_pot_node.wavegenFraction == 0) ? 1 : pNode->FRA_pot_node.wavegenFraction; //make sure frequency doesn't equal zero

  /* Re-calculate signal frequency based on integer timer values */
  fSignal = (double)pNode->FRA_pot_node.wavegenFraction / 268435456.0 * 25e6 / wavegenClkdiv;
  pNode->FRA_pot_node.frequency = fSignal;

  /* (2) Calculate ADCac buffer size and sampling frequency*/
  int n = ADCacBUF_SIZE;
  double fSample;
  uint32_t ADCclkdiv = 1;

  if (pNode->FRA_pot_node.freqRange == HF_RANGE)
    fSample = (n - 1) * fSignal / n;
  else
    fSample = fSignal * n;
  //uint64_t TimerPeriod = (uint64_t)(100.0e6 / fSample / ADCclkdiv);
  uint64_t TimerPeriod = (uint64_t)(25.0e6 / fSample / ADCclkdiv);

  while (1)
  {
    while (TimerPeriod > 4294967296)
    {
      pNode->FRA_pot_node.ADCacTimerClkDiv++;
      TimerPeriod >>= 1;
      ADCclkdiv <<= 1;
    }

    /* Recalculate sampling frequency and ADCbufsize based on integer timer values */
    //fSample = 100.0e6 / ADCclkdiv / TimerPeriod;
    fSample = 25.0e6 / ADCclkdiv / TimerPeriod;
    if (pNode->FRA_pot_node.freqRange == HF_RANGE)
    {
      double denom = fSignal - fSample;
      double x = ABS(fSignal / denom);
      if (fSample > fSignal || denom == 0 || x > ADCacBUF_SIZE)
      {
        TimerPeriod++;
        continue;
      }
      else
      {
        n = (uint32_t)x;
        if (ADCacBUF_SIZE / n > 1)
          n *= ADCacBUF_SIZE / n;
        break;
      }
    }
    else
    {
      n = (uint32_t)round(fSample / fSignal);
      if (n > ADCacBUF_SIZE)
      {
        TimerPeriod++;
        continue;
      }
      else
        break;
    }
  }
  pNode->FRA_pot_node.ADCacBufSize = n;
  pNode->FRA_pot_node.ADCacTimerPeriod = (uint32_t)TimerPeriod;

  /* (3) Calculate signal amplitude */
  //TODO: get a frequency-dependant transfer function
  pNode->FRA_pot_node.amplitude = MIN((int16_t) (amplitude * calData->m_DACac / 1000), DAC_AC_RESOLUTION);
}

double ExperimentCalcHelperClass::calcNumberOfCycles(const ExperimentalAcData acDataHeader)
{
  double samplingFreq = 1.0e8 / (1 << acDataHeader.ADCacTimerDiv) / acDataHeader.ADCacTimerPeriod;
  
  if (acDataHeader.freqRange == HF_RANGE)
  {
    double n = acDataHeader.frequency / (acDataHeader.frequency - samplingFreq);
    return acDataHeader.ADCacBufSize / n;
  }
  else
  {
    return acDataHeader.ADCacBufSize / (samplingFreq / acDataHeader.frequency);
  }
}