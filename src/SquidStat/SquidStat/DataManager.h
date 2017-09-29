#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "global_typedefs.h"
#include "DAC_ADC_Buffer_types.h"
//#include "CommonObjs.h"
//#include "PhyLevel.h"
//#include "DerivCalculator.h"
//#include "CapacityTracker.h"
#include <cal.h>
#include <math.h>
#include "qstring.h"
#include "qmap.h"

#define MAX_NUM_BIAS_RESETS 10
#define MAX_NUM_TRIES 30
#define STATUS_HISTORY_LENGTH MAX_NUM_TRIES
#define MAX_NUM_FLIP_FLOPS 3

/* Analysis status flags */
/* "Symptom" flags */
#define I_BIAS_ERROR_FLAG                   1
#define WE_BIAS_ERROR_FLAG                  2
#define I_BIAS_RESET_LIMIT_REACHED_FLAG     4
#define WE_BIAS_RESET_LIMIT_REACHED_FLAG    8
#define I_AMP_TOO_BIG_FLAG                  0x10
#define WE_AMP_TOO_BIG_FLAG                 0x20
#define I_AMP_TOO_SMALL_FLAG                0x40
#define WE_AMP_TOO_SMALL_FLAG               0x80
#define CTRL_AMP_TOO_LARGE_FLAG             0x100
#define CTRL_AMP_TOO_SMALL_FLAG             0x200
#define CTRL_AMP_MAXED_OUT_FLAG             0x400
#define MAX_NUM_TRIES_EXCEEDED_FLAG         0x800
#define I_GAIN_FLIP_FLOP_FLAG               0x1000
#define WE_GAIN_FLIP_FLOP_FLAG              0x2000
#define I_RANGE_FLIP_FLOP_FLAG              0x4000

/* "Action" flags */
#define REDO_SAMPLE_FLAG                    0x00010000
#define END_EXPERIMENT_NODE_FLAG            0x00020000
#define I_BIAS_RESET_FLAG                   0x00040000
#define WE_BIAS_RESET_FLAG                  0x00080000
#define DECREASE_EWE_GAIN_FLAG              0x00100000
#define INCREASE_EWE_GAIN_FLAG              0x00200000
#define DECREASE_I_GAIN_FLAG                0x00400000
#define INCREASE_I_GAIN_FLAG                0x00800000
#define INCREASE_IRANGE_FLAG                0x01000000
#define DECREASE_IRANGE_FLAG                0x02000000
#define ADJUST_CTRL_AMP_FLAG                0x04000000

typedef enum {
    CURRENT_DATA, WE_DATA
}EWE_or_I_t;

class DataManager //: CommonObjs
{
public:
    //DataManager();
    //~DataManager();

    cal_t _cal;
    QString decomposeStatus(uint32_t status);

    void tasks();
    //void initReferences(PhyLevel * phy, ADCBuffer_t * ADCdc, uint16_t * ADCac_EWEBufRaw, uint16_t * ADCac_IBufRaw);
    uint16_t incrementADCMultCounter();
    uint16_t ADCMultCounterMax() { return _ADCmultCounterMax; };
    void toggleADCdcBuffers();
    //void initADCbufProperties(ExperimentNode_t * pNode);
    uint16_t incrPointsSkippedMCU();
    //void initDerivTracking(ExperimentNode_t * pNode);
    void stopDerivTracking();
    void initCapacityTracking();
    void stopCapacityTracking();
    void incrADCdcDataIndex(){ _ADCdcActiveBuf->dataIndex++; };

    /*********************** Setters ************************/
    void setMajorDataReadyFlag(){ _ADCdcInactiveBufFullFlag = true; };
    void setMinorDataReadyFlag();
    void setMaxPointsSkippedMCU(uint16_t num){ maxPointsSkippedMCU = num; };
    void setACDataFlag();


    /*********************** Getters ************************/
    bool isDCSampling() { return _isDCSampling; };
    bool isACSampling() { return _isACSampling; };
    bool majorDataReady() { return _majorDataReady; };
    bool minorDataReady() { return _minorDataReady; };
    volatile ADCBuffer_t * ADCdcActiveBuf() { return _ADCdcActiveBuf; };
    //bool isTrackingCapacity() { return capacity_tracker.isTracking; };
    //long double Capacity() { return capacity_tracker.getCapacity(); };
    //float dVdt() { return _dVdt.get_dVdt(&_cal); };
    //float dIdt() { return _dIdt.get_dIdt(&_cal, phyLevel->currentRange()); };
        //uint16_t * ACdata_I() { return phyLevel->ADCacBufAddr_I() == (uint32_t)_ACdata_I_lowerBuf ? _ACdata_I_upperBuf : _ACdata_I_lowerBuf; };
        //uint16_t * ACdata_EWE() { return phyLevel->ADCacBufAddr_EWE() == (uint32_t)_ACdata_EWE_lowerBuf ? _ACdata_EWE_upperBuf : _ACdata_EWE_lowerBuf; };
        //uint16_t * ACdata_I() { return phyLevel->ADCacActiveDMAbank() == 0 ? _ACdata_I_upperBuf : _ACdata_I_lowerBuf; };
        //uint16_t * ACdata_EWE() { return phyLevel->ADCacActiveDMAbank() == 0 ? _ACdata_EWE_upperBuf : _ACdata_EWE_lowerBuf; };
    //uint16_t * ACdata_I() { return phyLevel->ADCacActiveDMAbank() ? _ACdata_I_upperBuf : _ACdata_I_lowerBuf; };
    //uint16_t * ACdata_EWE() { return phyLevel->ADCacActiveDMAbank() ? _ACdata_EWE_upperBuf : _ACdata_EWE_lowerBuf; };

    /* AC data analysis */
    uint32_t ACDATA_ANALYSIS_isACDataAcceptable(float targetAmplitude, float WEgain, float Igain, currentRange_t currentRange, uint16_t ACamplitude);
    void ACDATA_ANALYSIS_resetAll();

//private:
    ExperimentalDataPoint_t averageDCdata(volatile ADCBuffer_t * buf);

    //PhyLevel * phyLevel;

    /* ADCac data */
    uint16_t * _ACdata_I_lowerBuf;
    uint16_t * _ACdata_I_upperBuf;
    uint16_t * _ACdata_EWE_lowerBuf;
    uint16_t * _ACdata_EWE_upperBuf;

    /* ADCdc data */
    volatile ADCBuffer_t * _ADCdcActiveBuf;
    volatile ADCBuffer_t * _ADCdcInactiveBuf;
    uint16_t dataIndex = 0;
    uint16_t maxPointsSkippedMCU = 1;
    uint16_t _pointsSkippedMCU = 0;
    bool _ADCdcInactiveBufFullFlag;
    bool _isDCSampling;
    bool _isACSampling;
    bool _majorDataReady;
    bool _minorDataReady;
    uint16_t _numSkippedTransmissions;
    //DerivCalculator _dVdt;
    //DerivCalculator _dIdt;
    //CapacityTracker capacity_tracker;

    /* For fast slew rate mode */
    uint16_t _ADCmultCounterMax = 1;
    uint16_t _ADCmultCounter = 0;

    /* AC data analysis */
    void ACDATA_ANALYSIS_smoothData(int16_t * rawData);
    bool ACDATA_ANALYSIS_checkBiasOffset(EWE_or_I_t EWE_or_I);
    float ACDATA_ANALYSIS_getACAmpError(float targetAmp, float gain, currentRange_t currentRange = OFF);
    uint32_t ACDATA_ANALYSIS_checkHistory(uint32_t flag);
    int ACDATA_ANALYSIS_numIBiasResets;
    int ACDATA_ANALYSIS_numWEBiasResets;
    float ACDATA_ANALYSIS_WEAmplitude;
    float ACDATA_ANALYSIS_IAmplitude;

    void ACDATA_ANALYSIS_resetStats();
    int ACDATA_ANALYSIS_checkForFlipFlops(uint32_t flag1, uint32_t flag2);
    uint16_t ACDATA_ANALYSIS_smoothedEweData[ADCacBUF_SIZE - ADCacROLLING_AVG_SIZE];
    uint16_t ACDATA_ANALYSIS_smoothedIData[ADCacBUF_SIZE - ADCacROLLING_AVG_SIZE];
    uint16_t ACDATA_ANALYSIS_num4095sEWE;
    uint16_t ACDATA_ANALYSIS_num4095sI;
    uint16_t ACDATA_ANALYSIS_numZeroesEWE;
    uint16_t ACDATA_ANALYSIS_numZeroesI;
    uint16_t ACDATA_ANALYSIS_EWEmax;
    uint16_t ACDATA_ANALYSIS_EWEmin;
    uint16_t ACDATA_ANALYSIS_Imax;
    uint16_t ACDATA_ANALYSIS_Imin;
    uint32_t ACDATA_ANALYSIS_statusHistory[STATUS_HISTORY_LENGTH];
    uint16_t ACDATA_ANALYSIS_historyIndex = 0;
    int ACDATA_ANALYSIS_numEWEgainFlipFlops = 0;
    int ACDATA_ANALYSIS_numIgainFlipFlops = 0;
    int ACDATA_ANALYSIS_numIrangeFlipFlops = 0;
};


#endif