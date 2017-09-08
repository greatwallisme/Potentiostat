#include "DiffPulseVoltammetry_galv.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"differential-pulse-voltammetry-element"

#define START_CURRENT_OBJ_NAME	      "start-current"
#define START_CURRENT_UNITS_OBJ_NAME	"start-current-units"
#define FINAL_CURRENT_OBJ_NAME	      "final-current"
#define FINAL_CURRENT_UNITS_OBJ_NAME	"final-current-units"
#define CURRENT_STEP_OBJ_NAME         "current-step"
#define CURRENT_STEP_UNITS_OBJ_NAME   "current-step-units"
#define PULSE_HEIGHT_OBJ_NAME   "pulse-height"
#define PULSE_HEIGHT_UNITS_OBJ_NAME "pulse-height-units"
#define PULSE_WIDTH_OBJ_NAME	"pulse-width"
#define PULSE_PERIOD_OBJ_NAME	"pulse-period"

#define START_CURRENT_DEFAULT	-0.5
#define FINAL_CURRENT_DEFAULT	0.5
#define CURRENT_STEP_DEFAULT	0.01
#define PULSE_HEIGHT_DEFAULT 0.1
#define PULSE_WIDTH_DEFAULT		50
#define PULSE_PERIOD_DEFAULT	100

QString DiffPulseVoltammetry_galv::GetFullName() const {
	return "Differential pulse voltammetry, galvanostatic";
}
QStringList DiffPulseVoltammetry_galv::GetCategory() const {
	return QStringList() <<
		"Galvanostatic control" <<
		"Basic voltammetry" <<
    "Pulse voltammetry";
}
QPixmap DiffPulseVoltammetry_galv::GetImage() const {
	return QPixmap(":/GUI/DiffPulseVoltammetry_galv");
}
ExperimentType DiffPulseVoltammetry_galv::GetType() const {
	return ET_DC;
}
QWidget* DiffPulseVoltammetry_galv::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

  int row = 0;
  _INSERT_RIGHT_ALIGN_COMMENT("Starting current = ", row, 0);
  _INSERT_TEXT_INPUT(START_CURRENT_DEFAULT, START_CURRENT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(START_CURRENT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Current step = ", row, 0);
  _INSERT_TEXT_INPUT(CURRENT_STEP_DEFAULT, CURRENT_STEP_OBJ_NAME, row, 1);
  _START_DROP_DOWN(CURRENT_STEP_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Final current = ", row, 0);
  _INSERT_TEXT_INPUT(FINAL_CURRENT_DEFAULT, FINAL_CURRENT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(FINAL_CURRENT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

  ++row;
  _INSERT_VERTICAL_SPACING(row);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Pulse height", row, 0);
  _INSERT_TEXT_INPUT(PULSE_HEIGHT_DEFAULT, PULSE_HEIGHT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(PULSE_HEIGHT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

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

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(4, 1);

	USER_INPUT_END();
}
NodesData DiffPulseVoltammetry_galv::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double startCurrent = inputs[START_CURRENT_OBJ_NAME].toDouble();
  double IStep = inputs[CURRENT_STEP_OBJ_NAME].toDouble();
  double IFinal = inputs[FINAL_CURRENT_OBJ_NAME].toDouble();
  double pulseHeight = inputs[PULSE_HEIGHT_OBJ_NAME].toDouble();
  double pulseWidth = inputs[PULSE_WIDTH_OBJ_NAME].toDouble();
  double pulsePeriod = inputs[PULSE_PERIOD_OBJ_NAME].toDouble();
  if (pulseWidth >= pulsePeriod)
    pulsePeriod += pulseWidth;

  startCurrent *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[START_CURRENT_UNITS_OBJ_NAME].toString());
  IStep *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[CURRENT_STEP_UNITS_OBJ_NAME].toString());
  IFinal *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[FINAL_CURRENT_UNITS_OBJ_NAME].toString());
  pulseHeight *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[PULSE_HEIGHT_UNITS_OBJ_NAME].toString());

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_DIFFPULSE_GALV;
  exp.tMin = 0;
  exp.tMax = 0xFFFFFFFFFFFFFFFF;
  currentRange_t range = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, MAX(fabs(startCurrent),fabs(IFinal)));
  exp.currentRangeMode = exp.DCPulseDiff_galv.Irange = range;
  ExperimentCalcHelperClass::GetSamplingParameters_pulse(hwVersion.hwModel, (qint32)round(pulsePeriod), (qint32)round(pulseWidth), &exp);
  exp.DCPulseDiff_galv.IStart = ExperimentCalcHelperClass::GetBINCurrent(&calData, range, startCurrent);
  exp.DCPulseDiff_galv.IEnd = ExperimentCalcHelperClass::GetBINCurrent(&calData, range, IFinal);
  exp.DCPulseDiff_galv.IHeight = (int16_t)round((calData.m_DACdcP_I[range] + calData.m_DACdcN_I[range]) / 2 * pulseHeight);
  exp.DCPulseDiff_galv.IStep = (calData.m_DACdcP_I[range] + calData.m_DACdcN_I[range]) / 2 * IStep;
  exp.MaxPlays = 1;
  PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}