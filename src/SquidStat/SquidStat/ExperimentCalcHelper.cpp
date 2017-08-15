#define _USE_MATH_DEFINES

#include "ExperimentCalcHelper.h"
#include "Log.h"  //debugging only
#include <qdebug.h>  //debugging only
#include <iostream> //debugging only
#include <fstream> //debugging only

/* DC methods */

uint32_t ExperimentCalcHelperClass::GetSamplingParams_staticDAC(HardwareModel_t HWversion, ExperimentNode_t * pNode, double t_sample_period)
{
  uint32_t Filtersize = 1;
	int dt_min = 50000;
  int ADCbufsize = 256;
  int DACbufsize = 256;
	switch (HWversion)
	{
	case PRIME:
	case EDGE:
	case PICO:
	case SOLO:
  case PLUS:
		dt_min = SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD;
    ADCbufsize = ADCdcBUF_SIZE_TEENSY;
    DACbufsize = DACdcBUF_SIZE_TEENSY;
		break;
	case PLUS_2_0:
	case SOLO_2_0:
	case PRIME_2_0:
		dt_min = SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD;
    ADCbufsize = ADCdcBUF_SIZE_PIC;
    DACbufsize = DACdcBUF_SIZE_PIC;
		break;
	default:
		break;
	}

	pNode->samplingParams.ADCTimerDiv = 0;
	pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.ADCBufferSizeOdd = 1;

	/* Minimize dt */
	uint64_t dt;
  do
  {
    pNode->samplingParams.ADCBufferSizeEven <<= 1;
    pNode->samplingParams.ADCBufferSizeOdd <<= 1;
    dt = (uint64_t)(t_sample_period * SECONDS / pNode->samplingParams.ADCBufferSizeEven);
  } while (dt / dt_min > 1 && pNode->samplingParams.ADCBufferSizeEven < ADCbufsize);

  /* if dt is too big for uint32_t, increase Filtersize */
  while (dt >= 4294967296)
  {
    Filtersize++;
    dt = (uint64_t)(t_sample_period * SECONDS / pNode->samplingParams.ADCBufferSizeEven / Filtersize);
  }

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

  return Filtersize;
}

uint32_t ExperimentCalcHelperClass::GetSamplingParams_potSweep(HardwareModel_t HWversion, const cal_t * calData, ExperimentNode_t * pNode,
  double dEdt, double samplingInterval)
{
  if (dEdt == 0) return 1;

  uint32_t FilterSize = 1;
  int dt_min = 50000;
  int ADCbufsize = 256;
  int DACbufsize = 256;
  switch (HWversion)
  {
    case PRIME:
    case PICO:
    case EDGE:
    case SOLO:
    case PLUS:
      dt_min = SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD;
      ADCbufsize = ADCdcBUF_SIZE_TEENSY;
      DACbufsize = DACdcBUF_SIZE_TEENSY;
    break;
    case PLUS_2_0:
    case PRIME_2_0:
    case SOLO_2_0:
      dt_min = SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD;
      ADCbufsize = ADCdcBUF_SIZE_PIC;
      DACbufsize = DACdcBUF_SIZE_PIC;
    break;
  default:
    break;
  }

  /* 1) Minimize dt, maximize DACMult */
  pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd = 1;
  pNode->DCSweep_pot.VStep = 1;
  uint64_t dt;
  double ticksPerStep = abs(1 / dEdt / (calData->m_DACdcP_V / 2 + calData->m_DACdcN_V / 2) * SECONDS * 1000);
  do
  {
    dt = (uint64_t)round(ticksPerStep / pNode->samplingParams.DACMultEven);
    if (dt / dt_min > 1 && pNode->samplingParams.DACMultEven < DACbufsize)
    {
      if (pNode->samplingParams.DACMultEven << 1 < DACbufsize)
      {
        pNode->samplingParams.DACMultEven <<= 1;
        pNode->samplingParams.DACMultOdd <<= 1;
      }
      else
        break;
    }
  } while (dt / dt_min > 1 && pNode->samplingParams.DACMultEven < DACbufsize);

  /* 2) Increase VStep, if necessary (if dt is too small, or if DACbuf is expended too quickly */
  while (dt < dt_min || pNode->samplingParams.DACMultEven * dt * DACbufsize < MIN_TICKS_FOR_USB_TRANSMISSION)
  {
    pNode->DCSweep_pot.VStep++;
    dt = (uint64_t)round(ticksPerStep / pNode->samplingParams.DACMultEven * pNode->DCSweep_pot.VStep);
  }

  /* 3) Calculate ADCMult. If ADC sampling period is greater than maxSamplingInterval, reduce ADCBufSize */
  if (samplingInterval == 0) //if auto-calculate mode is selected, then use dt * DACdcBUF_SIZE
    samplingInterval = dt * pNode->samplingParams.DACMultEven;
  while (round(samplingInterval * SECONDS / dt / FilterSize) > ADCbufsize)
    FilterSize++;
  pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.ADCBufferSizeOdd = (uint16_t) round(samplingInterval * SECONDS / dt / FilterSize);

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
  int ADCbufsize = 256;
  int DACbufsize = 256;
  switch (HWversion)
  {
  case PRIME:
  case PICO:
  case EDGE:
  case SOLO:
  case PLUS:
    dt_min = SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD;
    ADCbufsize = ADCdcBUF_SIZE_TEENSY;
    DACbufsize = DACdcBUF_SIZE_TEENSY;
    break;
  case PLUS_2_0:
  case PRIME_2_0:
  case SOLO_2_0:
    dt_min = SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD;
    ADCbufsize = ADCdcBUF_SIZE_PIC;
    DACbufsize = DACdcBUF_SIZE_PIC;
    break;
  default:
    break;
  }

  /* 1) Minimize dt, maximize DACMult */
  pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd = 1;
  pNode->DCSweep_galv.IStep = 1;
  uint64_t dt;
  double ticksPerStep = abs(1 / dIdt / (calData->m_DACdcP_I[(int)currentRange] / 2 + calData->m_DACdcN_I[currentRange] / 2) * SECONDS);
  do
  {
    dt = (uint64_t)round(ticksPerStep / pNode->samplingParams.DACMultEven);
    if (dt / dt_min > 1 && pNode->samplingParams.DACMultEven < DACbufsize)
    {
      if (pNode->samplingParams.DACMultEven << 1 < DACbufsize)
      {
        pNode->samplingParams.DACMultEven <<= 1;
        pNode->samplingParams.DACMultOdd <<= 1;
      }
      else
        break;
    }
  } while (dt / dt_min > 1 && pNode->samplingParams.DACMultEven < DACbufsize);

  /* 2) Increase VStep, if necessary (if dt is too small, or if DACbuf is expended too quickly */
  while (dt < dt_min || pNode->samplingParams.DACMultEven * dt * DACbufsize < MIN_TICKS_FOR_USB_TRANSMISSION)
  {
    pNode->DCSweep_galv.IStep++;
    dt = (uint64_t)round(ticksPerStep / pNode->samplingParams.DACMultEven * pNode->DCSweep_galv.IStep);
  }

  /* 3) Calculate ADCMult. If ADC sampling period is greater than maxSamplingInterval, reduce ADCBufSize */
  if (samplingInterval == 0) //if auto-calculate mode is selected, then use dt * DACdcBUF_SIZE
    samplingInterval = dt * pNode->samplingParams.DACMultEven;
  while (round(samplingInterval / dt / FilterSize) > ADCbufsize)
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
  int ADCbufsize = 256;
  int DACbufsize = 256;
  switch (HWversion)
  {
  case PRIME:
  case PICO:
  case EDGE:
  case SOLO:
  case PLUS:
    dt_min = SQUIDSTAT_TEENSY_MIN_ADCDC_TIMER_PERIOD;
    ADCbufsize = ADCdcBUF_SIZE_TEENSY;
    DACbufsize = DACdcBUF_SIZE_TEENSY;
    break;
  case PLUS_2_0:
  case PRIME_2_0:
  case SOLO_2_0:
    dt_min = SQUIDSTAT_PIC_MIN_ADCDC_TIMER_PERIOD;
    ADCbufsize = ADCdcBUF_SIZE_PIC;
    DACbufsize = DACdcBUF_SIZE_PIC;
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
    if (dt / dt_min > 1 && bufMult < DACbufsize && bufMult < ADCbufsize)
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
    if (ABS(targetCurrentAmp) > inv_slope * OVERCURRENT_LIMIT_AC && range > 0)
      range--;
    else
      break;
  }
  return (currentRange_t)range;
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
  uint64_t TimerPeriod = (uint64_t)(100.0e6 / fSample / ADCclkdiv);

  while (1)
  {
    while (TimerPeriod > 4294967296)
    {
      pNode->FRA_pot_node.ADCacTimerClkDiv++;
      TimerPeriod >>= 1;
      ADCclkdiv <<= 1;
    }

    /* Recalculate sampling frequency and ADCbufsize based on integer timer values */
    fSample = 100.0e6 / ADCclkdiv / TimerPeriod;
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
  if (pNode->nodeType == FRA_NODE_POT || pNode->nodeType == FRA_NODE_PSEUDOGALV)
  {
    pNode->FRA_pot_node.amplitudeTarget = amplitude;
    pNode->FRA_pot_node.amplitudeBIN = MIN((int16_t)(amplitude / 1000 * calData->m_DACac), DAC_AC_RESOLUTION);
  }
  else if (pNode->nodeType == FRA_NODE_GALV)
  {
    pNode->FRA_galv_node.amplitudeTarget = amplitude;
    double m_DACac_I = calData->m_DACac * (calData->m_DACdcP_I[pNode->currentRangeMode] / calData->m_DACdcP_V);
    pNode->FRA_galv_node.amplitudeBIN = m_DACac_I * amplitude;
  }
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

/* Sinusoidal curve fitting */
ComplexDataPoint_t ExperimentCalcHelperClass::AnalyzeFRA(double frequency, int16_t * bufEWE, int16_t * bufCurrent, double gainEWE, double gainI, uint16_t len, double approxNumCycles)
{
  /* debugging only */
  std::ofstream fout;
  QString filename = "C:/Users/Matt/Desktop/results";
  filename.append(QString::number(frequency));
  filename.append(".txt");
  fout.open(filename.toStdString(), std::ofstream::out | std::ofstream::app);
  fout << "Next data set" << endl;
  for (int i = 0; i < len; i++)
  {
    fout << bufEWE[i] << '\t' << bufCurrent[i] << '\n';
  }


  //todo: add error analysis, THD
  //int rollingAvgNum = (int) round(((double)len) / approxNumCycles / 50);     //todo: replace 50 with frequency-dependent number
  int rollingAvgNum = 25;
  int newLen = len - rollingAvgNum;
  double * t_data = new double[newLen];
  double * filteredEWEdata = filterData(bufEWE, len, rollingAvgNum);
  double * filteredCurrentData = filterData(bufCurrent, len, rollingAvgNum);

  for (int i = 0; i < newLen; i++)
    t_data[i] = i;

  /* Part 1: least squares regression first guess*/
  double resultsEWE[4];
  double resultsCurrent[4];
  sinusoidLeastSquaresFit(t_data, filteredEWEdata, newLen, resultsEWE);
  sinusoidLeastSquaresFit(t_data, filteredCurrentData, newLen, resultsCurrent);

  /* Part 2: Newton-Raphson method */  //TODO: don't just have this iterate 10 times, set an error limit
  for (int i = 0; i < 10; i++)
  {
    NewtonRaphson(resultsEWE, t_data, filteredEWEdata, newLen, resultsEWE);
    NewtonRaphson(resultsCurrent, t_data, filteredCurrentData, newLen, resultsCurrent);
  }

  delete[] t_data;  //todo: check for memory leaks
  delete[] filteredEWEdata;
  delete[] filteredCurrentData;

  ComplexDataPoint_t pt;
  double MagEWE = sqrt(pow(resultsEWE[2], 2) + pow(resultsEWE[3], 2)) / gainEWE;
  double MagCurrent = sqrt(pow(resultsCurrent[2], 2) + pow(resultsCurrent[3], 2)) / gainI;
  double phaseEWE = atan2(resultsEWE[2], resultsEWE[3]) * 180 / M_PI;
  double phaseCurrent = atan2(resultsCurrent[2], resultsCurrent[3]) * 180 / M_PI;
  pt.frequency = frequency;
  pt.ImpedanceMag = MagEWE / MagCurrent;      //todo: incorporate calibration data
  pt.phase = phaseCurrent - phaseEWE;
  pt.ImpedanceReal = pt.ImpedanceMag * cos(pt.phase * M_PI / 180);
  pt.ImpedanceImag = pt.ImpedanceMag * sin(pt.phase * M_PI / 180);
  return pt;
}

void ExperimentCalcHelperClass::sinusoidLeastSquaresFit(double * xbuf, double * ybuf, int size, double * results)
{
  double * S = new double[size];
  double * SS = new double[size];
  S[0] = SS[0] = 0;
  for (int i = 1; i < size; i++)
  {
    S[i] = S[i - 1] + 0.5*(ybuf[i] + ybuf[i - 1])*(xbuf[i] - xbuf[i - 1]);
    SS[i] = SS[i - 1] + 0.5*(S[i] + S[i - 1])*(xbuf[i] - xbuf[i - 1]);
  }

  double sum_x = 0, sum_x2 = 0, sum_x3 = 0, sum_x4 = 0, sum_SS = 0, sum_SS2 = 0, sum_xSS = 0, sum_x2SS = 0, sum_y = 0, sum_yx = 0, sum_yx2 = 0, sum_ySS = 0;

  for (int i = 0; i < size; i++)
  {
    sum_x += xbuf[i];
    sum_x2 += xbuf[i] * xbuf[i];
    sum_x3 += xbuf[i] * xbuf[i] * xbuf[i];
    sum_x4 += xbuf[i] * xbuf[i] * xbuf[i] * xbuf[i];
    sum_SS += SS[i];
    sum_SS2 += SS[i] * SS[i];
    sum_xSS += xbuf[i] * SS[i];
    sum_x2SS += xbuf[i] * xbuf[i] * SS[i];
    sum_y += ybuf[i];
    sum_yx += ybuf[i] * xbuf[i];
    sum_yx2 += ybuf[i] * xbuf[i] * xbuf[i];
    sum_ySS += ybuf[i] * SS[i];
  }

  double ** matrix1 = createMatrix(4, 4);
  double ** matrix2 = createMatrix(4, 1);

  /* Initialize matrix1 */
  matrix1[0][0] = sum_SS2;
  matrix1[0][1] = sum_x2SS;
  matrix1[0][2] = sum_xSS;
  matrix1[0][3] = sum_SS;

  matrix1[1][0] = sum_x2SS;
  matrix1[1][1] = sum_x4;
  matrix1[1][2] = sum_x3;
  matrix1[1][3] = sum_x2;

  matrix1[2][0] = sum_xSS;
  matrix1[2][1] = sum_x3;
  matrix1[2][2] = sum_x2;
  matrix1[2][3] = sum_x;

  matrix1[3][0] = sum_SS;
  matrix1[3][1] = sum_x2;
  matrix1[3][2] = sum_x;
  matrix1[3][3] = size;

  /* initialize matrix2 */
  matrix2[0][0] = sum_ySS;
  matrix2[1][0] = sum_yx2;
  matrix2[2][0] = sum_yx;
  matrix2[3][0] = sum_y;

  double ** invMatrix1 = invertMatrix(matrix1, 4);
  double ** matrixABCD = matrixMult(invMatrix1, 4, 4, matrix2, 4, 1);
  double A1 = matrixABCD[0][0];
  double B1 = matrixABCD[1][0];
  double C1 = matrixABCD[2][0];
  double D1 = matrixABCD[3][0];
  double w1 = sqrt(-A1);
  double a1 = 2 * B1 / w1;
  double b1 = (B1 * xbuf[0] * xbuf[0] + C1 * xbuf[0] + D1 - a1) * sin(w1 * xbuf[0]) + (C1 + 2 * B1 * xbuf[0]) / w1 * cos(w1 * xbuf[0]);
  double c1 = (B1 * xbuf[0] * xbuf[0] + C1 * xbuf[0] + D1 - a1) * cos(w1 * xbuf[0]) + (C1 + 2 * B1 * xbuf[0]) / w1 * sin(w1 * xbuf[0]);

  /* Part 2 */
  double a2 = a1;
  double p1 = sqrt(b1 * b1 + c1 * c1);
  double p2 = p1;
  double theta1 = 0;
  if (b1 > 0)
    theta1 = atan(c1 / b1);
  else if (b1 < 0)
    theta1 = atan(c1 / b1) + M_PI;
  else if (b1 == 0)
  {
    if (c1 > 0)
      theta1 = M_PI / 2;
    else
      theta1 = -M_PI / 2;
  }

  double * K = new double[size];
  for (int i = 0; i < size; i++)
  {
    K[i] = round((w1 * xbuf[i] + theta1) / M_PI);
  }

  double * Q = new double[size];
  for (int i = 0; i < size; i++)
  {
    if (p2*p2 > pow(ybuf[i] - a2, 2))
      Q[i] = pow(-1, K[i]) * atan((ybuf[i] - a2) / (p2*p2 - pow((ybuf[i] - a2), 2))) + M_PI * K[i];
    else
    {
      if (ybuf[i] > a2)
      {
        Q[i] = M_PI / 2 * pow(-1, K[i]) + M_PI * K[i];
      }
      else
      {
        Q[i] = -M_PI / 2 * pow(-1, K[i]) + M_PI * K[i];
      }
    }
  }

  double sum_Q = 0, sum_Qx = 0;
  for (int i = 0; i < size; i++)
  {
    sum_Q += Q[i];
    sum_Qx += Q[i] * xbuf[i];
  }
  double ** matrix3 = createMatrix(2, 2);
  matrix3[0][0] = sum_x2;
  matrix3[0][1] = sum_x;
  matrix3[1][0] = sum_x;
  matrix3[1][1] = size;
  double ** matrix4 = createMatrix(2, 1);
  matrix4[0][0] = sum_Qx;
  matrix4[1][0] = sum_Q;
  double ** invMatrix3 = invertMatrix(matrix3, 2);
  double ** matrixEF = matrixMult(invMatrix3, 2, 2, matrix4, 2, 1);
  double w2 = matrixEF[0][0];
  double theta2 = matrixEF[1][0];
  double b2 = p2 * cos(theta2);
  double c2 = p2 * sin(theta2);

  /* Part 3 */
  double w3 = w2;
  double sum_sin_w3x = 0, sum_cos_w3x = 0, sum_sin2_w3x = 0, sum_cos2_w3x = 0, sum_sincos_w3x = 0, sum_ysin_w3x = 0, sum_ycos_w3x = 0;
  for (int i = 0; i < size; i++)
  {
    double arg = w3 * xbuf[i];
    sum_sin_w3x += sin(arg);
    sum_cos_w3x += cos(arg);
    sum_sin2_w3x += pow(sin(arg), 2);
    sum_cos2_w3x += pow(cos(arg), 2);
    sum_sincos_w3x += sin(arg) * cos(arg);
    sum_ysin_w3x += ybuf[i] * sin(arg);
    sum_ycos_w3x += ybuf[i] * cos(arg);
  }

  double ** matrix5 = createMatrix(3, 3);
  matrix5[0][0] = size;
  matrix5[0][1] = sum_sin_w3x;
  matrix5[0][2] = sum_cos_w3x;
  matrix5[1][0] = sum_sin_w3x;
  matrix5[1][1] = sum_sin2_w3x;
  matrix5[1][2] = sum_sincos_w3x;
  matrix5[2][0] = sum_cos_w3x;
  matrix5[2][1] = sum_sincos_w3x;
  matrix5[2][2] = sum_cos2_w3x;

  double ** matrix6 = createMatrix(3, 1);
  matrix6[0][0] = sum_y;
  matrix6[1][0] = sum_ysin_w3x;
  matrix6[2][0] = sum_ycos_w3x;

  double ** invMatrix5 = invertMatrix(matrix5, 3);

  double ** matrixGHI = matrixMult(invMatrix5, 3, 3, matrix6, 3, 1);
  results[0] = w3;
  results[1] = matrixGHI[0][0];
  results[2] = matrixGHI[1][0];
  results[3] = matrixGHI[2][0];


  /* clean up */
  deleteMatrix(matrix1, 4);
  deleteMatrix(matrixABCD, 4);
  deleteMatrix(matrix2, 4);
  deleteMatrix(invMatrix1, 4);
  deleteMatrix(matrix3, 2);
  deleteMatrix(matrix4, 2);
  deleteMatrix(invMatrix3, 2);
  deleteMatrix(matrixEF, 2);
  deleteMatrix(matrix5, 3);
  deleteMatrix(matrix6, 3);
  deleteMatrix(invMatrix5, 3);
  deleteMatrix(matrixGHI, 3);
  delete[] S;
  delete[] SS;
  delete[] K;
  delete[] Q;
}

double * ExperimentCalcHelperClass::filterData(int16_t * rawData, int length, int rollingAvgSize)
{
  int newLength = length - rollingAvgSize;
  double * filteredData = new double[newLength];
  
  /* low-pass filter */
  for (int i = 0; i < newLength; i++)
  {
    filteredData[i] = 0;
    for (int j = 0; j < rollingAvgSize; j++)
    {
      filteredData[i] += rawData[i + j];
    }
    filteredData[i] /= rollingAvgSize;
  }

  /* "high-pass filter" (remove average value) */
  double avg = 0;
  for (int i = 0; i < newLength; i++)
  {
    avg += filteredData[i];
  }
  avg /= newLength;
  for (int i = 0; i < newLength; i++)
  {
    filteredData[i] -= avg;
  }

  return filteredData;
}

double ** ExperimentCalcHelperClass::createMatrix(int rows, int cols)
{
  double ** matrix = new double*[rows];
  for (int i = 0; i < rows; i++)
  {
    matrix[i] = new double[cols];
  }

  return matrix;
}
void ExperimentCalcHelperClass::deleteMatrix(double ** matrix, int rows)
{
  for (int i = 0; i < rows; i++)
  {
    delete[] matrix[i];
  }
  delete[] matrix;
}
double ** ExperimentCalcHelperClass::invertMatrix(double ** matrix, int size)
{

  /* Get matrix of minors */
  double ** matrixOfMinors = createMatrix(size, size);
  for (int r = 0; r < size; r++)
  {
    for (int c = 0; c < size; c++)
    {
      double ** minorMatrix = getMinorMatrix(matrix, r, c, size);
      matrixOfMinors[r][c] = determinant(minorMatrix, size - 1);
      deleteMatrix(minorMatrix, size - 1);
    }
  }

  /* matrix of cofactors */
  for (int r = 0; r < size; r++)
  {
    for (int c = 0; c < size; c++)
    {
      matrixOfMinors[r][c] *= ((c % 2) == 0 ? 1 : -1) * ((r % 2) == 0 ? 1 : -1);
    }
  }

  /* get Adjugate */
  double ** inverseMatrix = createMatrix(size, size);
  double determinant_ = determinant(matrix, size);
  for (int r = 0; r < size; r++)
  {
    for (int c = 0; c < size; c++)
    {
      inverseMatrix[c][r] = matrixOfMinors[r][c];
      inverseMatrix[c][r] /= determinant_;
    }
  }


  deleteMatrix(matrixOfMinors, size);
  return inverseMatrix;
}
double ExperimentCalcHelperClass::determinant(double ** matrix, int size)
{
  if (size == 1)
    return matrix[0][0];
  else
  {
    double sum = 0;
    for (int i = 0; i < size; i++)
    {
      double ** minorMatrix = getMinorMatrix(matrix, 0, i, size);
      double term = matrix[0][i] * determinant(minorMatrix, size - 1);
      term *= (i % 2) == 0 ? 1 : -1;
      sum += term;
      deleteMatrix(minorMatrix, size - 1);
    }
    return sum;
  }
}
double ** ExperimentCalcHelperClass::getMinorMatrix(double ** matrix, int row, int col, int size)
{
  double ** minorMatrix = createMatrix(size - 1, size - 1);
  for (int c = 0, srcCol = 0; c < size - 1; c++, srcCol++)
  {
    if (c == col)
      srcCol++;
    for (int r = 0, srcRow = 0; r < size - 1; r++, srcRow++)
    {
      if (r == row)
        srcRow++;
      minorMatrix[r][c] = matrix[srcRow][srcCol];
    }
  }
  return minorMatrix;
}
double ** ExperimentCalcHelperClass::matrixMult(double ** matrix1, int rows1, int cols1, double ** matrix2, int rows2, int cols2)
{
  if (cols1 != rows2)
    return NULL;

  double ** result = createMatrix(rows1, cols2);
  for (int r = 0; r < rows1; r++)
  {
    for (int c = 0; c < cols2; c++)
    {
      double sum = 0;
      for (int i = 0; i < cols1; i++)
        sum += matrix1[r][i] * matrix2[i][c];
      result[r][c] = sum;
    }
  }
  return result;
}

void ExperimentCalcHelperClass::NewtonRaphson(double * initialGuessParams, double * xbuf, double * ybuf, int length, double * resultsBuf)
{
  double ** doubleDerivMatrix = createMatrix(4, 4);

  double(*ptrFnDblDerivs[4][4])(double *, double);
  ptrFnDblDerivs[0][0] = d2ydw2;
  ptrFnDblDerivs[0][1] = d2ydwda;
  ptrFnDblDerivs[0][2] = d2ydwdb;
  ptrFnDblDerivs[0][3] = d2ydwdc;
  ptrFnDblDerivs[1][0] = d2ydwda;
  ptrFnDblDerivs[1][1] = d2yda2;
  ptrFnDblDerivs[1][2] = d2ydadb;
  ptrFnDblDerivs[1][3] = d2ydadc;
  ptrFnDblDerivs[2][0] = d2ydwdb;
  ptrFnDblDerivs[2][1] = d2ydadb;
  ptrFnDblDerivs[2][2] = d2ydb2;
  ptrFnDblDerivs[2][3] = d2ydbdc;
  ptrFnDblDerivs[3][0] = d2ydwdc;
  ptrFnDblDerivs[3][1] = d2ydadc;
  ptrFnDblDerivs[3][2] = d2ydbdc;
  ptrFnDblDerivs[3][3] = d2ydc2;

  double(*ptrFnDerivs[4])(double *, double);
  ptrFnDerivs[0] = dydw;
  ptrFnDerivs[1] = dyda;
  ptrFnDerivs[2] = dydb;
  ptrFnDerivs[3] = dydc;

  for (int r = 0; r < 4; r++)
  {
    for (int c = 0; c < 4; c++)
    {
      doubleDerivMatrix[r][c] = de2dXdY(initialGuessParams, xbuf, ybuf, length, ptrFnDerivs[r], ptrFnDerivs[c], ptrFnDblDerivs[r][c]);
    }
  }

  double ** singleDerivMatrix = createMatrix(4, 1);
  for (int r = 0; r < 4; r++)
  {
    singleDerivMatrix[r][0] = dedX(initialGuessParams, xbuf, ybuf, length, ptrFnDerivs[r]);
  }

  double ** dblDerivMatrInverse = invertMatrix(doubleDerivMatrix, 4);
  double ** results = matrixMult(dblDerivMatrInverse, 4, 4, singleDerivMatrix, 4, 1);
  for (int i = 0; i < 4; i++)
  {
    resultsBuf[i] = initialGuessParams[i] - results[i][0];
  }

  deleteMatrix(dblDerivMatrInverse, 4);
  deleteMatrix(doubleDerivMatrix, 4);
  deleteMatrix(singleDerivMatrix, 4);
  deleteMatrix(results, 4);

}

double ExperimentCalcHelperClass::y_model(double * paramsBuf, double x)
{
  double w = paramsBuf[0];
  double a = paramsBuf[1];
  double b = paramsBuf[2];
  double c = paramsBuf[3];
  return a + b*sin(w*x) + c*cos(w*x);
}
double ExperimentCalcHelperClass::dedX(double * paramsBuf, double * xbuf, double * ybuf, int size, double(*dydX)(double *, double))
{
  double sum = 0;
  for (int i = 0; i < size; i++)
  {
    sum += 2 * (y_model(paramsBuf, xbuf[i]) - ybuf[i]) * dydX(paramsBuf, xbuf[i]);
  }
  return sum;
}
double ExperimentCalcHelperClass::de2dXdY(double * paramsBuf, double * xbuf, double * ybuf, int size,
  double(*dydX)(double *, double),
  double(*dydY)(double *, double),
  double(*d2ydXdY)(double *, double))
{
  double sum = 0;
  for (int i = 0; i < size; i++)
  {
    sum += 2 * (y_model(paramsBuf, xbuf[i]) - ybuf[i])*d2ydXdY(paramsBuf, xbuf[i]) + 2 * dydX(paramsBuf, xbuf[i])*dydY(paramsBuf, xbuf[i]);
  }
  return sum;
}
double ExperimentCalcHelperClass::dydw(double * paramsBuf, double x)
{
  double w = paramsBuf[0];
  double b = paramsBuf[2];
  double c = paramsBuf[3];
  return x*(b*cos(w*x) - c*sin(w*x));
}
double ExperimentCalcHelperClass::dyda(double * paramsBuf, double x)
{
  return 1;
}
double ExperimentCalcHelperClass::dydb(double * paramsBuf, double x)
{
  double w = paramsBuf[0];
  return sin(w*x);
}
double ExperimentCalcHelperClass::dydc(double * paramsBuf, double x)
{
  double w = paramsBuf[0];
  return cos(w*x);
}
double ExperimentCalcHelperClass::d2ydw2(double * paramsBuf, double x)
{
  double w = paramsBuf[0];
  double b = paramsBuf[2];
  double c = paramsBuf[3];
  return -x*x*b*sin(w*x) - x*x*c*cos(w*x);
}
double ExperimentCalcHelperClass::d2ydwda(double * paramsBuf, double x)
{
  return 0;
}
double ExperimentCalcHelperClass::d2ydwdb(double * paramsBuf, double x)
{
  double w = paramsBuf[0];
  return x*cos(w*x);
}
double ExperimentCalcHelperClass::d2ydwdc(double * paramsBuf, double x)
{
  double w = paramsBuf[0];
  return -x*sin(w*x);
}
double ExperimentCalcHelperClass::d2yda2(double * paramsBuf, double x)
{
  return 0;
}
double ExperimentCalcHelperClass::d2ydadb(double * paramsBuf, double x)
{
  return 0;
}
double ExperimentCalcHelperClass::d2ydadc(double * paramsBuf, double x)
{
  return 0;
}
double ExperimentCalcHelperClass::d2ydb2(double * paramsBuf, double x)
{
  return 0;
}
double ExperimentCalcHelperClass::d2ydbdc(double * paramsBuf, double x)
{
  return 0;
}
double ExperimentCalcHelperClass::d2ydc2(double * paramsBuf, double x)
{
  return 0;
}