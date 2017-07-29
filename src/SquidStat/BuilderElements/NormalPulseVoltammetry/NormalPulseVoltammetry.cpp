#include "NormalPulseVoltammetry.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"normal-pulse-voltammetry-element"

#define START_VOLTAGE_OBJ_NAME	"start-voltage"
#define START_V_VS_OCP_OBJ_NAME	"start-voltage-vs-ocp"
#define FINAL_VOLTAGE_OBJ_NAME	"final-voltage"
#define FINAL_V_VS_OCP_OBJ_NAME	"final-voltage-vs-ocp"
#define VOLTAGE_STEP_OBJ_NAME   "voltage-step"
#define PULSE_WIDTH_OBJ_NAME	"pulse-width"
#define PULSE_PERIOD_OBJ_NAME	"pulse-period"
#define AUTORANGE_MODE_OBJ_NAME "Autorange-mode"
#define MAX_CURRENT_OBJ_NAME "max-current"
#define MAX_CURRENT_UNITS_OBJ_NAME "max-current-units"

#define START_VOLTAGE_DEFAULT	-0.5
#define FINAL_VOLTAGE_DEFAULT	0.5
#define VOLTAGE_STEP_DEFAULT	0.01
#define PULSE_WIDTH_DEFAULT		50
#define PULSE_PERIOD_DEFAULT	100
#define MAX_CURRENT_DEFAULT 100

QString NormalPulseVoltammetry::GetFullName() const {
	return "Normal pulse voltammetry";
}
QStringList NormalPulseVoltammetry::GetCategory() const {
	return QStringList() <<
		"Potentiostatic control" <<
		"Basic voltammetry" <<
    "Pulse voltammetry";
}
QPixmap NormalPulseVoltammetry::GetImage() const {
	return QPixmap(":/GUI/NormalPulseVoltammetry");
}
ExperimentType NormalPulseVoltammetry::GetType() const {
	return ET_DC;
}
QWidget* NormalPulseVoltammetry::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

  int row = 0;
  _INSERT_RIGHT_ALIGN_COMMENT("Baseline potential = ", row, 0);
  _INSERT_TEXT_INPUT(START_VOLTAGE_DEFAULT, START_VOLTAGE_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("with respect to", row, 0);
  _START_DROP_DOWN_EXT(START_V_VS_OCP_OBJ_NAME, row, 1, 1, -1);
  _ADD_DROP_DOWN_ITEM("open circuit");
  _ADD_DROP_DOWN_ITEM("reference");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Voltage step = ", row, 0);
  _INSERT_TEXT_INPUT(VOLTAGE_STEP_DEFAULT, VOLTAGE_STEP_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Final potential = ", row, 0);
  _INSERT_TEXT_INPUT(FINAL_VOLTAGE_DEFAULT, FINAL_VOLTAGE_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
  _START_DROP_DOWN_EXT(FINAL_V_VS_OCP_OBJ_NAME, row, 1, 1, -1);
  _ADD_DROP_DOWN_ITEM("open circuit");
  _ADD_DROP_DOWN_ITEM("reference");
  _END_DROP_DOWN();

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Pulse width = ", row, 0);
  _INSERT_TEXT_INPUT(PULSE_WIDTH_DEFAULT, PULSE_WIDTH_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("ms", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Pulse period = ", row, 0);
  _INSERT_TEXT_INPUT(PULSE_PERIOD_DEFAULT, PULSE_PERIOD_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("ms", row, 2);

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
	_SET_COL_STRETCH(2, 1);

	USER_INPUT_END();
}
NodesData NormalPulseVoltammetry::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double startVoltage = inputs[START_VOLTAGE_OBJ_NAME].toDouble();
  double VStep = inputs[VOLTAGE_STEP_OBJ_NAME].toDouble();
  double VFinal = inputs[FINAL_VOLTAGE_OBJ_NAME].toDouble();
  double pulseWidth = inputs[PULSE_WIDTH_OBJ_NAME].toDouble();
  double pulsePeriod = inputs[PULSE_PERIOD_OBJ_NAME].toDouble();
  if (pulseWidth >= pulsePeriod)
    pulsePeriod += pulseWidth;

  QString RangeMode_str = inputs[AUTORANGE_MODE_OBJ_NAME].toString();
  double maxCurrent = inputs[MAX_CURRENT_OBJ_NAME].toDouble();
  maxCurrent *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[MAX_CURRENT_UNITS_OBJ_NAME].toString());

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_NORMALPULSE_POT;
  exp.tMin = 0;
  exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = RangeMode_str.contains("Auto") ? AUTORANGE : ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, maxCurrent);
  ExperimentCalcHelperClass::GetSamplingParameters_pulse(hwVersion.hwModel, (qint32)round(pulsePeriod), (qint32)round(pulseWidth), &exp);
  exp.DCPulseNormal_pot.VBaselineUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, startVoltage);
  exp.DCPulseNormal_pot.VBaselineVsOCP = inputs[START_V_VS_OCP_OBJ_NAME].toString().contains("open circuit");
  exp.DCPulseNormal_pot.VEndUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, VFinal);
  exp.DCPulseNormal_pot.VEndVsOCP = inputs[FINAL_V_VS_OCP_OBJ_NAME].toString().contains("open circuit");
  exp.DCPulseNormal_pot.VStep = (float)((calData.m_DACdcN_V + calData.m_DACdcP_V) / 2 * VStep);
  exp.MaxPlays = 1;
  PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}