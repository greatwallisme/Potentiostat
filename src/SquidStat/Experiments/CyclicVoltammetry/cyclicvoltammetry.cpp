#include "cyclicvoltammetry.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Cyclic-Voltammetry"

#define START_VOLTAGE_OBJ_NAME	"start-voltage"
#define START_V_VS_OCP_OBJ_NAME	"start-voltage-vs-ocp"
#define UPPER_VOLTAGE_OBJ_NAME	"upper-voltage"
#define UPPER_V_VS_OCP_OBJ_NAME	"upper-voltage-vs-ocp"
#define LOWER_VOLTAGE_OBJ_NAME  "lower-voltage"
#define LOWER_V_VS_OCP_OBJ_NAME	"lower-voltage-vs-ocp"
#define SCAN_RATE_OBJ_NAME		"scan-rate"
#define SAMPLING_INT_OBJ_NAME "sampling-interval"
#define SAMPLING_INT_UNITS_OBJ_NAME "sampling-interval-units"
#define CYCLES_OBJ_NAME			"cycles"
#define CURRENT_RANGE_OBJ_NAME  "current-range-mode"
#define CURRENT_RANGE_VALUE_OBJ_NAME  "approx-current-max-value"
#define CURRENT_RANGE_UNITS_OBJ_NAME  "current-range-units"

#define START_VOLTAGE_DEFAULT	0
#define UPPER_VOLTAGE_DEFAULT	0.5
#define LOWER_VOLTAGE_DEFAULT	-0.5
#define SCAN_RATE_DEFAULT		1
#define SAMPLING_INT_DEFAULT 1
#define CYCLES_DEFAULT			3
#define CURRENT_RANGE_VALUE_DEFAULT 100

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Elapsed time (s)"
#define PLOT_VAR_ELAPSED_TIME_HR      "Elapsed time (hr)"
#define PLOT_VAR_EWE					"Working electrode (V)"
#define PLOT_VAR_CURRENT				"Current (mA)"
#define PLOT_VAR_ECE					"Counter electrode (V)"
#define PLOT_VAR_CURRENT_INTEGRAL		"Cumulative charge (mAh)"

QString CyclicVoltammetry::GetShortName() const {
	return "Cyclic Voltammetry";
}
QString CyclicVoltammetry::GetFullName() const {
	return "Cyclic Voltammetry (Basic)";
}
QString CyclicVoltammetry::GetDescription() const {
	return "This experiment sweeps the potential of the working electrode back and forth between <b>upper potential</b> and <b>lower potential</b> at a constant <b>scan rate dE/dT</b> for a specified number of <b>cycles</b>.";
}
QStringList CyclicVoltammetry::GetCategory() const {
	return QStringList() <<
		"Basic voltammetry";

}
ExperimentTypeList CyclicVoltammetry::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap CyclicVoltammetry::GetImage() const {
	return QPixmap(":/Experiments/CyclicVoltammetry");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* CyclicVoltammetry::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;

  //TODO: add current ranging input

	_INSERT_RIGHT_ALIGN_COMMENT("Starting potential = ", row, 0);
	_INSERT_TEXT_INPUT(START_VOLTAGE_DEFAULT, START_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to", row, 0);
	_START_DROP_DOWN(START_V_VS_OCP_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Upper scan limit = ", row, 0);
	_INSERT_TEXT_INPUT(UPPER_VOLTAGE_DEFAULT, UPPER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
	_START_DROP_DOWN(UPPER_V_VS_OCP_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Lower scan limit = ", row, 0);
	_INSERT_TEXT_INPUT(LOWER_VOLTAGE_DEFAULT, LOWER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
	_START_DROP_DOWN(LOWER_V_VS_OCP_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Scan rate (dE/dt) = ", row, 0);
	_INSERT_TEXT_INPUT(SCAN_RATE_DEFAULT, SCAN_RATE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mV/s", row, 2);
	
  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Sample at intervals of: ", row, 0);
  _INSERT_TEXT_INPUT(SAMPLING_INT_DEFAULT, SAMPLING_INT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(SAMPLING_INT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mV");
  _ADD_DROP_DOWN_ITEM("s");
  _END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Repeats = ", row, 0);
	_INSERT_TEXT_INPUT(CYCLES_DEFAULT, CYCLES_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("", row, 2);

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_LEFT_ALIGN_COMMENT("<b>Current ranging</b>", row, 1);

  ++row;
  _START_RADIO_BUTTON_GROUP(CURRENT_RANGE_OBJ_NAME);
  _INSERT_RADIO_BUTTON("Autorange", row, 0);
  ++row;
  _INSERT_RADIO_BUTTON("Approx. max current: ", row, 0);
  _END_RADIO_BUTTON_GROUP();
  _INSERT_TEXT_INPUT(CURRENT_RANGE_VALUE_DEFAULT, CURRENT_RANGE_VALUE_OBJ_NAME, row, 1);
  _START_DROP_DOWN(CURRENT_RANGE_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	_SET_ROW_STRETCH(++row, 1);
	USER_INPUT_END();
}
NodesData CyclicVoltammetry::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	double startVoltage;
  QString startVoltageVsOCP_str;
	bool startVoltageVsOCP;
	double upperVoltage;
  QString upperVoltageVsOCP_str;
	bool upperVoltageVsOCP;
	double lowerVoltage;
  QString lowerVoltageVsOCP_str;
	bool lowerVoltageVsOCP;
	double dEdt;
  double sampling_int;
  QString sampling_int_units_str;
	qint32 cycles;
  QString currentRangeMode_str;
  QString currentRangeUnits_str;
  currentRange_t currentRangeMode;
  double approxMaxCurrent;

	GET_TEXT_INPUT_VALUE_DOUBLE(startVoltage, START_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(upperVoltage, UPPER_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(lowerVoltage, LOWER_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(dEdt, SCAN_RATE_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(sampling_int, SAMPLING_INT_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(sampling_int_units_str, SAMPLING_INT_UNITS_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(cycles, CYCLES_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(startVoltageVsOCP_str, START_V_VS_OCP_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(upperVoltageVsOCP_str, UPPER_V_VS_OCP_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(lowerVoltageVsOCP_str, LOWER_V_VS_OCP_OBJ_NAME);
  GET_SELECTED_RADIO(currentRangeMode_str, CURRENT_RANGE_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(approxMaxCurrent, CURRENT_RANGE_VALUE_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(currentRangeUnits_str, CURRENT_RANGE_UNITS_OBJ_NAME);

  startVoltageVsOCP = startVoltageVsOCP_str.contains("open circuit");
  upperVoltageVsOCP = upperVoltageVsOCP_str.contains("open circuit");
  lowerVoltageVsOCP = lowerVoltageVsOCP_str.contains("open circuit");

  if (sampling_int_units_str.contains("mV"))
      sampling_int = sampling_int / dEdt;       //convert from mV to s

  approxMaxCurrent *= ExperimentCalcHelperClass::GetUnitsMultiplier(currentRangeUnits_str);
  if (currentRangeMode_str.contains("Autorange"))
    currentRangeMode = AUTORANGE;
  else
    currentRangeMode = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, approxMaxCurrent);

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e7;
	exp.tMax = 2e8;
  exp.currentRangeMode = currentRangeMode;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 0.25);
	exp.DCPoint_pot.VPointUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, startVoltage);
	exp.DCPoint_pot.VPointVsOCP = startVoltageVsOCP;
	exp.DCPoint_pot.Imax = MAX_CURRENT;
	exp.DCPoint_pot.Imin = 0;
  exp.DCPoint_pot.dIdtMin = 0;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 1e7;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = currentRangeMode;
  ExperimentCalcHelperClass::GetSamplingParams_potSweep(hwVersion.hwModel, &calData, &exp, dEdt, sampling_int);
	exp.DCSweep_pot.VStartUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, startVoltage);
	exp.DCSweep_pot.VStartVsOCP = startVoltageVsOCP;
	exp.DCSweep_pot.VEndUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, upperVoltage);
	exp.DCSweep_pot.VEndVsOCP = upperVoltageVsOCP;
	exp.DCSweep_pot.Imax = MAX_CURRENT;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = true;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 1e7;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = currentRangeMode;
  ExperimentCalcHelperClass::GetSamplingParams_potSweep(hwVersion.hwModel, &calData, &exp, dEdt, sampling_int);
	exp.DCSweep_pot.VStartUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, upperVoltage);
	exp.DCSweep_pot.VStartVsOCP = upperVoltageVsOCP;
	exp.DCSweep_pot.VEndUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, lowerVoltage);
	exp.DCSweep_pot.VEndVsOCP = lowerVoltageVsOCP;
  exp.DCSweep_pot.Imax = MAX_CURRENT;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = true;
	exp.branchHeadIndex = 2;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 1e7;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = currentRangeMode;
  ExperimentCalcHelperClass::GetSamplingParams_potSweep(hwVersion.hwModel, &calData, &exp, dEdt, sampling_int);
  exp.DCSweep_pot.VStartUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, lowerVoltage);
	exp.DCSweep_pot.VStartVsOCP = lowerVoltageVsOCP;
  exp.DCSweep_pot.VEndUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, upperVoltage);
	exp.DCSweep_pot.VEndVsOCP = upperVoltageVsOCP;
  exp.DCSweep_pot.Imax = MAX_CURRENT;
	exp.MaxPlays = cycles;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList CyclicVoltammetry::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
    ret <<
      PLOT_VAR_EWE <<
      PLOT_VAR_CURRENT <<
      PLOT_VAR_TIMESTAMP_NORMALIZED <<
      PLOT_VAR_ELAPSED_TIME_HR;

	}

	return ret;
}
QStringList CyclicVoltammetry::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			PLOT_VAR_CURRENT <<
			PLOT_VAR_EWE <<
			PLOT_VAR_ECE <<
			PLOT_VAR_CURRENT_INTEGRAL;
	}

	return ret;
}
void CyclicVoltammetry::PUSH_NEW_DC_DATA_DEFINITION {
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / SECONDS;
  ProcessedDCData processedDCData = ExperimentCalcHelperClass::ProcessDCDataPoint(&calData, expData);

	if (container[PLOT_VAR_CURRENT_INTEGRAL].data.isEmpty()) {
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, processedDCData.current / timestamp / 3600.0);
	}
	else {
		qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].data.last();
		newVal += (container[PLOT_VAR_CURRENT].data.last() + processedDCData.current) * (timestamp - container[PLOT_VAR_TIMESTAMP].data.last()) / 3600.0 / 2.;
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, newVal);
	}

	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP, timestamp);
	PUSH_BACK_DATA(PLOT_VAR_EWE, processedDCData.EWE);
	PUSH_BACK_DATA(PLOT_VAR_ECE, processedDCData.ECE);
	PUSH_BACK_DATA(PLOT_VAR_CURRENT, processedDCData.current);

	if (!timestampOffset.contains(&container)) {
		timestampOffset[&container] = timestamp;
	}
	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED, timestamp - timestampOffset[&container]);
  PUSH_BACK_DATA(PLOT_VAR_ELAPSED_TIME_HR, (timestamp - timestampOffset[&container]) / 3600);
}
void CyclicVoltammetry::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}

void CyclicVoltammetry::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_END();
}