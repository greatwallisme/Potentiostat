#include "DataManager.h"

//DataManager::DataManager() {}

//DataManager::~DataManager() {}

/*void DataManager::tasks()
{  
  if (phyLevel->DCsamplingOn())
  {
    if (_ADCdcInactiveBufFullFlag)
    {
        _ADCdcInactiveBufFullFlag = false;
        if(numInvalidBuffers() == 0)
        {
          setADCdc_majorDataPoint(averageDCdata(_ADCdcInactiveBuf));
          SetMajorDataEventFlag();
          incrPointsSkippedMCU();
          if (incrPointsSkippedMCU() == 0)
              mUSBSerial.sendMessage(ADCDC_DATA, _channelNum, (uint8_t *)ADCdc_majorDataPoint(), sizeof(ExperimentalDataPoint_t));
          if (_dVdt.is_Tracking())
              _dVdt.appendPoint(*ADCdc_majorDataPoint(), _ADCdcInactiveBuf->buf);
          if (_dIdt.is_Tracking())
              _dIdt.appendPoint(*ADCdc_majorDataPoint(), _ADCdcInactiveBuf->buf);
        }
        else
            decrNumInvalidBuffers();
    }
  }
//  if (phyLevel->ACsamplingOn() && phyLevel->getACdataSamplingFlags())
//  {
//      phyLevel->setADCacBuffers(AClowerBuf ? _ACdata_I_upperBuf : _ACdata_I_lowerBuf,
//                                AClowerBuf ? _ACdata_EWE_upperBuf : _ACdata_EWE_lowerBuf);
//      AClowerBuf ^= 1;
//      
////#define DMA_CH_DEST_HALF_FULL_MASK (1<<4)
////#define DMA_CH_DEST_FULL_MASK (1<<5)
////      uint32_t flags = phyLevel->getACdataSamplingFlags();
////      if (flags & (DMA_CH_DEST_HALF_FULL_MASK | DMA_CH_DEST_FULL_MASK))
////      {
////        SetACDataEventFlag();
////        AClowerBuf = !(flags == DMA_CH_DEST_FULL_MASK);
////        phyLevel->clearACdataSamplingFlags(flags);
////      }
//  }
}
*/
//void DataManager::setACDataFlag()
//{
//    phyLevel->setADCacBuffers(phyLevel->ADCacBufAddr_I() == (uint32_t)_ACdata_I_lowerBuf ? _ACdata_I_upperBuf : _ACdata_I_lowerBuf,
//                            phyLevel->ADCacBufAddr_EWE() == (uint32_t)_ACdata_EWE_lowerBuf ? _ACdata_EWE_upperBuf : _ACdata_EWE_lowerBuf);
//    SetACDataEventFlag();
//}

/*uint16_t DataManager::incrPointsSkippedMCU()
{
    _pointsSkippedMCU = (_pointsSkippedMCU + 1) % maxPointsSkippedMCU;
    return _pointsSkippedMCU;
}*/

/*void DataManager::initDerivTracking(ExperimentNode_t * pNode)
{
  switch (pNode->nodeType)
  {
  case DCNODE_OCP:
  case DCNODE_POINT_GALV:
    _dVdt.initTracking(pNode->DCsamplingParams.ADCBufferSizeEven, phyLevel->getMinorDataEventInterval());
    break;
  case DCNODE_POINT_POT:
    _dIdt.initTracking(pNode->DCsamplingParams.ADCBufferSizeEven, phyLevel->getMinorDataEventInterval());
  default:
    _dVdt.stopTracking(); _dIdt.stopTracking();
    break;
  }
}*/

/*void DataManager::stopDerivTracking()
{
  _dVdt.stopTracking(); _dIdt.stopTracking();
}*/

/*void DataManager::initCapacityTracking()
{
  capacity_tracker.isTracking = true;
  capacity_tracker.reset();
}*/

/*void DataManager::stopCapacityTracking()
{
  capacity_tracker.isTracking = false;
}*/

/*void DataManager::initReferences(PhyLevel * phy, ADCBuffer_t * ADCdc, uint16_t * ADCac_IBufRaw, uint16_t * ADCac_EWEBufRaw)
{
  phyLevel = phy;
  _ADCdcActiveBuf = &ADCdc[0];
  _ADCdcInactiveBuf = &ADCdc[1];
  _ACdata_I_lowerBuf = ADCac_IBufRaw;
  _ACdata_I_upperBuf = &ADCac_IBufRaw[ADCacBUF_SIZE];
  _ACdata_EWE_lowerBuf = ADCac_EWEBufRaw;
  _ACdata_EWE_upperBuf = &ADCac_EWEBufRaw[ADCacBUF_SIZE];

  _dVdt.setMode(GALVANOSTATIC);
  _dIdt.setMode(POTENTIOSTATIC);
}*/

/*void DataManager::toggleADCdcBuffers()
{
  auto * temp = _ADCdcActiveBuf;
  _ADCdcActiveBuf = _ADCdcInactiveBuf;
  _ADCdcInactiveBuf = temp;
  phyLevel->setADCdcBuffer(_ADCdcActiveBuf);
  _ADCdcActiveBuf->dataIndex = 0;
  
  _ADCdcInactiveBufFullFlag = true;
}*/

/*void DataManager::initADCbufProperties(ExperimentNode_t * pNode)
{
    //todo
//    if (pNode->DCsamplingParams.isFastSlewRate)
//    {
//        _ADCmultCounterMax = pNode->DCsamplingParams.ADCBufferSizeEven;
//        _phyLevel->_ADCmultNum = ...
//    }
  
  _ADCdcActiveBuf->bufSize = pNode->DCsamplingParams.ADCBufferSizeEven;
  _ADCdcInactiveBuf->bufSize = pNode->DCsamplingParams.ADCBufferSizeOdd;
  _ADCdcActiveBuf->numIgnoredPoints = _ADCdcInactiveBuf->numIgnoredPoints = pNode->DCsamplingParams.minorPointsDiscarded;
  phyLevel->setADCdcBuffer(_ADCdcActiveBuf);
}*/

//uint16_t DataManager::incrementADCMultCounter()
//{
//  uint16_t retVal = _ADCmultCounter++;
//  _ADCmultCounter %= _ADCmultCounterMax;
//  return retVal;
//}
//
//void DataManager::setMinorDataReadyFlag()
//{
//  SetMinorDataEventFlag();
//}
//
//ExperimentalDataPoint_t DataManager::averageDCdata(volatile ADCBuffer_t * buf)
//{
//  int32_t sumEWE = 0;
//  int32_t sumECE = 0;
//  int32_t sumREF = 0;
//  int32_t sumI = 0;
//  ExperimentalDataPoint_t pt;
//  int32_t N = buf->dataIndex - buf->numIgnoredPoints;
//  if (N > 0)
//  {
//    for (uint16_t i = buf->numIgnoredPoints; i < buf->dataIndex; i++)
//    {
//      sumEWE += buf->buf[i].ewe;
//      sumECE += buf->buf[i].ece;
//      sumREF += buf->buf[i].ref;
//      sumI += buf->buf[i].current;
//    }
//    pt.ADCrawData.ewe = sumEWE / N;
//    pt.ADCrawData.ece = sumECE / N;
//    pt.ADCrawData.ref = sumREF / N;
//    pt.ADCrawData.current = phyLevel->currentRange() == OFF ? 0 : sumI / N;
//  }
//  else
//  {
//    pt.ADCrawData.ewe = _ADCdc_current_point->ewe;
//    pt.ADCrawData.ece = _ADCdc_current_point->ece;
//    pt.ADCrawData.ref = _ADCdc_current_point->ref;
//    pt.ADCrawData.current = phyLevel->currentRange() == OFF ? 0 : _ADCdc_current_point->current;
//  }
//  pt.timestamp = phyLevel->getNow64();
//  pt.currentRange = phyLevel->currentRange();
//
//  return pt;
//}

/* AC data analysis */
void DataManager::ACDATA_ANALYSIS_smoothData(int16_t * rawData)
{
    ACDATA_ANALYSIS_resetStats();

    int16_t * rawEWE = &rawData[1024];
    int16_t * rawI = rawData;

    /* Apply rolling avg filter to incoming data */
    uint32_t sumEWE = 0, sumI = 0;
    for (int i = 0; i < ADCacROLLING_AVG_SIZE; i++)
    {
        sumEWE += rawEWE[i];
        sumI += rawI[i];
    }
    for (int i = 0; i < ADCacBUF_SIZE - ADCacROLLING_AVG_SIZE; i++)
    {
        ACDATA_ANALYSIS_smoothedEweData[i] = sumEWE / ADCacROLLING_AVG_SIZE;
        ACDATA_ANALYSIS_smoothedIData[i] = sumI / ADCacROLLING_AVG_SIZE;
        sumEWE += rawEWE[i + ADCacROLLING_AVG_SIZE] - rawEWE[i];
        sumI += rawI[i + ADCacROLLING_AVG_SIZE] - rawI[i];
    }

    /* Analyze data stats */
    for (int i = 0; i < ADCacBUF_SIZE - ADCacROLLING_AVG_SIZE; i++)
    {
        if (ACDATA_ANALYSIS_smoothedEweData[i] == 4095)
        {
            ACDATA_ANALYSIS_num4095sEWE++;
            ACDATA_ANALYSIS_EWEmax = 4095;
        }
        else if (ACDATA_ANALYSIS_smoothedEweData[i] == 0)
        {
            ACDATA_ANALYSIS_numZeroesEWE++;
            ACDATA_ANALYSIS_EWEmin = 0;
        }
        else if (ACDATA_ANALYSIS_smoothedEweData[i] > ACDATA_ANALYSIS_EWEmax)
        {
            ACDATA_ANALYSIS_EWEmax = ACDATA_ANALYSIS_smoothedEweData[i];
        }
        else if (ACDATA_ANALYSIS_smoothedEweData[i] < ACDATA_ANALYSIS_EWEmin)
        {
            ACDATA_ANALYSIS_EWEmin = ACDATA_ANALYSIS_smoothedEweData[i];
        }

        if (ACDATA_ANALYSIS_smoothedIData[i] == 4095)
        {
            ACDATA_ANALYSIS_num4095sI++;
            ACDATA_ANALYSIS_Imax = 4095;
        }
        else if (ACDATA_ANALYSIS_smoothedIData[i] == 0)
        {
            ACDATA_ANALYSIS_numZeroesI++;
            ACDATA_ANALYSIS_Imin = 0;
        }
        else if (ACDATA_ANALYSIS_smoothedIData[i] > ACDATA_ANALYSIS_Imax)
        {
            ACDATA_ANALYSIS_Imax = ACDATA_ANALYSIS_smoothedIData[i];
        }
        else if (ACDATA_ANALYSIS_smoothedIData[i] < ACDATA_ANALYSIS_Imin)
        {
            ACDATA_ANALYSIS_Imin = ACDATA_ANALYSIS_smoothedIData[i];
        }
        ACDATA_ANALYSIS_WEAmplitude = (float)(ACDATA_ANALYSIS_EWEmax - ACDATA_ANALYSIS_EWEmin) / 2;
        ACDATA_ANALYSIS_IAmplitude = (float)(ACDATA_ANALYSIS_Imax - ACDATA_ANALYSIS_Imin) / 2;
    }
}

void DataManager::ACDATA_ANALYSIS_resetAll()
{
    ACDATA_ANALYSIS_resetStats();
    for (int i = 0; i < STATUS_HISTORY_LENGTH; i++)
        ACDATA_ANALYSIS_statusHistory[i] = 0;
    ACDATA_ANALYSIS_historyIndex = 0;
    ACDATA_ANALYSIS_numIBiasResets = 0;
    ACDATA_ANALYSIS_numWEBiasResets = 0;
}

void DataManager::ACDATA_ANALYSIS_resetStats()
{
    ACDATA_ANALYSIS_EWEmax = 0;
    ACDATA_ANALYSIS_EWEmin = 4095;
    ACDATA_ANALYSIS_Imax = 0;
    ACDATA_ANALYSIS_Imin = 4095;
    ACDATA_ANALYSIS_num4095sEWE = 0;
    ACDATA_ANALYSIS_num4095sI = 0;
    ACDATA_ANALYSIS_numZeroesEWE = 0;
    ACDATA_ANALYSIS_numZeroesI = 0;
}

bool DataManager::ACDATA_ANALYSIS_checkBiasOffset(EWE_or_I_t EWE_or_I)
{
    if (EWE_or_I == WE_DATA)
    {
        if ((ACDATA_ANALYSIS_numZeroesEWE > ADCacBUF_SIZE / 50 && ACDATA_ANALYSIS_num4095sEWE == 0) ||
            (ACDATA_ANALYSIS_num4095sEWE > ADCacBUF_SIZE / 50 && ACDATA_ANALYSIS_numZeroesEWE == 0))
            return true;
        else
            return false;
    }
    else
    {
        if ((ACDATA_ANALYSIS_numZeroesI > ADCacBUF_SIZE / 50 && ACDATA_ANALYSIS_num4095sI == 0) ||
            (ACDATA_ANALYSIS_num4095sI > ADCacBUF_SIZE / 50 && ACDATA_ANALYSIS_numZeroesI == 0))
            return true;
        else
            return false;
    }
}

float DataManager::ACDATA_ANALYSIS_getACAmpError(float targetAmp, float gain, currentRange_t currentRange)
{
    if (currentRange == OFF)     //omitted "currentRange" argument means that we're in potentiostatic mode
        return ACDATA_ANALYSIS_WEAmplitude / (targetAmp * gain / 3.3 * 4096);
    else //galvanostatic mode
    {
        float m_ADCac_I = 3.3 / 4096 * _cal.m_iP[(int)currentRange] / _cal.m_refP;
        return fabs(ACDATA_ANALYSIS_IAmplitude * m_ADCac_I / (targetAmp * gain));
    }
}

uint32_t DataManager::ACDATA_ANALYSIS_checkHistory(uint32_t status)
{
    if (ACDATA_ANALYSIS_historyIndex >= STATUS_HISTORY_LENGTH)
        return MAX_NUM_TRIES_EXCEEDED_FLAG;

    ACDATA_ANALYSIS_statusHistory[ACDATA_ANALYSIS_historyIndex++] = status;
    uint32_t retVal = 0;
    if (ACDATA_ANALYSIS_checkForFlipFlops(INCREASE_I_GAIN_FLAG, DECREASE_I_GAIN_FLAG) > MAX_NUM_FLIP_FLOPS)
        retVal |= I_GAIN_FLIP_FLOP_FLAG;
    if (ACDATA_ANALYSIS_checkForFlipFlops(INCREASE_EWE_GAIN_FLAG, DECREASE_EWE_GAIN_FLAG) > MAX_NUM_FLIP_FLOPS)
        retVal |= WE_GAIN_FLIP_FLOP_FLAG;
    if (ACDATA_ANALYSIS_checkForFlipFlops(INCREASE_IRANGE_FLAG, DECREASE_IRANGE_FLAG) > MAX_NUM_FLIP_FLOPS)
        retVal |= I_RANGE_FLIP_FLOP_FLAG;

    return retVal;
}

int DataManager::ACDATA_ANALYSIS_checkForFlipFlops(uint32_t flag1, uint32_t flag2)
{
    bool Q = false;
    int numFlipFlops = 0;
    for (int i = 0; i < ACDATA_ANALYSIS_historyIndex; i++)
    {
        bool J = ACDATA_ANALYSIS_statusHistory[i] & flag1;
        bool K = ACDATA_ANALYSIS_statusHistory[i] & flag2;
        if (J && !K && !Q)
        {
            numFlipFlops++;
            Q = true;
        }
        else if (!J && K && Q)
        {
            numFlipFlops++;
            Q = false;
        }
    }
    return numFlipFlops;
}

uint32_t DataManager::ACDATA_ANALYSIS_isACDataAcceptable(float targetAmplitude, float WEgain, float Igain, currentRange_t currentRange, uint16_t ACamplitude)
{
  uint32_t status = 0;
  //ACDATA_ANALYSIS_smoothData();

  /* 1) Activate "symptom" flags */
  /* a) Ewe signal is shifted out of frame */
  if (ACDATA_ANALYSIS_checkBiasOffset(WE_DATA))
  {
    status |= WE_BIAS_ERROR_FLAG;
    if (++ACDATA_ANALYSIS_numWEBiasResets > MAX_NUM_BIAS_RESETS)
    {
      status |= WE_BIAS_RESET_LIMIT_REACHED_FLAG;
      ACDATA_ANALYSIS_numWEBiasResets = 0;
    }
  }
  /* b) I signal is shifted out of frame */
  if (ACDATA_ANALYSIS_checkBiasOffset(CURRENT_DATA))
  {
    status |= I_BIAS_ERROR_FLAG;
    if (++ACDATA_ANALYSIS_numIBiasResets > MAX_NUM_BIAS_RESETS)
    {
      status |= I_BIAS_RESET_LIMIT_REACHED_FLAG;
      ACDATA_ANALYSIS_numIBiasResets = 0;
    }
  }
  /* c) I and/or EWE amplitudes are too large */
  if (ACDATA_ANALYSIS_IAmplitude > 1750)
    status |= I_AMP_TOO_BIG_FLAG;
  if (ACDATA_ANALYSIS_WEAmplitude > 1750)
    status |= WE_AMP_TOO_BIG_FLAG;
  /* d) I and/or EWE amplitudes are too small */
  if (!(status & I_BIAS_ERROR_FLAG) && (ACDATA_ANALYSIS_IAmplitude < 250))
    status |= I_AMP_TOO_SMALL_FLAG;
  if (!(status & WE_BIAS_ERROR_FLAG) && (ACDATA_ANALYSIS_WEAmplitude < 250))
    status |= WE_AMP_TOO_SMALL_FLAG;
  /* e) DACac amplitude is too large or too small */
  float ACAmpError;
  //if (phyLevel->opmode() == POTENTIOSTATIC)
  if(true)
    ACAmpError = ACDATA_ANALYSIS_getACAmpError(targetAmplitude, /*phyLevel->*/WEgain/*()*/);
  else  //galvanostatic mode
    ACAmpError = ACDATA_ANALYSIS_getACAmpError(targetAmplitude, /*phyLevel->*/Igain/*()*/, /*phyLevel->*/currentRange/*()*/);
  if (ACAmpError > 1.5)
    status |= CTRL_AMP_TOO_LARGE_FLAG;
  else if (ACAmpError < 0.67)
    status |= CTRL_AMP_TOO_SMALL_FLAG;
  if (/*phyLevel->*/ACamplitude/*()*/ == DAC_AC_RESOLUTION - 1)
    status |= CTRL_AMP_MAXED_OUT_FLAG;

  /* 2) Activate "action" flags */
  /* a) EWE is out of frame -> reset EWE bias.
  if number of resets > Max -> zoom out */
  if (status & WE_BIAS_ERROR_FLAG)
  {
    if (status & WE_BIAS_RESET_LIMIT_REACHED_FLAG)
      status |= DECREASE_EWE_GAIN_FLAG;
  }
  /* b) I is out of frame -> reset I bias.
  if number of resets > Max -> zoom out or increment current range */
  if (status & I_BIAS_ERROR_FLAG)
  {
    if (status & I_BIAS_RESET_LIMIT_REACHED_FLAG)
      status |= DECREASE_I_GAIN_FLAG;
  }
  /* c1) Calculate the minimum DACac amplitude based on calibration constants */
  uint16_t MinDACAmpBIN;
  //if (phyLevel->opmode() == POTENTIOSTATIC)
  if(true)
    MinDACAmpBIN = (uint16_t)(targetAmplitude * _cal.m_DACac);
  else
    MinDACAmpBIN = (uint16_t)(targetAmplitude * _cal.m_DACac * _cal.b_DACdc_I[/*phyLevel->*/currentRange/*()*/] / _cal.b_DACdc_V);
  MinDACAmpBIN = MIN(DAC_AC_RESOLUTION - 1, MinDACAmpBIN);

  /* c2) Correct the ACAmplitude based on feedback from the last run */
  //if ((phyLevel->opmode() == POTENTIOSTATIC) && !(status & WE_BIAS_ERROR_FLAG) ||             //if EWE is the controlled parameter and EWEBias is ok, OR
  //  (phyLevel->opmode() == GALVANOSTATIC) && !(status & I_BIAS_ERROR_FLAG))             //if I is the controlled parameter and IBias is ok
  if (!(status & WE_BIAS_ERROR_FLAG))
  {
    status |= ADJUST_CTRL_AMP_FLAG;

    /* if control amplitude is overly large (>1.5) or overly small (<0.6) redo the sample
    if the sample is overly small and the DACac is maxed out, increment the current range */
    if ((status & CTRL_AMP_TOO_LARGE_FLAG) && /*phyLevel->*/ACamplitude/*()*/ != MinDACAmpBIN)
      status |= REDO_SAMPLE_FLAG;
    else if (status & CTRL_AMP_TOO_SMALL_FLAG && !(status & CTRL_AMP_MAXED_OUT_FLAG))
      status |= REDO_SAMPLE_FLAG;
    else if (status & CTRL_AMP_TOO_SMALL_FLAG && status & CTRL_AMP_MAXED_OUT_FLAG)
    {
      if (/*phyLevel->*/currentRange/*()*/ != RANGE0)
        status |= INCREASE_IRANGE_FLAG;
      else
        status |= END_EXPERIMENT_NODE_FLAG;
    }
  }

  /* d) Catch the case where the bias is slightly off, but the amplitude is clearly too high anyway */
 // if ((phyLevel->opmode() == POTENTIOSTATIC && (status & WE_BIAS_ERROR_FLAG) && (ACAmpError > 1)) ||
  //  (phyLevel->opmode() == GALVANOSTATIC && (status & I_BIAS_ERROR_FLAG) && (ACAmpError > 1)))
      if ((status & WE_BIAS_ERROR_FLAG) && (ACAmpError > 1))
  {
    status |= ADJUST_CTRL_AMP_FLAG;
  }

  /* e) Check the gain levels */
  if (!(status & WE_BIAS_ERROR_FLAG))
  {
    if (!(status & CTRL_AMP_TOO_SMALL_FLAG) && (status & WE_AMP_TOO_SMALL_FLAG))
      status |= INCREASE_EWE_GAIN_FLAG;
    else if ((!(status & CTRL_AMP_TOO_LARGE_FLAG) || /*phyLevel->*/ACamplitude/*()*/ == MinDACAmpBIN) && (status & WE_AMP_TOO_BIG_FLAG))
      status |= DECREASE_EWE_GAIN_FLAG;
  }
  if (!(status & I_BIAS_ERROR_FLAG))
  {
    if (!(status & CTRL_AMP_TOO_SMALL_FLAG) && (status & I_AMP_TOO_SMALL_FLAG))
      status |= INCREASE_I_GAIN_FLAG;
    else if ((!(status & CTRL_AMP_TOO_LARGE_FLAG) ||/* phyLevel->*/ACamplitude/*()*/ == MinDACAmpBIN) && (status & I_AMP_TOO_BIG_FLAG))
      status |= DECREASE_I_GAIN_FLAG;
  }

  /* Catch the case where the dependent var bias is slightly off, but the amplitude is clearly too high anyway */
 // if (phyLevel->opmode() == POTENTIOSTATIC && (status & I_BIAS_ERROR_FLAG) && (status & I_AMP_TOO_BIG_FLAG))
  if ((status & I_BIAS_ERROR_FLAG) && (status & I_AMP_TOO_BIG_FLAG))
    status |= DECREASE_I_GAIN_FLAG;
  //else if (phyLevel->opmode() == GALVANOSTATIC && (status & WE_BIAS_ERROR_FLAG) && (status & WE_AMP_TOO_BIG_FLAG))
  //  status |= DECREASE_EWE_GAIN_FLAG;

  /* 3) Check status history for flip-flops and to check # of tries */
  status |= ACDATA_ANALYSIS_checkHistory(status);

  /* 4) Apply the effects of the flags */
  if (status & I_BIAS_ERROR_FLAG)
    status |= I_BIAS_RESET_FLAG | REDO_SAMPLE_FLAG;
  if (status & WE_BIAS_ERROR_FLAG)
    status |= WE_BIAS_RESET_FLAG | REDO_SAMPLE_FLAG;

  /* Divide by ACAmpError, or double if amplitude == 0 */
  /*if (status & ADJUST_CTRL_AMP_FLAG)
  {
    if (ACAmpError != 0.0)
      phyLevel->setACamplitude((uint16_t)MAX(MIN(DAC_AC_RESOLUTION - 1, (float)phyLevel->ACamplitude() / ACAmpError), MinDACAmpBIN));
    else
      phyLevel->setACamplitude(MAX(MIN(DAC_AC_RESOLUTION - 1, (phyLevel->ACamplitude() + 1) * 2), MinDACAmpBIN));
  }*/

  if ((status & DECREASE_EWE_GAIN_FLAG) && !(status & WE_GAIN_FLIP_FLOP_FLAG))
  {
    //if (phyLevel->WEgainSetting() != AC_GAIN1)
        if (true)
    {
      //phyLevel->decrementWEgain();  ACDATA_ANALYSIS_numWEBiasResets = 0;
      //BAREMETAL_PINS_delay(5 * MILLISECONDS);
      status |= REDO_SAMPLE_FLAG;
    }
    else
      status |= END_EXPERIMENT_NODE_FLAG;
  }
  else if (status & INCREASE_EWE_GAIN_FLAG && !(status & WE_GAIN_FLIP_FLOP_FLAG) /*&& phyLevel->WEgainSetting() != AC_GAIN1000*/)
  {
    //phyLevel->incrementWEgain();
    //BAREMETAL_PINS_delay(5 * MILLISECONDS);
    status |= REDO_SAMPLE_FLAG | WE_BIAS_RESET_FLAG;
  }
  if ((status & DECREASE_I_GAIN_FLAG) && !(status & I_GAIN_FLIP_FLOP_FLAG) && !(status & INCREASE_IRANGE_FLAG))
  {
    //if (phyLevel->IgainSetting() != AC_GAIN1)
      if(true)
    {
      //phyLevel->decrementIgain();  ACDATA_ANALYSIS_numIBiasResets = 0;
      //BAREMETAL_PINS_delay(5 * MILLISECONDS);
      status |= REDO_SAMPLE_FLAG;
    }
    //else if (phyLevel->currentRange() != RANGE0)
    else if (true)
    {
      status |= INCREASE_IRANGE_FLAG;
    }
    else
      status |= END_EXPERIMENT_NODE_FLAG;
  }
  else if ((status & INCREASE_I_GAIN_FLAG) && !(status & I_GAIN_FLIP_FLOP_FLAG) /*&& (phyLevel->IgainSetting() != AC_GAIN1000)*/)
  {
    //phyLevel->incrementIgain();
    //BAREMETAL_PINS_delay(5 * MILLISECONDS);
    status |= REDO_SAMPLE_FLAG | I_BIAS_RESET_FLAG;
  }
  if ((status & INCREASE_IRANGE_FLAG) && !(status & I_RANGE_FLIP_FLOP_FLAG))
  {
    //phyLevel->incrementCurrentRange();
    //BAREMETAL_PINS_delay(5 * MILLISECONDS);
    ACDATA_ANALYSIS_numIBiasResets = 0;
    status |= I_BIAS_RESET_FLAG | REDO_SAMPLE_FLAG;
  }
  else if ((status & DECREASE_IRANGE_FLAG) && !(status & I_RANGE_FLIP_FLOP_FLAG))
  {
    //phyLevel->decrementCurrentRange();
    //BAREMETAL_PINS_delay(5 * MILLISECONDS);
    status |= I_BIAS_RESET_FLAG | REDO_SAMPLE_FLAG;
  }
  ///if (status & I_BIAS_RESET_FLAG)
    //phyLevel->zeroIbias();
  //if (status & WE_BIAS_RESET_FLAG)
    //phyLevel->zeroWEbias();

  //mUSBSerial.debug_log("analysis status:");
  //mUSBSerial.debug_log_hex(status);
  
//if (status & END_EXPERIMENT_NODE_FLAG || status & MAX_NUM_TRIES_EXCEEDED_FLAG)
//    return true;
//else  
//  return !(status & REDO_SAMPLE_FLAG);
  return status;
}

QString DataManager::decomposeStatus(uint32_t status)
{
    QMap<uint32_t, QString> statusStringsMap;
    statusStringsMap.insert(I_BIAS_ERROR_FLAG, "I bias error flag");
    statusStringsMap.insert(WE_BIAS_ERROR_FLAG, "WE bias error flag");
    statusStringsMap.insert(I_BIAS_RESET_LIMIT_REACHED_FLAG, "I bias reset limit reached");
    statusStringsMap.insert(WE_BIAS_RESET_LIMIT_REACHED_FLAG, "WE_BIAS_RESET_LIMIT_REACHED_FLAG");
    statusStringsMap.insert(I_AMP_TOO_BIG_FLAG, "I_AMP_TOO_BIG_FLAG");
    statusStringsMap.insert(WE_AMP_TOO_BIG_FLAG, "WE_AMP_TOO_BIG_FLAG");
    statusStringsMap.insert(I_AMP_TOO_SMALL_FLAG, "I_AMP_TOO_SMALL_FLAG");
    statusStringsMap.insert(WE_AMP_TOO_SMALL_FLAG, "WE_AMP_TOO_SMALL_FLAG");
    statusStringsMap.insert(CTRL_AMP_TOO_LARGE_FLAG, "CTRL_AMP_TOO_LARGE_FLAG");
    statusStringsMap.insert(CTRL_AMP_TOO_SMALL_FLAG, "CTRL_AMP_TOO_SMALL_FLAG");
    statusStringsMap.insert(CTRL_AMP_MAXED_OUT_FLAG, "CTRL_AMP_MAXED_OUT_FLAG");
    statusStringsMap.insert(MAX_NUM_TRIES_EXCEEDED_FLAG, "MAX_NUM_TRIES_EXCEEDED_FLAG");
    statusStringsMap.insert(I_GAIN_FLIP_FLOP_FLAG, "I_GAIN_FLIP_FLOP_FLAG");
    statusStringsMap.insert(WE_GAIN_FLIP_FLOP_FLAG, "WE_GAIN_FLIP_FLOP_FLAG");
    statusStringsMap.insert(I_RANGE_FLIP_FLOP_FLAG, "I_RANGE_FLIP_FLOP_FLAG");

    statusStringsMap.insert(REDO_SAMPLE_FLAG, "REDO_SAMPLE_FLAG");
    statusStringsMap.insert(END_EXPERIMENT_NODE_FLAG, "END_EXPERIMENT_NODE_FLAG");
    statusStringsMap.insert(I_BIAS_RESET_FLAG, "I_BIAS_RESET_FLAG");
    statusStringsMap.insert(WE_BIAS_RESET_FLAG, "WE_BIAS_RESET_FLAG");
    statusStringsMap.insert(DECREASE_EWE_GAIN_FLAG, "DECREASE_EWE_GAIN_FLAG");
    statusStringsMap.insert(INCREASE_EWE_GAIN_FLAG, "INCREASE_EWE_GAIN_FLAG");
    statusStringsMap.insert(DECREASE_I_GAIN_FLAG, "DECREASE_I_GAIN_FLAG");
    statusStringsMap.insert(INCREASE_I_GAIN_FLAG, "INCREASE_I_GAIN_FLAG");
    statusStringsMap.insert(INCREASE_IRANGE_FLAG, "INCREASE_IRANGE_FLAG");
    statusStringsMap.insert(DECREASE_IRANGE_FLAG, "DECREASE_IRANGE_FLAG");
    statusStringsMap.insert(ADJUST_CTRL_AMP_FLAG, "ADJUST_CTRL_AMP_FLAG");

    QString retVal;
    for (int i = 0; i < 32; i++)
    {
        if (1 << i & status)
        {
            retVal.append(statusStringsMap[1 << i & status]);
            retVal.append("\n");
        }
    }
    return retVal;
}