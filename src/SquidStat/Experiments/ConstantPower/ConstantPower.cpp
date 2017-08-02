#include "ConstantPower.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"constant-power"

#define POWER_OBJ_NAME	"target-power"
#define POWER_UNITS_OBJ_NAME "target-power-units"
#define VMAX_OBJ_NAME       "maximum-voltage"
#define VMIN_OBJ_NAME       "minimum-voltage"
#define T_MAX_OBJ_NAME      "maximum-time"
#define T_MAX_UNITS         "time-units"
#define SAMPLING_INT_OBJ_NAME "sampling-interval"

#define POWER_DEFAULT	1
#define V_MAX_DEFAULT 1
#define V_MIN_DEFAULT		0
#define T_MAX_DEFAULT 60
#define SAMPLING_INT_DEFAULT 1

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Elapsed time (s)"
#define PLOT_VAR_ELAPSED_TIME_HR      "Elapsed time (hr)"
#define PLOT_VAR_EWE					"Working electrode (V)"
#define PLOT_VAR_CURRENT				"Current (mA)"
#define PLOT_VAR_POWER   "Power (mW)"
#define PLOT_VAR_ECE					"Counter electrode (V)"
#define PLOT_VAR_CURRENT_INTEGRAL		"Cumulative charge (mAh)"

QString ConstantPower::GetShortName() const {
	return "Constant Power";
}
QString ConstantPower::GetFullName() const {
	return "Constant Power Charge/Discharge";
}
QString ConstantPower::GetDescription() const {
	return "This experiment sweeps the <b>potential</b> of the working electrode from the <b>starting potential</b> to the <b>end potential</b> at constant scan rate <b>dE/dT</b>";
}
QStringList ConstantPower::GetCategory() const {
  return QStringList() <<
    "Basic voltammetry";
}
ExperimentTypeList ConstantPower::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap ConstantPower::GetImage() const {
	return QPixmap(":/Experiments/LinearSweepVoltammetry");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* ConstantPower::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Load power = ", row, 0);
	_INSERT_TEXT_INPUT(POWER_DEFAULT, POWER_OBJ_NAME, row, 1);
  _START_DROP_DOWN(POWER_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mW");
  _ADD_DROP_DOWN_ITEM("W");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Maximum cell voltage = ", row, 0);
  _INSERT_TEXT_INPUT(V_MAX_DEFAULT, VMAX_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Minimum cell voltage = ", row, 0);
	_INSERT_TEXT_INPUT(V_MIN_DEFAULT, VMIN_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Maximum duration = ", row, 0);
  _INSERT_TEXT_INPUT(T_MAX_DEFAULT, T_MAX_OBJ_NAME, row, 1);
  _START_DROP_DOWN(T_MAX_UNITS, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Sampling interval = ", row, 0);
  _INSERT_TEXT_INPUT(SAMPLING_INT_DEFAULT, SAMPLING_INT_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(3, 1);

	USER_INPUT_END();
}
NodesData ConstantPower::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	double power;
  QString powerUnits_str;
  double VMax;
  double VMin;
  double tmax;
  QString tmaxUnits_str;
  double dt;

	GET_TEXT_INPUT_VALUE_DOUBLE(power, POWER_OBJ_NAME);
	GET_SELECTED_DROP_DOWN(powerUnits_str, POWER_UNITS_OBJ_NAME);
  GET_TEXT_INPUT_VALUE(VMax, VMAX_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(VMin, VMIN_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(tmax, T_MAX_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(tmaxUnits_str, T_MAX_UNITS);
  GET_TEXT_INPUT_VALUE_DOUBLE(dt, SAMPLING_INT_OBJ_NAME);

  power *= ExperimentCalcHelperClass::GetUnitsMultiplier(powerUnits_str);
  tmax *= ExperimentCalcHelperClass::GetUnitsMultiplier(tmaxUnits_str);

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_CONST_POWER;
  exp.tMin = 5 * MILLISECONDS;
  exp.tMax = (uint64_t) (tmax * SECONDS);
  exp.currentRangeMode = AUTORANGE;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, dt);
  exp.DCConstPower.power = power;
  exp.DCConstPower.ICtrl = 0;
  exp.DCConstPower.VMax = (float)VMax;
  exp.DCConstPower.VMin = (float)VMin;
  exp.numPlays = 1;
  PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList ConstantPower::GetXAxisParameters(ExperimentType type) const {
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
QStringList ConstantPower::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			PLOT_VAR_CURRENT <<
			PLOT_VAR_EWE <<
      PLOT_VAR_POWER <<
			PLOT_VAR_ECE <<
			PLOT_VAR_CURRENT_INTEGRAL;
	}

	return ret;
}
void ConstantPower::PUSH_NEW_DC_DATA_DEFINITION {
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / SECONDS;
  ProcessedDCData processedDCdata = ExperimentCalcHelperClass::ProcessDCDataPoint(&calData, expData);

	if (container[PLOT_VAR_CURRENT_INTEGRAL].data.isEmpty()) {
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, processedDCdata.current / timestamp / 3600.0);
	}
	else {
		qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].data.last();
		newVal += (container[PLOT_VAR_CURRENT].data.last() + processedDCdata.current) * (timestamp - container[PLOT_VAR_TIMESTAMP].data.last()) / 3600.0 / 2.;
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, newVal);
	}

	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP, timestamp);
	PUSH_BACK_DATA(PLOT_VAR_EWE, processedDCdata.EWE);
	PUSH_BACK_DATA(PLOT_VAR_ECE, processedDCdata.ECE);
	PUSH_BACK_DATA(PLOT_VAR_CURRENT, processedDCdata.current);
  PUSH_BACK_DATA(PLOT_VAR_POWER, processedDCdata.current * (processedDCdata.EWE - processedDCdata.ECE));

	if (!timestampOffset.contains(&container)) {
		timestampOffset[&container] = timestamp;
	}
	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED, timestamp - timestampOffset[&container]);
  PUSH_BACK_DATA(PLOT_VAR_ELAPSED_TIME_HR, (timestamp - timestampOffset[&container]) / 3600);
}
void ConstantPower::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
  SAVE_DC_DATA_HEADER(PLOT_VAR_POWER);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}

void ConstantPower::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
  SAVE_DATA(PLOT_VAR_POWER);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);
	
	SAVE_DATA_END();
}