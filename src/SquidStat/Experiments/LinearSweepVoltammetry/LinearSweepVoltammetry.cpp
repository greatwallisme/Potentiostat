#include "LinearSweepVoltammetry.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"linear-sweep-voltammetry"

#define START_VOLTAGE_OBJ_NAME	"start-voltage"
#define START_VOLTAGE_VS_OCP_OBJ_NAME "start-voltage-vs-ocp"
#define END_VOLTAGE_OBJ_NAME	"end-voltage"
#define END_VOLTAGE_VS_OCP_OBJ_NAME "end-voltage-vs-ocp"
#define SCAN_RATE_OBJ_NAME	"scan-rate"
#define CURRENT_RANGE_OBJ_NAME  "current-range-mode"
#define CURRENT_RANGE_VALUE_OBJ_NAME  "approx-current-max-value"
#define CURRENT_RANGE_UNITS_OBJ_NAME  "current-range-units"

#define START_VOLTAGE_DEFAULT	0
#define END_VOLTAGE_DEFAULT		1
#define SCAN_RATE_DEFAULT	1
#define CURRENT_RANGE_VALUE_DEFAULT 100

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Timestamp (normalized)"
#define PLOT_VAR_EWE					"Ewe"
#define PLOT_VAR_CURRENT				"Current"
#define PLOT_VAR_ECE					"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL		"Integral d(Current)/d(time)"

QString LinearSweepVoltammetry::GetShortName() const {
	return "Linear Sweep Voltammetry";
}
QString LinearSweepVoltammetry::GetFullName() const {
	return "Linear Sweep Voltammetry";
}
QString LinearSweepVoltammetry::GetDescription() const {
	return "This experiment sweeps the <b>potential</b> of the working electrode from the <b>starting potential</b> to the <b>end potential</b> at constant scan rate <b>dE/dT</b>";
}
QStringList LinearSweepVoltammetry::GetCategory() const {
  return QStringList() <<
    "Basic voltammetry";
}
ExperimentTypeList LinearSweepVoltammetry::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap LinearSweepVoltammetry::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* LinearSweepVoltammetry::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	
  //TODO: add current ranging input

	_INSERT_RIGHT_ALIGN_COMMENT("Starting potential = ", row, 0);
	_INSERT_TEXT_INPUT(START_VOLTAGE_DEFAULT, START_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
  _START_DROP_DOWN(START_VOLTAGE_VS_OCP_OBJ_NAME, row, 1);
  _ADD_DROP_DOWN_ITEM("open circuit");
  _ADD_DROP_DOWN_ITEM("reference");
  _END_DROP_DOWN();

  ++row;
  _INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("End Potential = ", row, 0);
	_INSERT_TEXT_INPUT(END_VOLTAGE_DEFAULT, END_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
  _START_DROP_DOWN(END_VOLTAGE_VS_OCP_OBJ_NAME, row, 1);
  _ADD_DROP_DOWN_ITEM("open circuit");
  _ADD_DROP_DOWN_ITEM("reference");
  _END_DROP_DOWN();

  ++row;
  _INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("dE/dt = ", row, 0);
	_INSERT_TEXT_INPUT(SCAN_RATE_DEFAULT, SCAN_RATE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mV/s", row, 2);

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_LEFT_ALIGN_COMMENT("<b>Current ranging</b>", row, 1);
  
  ++row;
  _START_RADIO_BUTTON_GROUP("CURRENT_RANGE_OBJ_NAME");
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

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(3, 1);

	USER_INPUT_END();
}
NodesData LinearSweepVoltammetry::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	double startVoltage;
	double endVoltage;
	double dEdt;
  QString startVoltageVsOCP_str;
  QString endVoltageVsOCP_str;
  bool startVoltageVsOCP;
  bool endVoltageVsOCP;
  QString currentRangeMode_str;
  QString currentRangeUnits_str;
  currentRange_t currentRangeMode;
  double approxMaxCurrent;

	GET_TEXT_INPUT_VALUE_DOUBLE(startVoltage, START_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(endVoltage, END_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(dEdt, SCAN_RATE_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(startVoltageVsOCP_str, START_VOLTAGE_VS_OCP_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(endVoltageVsOCP_str, END_VOLTAGE_VS_OCP_OBJ_NAME);
  GET_SELECTED_RADIO(currentRangeMode_str, CURRENT_RANGE_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(approxMaxCurrent, CURRENT_RANGE_VALUE_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(currentRangeUnits_str, CURRENT_RANGE_UNITS_OBJ_NAME);

  startVoltageVsOCP = startVoltageVsOCP_str.contains("open circuit");
  endVoltageVsOCP = endVoltageVsOCP_str.contains("open circuit");

  if (currentRangeMode_str.contains("Autorange"))
    currentRangeMode = AUTORANGE;
  else
  {
    if (currentRangeUnits_str.contains("mA"))
      approxMaxCurrent *= 1;
    else if (currentRangeUnits_str.contains("uA"))
      approxMaxCurrent *= 1e-3;
    else if (currentRangeUnits_str.contains("nA"))
      approxMaxCurrent *= 1e-6;
    currentRangeMode = ExperimentCalcHelperClass::GetCurrentRange(hwVersion.hwModel, &calData, approxMaxCurrent);
  }

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_POINT_POT;
  exp.tMin = 5 * MILLISECONDS;
  exp.tMax = 5 * MILLISECONDS;
  exp.currentRangeMode = currentRangeMode;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 1);
  exp.DCPoint_pot.dIdtMin = 0;
  exp.DCPoint_pot.Imax = 32767;
  exp.DCPoint_pot.IrangeMax = RANGE0;
  exp.DCPoint_pot.Imin = 0;
  exp.DCPoint_pot.IrangeMin = RANGE7;
  exp.DCPoint_pot.VPointUserInput = ExperimentCalcHelperClass::GetBINVoltage(&calData, startVoltage);
  exp.DCPoint_pot.VPointVsOCP = startVoltageVsOCP;
  exp.numPlays = 1;

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 10 * MICROSECONDS;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = currentRangeMode;
  ExperimentCalcHelperClass::GetSamplingParams_potSweep(hwVersion.hwModel, &calData, &exp, dEdt);
  exp.DCSweep_pot.VStartUserInput = ExperimentCalcHelperClass::GetBINVoltage(&calData, startVoltage);
  exp.DCSweep_pot.VStartVsOCP = startVoltageVsOCP;
	exp.DCSweep_pot.VEndUserInput = ExperimentCalcHelperClass::GetBINVoltage(&calData, endVoltage);
  exp.DCSweep_pot.VEndVsOCP = endVoltageVsOCP;
	exp.DCSweep_pot.Imax = 32767;
	exp.DCSweep_pot.IRangeMax = RANGE0;
	exp.DCSweep_pot.Imin = 0;
	exp.DCSweep_pot.IRangeMin = RANGE7;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList LinearSweepVoltammetry::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			PLOT_VAR_TIMESTAMP <<
			PLOT_VAR_TIMESTAMP_NORMALIZED <<
			PLOT_VAR_EWE <<
			PLOT_VAR_CURRENT;
	}

	return ret;
}
QStringList LinearSweepVoltammetry::GetYAxisParameters(ExperimentType type) const {
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
void LinearSweepVoltammetry::PushNewDcData(const ExperimentalDcData &expData, DataMap &container, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / 100000000UL;
  ProcessedDCData processedDCdata = ExperimentCalcHelperClass::ProcessDCDataPoint(&calData, expData);

	if (container[PLOT_VAR_CURRENT_INTEGRAL].data.isEmpty()) {
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, processedDCdata.current / timestamp / 3600.0);
	}
	else {
		qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].data.last();
		newVal += (container[PLOT_VAR_CURRENT].data.last() + processedDCdata.current) * (timestamp + container[PLOT_VAR_TIMESTAMP].data.last()) / 3600.0 / 2.;
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, newVal);
	}

	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP, timestamp);
	PUSH_BACK_DATA(PLOT_VAR_EWE, processedDCdata.EWE);
	PUSH_BACK_DATA(PLOT_VAR_ECE, processedDCdata.ECE);
	PUSH_BACK_DATA(PLOT_VAR_CURRENT, processedDCdata.current);

	if (!timestampOffset.contains(&container)) {
		timestampOffset[&container] = timestamp;
	}
	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED, timestamp - timestampOffset[&container]);
}
void LinearSweepVoltammetry::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}

void LinearSweepVoltammetry::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_TIMESTAMP);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);
	
	SAVE_DATA_END();
}