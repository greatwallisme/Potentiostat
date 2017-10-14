#include "DCCurrentSweep.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"dc-current-sweep-element"

#define STARTING_CURRENT_OBJ_NAME	"starting-current"
#define STARTING_CURRENT_UNITS_OBJ_NAME	"starting-current-units"
#define ENDING_CURRENT_OBJ_NAME	"ending-current"
#define ENDING_CURRENT_UNITS_OBJ_NAME	"ending-current-units"
#define SWEEP_RATE_OBJ_NAME         "sweep-rate"
#define SWEEP_RATE_UNITS_OBJ_NAME "sweep-rate-units"
#define SAMPLING_INT_OBJ_NAME "sampling-interval"
#define SAMPLING_INT_UNITS_OBJ_NAME "sampling-interval-units"

#define STARTING_CURRENT_DEFAULT	-10
#define ENDING_CURRENT_DEFAULT  10
#define SWEEP_RATE_DEFAULT 0.1
#define SAMPLING_INT_DEFAULT 1

QString DCCurrentSweep::GetFullName() const {
	return "DC Current Sweep";
}
QStringList DCCurrentSweep::GetCategory() const {
	return QStringList() <<
		"Galvanostatic control" <<
		"Basic voltammetry";
}
QPixmap DCCurrentSweep::GetImage() const {
	return QPixmap(":/GUI/DCCurrentSweep");
}
ExperimentType DCCurrentSweep::GetType() const {
	return ET_DC;
}
QWidget* DCCurrentSweep::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Starting current = ", row, 0);
	_INSERT_TEXT_INPUT(STARTING_CURRENT_DEFAULT, STARTING_CURRENT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(STARTING_CURRENT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Ending current = ", row, 0);
  _INSERT_TEXT_INPUT(ENDING_CURRENT_DEFAULT, ENDING_CURRENT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(ENDING_CURRENT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Scan rate = ", row, 0);
  _INSERT_TEXT_INPUT(SWEEP_RATE_DEFAULT, SWEEP_RATE_OBJ_NAME, row, 1);
  _START_DROP_DOWN(SWEEP_RATE_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA/s");
  _ADD_DROP_DOWN_ITEM("uA/s");
  _ADD_DROP_DOWN_ITEM("nA/s");
  _END_DROP_DOWN();

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Sampling interval</b>", row);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Sample at intervals of: ", row, 0);
  _INSERT_TEXT_INPUT(SAMPLING_INT_DEFAULT, SAMPLING_INT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(SAMPLING_INT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(4, 1);

	USER_INPUT_END();
}
NodesData DCCurrentSweep::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double IStart = inputs[STARTING_CURRENT_OBJ_NAME].toDouble();
  double IEnd = inputs[ENDING_CURRENT_OBJ_NAME].toDouble();
  double dIdt = inputs[SWEEP_RATE_OBJ_NAME].toDouble();
  QString IStartUnits_str = inputs[STARTING_CURRENT_UNITS_OBJ_NAME].toString();
  QString IEndUnits_str = inputs[ENDING_CURRENT_UNITS_OBJ_NAME].toString();
  QString dIdtUnits_str = inputs[SWEEP_RATE_UNITS_OBJ_NAME].toString();
  double sampInterval = inputs[SAMPLING_INT_OBJ_NAME].toDouble();

  IStart *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[STARTING_CURRENT_UNITS_OBJ_NAME].toString());
  IEnd *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[ENDING_CURRENT_UNITS_OBJ_NAME].toString());
  dIdt *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[SWEEP_RATE_UNITS_OBJ_NAME].toString());
  
  if (!inputs[SAMPLING_INT_UNITS_OBJ_NAME].toString().contains("s"))
  {
      sampInterval = sampInterval / dIdt;       //convert from mA to s
      sampInterval *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[SAMPLING_INT_UNITS_OBJ_NAME].toString());
  }
  
  currentRange_t currentRange = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, MAX(ABS(IStart), ABS(IEnd)));

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_GALV;
	exp.tMin = 5 * MILLISECONDS;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = currentRange;
  ExperimentCalcHelperClass::GetSamplingParams_galvSweep(hwVersion.hwModel, &calData, &exp, dIdt, currentRange, sampInterval);
  exp.DCSweep_galv.IStart = ExperimentCalcHelperClass::GetBINCurrent(&calData, currentRange, IStart);
  exp.DCSweep_galv.IEnd = ExperimentCalcHelperClass::GetBINCurrent(&calData, currentRange, IEnd);
  exp.DCSweep_galv.Irange = currentRange;
  exp.DCSweep_galv.Vmin = -MAX_VOLTAGE;
  exp.DCSweep_galv.Vmax = MAX_VOLTAGE;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}