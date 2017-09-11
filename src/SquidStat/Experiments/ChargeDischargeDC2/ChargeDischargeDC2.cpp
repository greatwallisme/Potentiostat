#include "ChargeDischargeDC2.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Charge-Discharge-DC2"

#define STARTING_PHASE_OPT_OBJ_NAME "starting-phase-option"
#define SAMP_INTERVAL_OBJ_NAME	"sampling-interval"
#define CYCLES_OBJ_NAME			"cycles"

#define CHG_POWER_OBJ_NAME	"charge-power"
#define CHG_POWER_UNITS_OBJ_NAME "charge-power-units"
#define UPPER_VOLTAGE_OBJ_NAME	"upper-voltage"
#define MAXIMUM_CHG_CAPACITY_OBJ_NAME "Charging-capacity-limit"

#define RESISTANCE_OBJ_NAME	"load-resistance"
#define RESISTANCE_UNITS_OBJ_NAME "load-resistance-units"
#define LOWER_VOLTAGE_OBJ_NAME  "lower-voltage"
#define MAXIMUM_DISCHG_CAPACITY_OBJ_NAME "Discharging-capacity-limit"

#define REST_PERIOD_OBJ			"rest_period"
#define REST_PERIOD_INT_OBJ		"rest-period-interval"

#define SAMP_INTERVAL_DEFAULT	10
#define CHG_POWER_DEFAULT		100		//(in mW)
#define UPPER_VOLTAGE_DEFAULT 3.6 
#define MAXIMUM_CHG_CAPACITY_DEFAULT 100 //(in mAh)
#define RESISTANCE_DEFAULT	100	//(in Ohms)
#define LOWER_VOLTAGE_DEFAULT 2.7
#define MAXIMUM_DISCHG_CAPACITY_DEFAULT 100 //(in mAh)
#define REST_PERIOD_DEFAULT		120
#define REST_PERIOD_INT_DEFAULT 1
#define CYCLES_DEFAULT			10

#define PLOT_VAR_DATETIME "Date/time"
#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Elapsed time (s)"
#define PLOT_VAR_ELAPSED_TIME_HR "Elapsed time (hr)"
#define PLOT_VAR_EWE					"Working electrode (V)"
#define PLOT_VAR_CURRENT				"Current (mA)"
#define PLOT_VAR_ECE					"Counter electrode (V)"
#define PLOT_VAR_CURRENT_INTEGRAL		"Cumulative charge (mAh)"

QString ChargeDischargeDC2::GetShortName() const {
	return "Charge/Discharge 2";
}
QString ChargeDischargeDC2::GetFullName() const {
	return "Charge/Discharge (Const R discharge, const P charge)";
}
QString ChargeDischargeDC2::GetDescription() const {
	return "This experiment discharges the cell under test with a <b>constant resistance</b> load, and charges the cell with a <b>constant power</b> source.";
}
QStringList ChargeDischargeDC2::GetCategory() const {
	return QStringList() <<
		"Energy storage";
}
ExperimentTypeList ChargeDischargeDC2::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap ChargeDischargeDC2::GetImage() const {
	return QPixmap(":/Experiments/ChargeDischargeDC");
}

QWidget* ChargeDischargeDC2::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Starting phase", row, 0);
	_START_DROP_DOWN(STARTING_PHASE_OPT_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("Charge first");
	_ADD_DROP_DOWN_ITEM("Discharge first");
	_END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Sampling interval = ", row, 0);
  _INSERT_TEXT_INPUT(SAMP_INTERVAL_DEFAULT, SAMP_INTERVAL_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Cycles = ", row, 0);
  _INSERT_TEXT_INPUT(CYCLES_DEFAULT, CYCLES_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("", row, 2);

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Constant power charge</b>", row);
	
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Charging power = ", row, 0);
	_INSERT_TEXT_INPUT(CHG_POWER_DEFAULT, CHG_POWER_OBJ_NAME, row, 1);
  _START_DROP_DOWN(CHG_POWER_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mW");
  _ADD_DROP_DOWN_ITEM("uW");
  _ADD_DROP_DOWN_ITEM("nW");
  _END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Upper voltage limit = ", row, 0);
	_INSERT_TEXT_INPUT(UPPER_VOLTAGE_DEFAULT, UPPER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Maximum capacity", row, 0);
  _INSERT_TEXT_INPUT(MAXIMUM_CHG_CAPACITY_DEFAULT, MAXIMUM_CHG_CAPACITY_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("mAh", row, 2);

  ++row;
  _INSERT_CENTERED_COMMENT("<I>(Enter \"0\" for unlimited capacity)</I>", row);

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Constant resistance discharge</b>", row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Load resistance = ", row, 0);
	_INSERT_TEXT_INPUT(RESISTANCE_DEFAULT, RESISTANCE_OBJ_NAME, row, 1);
  _START_DROP_DOWN(RESISTANCE_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("Ohms");
  _ADD_DROP_DOWN_ITEM("kOhms");
  _ADD_DROP_DOWN_ITEM("MOhms");
  _END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Lower voltage limit = ", row, 0);
	_INSERT_TEXT_INPUT(LOWER_VOLTAGE_DEFAULT, LOWER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Maximum capacity", row, 0);
  _INSERT_TEXT_INPUT(MAXIMUM_DISCHG_CAPACITY_DEFAULT, MAXIMUM_DISCHG_CAPACITY_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("mAh", row, 2);

  ++row;
  _INSERT_CENTERED_COMMENT("<I>(Enter \"0\" for unlimited capacity)</I>", row);

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Rest period</b>", row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Duration = ", row, 0);
	_INSERT_TEXT_INPUT(REST_PERIOD_DEFAULT, REST_PERIOD_OBJ, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Sampling interval = ", row, 0);
	_INSERT_TEXT_INPUT(REST_PERIOD_INT_DEFAULT, REST_PERIOD_INT_OBJ, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	_SET_ROW_STRETCH(++row, 1);
	USER_INPUT_END();
}
NodesData ChargeDischargeDC2::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	//TODO: what to do about cells hooked up backwards?
	//TODO: incorporate max charge/discharge capacity? Or incorporate this into another experiment?
	//TODO: make a const. power discharge node type

  QString firstPhase_str;
	bool chargeFirst= false;
  double sampInterval;
  qint32 cycles;

  double chgPower;
  QString chgPowerUnits_str;
	double upperVoltage;
  double maxChgCapacity;

  double resistance;
  QString ResistanceUnits_str;
	double lowerVoltage;
  double maxDischgCapacity;
	
	double restPeriodDuration;
	double restPeriodInterval;
	
	
	GET_SELECTED_DROP_DOWN(firstPhase_str, STARTING_PHASE_OPT_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(sampInterval, SAMP_INTERVAL_OBJ_NAME);
  GET_TEXT_INPUT_VALUE(cycles, CYCLES_OBJ_NAME);

  GET_TEXT_INPUT_VALUE_DOUBLE(chgPower, CHG_POWER_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(chgPowerUnits_str, CHG_POWER_UNITS_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(upperVoltage, UPPER_VOLTAGE_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(maxChgCapacity, MAXIMUM_CHG_CAPACITY_OBJ_NAME);

  GET_TEXT_INPUT_VALUE_DOUBLE(resistance, RESISTANCE_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(ResistanceUnits_str, RESISTANCE_UNITS_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(lowerVoltage, LOWER_VOLTAGE_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(maxDischgCapacity, MAXIMUM_DISCHG_CAPACITY_OBJ_NAME);

	GET_TEXT_INPUT_VALUE_DOUBLE(restPeriodDuration, REST_PERIOD_OBJ);
	GET_TEXT_INPUT_VALUE_DOUBLE(restPeriodInterval, REST_PERIOD_INT_OBJ);

  chgPower *= ExperimentCalcHelperClass::GetUnitsMultiplier(chgPowerUnits_str);
  resistance *= ExperimentCalcHelperClass::GetUnitsMultiplier(ResistanceUnits_str);

	if (!firstPhase_str.contains("Discharge first"))
	{
		chargeFirst = true;
	}

  if (chargeFirst)
  {
    exp.isHead = true;
    exp.isTail = false;
    exp.nodeType = DCNODE_CONST_POWER;
    exp.tMin = 2 * SECONDS;
    exp.tMax = 0xffffffffffffffff;
    ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, sampInterval, 0.05);
    exp.DCConstPower.VMax = upperVoltage;
    exp.DCConstPower.VMin = -MAX_VOLTAGE;
    exp.DCConstPower.power = abs(chgPower);
    exp.currentRangeMode = AUTORANGE;
    exp.MaxPlays = 1;
    PUSH_NEW_NODE_DATA();

    exp.isHead = exp.isTail = false;
    exp.nodeType = DCNODE_OCP;
    exp.DCocp.Vmin = -MAX_VOLTAGE;
    exp.DCocp.Vmax = MAX_VOLTAGE;
    exp.DCocp.dVdtMin = 0;
    ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, restPeriodInterval);
    exp.tMin = exp.tMax = restPeriodDuration * SECONDS;
    exp.MaxPlays = 1;
    PUSH_NEW_NODE_DATA();

    exp.isHead = exp.isTail = false;
    exp.nodeType = DCNODE_CONST_RESISTANCE;
    exp.tMax = 0xffffffffffffffff;
    exp.tMin = 2 * SECONDS;
    ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, sampInterval, 0.05);
    exp.DCConstResistance.resistance = abs(resistance);
    exp.DCConstResistance.VMin = lowerVoltage;
    exp.DCConstResistance.VMax = MAX_VOLTAGE;
    exp.MaxPlays = 1;
    exp.currentRangeMode = AUTORANGE;
    PUSH_NEW_NODE_DATA();

    exp.isHead = false;
    exp.isTail = true;
    exp.branchHeadIndex = 0;
    exp.nodeType = DCNODE_OCP;
    exp.DCocp.Vmin = -MAX_VOLTAGE;
    exp.DCocp.Vmax = MAX_VOLTAGE;
    exp.DCocp.dVdtMin = 0;
    ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, restPeriodInterval);
    exp.tMin = restPeriodDuration * SECONDS;
    exp.tMax = restPeriodDuration * SECONDS;
    exp.MaxPlays = cycles;
    PUSH_NEW_NODE_DATA();
  }

  else
  {
    exp.isHead = true;
    exp.isTail = false;
    exp.nodeType = DCNODE_CONST_RESISTANCE;
    exp.tMax = 0xffffffffffffffff;
    exp.tMin = 2 * SECONDS;
    ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, sampInterval, 0.05);
    exp.DCConstResistance.resistance = abs(resistance);
    exp.DCConstResistance.VMin = lowerVoltage;
    exp.DCConstResistance.VMax = MAX_VOLTAGE;
    exp.MaxPlays = 1;
    exp.currentRangeMode = AUTORANGE;
    PUSH_NEW_NODE_DATA();

    exp.isHead = exp.isTail = false;
    exp.nodeType = DCNODE_OCP;
    exp.DCocp.Vmin = -MAX_VOLTAGE;
    exp.DCocp.Vmax = MAX_VOLTAGE;
    exp.DCocp.dVdtMin = 0;
    ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, restPeriodInterval);
    exp.tMin = exp.tMax = restPeriodDuration * SECONDS;
    exp.MaxPlays = 1;
    PUSH_NEW_NODE_DATA();

    exp.isHead = exp.isTail = false;
    exp.nodeType = DCNODE_CONST_POWER;
    exp.tMin = 2 * SECONDS;
    exp.tMax = 0xffffffffffffffff;
    ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, sampInterval, 0.05);
    exp.DCConstPower.VMax = upperVoltage;
    exp.DCConstPower.VMin = -MAX_VOLTAGE;
    exp.DCConstPower.power = abs(chgPower);
    exp.currentRangeMode = AUTORANGE;
    exp.MaxPlays = 1;
    PUSH_NEW_NODE_DATA();

    exp.isHead = false;
    exp.isTail = true;
    exp.branchHeadIndex = 0;
    exp.nodeType = DCNODE_OCP;
    exp.DCocp.Vmin = -MAX_VOLTAGE;
    exp.DCocp.Vmax = MAX_VOLTAGE;
    exp.DCocp.dVdtMin = 0;
    ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, restPeriodInterval);
    exp.tMin = restPeriodDuration * SECONDS;
    exp.tMax = restPeriodDuration * SECONDS;
    exp.MaxPlays = cycles;
    PUSH_NEW_NODE_DATA();
  }

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}
QStringList ChargeDischargeDC2::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			PLOT_VAR_ELAPSED_TIME_HR <<
			PLOT_VAR_TIMESTAMP_NORMALIZED <<
			PLOT_VAR_EWE <<
			PLOT_VAR_CURRENT;
	}

	return ret;
}
QStringList ChargeDischargeDC2::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			PLOT_VAR_EWE <<
			PLOT_VAR_CURRENT <<
			PLOT_VAR_ECE <<
			PLOT_VAR_CURRENT_INTEGRAL;
	}

	return ret;
}
void ChargeDischargeDC2::PUSH_NEW_DC_DATA_DEFINITION{
  static QMap<DataMap*, qreal> timestampOffset;
  qreal timestamp = (qreal)expData.timestamp / SECONDS;
  ProcessedDCData processedData = ExperimentCalcHelperClass::ProcessDCDataPoint(&calData, expData);
  
  if (container[PLOT_VAR_CURRENT_INTEGRAL].data.isEmpty()) {
    PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, 0);
  }
  else {
    qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].data.last();
    newVal += (container[PLOT_VAR_CURRENT].data.last() + processedData.current) * (timestamp - container[PLOT_VAR_TIMESTAMP].data.last()) / 2. / 3600.0;
    PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, newVal);
  }

  PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP, timestamp);
  PUSH_BACK_DATA(PLOT_VAR_EWE, processedData.EWE);
  PUSH_BACK_DATA(PLOT_VAR_ECE, processedData.ECE);
  PUSH_BACK_DATA(PLOT_VAR_CURRENT, processedData.current);

  if (!timestampOffset.contains(&container)) {
    timestampOffset[&container] = timestamp;
  }
  PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED, timestamp - timestampOffset[&container]);
  PUSH_BACK_DATA(PLOT_VAR_ELAPSED_TIME_HR, (timestamp - timestampOffset[&container]) / 3600);
}

void ChargeDischargeDC2::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	//SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP);
  //SAVE_DC_DATA_HEADER(PLOT_VAR_DATETIME);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
  SAVE_DC_DATA_HEADER(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}

void ChargeDischargeDC2::SaveDcData(QFile &saveFile, const DataMap &container) const {
  SAVE_DATA_START();
  SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
  SAVE_DATA(PLOT_VAR_ELAPSED_TIME_HR);
  SAVE_DATA(PLOT_VAR_EWE);
  SAVE_DATA(PLOT_VAR_CURRENT);
  SAVE_DATA(PLOT_VAR_ECE);
  SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);
  SAVE_DATA_END();
}
