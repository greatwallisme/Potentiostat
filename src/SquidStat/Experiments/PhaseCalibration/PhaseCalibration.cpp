#include "PhaseCalibration.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Phase-Calibration"
#define EXPERIMENT_TYPE_OBJ_NAME			"Experiment-type"

#define PLOT_VAR_FREQ					"Frequency"
#define PLOT_VAR_IMPEDANCE				"|Z|"
#define PLOT_VAR_PHASE					"Phase (degrees)"
#define PLOT_VAR_WEGAIN         "Working electrode gain"
#define PLOT_VAR_IGAIN          "Current gain"
#define PLOT_VAR_IRANGE         "Current range"

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
  _INSERT_RIGHT_ALIGN_COMMENT("Experiment type: ", row, 0);
  _START_DROP_DOWN(EXPERIMENT_TYPE_OBJ_NAME, row, 1);
  _ADD_DROP_DOWN_ITEM("Phase delay, 100R load");
  _ADD_DROP_DOWN_ITEM("DC gain, 100R load");
  _ADD_DROP_DOWN_ITEM("HF gain, 100R load");
  _END_DROP_DOWN();
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	_SET_ROW_STRETCH(++row, 1);
	USER_INPUT_END();
}
NodesData PhaseCalibration::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
    NODES_DATA_START(wdg, TOP_WIDGET_NAME);

    QString exp_type;

    GET_SELECTED_DROP_DOWN(exp_type, EXPERIMENT_TYPE_OBJ_NAME);

    struct phase_cal_experiment_struct {
        double frequency;
        double load;
        ACgain_t gainWE;
        ACgain_t gainI;
        currentRange_t irange;
        double amplitude;
    };

    /* Experiment 1 (Phase delay w.r.t. each gain stage) */
    QList<phase_cal_experiment_struct> calExpStructs_1;
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN1, AC_GAIN1, RANGE2, 10 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN2, AC_GAIN1, RANGE2, 5 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN5, AC_GAIN1, RANGE2, 2 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN10, AC_GAIN1, RANGE2, 1 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN10_alt1, AC_GAIN1, RANGE2, 1 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN10_alt2, AC_GAIN1, RANGE2, 1 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN1, AC_GAIN2, RANGE2, 5 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN1, AC_GAIN5, RANGE2, 2 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN1, AC_GAIN10, RANGE2, 1 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN1, AC_GAIN10_alt1, RANGE2, 1 });
    calExpStructs_1.append(phase_cal_experiment_struct{ 1e6, 100, AC_GAIN1, AC_GAIN10_alt2, RANGE2, 1 });

    /* Experiment 2 (DC gain for each gain stage) */
    QList<phase_cal_experiment_struct> calExpStructs_2;
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN1, RANGE2, 10 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN2, AC_GAIN1, RANGE2, 5 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN5, AC_GAIN1, RANGE2, 2 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN10, AC_GAIN1, RANGE2, 1 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN10_alt1, AC_GAIN1, RANGE2, 1 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN10_alt2, AC_GAIN1, RANGE2, 1 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN2, RANGE2, 5 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN5, RANGE2, 2 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN10, RANGE2, 1 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN10_alt1, RANGE2, 1 });
    calExpStructs_2.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN10_alt2, RANGE2, 1 });

    /* Experiment 3 (HF gain for each gain stage) */
    QList<phase_cal_experiment_struct> calExpStructs_3;
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN1, RANGE2, 10 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN2, AC_GAIN1, RANGE2, 5 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN5, AC_GAIN1, RANGE2, 2 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN10, AC_GAIN1, RANGE2, 1 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN10_alt1, AC_GAIN1, RANGE2, 1 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN10_alt2, AC_GAIN1, RANGE2, 1 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN2, RANGE2, 5 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN5, RANGE2, 2 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN10, RANGE2, 1 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN10_alt1, RANGE2, 1 });
    calExpStructs_3.append(phase_cal_experiment_struct{ 50, 100, AC_GAIN1, AC_GAIN10_alt2, RANGE2, 1 });


    QMap<QString, QList<phase_cal_experiment_struct>> calExpStructMap;
    calExpStructMap.insert("Phase delay, 100R load", calExpStructs_1);
    calExpStructMap.insert("DC gain, 100R load", calExpStructs_2);
    calExpStructMap.insert("HF gain, 100R load", calExpStructs_2);

  for (int n = 0; n < calExpStructMap[exp_type].count(); n++)
  {
      exp.isHead = false;
      exp.isTail = false;
      exp.nodeType = DCNODE_POINT_GALV;
      exp.currentRangeMode = exp.DCPoint_galv.Irange = calExpStructMap[exp_type][n].irange;
      exp.DCPoint_galv.dVdtMin = 0;
      exp.DCPoint_galv.IPoint = 0;
      exp.DCPoint_galv.isTrackingCapacity = false;
      exp.DCPoint_galv.Vmax = MAX_VOLTAGE;
      exp.DCPoint_galv.Vmin = -MAX_VOLTAGE;
      exp.tMin = exp.tMax = 2 * SECONDS;
      ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 4);
      PUSH_NEW_NODE_DATA();

      if (exp_type.contains("DC gain") || exp_type.contains("Phase delay"))
      {
          exp.isHead = exp.isTail = false;
          exp.tMin = 0;
          exp.tMax = 0xffffffffffffffff;
          exp.MaxPlays = 10;
          exp.isStartingNewFile = false;

          exp.nodeType = FRA_FIXED_GAINS_AND_RANGE;
          exp.ACsamplingParams.frequency = calExpStructMap[exp_type][n].frequency;

          exp.ACsamplingParams.firstTime = n == 0;
          exp.currentRangeMode = exp.FRA_fixedGainsAndRange_node.IRange = calExpStructMap[exp_type][n].irange;
          exp.FRA_fixedGainsAndRange_node.IBias = 0;
          exp.FRA_fixedGainsAndRange_node.IGainSetting = calExpStructMap[exp_type][n].gainI;
          exp.FRA_fixedGainsAndRange_node.WEGainSetting = calExpStructMap[exp_type][n].gainWE;
          exp.ACsamplingParams.amplitudeTarget = (float)calExpStructMap[exp_type][n].amplitude;
          ExperimentCalcHelperClass::calcACSamplingParams(&calData, &exp);

          PUSH_NEW_NODE_DATA();
      }
      else if (exp_type.contains("HF gain"))
      {
          QList<float> frequencyList = { 1e6, 8.6e5, 7.3e5, 5.9e5, 4.6e5, 3.2e5, 1.9e5, 5.1e4 };
          for (int i = 0; i < frequencyList.count(); i++)
          {
              exp.isHead = exp.isTail = false;
              exp.tMin = 0;
              exp.tMax = 0xffffffffffffffff;
              exp.MaxPlays = 10;
              exp.isStartingNewFile = false;

              exp.nodeType = FRA_FIXED_GAINS_AND_RANGE;
              exp.ACsamplingParams.frequency = frequencyList[i];

              exp.ACsamplingParams.firstTime = i == 0;
              exp.currentRangeMode = exp.FRA_fixedGainsAndRange_node.IRange = calExpStructMap[exp_type][n].irange;
              exp.FRA_fixedGainsAndRange_node.IBias = 0;
              exp.FRA_fixedGainsAndRange_node.IGainSetting = calExpStructMap[exp_type][n].gainI;
              exp.FRA_fixedGainsAndRange_node.WEGainSetting = calExpStructMap[exp_type][n].gainWE;
              exp.ACsamplingParams.amplitudeTarget = (float)calExpStructMap[exp_type][n].amplitude;
              ExperimentCalcHelperClass::calcACSamplingParams(&calData, &exp);

              PUSH_NEW_NODE_DATA();
          }
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
    PUSH_BACK_DATA(PLOT_VAR_WEGAIN, dataheader.gainSettingVoltage);
    PUSH_BACK_DATA(PLOT_VAR_IGAIN, dataheader.gainSettingCurrent);
    PUSH_BACK_DATA(PLOT_VAR_IRANGE, (int)dataheader.IRange);
}

void PhaseCalibration::SaveAcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
    SAVE_DATA_HEADER_START();

    SAVE_AC_DATA_HEADER(PLOT_VAR_FREQ);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IMPEDANCE);
    SAVE_AC_DATA_HEADER(PLOT_VAR_PHASE);
    SAVE_AC_DATA_HEADER(PLOT_VAR_WEGAIN);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IGAIN);
    SAVE_AC_DATA_HEADER(PLOT_VAR_IRANGE);

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

    SAVE_DATA_END();
}