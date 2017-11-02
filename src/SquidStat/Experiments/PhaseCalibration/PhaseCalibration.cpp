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
#define FIXED_GAIN_MODE_OBJ_NAME      "Fixed-gain-mode"
#define GAIN_SELECT_OBJ_NAME          "Gain-select"

#define UPPER_FREQ_DEFAULT		1000		//(in kHz)
#define LOWER_FREQ_DEFAULT		1	//(in kHz)
#define STEPS_PER_DEC_DEFAULT	5
#define LOAD_RESISTANCE_DEFAULT			0.1

#define PLOT_VAR_IMPEDANCE				"|Z|"
#define PLOT_VAR_PHASE					"Phase (degrees)"
#define PLOT_VAR_ABS_PHASE      "|Phase| (degrees)"
#define PLOT_VAR_IMP_REAL				"Z\'"
#define PLOT_VAR_IMP_IMAG				"Z\""
#define PLOT_VAR_NEG_IMP_IMAG			"-Z\""
#define PLOT_VAR_FREQ					"Frequency"

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
  _ADD_DROP_DOWN_ITEM("1");
  _ADD_DROP_DOWN_ITEM("10");
  _ADD_DROP_DOWN_ITEM("56");
  _ADD_DROP_DOWN_ITEM("270");
  _ADD_DROP_DOWN_ITEM("510");
  _ADD_DROP_DOWN_ITEM("2.4k");
  _ADD_DROP_DOWN_ITEM("4.7k");
  _ADD_DROP_DOWN_ITEM("10k");
  _END_DROP_DOWN();
  _INSERT_LEFT_ALIGN_COMMENT("Ohms", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Fixed gain mode: ", row, 0);
  _START_DROP_DOWN(FIXED_GAIN_MODE_OBJ_NAME, row, 1);
  _ADD_DROP_DOWN_ITEM("Fixed current gain");
  _ADD_DROP_DOWN_ITEM("Fixed voltage gain");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Gain: ", row, 0);
  _START_DROP_DOWN(GAIN_SELECT_OBJ_NAME, row, 1);
  _ADD_DROP_DOWN_ITEM("1");
  _ADD_DROP_DOWN_ITEM("2");
  _ADD_DROP_DOWN_ITEM("5");
  _ADD_DROP_DOWN_ITEM("10");
  _ADD_DROP_DOWN_ITEM("20");
  _ADD_DROP_DOWN_ITEM("50");
  _ADD_DROP_DOWN_ITEM("100");
  _ADD_DROP_DOWN_ITEM("200");
  _ADD_DROP_DOWN_ITEM("500");
  _ADD_DROP_DOWN_ITEM("1000");
  _END_DROP_DOWN();
	
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

    /* Experiment 1 */
    /* phase vs amplitude @ const gain, variable range and loadm*/
    QList<phase_cal_experiment_struct> calExpStructs_1, calExpStructs_2, calExpStructs_3;
    calExpStructs_1.append(phase_cal_experiment_struct{ 1, 1, AC_GAIN1, AC_GAIN1, RANGE0, 25 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1, 1, AC_GAIN1, AC_GAIN1, RANGE0, 50 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1, 1, AC_GAIN1, AC_GAIN1, RANGE0, 100 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1, 1, AC_GAIN1, AC_GAIN1, RANGE0, 150 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 1, 10, AC_GAIN1, AC_GAIN1, RANGE1, 10 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 1, 10, AC_GAIN1, AC_GAIN1, RANGE1, 5 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 1, 10, AC_GAIN1, AC_GAIN1, RANGE1, .2 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 1, 10, AC_GAIN1, AC_GAIN1, RANGE1, 1 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 1, 56, AC_GAIN1, AC_GAIN1, RANGE2, 2 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 1, 56, AC_GAIN1, AC_GAIN1, RANGE2, 1 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 1, 56, AC_GAIN1, AC_GAIN1, RANGE2, 0.5 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 1, 56, AC_GAIN1, AC_GAIN1, RANGE2, 0.2 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 1, 56, AC_GAIN1, AC_GAIN1, RANGE2, 0.1 });

    QMap<QString, QList<phase_cal_experiment_struct>> calExpStructMap;
    calExpStructMap.insert("1", calExpStructs_1);
    calExpStructMap.insert("10", calExpStructs_2);
    calExpStructMap.insert("56", calExpStructs_3);
    
  QList<double> frequencyList = ExperimentCalcHelperClass::calculateFrequencyList(lowerFreq, upperFreq, stepsPerDecade);

  for (int n = 0; n < calExpStructs_1.count(); n++)
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
          exp.isStartingNewFile = false;

          exp.nodeType = FRA_FIXED_GAINS_AND_RANGE;
          exp.ACsamplingParams.frequency = frequencyList[i];

          exp.ACsamplingParams.firstTime = i == 0;
          exp.currentRangeMode = exp.FRA_fixedGainsAndRange_node.IRange = calExpStructMap[load_str][n].irange;
          exp.FRA_fixedGainsAndRange_node.IBias = 0;
          exp.FRA_fixedGainsAndRange_node.IGainSetting = calExpStructMap[load_str][n].gainI;
          exp.FRA_fixedGainsAndRange_node.WEGainSetting = calExpStructMap[load_str][n].gainWE;
          exp.ACsamplingParams.amplitudeTarget = calExpStructMap[load_str][n].amplitude;
          ExperimentCalcHelperClass::calcACSamplingParams(&calData, &exp);

          if (i == 0)
          {
              exp.isHead = true;
          }
          if (i == frequencyList.count() - 1)
          {
              exp.isTail = true;
              exp.MaxPlays = 3;
              exp.branchHeadIndex = n * (frequencyList.count() + 1) + 1;
          }

          PUSH_NEW_NODE_DATA();
      }
      /* exp.nodeType = DUMMY_NODE;
       exp.MaxPlays = 1;
       exp.isHead = exp.isTail = false;
       exp.isStartingNewFile = true;
       PUSH_NEW_NODE_DATA();*/
       //}
  }

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList PhaseCalibration::GetXAxisParameters(ExperimentType type) const {
    QStringList ret;

    if (type == ET_AC) {
        ret <<
            PLOT_VAR_FREQ <<
            PLOT_VAR_IMP_REAL;
    }

    return ret;
}
QStringList PhaseCalibration::GetYAxisParameters(ExperimentType type) const {
    QStringList ret;

    if (type == ET_AC) {
        ret <<
            PLOT_VAR_IMPEDANCE <<
            PLOT_VAR_PHASE <<
            PLOT_VAR_ABS_PHASE <<
            PLOT_VAR_IMP_REAL <<
            PLOT_VAR_IMP_IMAG <<
            PLOT_VAR_NEG_IMP_IMAG;
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
    PUSH_BACK_DATA(PLOT_VAR_ABS_PHASE, abs(dataPoint.phase));
    PUSH_BACK_DATA(PLOT_VAR_IMP_REAL, dataPoint.ImpedanceReal);
    PUSH_BACK_DATA(PLOT_VAR_IMP_IMAG, dataPoint.ImpedanceImag);
    PUSH_BACK_DATA(PLOT_VAR_NEG_IMP_IMAG, -dataPoint.ImpedanceImag);
}

void PhaseCalibration::SaveAcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
    SAVE_DATA_HEADER_START();

    SAVE_AC_DATA_HEADER(PLOT_VAR_FREQ);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IMPEDANCE);
    SAVE_AC_DATA_HEADER(PLOT_VAR_PHASE);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IMP_REAL);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IMP_IMAG);
    SAVE_AC_DATA_HEADER(PLOT_VAR_NEG_IMP_IMAG);

    SAVE_DATA_HEADER_END();
}

void PhaseCalibration::SaveAcData(QFile &saveFile, const DataMap &container) const {
    SAVE_DATA_START();

    SAVE_DATA(PLOT_VAR_FREQ);
    SAVE_DATA(PLOT_VAR_IMPEDANCE);
    SAVE_DATA(PLOT_VAR_PHASE);
    SAVE_DATA(PLOT_VAR_IMP_REAL);
    SAVE_DATA(PLOT_VAR_IMP_IMAG);
    SAVE_DATA(PLOT_VAR_NEG_IMP_IMAG);

    SAVE_DATA_END();
}