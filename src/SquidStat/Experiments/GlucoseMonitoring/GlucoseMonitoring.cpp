#include "GlucoseMonitoring.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"GlucoseMonitoring"

#define V1_OBJECT_NAME			"voltage-1"
#define T1_OBJECT_NAME			"sampling-interval"
#define T1_UNITS_OBJ_NAME   "sampling-interval-units"
#define NUM_REPEATS_OBJ_NAME	"number-of-repeats"
#define THIRD_POLYNOMIAL_PARAM_OBJ_NAME "third-polynomial-parameter"
#define SECOND_POLYNOMIAL_PARAM_OBJ_NAME "second-polynomial-parameter"
#define FIRST_POLYNOMIAL_PARAM_OBJ_NAME "first-polynomial-parameter"
#define ZEROETH_POLYNOMIAL_PARAM_OBJ_NAME "zeroeth-polynomial-parameter"

#define V1_DEFAULT				0.1
#define T1_DEFAULT				10
#define NUM_REPEATS_DEFAULT 3
#define THIRD_POLYNOMIAL_PARAM_DEFAULT 0
#define SECOND_POLYNOMIAL_PARAM_DEFAULT 0.01
#define FIRST_POLYNOMIAL_PARAM_DEFAULT 0.1
#define ZEROETH_POLYNOMIAL_PARAM_DEFAULT 0.2

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Elapsed time (s)"
#define PLOT_VAR_ELAPSED_TIME_HR      "Elapsed time (hr)"
#define PLOT_VAR_EWE					"Working electrode (V)"
#define PLOT_VAR_CURRENT				"Current (mA)"
#define PLOT_VAR_GLUCOSE_CONC   "Glucose concentration (mmol)"

QString GlucoseMonitoring::GetShortName() const {
	return "Glucose Monitoring";
}
QString GlucoseMonitoring::GetFullName() const {
	return "Glucose Monitoring";
}
QString GlucoseMonitoring::GetDescription() const {
	return "This experiment holds the working electrode at a constant potential and correlates the response current to a sample glucose concentration.";
}
QStringList GlucoseMonitoring::GetCategory() const {
	return QStringList() <<
		"Special applications";

}
ExperimentTypeList GlucoseMonitoring::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap GlucoseMonitoring::GetImage() const {
	return QPixmap(":/Experiments/GlucoseMonitoring");
}

QWidget* GlucoseMonitoring::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;

	_INSERT_RIGHT_ALIGN_COMMENT("Holding potential = ", row, 0);
	_INSERT_TEXT_INPUT(V1_DEFAULT, V1_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Sample hold time = ", row, 0);
  _INSERT_TEXT_INPUT(T1_DEFAULT, T1_OBJECT_NAME, row, 1);
  _START_DROP_DOWN(T1_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();
  
  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Number of samples = ", row, 0);
  _INSERT_TEXT_INPUT(NUM_REPEATS_DEFAULT, NUM_REPEATS_OBJ_NAME, row, 1);

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Calibration constants</b>", row);
  
  ++row;
  _INSERT_CENTERED_COMMENT("For polynomial f(current/uA) = concentration/mM:", row);
  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Cubic term: ", row, 0);
  _INSERT_TEXT_INPUT(THIRD_POLYNOMIAL_PARAM_DEFAULT, THIRD_POLYNOMIAL_PARAM_OBJ_NAME, row, 1);
  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Quadratic term: ", row, 0);
  _INSERT_TEXT_INPUT(SECOND_POLYNOMIAL_PARAM_DEFAULT, SECOND_POLYNOMIAL_PARAM_OBJ_NAME, row, 1);
  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Linear term: ", row, 0);
  _INSERT_TEXT_INPUT(FIRST_POLYNOMIAL_PARAM_DEFAULT, FIRST_POLYNOMIAL_PARAM_OBJ_NAME, row, 1);
  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Constant term: ", row, 0);
  _INSERT_TEXT_INPUT(ZEROETH_POLYNOMIAL_PARAM_DEFAULT, ZEROETH_POLYNOMIAL_PARAM_OBJ_NAME, row, 1);

	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	_SET_ROW_STRETCH(++row, 1);
	USER_INPUT_END();
}
NodesData GlucoseMonitoring::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double max_current_expected = 1.0 / 50.0;  //in mA
	double v1, t1;
  QString t1Units_str;
  uint32_t num_repeats;
	GET_TEXT_INPUT_VALUE_DOUBLE(v1, V1_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(t1, T1_OBJECT_NAME);
  GET_SELECTED_DROP_DOWN(t1Units_str, T1_UNITS_OBJ_NAME);
  GET_TEXT_INPUT_VALUE(num_repeats, NUM_REPEATS_OBJ_NAME);

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 0;
  exp.tMax = t1 * SECONDS * ExperimentCalcHelperClass::GetUnitsMultiplier(t1Units_str);
  exp.currentRangeMode = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, max_current_expected);
	ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 20, 0.25);
  exp.DCPoint_pot.VPointUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, v1);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.DCPoint_pot.Imax = MAX_CURRENT;
	exp.DCPoint_pot.Imin = 0;
  exp.DCPoint_pot.dIdtMin = 0;
	exp.MaxPlays = num_repeats;
  PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList GlucoseMonitoring::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
      ret <<
          PLOT_VAR_TIMESTAMP_NORMALIZED <<
          PLOT_VAR_ELAPSED_TIME_HR;
	}

	return ret;
}
QStringList GlucoseMonitoring::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
      ret <<
          PLOT_VAR_GLUCOSE_CONC <<
          PLOT_VAR_CURRENT <<
          PLOT_VAR_EWE;
	}

	return ret;
}
void GlucoseMonitoring::PUSH_NEW_DC_DATA_DEFINITION {
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / SECONDS;
  ProcessedDCData processedData = ExperimentCalcHelperClass::ProcessDCDataPoint(&calData, expData);
  
  double slope = 1.988, intercept = -3.6719;

	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP, timestamp);
	PUSH_BACK_DATA(PLOT_VAR_EWE, processedData.EWE);
	PUSH_BACK_DATA(PLOT_VAR_CURRENT, processedData.current);
  PUSH_BACK_DATA(PLOT_VAR_GLUCOSE_CONC, (processedData.current * 1000) * slope + intercept);

	if (!timestampOffset.contains(&container)) {
		timestampOffset[&container] = timestamp;
	}
	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED, timestamp - timestampOffset[&container]);
  PUSH_BACK_DATA(PLOT_VAR_ELAPSED_TIME_HR, (timestamp - timestampOffset[&container]) / 3600);
}
void GlucoseMonitoring::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
  SAVE_DC_DATA_HEADER(PLOT_VAR_GLUCOSE_CONC);

	SAVE_DATA_HEADER_END();
}

void GlucoseMonitoring::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_GLUCOSE_CONC);

	SAVE_DATA_END();
}