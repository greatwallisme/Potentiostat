#include "PhaseCalibration.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Phase-Calibration"

#define UPPER_FREQ_OBJ_NAME		"upper-frequency"
#define LOWER_FREQ_OBJ_NAME		"lower-frequency"
#define UPPER_FREQ_UNITS_OBJ_NAME "upper-frequency-units"
#define LOWER_FREQ_UNITS_OBJ_NAME "lower-frequency-units"
#define STEPS_PER_DEC_OBJ_NAME	"step-per-decade"
#define LOAD_RESISTANCE_OBJ_NAME			"Load-resistance"

#define UPPER_FREQ_DEFAULT		1000		//(in kHz)
#define LOWER_FREQ_DEFAULT		1	//(in kHz)
#define STEPS_PER_DEC_DEFAULT	5
#define LOAD_RESISTANCE_DEFAULT			0.1

#define PLOT_VAR_EXP_NUM        "Experiment leg"
#define PLOT_VAR_FREQ					"Frequency"
#define PLOT_VAR_IMPEDANCE				"|Z|"
#define PLOT_VAR_PHASE					"Phase (degrees)"
#define PLOT_VAR_WEGAIN         "Working electrode gain"
#define PLOT_VAR_IGAIN          "Current gain"
#define PLOT_VAR_IRANGE         "Current range"
#define PLOT_VAR_LOAD           "Load (Ohms)"
#define PLOT_VAR_ERR            "Error (Ohms)"

QString PhaseCalibration::GetShortName() const {
	return "Calibration -- phase angle";
}
QString PhaseCalibration::GetFullName() const {
	return "EIS phase angle calibration";
}
QString PhaseCalibration::GetDescription() const {
	return "...";
}
QStringList PhaseCalibration::GetCategory() const {
    return QStringList() <<
        "Calibration";

}
ExperimentTypeList PhaseCalibration::GetTypes() const {
	return ExperimentTypeList() << ET_AC;
}
QPixmap PhaseCalibration::GetImage() const {
	return QPixmap(":/Experiments/NormalPulseVoltammetry");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* PhaseCalibration::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

  int row = 0;
  _INSERT_RIGHT_ALIGN_COMMENT("Upper frequency", row, 0);
  _INSERT_TEXT_INPUT(UPPER_FREQ_DEFAULT, UPPER_FREQ_OBJ_NAME, row, 1);
  _START_DROP_DOWN(UPPER_FREQ_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("kHz");
  _ADD_DROP_DOWN_ITEM("Hz");
  _ADD_DROP_DOWN_ITEM("mHz");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Lower frequency", row, 0);
  _INSERT_TEXT_INPUT(LOWER_FREQ_DEFAULT, LOWER_FREQ_OBJ_NAME, row, 1);
  _START_DROP_DOWN(LOWER_FREQ_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("kHz");
  _ADD_DROP_DOWN_ITEM("Hz");
  _ADD_DROP_DOWN_ITEM("mHz");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Steps per decade", row, 0);
  _INSERT_TEXT_INPUT(STEPS_PER_DEC_DEFAULT, STEPS_PER_DEC_OBJ_NAME, row, 1);

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Load resistance: ", row, 0);
  _START_DROP_DOWN(LOAD_RESISTANCE_OBJ_NAME, row, 1);
  _ADD_DROP_DOWN_ITEM("10");
  _ADD_DROP_DOWN_ITEM("100");
  _ADD_DROP_DOWN_ITEM("1k");
  _ADD_DROP_DOWN_ITEM("10k");
  _END_DROP_DOWN();
  _INSERT_LEFT_ALIGN_COMMENT("Ohms", row, 2);
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	_SET_ROW_STRETCH(++row, 1);
	USER_INPUT_END();
}
NodesData PhaseCalibration::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
    NODES_DATA_START(wdg, TOP_WIDGET_NAME);

    double upperFreq;
    double lowerFreq;
    QString upperFreqUnits_str;
    QString lowerFreqUnits_str;
    double stepsPerDecade;
    QString load_str;
    //QString fixedGainMode_str;
    //QString gainSelection_str;

    GET_TEXT_INPUT_VALUE_DOUBLE(upperFreq, UPPER_FREQ_OBJ_NAME);
    GET_TEXT_INPUT_VALUE_DOUBLE(lowerFreq, LOWER_FREQ_OBJ_NAME);
    GET_SELECTED_DROP_DOWN(upperFreqUnits_str, UPPER_FREQ_UNITS_OBJ_NAME);
    GET_SELECTED_DROP_DOWN(lowerFreqUnits_str, LOWER_FREQ_UNITS_OBJ_NAME);
    GET_TEXT_INPUT_VALUE_DOUBLE(stepsPerDecade, STEPS_PER_DEC_OBJ_NAME);

    GET_SELECTED_DROP_DOWN(load_str, LOAD_RESISTANCE_OBJ_NAME);
    //GET_SELECTED_DROP_DOWN(fixedGainMode_str, FIXED_GAIN_MODE_OBJ_NAME);
    //GET_SELECTED_DROP_DOWN(gainSelection_str, GAIN_SELECT_OBJ_NAME);

    upperFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(upperFreqUnits_str);
    lowerFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(lowerFreqUnits_str);

    struct phase_cal_experiment_struct {
        int expNum;
        double load;
        ACgain_t gainWE;
        ACgain_t gainI;
        currentRange_t irange;
        double amplitude;
    };

    /* Experiment 1 (Does amplitude alone affect phase behavior?) */
    /* phase vs amplitude @ fixed range, fixed gain and fixed load */
    //QList<phase_cal_experiment_struct> calExpStructs_100, calExpStructs_56, calExpStructs_10;
    //calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN1, RANGE2, 10 });    //experiment num, load/ohms, WE gain, Igain, iRange, amp/mA
    //calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN1, RANGE2, 5 });
    //calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN1, RANGE2, 2 });
    //calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN1, RANGE2, 1 });
    //calExpStructs_10.append(phase_cal_experiment_struct{ 1, 10, AC_GAIN1, AC_GAIN1, RANGE1, 100 });    //experiment num, load/ohms, WE gain, Igain, iRange, amp/mA
    //calExpStructs_10.append(phase_cal_experiment_struct{ 1, 10, AC_GAIN1, AC_GAIN1, RANGE1, 50 });
    //calExpStructs_10.append(phase_cal_experiment_struct{ 1, 10, AC_GAIN1, AC_GAIN1, RANGE1, 20 });
    //calExpStructs_10.append(phase_cal_experiment_struct{ 1, 10, AC_GAIN1, AC_GAIN1, RANGE1, 10 });

    /* Experiment 2 (Calibrate phase w.r.t. gain) */
    /* phase vs freq @ variable Igain */
    /*QList<phase_cal_experiment_struct> calExpStructs_100, calExpStructs_1k, calExpStructs_10k;
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN1, RANGE2, 10 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN2, RANGE2, 5 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN5, RANGE2, 2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN10, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN20, RANGE2, 0.5 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN50, RANGE2, 0.2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN100, RANGE2, 0.1 });*/
    /*calExpStructs_1k.append(phase_cal_experiment_struct{ 1, 1000, AC_GAIN1, AC_GAIN10, RANGE2, 1 });
    calExpStructs_1k.append(phase_cal_experiment_struct{ 1, 1000, AC_GAIN1, AC_GAIN20, RANGE2, 0.5 });
    calExpStructs_1k.append(phase_cal_experiment_struct{ 1, 1000, AC_GAIN1, AC_GAIN50, RANGE2, 0.2 });
    calExpStructs_1k.append(phase_cal_experiment_struct{ 1, 1000, AC_GAIN1, AC_GAIN100, RANGE2, 0.1 });
    calExpStructs_10k.append(phase_cal_experiment_struct{ 1, 10000, AC_GAIN1, AC_GAIN100, RANGE2, 0.1 });
    calExpStructs_10k.append(phase_cal_experiment_struct{ 1, 10000, AC_GAIN1, AC_GAIN200, RANGE2, 0.05 });
    calExpStructs_10k.append(phase_cal_experiment_struct{ 1, 10000, AC_GAIN1, AC_GAIN500, RANGE2, 0.02 });
    calExpStructs_10k.append(phase_cal_experiment_struct{ 1, 10000, AC_GAIN1, AC_GAIN1000, RANGE2, 0.01 });*/

    /* Experiment 3 (Calibrate phase w.r.t. gain) */
    /* phase vs freq @ variable Vgain */
    /*QList<phase_cal_experiment_struct> calExpStructs_100, calExpStructs_1k, calExpStructs_10k;
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN1, RANGE2, 10 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN2, AC_GAIN1, RANGE2, 5 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN5, AC_GAIN1, RANGE2, 2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN10, AC_GAIN1, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN20, AC_GAIN1, RANGE2, 0.5 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN50, AC_GAIN1, RANGE2, 0.2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN100, AC_GAIN1, RANGE2, 0.1 });*/

    /* Experiment 3 (Phase w.r.t. each gain stage) */
    /*QList<phase_cal_experiment_struct> calExpStructs_100;
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN1, RANGE2, 10 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN2, AC_GAIN1, RANGE2, 5 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN5, AC_GAIN1, RANGE2, 2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN10, AC_GAIN1, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN10_alt1, AC_GAIN1, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN10_alt2, AC_GAIN1, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN2, RANGE2, 5 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN5, RANGE2, 2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN10, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN10_alt1, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN1, AC_GAIN10_alt2, RANGE2, 1 });*/

    /* Experiment 4 (phase calibration verification) */
    QList<phase_cal_experiment_struct> calExpStructs_100;
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN10, AC_GAIN20, RANGE2, 5 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN10, AC_GAIN50, RANGE2, 2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN100, AC_GAIN100, RANGE2, 2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN100, AC_GAIN200, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN100, AC_GAIN500, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN20, AC_GAIN10, RANGE2, 5 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN50, AC_GAIN10, RANGE2, 2 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN100, AC_GAIN100, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN200, AC_GAIN100, RANGE2, 1 });
    calExpStructs_100.append(phase_cal_experiment_struct{ 1, 100, AC_GAIN500, AC_GAIN100, RANGE2, 1 });


    QMap<QString, QList<phase_cal_experiment_struct>> calExpStructMap;
    calExpStructMap.insert("100", calExpStructs_100);
    //calExpStructMap.insert("1k", calExpStructs_1k);
    //calExpStructMap.insert("10k", calExpStructs_10k);
    
  QList<double> frequencyList = ExperimentCalcHelperClass::calculateFrequencyList(lowerFreq, upperFreq, stepsPerDecade);

  for (int n = 0; n < calExpStructMap[load_str].count(); n++)
  {
      exp.isHead = false;
      exp.isTail = false;
      exp.nodeType = DCNODE_POINT_GALV;
      exp.currentRangeMode = exp.DCPoint_galv.Irange = calExpStructMap[load_str][n].irange;
      exp.DCPoint_galv.dVdtMin = 0;
      exp.DCPoint_galv.IPoint = 0;
      exp.DCPoint_galv.isTrackingCapacity = false;
      exp.DCPoint_galv.Vmax = MAX_VOLTAGE;
      exp.DCPoint_galv.Vmin = -MAX_VOLTAGE;
      exp.tMin = exp.tMax = 2 * SECONDS;
      ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 4);
      PUSH_NEW_NODE_DATA();

      for (int i = 0; i < frequencyList.count(); i++)
      {
          exp.isHead = exp.isTail = false;
          exp.tMin = 0;
          exp.tMax = 0xffffffffffffffff;
          exp.MaxPlays = 4;
          exp.isStartingNewFile = false;

          exp.nodeType = FRA_FIXED_GAINS_AND_RANGE;
          exp.ACsamplingParams.frequency = frequencyList[i];

          exp.ACsamplingParams.firstTime = i == 0;
          exp.currentRangeMode = exp.FRA_fixedGainsAndRange_node.IRange = calExpStructMap[load_str][n].irange;
          exp.FRA_fixedGainsAndRange_node.IBias = 0;
          exp.FRA_fixedGainsAndRange_node.IGainSetting = calExpStructMap[load_str][n].gainI;
          exp.FRA_fixedGainsAndRange_node.WEGainSetting = calExpStructMap[load_str][n].gainWE;
          exp.ACsamplingParams.amplitudeTarget = (float)calExpStructMap[load_str][n].amplitude;
          ExperimentCalcHelperClass::calcACSamplingParams(&calData, &exp);

          PUSH_NEW_NODE_DATA();
      }
  }

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList PhaseCalibration::GetXAxisParameters(ExperimentType type) const {
    QStringList ret;

    if (type == ET_AC) {
        ret <<
            PLOT_VAR_FREQ;
    }

    return ret;
}
QStringList PhaseCalibration::GetYAxisParameters(ExperimentType type) const {
    QStringList ret;

    if (type == ET_AC) {
        ret <<
            PLOT_VAR_IMPEDANCE <<
            PLOT_VAR_PHASE;
    }

    return ret;
}
void PhaseCalibration::PUSH_NEW_AC_DATA_DEFINITION{
    ComplexDataPoint_t dataPoint;
    GET_COMPLEX_DATA_POINT(dataPoint);
    //double numCycles = ExperimentCalcHelperClass::estimatePeriod(*dataHeader);
    //dataPoint = ExperimentCalcHelperClass::AnalyzeFRA(dataHeader->frequency, ACrawdata, numACBuffers, dataHeader->gainVoltage, dataHeader->gainCurrent, numCycles, &calData, dataHeader->IRange);
    
    PUSH_BACK_DATA(PLOT_VAR_FREQ, dataPoint.frequency);
    PUSH_BACK_DATA(PLOT_VAR_IMPEDANCE, dataPoint.ImpedanceMag);
    PUSH_BACK_DATA(PLOT_VAR_PHASE, dataPoint.phase);
    PUSH_BACK_DATA(PLOT_VAR_WEGAIN, dataheader.gainVoltage);
    PUSH_BACK_DATA(PLOT_VAR_IGAIN, dataheader.gainCurrent);
    PUSH_BACK_DATA(PLOT_VAR_IRANGE, (int)dataheader.IRange);
    PUSH_BACK_DATA(PLOT_VAR_ERR, dataPoint.error);
}

void PhaseCalibration::SaveAcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
    SAVE_DATA_HEADER_START();

    SAVE_AC_DATA_HEADER(PLOT_VAR_FREQ);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IMPEDANCE);
    SAVE_AC_DATA_HEADER(PLOT_VAR_PHASE);
    SAVE_AC_DATA_HEADER(PLOT_VAR_WEGAIN);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IGAIN);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IRANGE);
    SAVE_AC_DATA_HEADER(PLOT_VAR_ERR);

    SAVE_DATA_HEADER_END();
}

void PhaseCalibration::SaveAcData(QFile &saveFile, const DataMap &container) const {
    SAVE_DATA_START();

    SAVE_DATA(PLOT_VAR_FREQ);
    SAVE_DATA(PLOT_VAR_IMPEDANCE);
    SAVE_DATA(PLOT_VAR_PHASE);
    SAVE_DATA(PLOT_VAR_WEGAIN);
    SAVE_DATA(PLOT_VAR_IGAIN);
    SAVE_DATA(PLOT_VAR_IRANGE);
    SAVE_DATA(PLOT_VAR_ERR);

    SAVE_DATA_END();
}