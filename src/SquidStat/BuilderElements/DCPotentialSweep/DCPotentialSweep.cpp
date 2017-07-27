#include "DCPotentialSweep.h"
#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"potential-sweep-element"

#define VSTART_OBJ_NAME           "starting-voltage"
#define VSTART_VS_OCP_OBJ_NAME    "starting-voltage-vs-ocp"
#define VEND_OBJ_NAME             "ending-voltage"
#define VEND_VS_OCP_OBJ_NAME      "ending-voltage-vs-ocp"
#define SLEWRATE_OBJ_NAME         "slewrate"
#define SAMPLING_MODE_OBJ_NAME    "sampling-mode"
#define SAMPLING_INT_OBJ_NAME     "sampling-interval-fixed"
#define MAX_CURRENT_OBJ_NAME      "max-current"
#define MAX_CURRENT_UNITS_OBJ_NAME "max-current-units"
#define AUTORANGE_MODE_OBJ_NAME   "autorange-mode"

#define VSTART_DEFAULT	    -0.1
#define VEND_DEFAULT        0.1
#define SLEWRATE_DEFAULT		10
#define SAMPLING_INT_DEFAULT 0.1
#define MAX_CURRENT_DEFAULT 100
#define MIN_CURRENT_DEFAULT 0

QString DCPotentialSweepElement::GetFullName() const {
	return "DC potential sweep";
}
QStringList DCPotentialSweepElement::GetCategory() const {
	return QStringList() <<
		"Potentiostatic control" <<
		"Basic voltammetry";
}
QPixmap DCPotentialSweepElement::GetImage() const {
	return QPixmap(":/GUI/DCPotentialSweep");
}
ExperimentType DCPotentialSweepElement::GetType() const {
	return ET_DC;
}
QWidget* DCPotentialSweepElement::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Starting Potential = ", row, 0);
	_INSERT_TEXT_INPUT(VSTART_DEFAULT, VSTART_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("With respect to ", row, 0);
  _START_DROP_DOWN_EXT(VSTART_VS_OCP_OBJ_NAME, row, 1, 1, -1);
  _ADD_DROP_DOWN_ITEM("reference");
  _ADD_DROP_DOWN_ITEM("open circuit");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Ending potential = ", row, 0);
  _INSERT_TEXT_INPUT(VEND_DEFAULT, VEND_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Slew rate = ", row, 0);
  _INSERT_TEXT_INPUT(SLEWRATE_DEFAULT, SLEWRATE_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("mV/s", row, 2);

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Sampling interval</b>", row);
  
  ++row;
  _START_RADIO_BUTTON_GROUP(SAMPLING_MODE_OBJ_NAME);
  _INSERT_RADIO_BUTTON_EXT("Auto-calculate (recommended)", row, 0, 1, -1);
  ++row;
  _INSERT_RADIO_BUTTON("Fixed interval: ", row, 0);
  _END_RADIO_BUTTON_GROUP();
  _INSERT_TEXT_INPUT(SAMPLING_INT_DEFAULT, SAMPLING_INT_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Current ranging</b>", row);

  ++row;
  _START_RADIO_BUTTON_GROUP(AUTORANGE_MODE_OBJ_NAME);
  _INSERT_RADIO_BUTTON("Autorange", row, 0);
  ++row;
  _INSERT_RADIO_BUTTON_EXT("Fixed range (based on maximum current)", row, 0, 1, -1);
  _END_RADIO_BUTTON_GROUP();
  //++row;
  //_INSERT_LEFT_ALIGN_COMMENT("       maximum current)", row, 1);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Max. expected current: ", row, 0);
  _INSERT_TEXT_INPUT(MAX_CURRENT_DEFAULT, MAX_CURRENT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(MAX_CURRENT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(4, 1);

	USER_INPUT_END();
}

NodesData DCPotentialSweepElement::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const
{
  NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double VStart = inputs[VSTART_OBJ_NAME].toDouble();
  bool VStartVsOCP = inputs[VSTART_VS_OCP_OBJ_NAME].toString().contains("open circuit");
  double VEnd = inputs[VEND_OBJ_NAME].toDouble();
  bool VEndVsOCP = inputs[VEND_VS_OCP_OBJ_NAME].toString().contains("open circuit");
  double dVdt = inputs[SLEWRATE_OBJ_NAME].toDouble();
  QString samplingMode_str = inputs[SAMPLING_MODE_OBJ_NAME].toString();
  double samplingInterval = inputs[SAMPLING_INT_OBJ_NAME].toDouble();
  double maxCurrent = inputs[MAX_CURRENT_OBJ_NAME].toDouble();
  QString currentRangeMode_str = inputs[AUTORANGE_MODE_OBJ_NAME].toString();
  currentRange_t currentRangeMode;

  samplingInterval = samplingMode_str.contains("Auto") ? 0 : inputs[SAMPLING_INT_OBJ_NAME].toDouble();
  
  maxCurrent *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[MAX_CURRENT_UNITS_OBJ_NAME].toString());

  if (currentRangeMode_str.contains("Autorange"))
    currentRangeMode = AUTORANGE;
  else
    currentRangeMode = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, maxCurrent);

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 0;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = currentRangeMode;

  //TODO: do something else with numPoints to deal with the fact that its a const function
  uint32_t numPoints = ExperimentCalcHelperClass::GetSamplingParams_potSweep(hwVersion.hwModel, &calData, &exp, dVdt, samplingInterval);
  (const_cast<DCPotentialSweepElement*>(this))->setNumIgnoredPoints(numPoints);

	exp.DCSweep_pot.VStartUserInput = ExperimentCalcHelperClass::GetBINVoltage(&calData, VStart);
  exp.DCSweep_pot.VStartVsOCP = VStartVsOCP;
  exp.DCSweep_pot.VEndUserInput = ExperimentCalcHelperClass::GetBINVoltage(&calData, VEnd);
  exp.DCSweep_pot.VEndVsOCP = VEndVsOCP;
  exp.DCSweep_pot.Imax = maxCurrent;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

void DCPotentialSweepElement::setNumIgnoredPoints(uint32_t numPoints)
{
  numIgnoredPoints = numPoints;
}