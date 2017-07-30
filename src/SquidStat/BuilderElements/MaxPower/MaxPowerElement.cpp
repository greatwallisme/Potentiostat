#include "MaxPowerElement.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"max-power-element"

#define VMAX_OBJ_NAME       "maximum-voltage"
#define VMIN_OBJ_NAME       "minimum-voltage"
#define T_MAX_OBJ_NAME      "maximum-time"
#define T_MAX_UNITS_OBJ_NAME         "time-units"
#define SAMPLING_INT_OBJ_NAME "sampling-interval"

#define POWER_DEFAULT	1
#define V_MAX_DEFAULT 1
#define V_MIN_DEFAULT		0
#define T_MAX_DEFAULT 60
#define SAMPLING_INT_DEFAULT 1

QString MaxPowerElement::GetFullName() const {
	return "Maximum power dischg";
}
QStringList MaxPowerElement::GetCategory() const {
	return QStringList() <<
		"Energy storage" <<
    "Charge-discharge";
}
QPixmap MaxPowerElement::GetImage() const {
	return QPixmap(":/GUI/MaxPower");
}
ExperimentType MaxPowerElement::GetType() const {
	return ET_DC;
}
QWidget* MaxPowerElement::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

  int row = 0;
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
  _START_DROP_DOWN(T_MAX_UNITS_OBJ_NAME, row, 2);
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
NodesData MaxPowerElement::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double VMin = inputs[VMIN_OBJ_NAME].toDouble();
  double VMax = inputs[VMAX_OBJ_NAME].toDouble();
  double tmax = inputs[T_MAX_OBJ_NAME].toDouble();
  double dt = inputs[SAMPLING_INT_OBJ_NAME].toDouble();

  tmax *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[T_MAX_UNITS_OBJ_NAME].toString());

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_MAX_POWER;
  exp.tMin = 10 * MILLISECONDS;
  exp.tMax = (uint64_t)(tmax * SECONDS);
  exp.currentRangeMode = AUTORANGE;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, dt);
  exp.DCMaxPower.VMax = (float)VMax;
  exp.DCMaxPower.VMin = (float)VMin;
  exp.numPlays = 1;
  PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}