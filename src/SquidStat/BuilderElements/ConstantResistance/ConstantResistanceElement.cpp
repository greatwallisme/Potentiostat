#include "ConstantResistanceElement.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"constant-resistance-element"

#define RESISTANCE_OBJ_NAME	"target-resistance"
#define RESISTANCE_UNITS_OBJ_NAME "target-resistance-units"
#define VMIN_OBJ_NAME       "minimum-voltage"
#define T_MAX_OBJ_NAME      "maximum-time"
#define T_MAX_UNITS         "time-units"
#define SAMPLING_INT_OBJ_NAME "sampling-interval"

#define RESISTANCE_DEFAULT	1000
#define V_MIN_DEFAULT		0
#define T_MAX_DEFAULT 60
#define SAMPLING_INT_DEFAULT 1

QString ConstantResistanceElement::GetFullName() const {
	return "Constant resistance discharge";
}
QStringList ConstantResistanceElement::GetCategory() const {
	return QStringList() <<
		"Energy storage" <<
    "Charge-discharge";
}
QPixmap ConstantResistanceElement::GetImage() const {
	return QPixmap(":/GUI/ConstResistance");
}
ExperimentType ConstantResistanceElement::GetType() const {
	return ET_DC;
}
QWidget* ConstantResistanceElement::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

  int row = 0;
  _INSERT_RIGHT_ALIGN_COMMENT("Load resistance = ", row, 0);
  _INSERT_TEXT_INPUT(RESISTANCE_DEFAULT, RESISTANCE_OBJ_NAME, row, 1);
  _START_DROP_DOWN(RESISTANCE_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("Ohms");
  _ADD_DROP_DOWN_ITEM("kOhms");
  _ADD_DROP_DOWN_ITEM("MOhms");
  _END_DROP_DOWN();

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
	_SET_COL_STRETCH(2, 1);

	USER_INPUT_END();
}
NodesData ConstantResistanceElement::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double resistance = inputs[RESISTANCE_OBJ_NAME].toDouble();
  double VMin = inputs[VMIN_OBJ_NAME].toDouble();
  double tmax = inputs[T_MAX_OBJ_NAME].toDouble();
  double dt = inputs[SAMPLING_INT_OBJ_NAME].toDouble();

  resistance *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[RESISTANCE_UNITS_OBJ_NAME].toString());
  tmax *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[T_MAX_UNITS].toString());

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_CONST_RESISTANCE;
  exp.tMin = 5 * MILLISECONDS;
  exp.tMax = (uint64_t)(tmax * SECONDS);
  exp.currentRangeMode = AUTORANGE;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, dt, 0.05);
  exp.DCConstResistance.resistance = resistance;
  exp.DCConstResistance.ICtrl = 0;
  exp.DCConstResistance.VMax = MAX_VOLTAGE;
  exp.DCConstResistance.VMin = (float)VMin;
  exp.numPlays = 1;
  PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}