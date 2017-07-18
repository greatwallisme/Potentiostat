#include "ConstPotElementAdv.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"constant-potential-adv-element"

#define CONST_POTENTIAL_OBJ_NAME	"constant-potential"
#define POTENTIAL_V_OCP_OBJ_NAME	"potential-vs-ocp"
#define DURATION_OBJ_NAME         "duration"
#define DURATION_UNITS_OBJ_NAME   "duration-units"
#define SAMPLING_INTERVAL_OBJ_NAME		"sampling-interval"
#define AUTORANGING_OBJ_NAME      "Autorange-selection"
#define I_MAX_OBJ_NAME            "Max-current"
#define I_MAX_UNITS_OBJ_NAME      "Current-units"
#define DI_DT_MIN_OBJ_NAME        "dIdt-min"
#define DI_DT_UNITS_OBJ_NAME      "dIdt-units"


#define CONST_POTENTIAL_DEFAULT	0.5
#define DURATION_DEFAULT  60
#define SAMPLING_INTERVAL_DEFAULT		0.1
#define DI_DT_MIN_DEFAULT 1
#define I_MAX_DEFAULT 500

QString ConstPotElementAdv::GetFullName() const {
	return "Constant Potential, Advanced";
}
QStringList ConstPotElementAdv::GetCategory() const {
	return QStringList() <<
		"Potentiostatic control" <<
		"Basic voltammetry";
}
QPixmap ConstPotElementAdv::GetImage() const {
	return QPixmap(":/GUI/Resources/element.png");
}
ExperimentType ConstPotElementAdv::GetType() const {
	return ET_DC;
}
QWidget* ConstPotElementAdv::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Potential = ", row, 0);
	_INSERT_TEXT_INPUT(CONST_POTENTIAL_DEFAULT, CONST_POTENTIAL_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("With respect to ", row, 0);
  _START_DROP_DOWN(POTENTIAL_V_OCP_OBJ_NAME, row, 1);
  _ADD_DROP_DOWN_ITEM("reference");
  _ADD_DROP_DOWN_ITEM("open circuit");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Duration: ", row, 0);
  _INSERT_TEXT_INPUT(DURATION_DEFAULT, DURATION_OBJ_NAME, row, 1);
  _START_DROP_DOWN(DURATION_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("seconds");
  _ADD_DROP_DOWN_ITEM("minutes");
  _ADD_DROP_DOWN_ITEM("hours");
  _END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Sampling interval: ", row, 0);
	_INSERT_TEXT_INPUT(SAMPLING_INTERVAL_DEFAULT, SAMPLING_INTERVAL_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("seconds", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Current range: ", row, 0);
  _START_RADIO_BUTTON_GROUP_HORIZONTAL_LAYOUT(AUTORANGING_OBJ_NAME, row, 1);
  _INSERT_RADIO_BUTTON_LAYOUT("Autorange");
  _INSERT_RADIO_BUTTON_LAYOUT("Fixed range");
  _END_RADIO_BUTTON_GROUP();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Maximum current: ", row, 0);
  _INSERT_TEXT_INPUT(I_MAX_DEFAULT, I_MAX_OBJ_NAME, row, 1);
  _START_DROP_DOWN(I_MAX_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Minimum dI/dt", row, 0);
  _INSERT_TEXT_INPUT(DI_DT_MIN_DEFAULT, DI_DT_MIN_OBJ_NAME, row, 1);
  _START_DROP_DOWN(DI_DT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA/s");
  _ADD_DROP_DOWN_ITEM("uA/s");
  _ADD_DROP_DOWN_ITEM("nA/s");
  _END_DROP_DOWN();

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(2, 1);

	USER_INPUT_END();
}
NodesData ConstPotElementAdv::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  QString vsOCP_string = inputs[POTENTIAL_V_OCP_OBJ_NAME].toString();
  bool vsOCP = vsOCP_string.contains("open circuit");

  int durationMultiplier = 1;
  QString durationUnits_str = inputs[DURATION_UNITS_OBJ_NAME].toInt();
  if (durationUnits_str.contains("seconds"))
    durationMultiplier = 1;
  else if (durationUnits_str.contains("minutes"))
    durationMultiplier = 60;
  else if (durationUnits_str.contains("hours"))
    durationMultiplier = 3600;

  QString Autorange_mode_str = inputs[AUTORANGING_OBJ_NAME].toString();
  bool Autorange_mode = Autorange_mode_str.contains("Autorange");

  double maxCurrent = inputs[I_MAX_OBJ_NAME].toDouble();
  double currentUnitsMultiplier = 1;
  QString currentUnits_str = inputs[I_MAX_UNITS_OBJ_NAME].toString();
  if (currentUnits_str.contains("mA"))
    currentUnitsMultiplier = 1;
  else if (currentUnits_str.contains("uA"))
    currentUnitsMultiplier = 1e-3;
  else if (currentUnits_str.contains("nA"))
    currentUnitsMultiplier = 1e-6;

  double dIdtUnitsMultiplier = 1;
  QString dIdtUnits_str = inputs[DI_DT_UNITS_OBJ_NAME].toString();
  if (dIdtUnits_str.contains("mA/s"))
    dIdtUnitsMultiplier = 1;
  else if (dIdtUnits_str.contains("uA/s"))
    dIdtUnitsMultiplier = 1e-3;
  else if (dIdtUnits_str.contains("nA/s"))
    dIdtUnitsMultiplier = 1e-6;

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 0;
	exp.tMax = (uint64_t)(inputs[DURATION_OBJ_NAME].toDouble() * durationMultiplier * 1e8);
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, inputs[SAMPLING_INTERVAL_OBJ_NAME].toDouble());
	exp.DCPoint_pot.VPointUserInput = ExperimentCalcHelperClass::GetBINVoltage(&calData, inputs[CONST_POTENTIAL_OBJ_NAME].toDouble());
  exp.DCPoint_pot.VPointVsOCP = vsOCP;
  exp.DCPoint_pot.IrangeMax = ExperimentCalcHelperClass::GetCurrentRange(hwVersion.hwModel, &calData, maxCurrent*currentUnitsMultiplier);
  exp.DCPoint_pot.Imax = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_pot.IrangeMax, maxCurrent*currentUnitsMultiplier);
  exp.DCPoint_pot.IrangeMin = ExperimentCalcHelperClass::GetCurrentRange(hwVersion.hwModel, &calData, 0);
  exp.DCPoint_pot.Imin = 0;
  exp.DCPoint_pot.dIdtMin = inputs[DI_DT_MIN_OBJ_NAME].toDouble()*dIdtUnitsMultiplier;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}