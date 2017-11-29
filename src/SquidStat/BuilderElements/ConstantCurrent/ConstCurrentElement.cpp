#include "ConstCurrentElement.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"constant-current-element"

#define CONST_CURRENT_OBJ_NAME	"constant-current"
#define CONST_CURRENT_UNITS_OBJ_NAME "constant-current-units"
#define DURATION_OBJ_NAME         "duration"
#define DURATION_UNITS_OBJ_NAME   "duration-units"
#define VMAX_OBJ_NAME             "Maximum-voltage"
#define VMIN_OBJ_NAME             "Minimum-voltage"
#define CAPACITY_OBJ_NAME     "capacity"
#define SAMPLING_INTERVAL_OBJ_NAME		"sampling-interval"
#define SAMPLING_INTERVAL_UNITS_OBJ_NAME   "sampling-interval-units"

#define CONST_CURRENT_DEFAULT	10
#define DURATION_DEFAULT  60
#define VMAX_DEFAULT    5
#define VMIN_DEFAULT  0
#define CAPACITY_DEFAULT 10
#define SAMPLING_INTERVAL_DEFAULT		0.5

QString ConstCurrentElement::GetFullName() const {
	return "Constant Current";
}
QStringList ConstCurrentElement::GetCategory() const {
	return QStringList() <<
		"Galvanostatic control" <<
		"Basic voltammetry";
}
QPixmap ConstCurrentElement::GetImage() const {
	return QPixmap(":/GUI/ConstantCurrent");
}
ExperimentType ConstCurrentElement::GetType() const {
	return ET_DC;
}
QWidget* ConstCurrentElement::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Current = ", row, 0);
	_INSERT_TEXT_INPUT(CONST_CURRENT_DEFAULT, CONST_CURRENT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(CONST_CURRENT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Sampling interval: ", row, 0);
  _INSERT_TEXT_INPUT(SAMPLING_INTERVAL_DEFAULT, SAMPLING_INTERVAL_OBJ_NAME, row, 1);
  _START_DROP_DOWN(SAMPLING_INTERVAL_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Ending conditions<b>", row);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Upper voltage limit: ", row, 0);
  _INSERT_TEXT_INPUT(VMAX_DEFAULT, VMAX_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Lower voltage limit: ", row, 0);
  _INSERT_TEXT_INPUT(VMIN_DEFAULT, VMIN_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Max duration: ", row, 0);
  _INSERT_TEXT_INPUT(DURATION_DEFAULT, DURATION_OBJ_NAME, row, 1);
  _START_DROP_DOWN(DURATION_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Max capacity", row, 0);
  _INSERT_TEXT_INPUT(CAPACITY_DEFAULT, CAPACITY_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("mAh", row, 2);

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(0, 3);
  _SET_COL_STRETCH(1, 1);
  _SET_COL_STRETCH(2, 2);

	USER_INPUT_END();
}
NodesData ConstCurrentElement::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double constCurrent = inputs[CONST_CURRENT_OBJ_NAME].toDouble();
  constCurrent *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[CONST_CURRENT_UNITS_OBJ_NAME].toString());
  double duration = inputs[DURATION_OBJ_NAME].toDouble();
  duration *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[DURATION_UNITS_OBJ_NAME].toString());
  double samplingInterval = inputs[SAMPLING_INTERVAL_OBJ_NAME].toDouble();
  samplingInterval *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[SAMPLING_INTERVAL_UNITS_OBJ_NAME].toString());
  double Vmax = inputs[VMAX_OBJ_NAME].toDouble();
  double Vmin = inputs[VMIN_OBJ_NAME].toDouble();
  double capacity = inputs[CAPACITY_OBJ_NAME].toDouble();
  capacity *= 3600; //convert from mAh to mC
  capacity = fabs(capacity);

  /* Make sure Vmax and Vmin aren't swapped */
  double tempV = MAX(Vmax, Vmin);
  Vmin = MIN(Vmax, Vmin);
  Vmax = tempV;

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_GALV;
	exp.tMin = 10 * MILLISECONDS;
	exp.tMax = (uint64_t)(duration * SECONDS);
  exp.currentRangeMode = AUTORANGE;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, samplingInterval, 0.5);
  exp.DCPoint_galv.Irange = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, constCurrent);
  exp.DCPoint_galv.IPoint = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, constCurrent);
  exp.DCPoint_galv.dVdtMin = 0;
  exp.DCPoint_galv.Vmax = Vmax;
  exp.DCPoint_galv.Vmin = Vmin;
  exp.DCPoint_galv.CapacityMax = capacity;
  exp.DCPoint_galv.isTrackingCapacity = (capacity != 0);
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}