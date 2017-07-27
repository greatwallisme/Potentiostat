#include "EISPotentiostatic.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"eis-potentiostatic-element"

#define UPPER_FREQUENCY_OBJ_NAME "upper-frequency"
#define UPPER_FREQUENCY_UNITS_OBJ "upper-frequency-units"
#define LOWER_FREQUENCY_OBJ_NAME "lower-frequency"
#define LOWER_FREQUENCY_UNITS_OBJ "lower-frequency-units"
#define STEPS_PER_DEC_OBJ_NAME "steps-per-decade"
#define DC_BIAS_POTENTIAL_OBJ_NAME "dc-bias-current"
#define DC_BIAS_VS_OCP_OBJ_NAME "dc-bias-vs-ocp"
#define AC_AMPLITUDE_OBJ_NAME "ac-amplitude"

#define UPPER_FREQUENCY_DEFAULT 100
#define LOWER_FREQUENCY_DEFAULT 1
#define STEPS_PER_DEC_DEFAULT 5
#define DC_BIAS_POTENTIAL_DEFAULT 0
#define AC_AMPLITUDE_DEFAULT 1

QString EISPotentiostatic::GetFullName() const {
	return "EIS, Potentiostatic";
}
QStringList EISPotentiostatic::GetCategory() const {
	return QStringList() <<
		"Potentiostatic control" <<
		"Impedance methods";
}
QPixmap EISPotentiostatic::GetImage() const {
	return QPixmap(":/GUI/EISPotentiostatic");
}
ExperimentType EISPotentiostatic::GetType() const {
	return ET_DC;
}
QWidget* EISPotentiostatic::CreateUserInput(UserInput &inputs) const {
  USER_INPUT_START(TOP_WIDGET_NAME);

  int row = 0;
  _INSERT_RIGHT_ALIGN_COMMENT("Upper frequency limit: ", row, 0);
  _INSERT_TEXT_INPUT(UPPER_FREQUENCY_DEFAULT, UPPER_FREQUENCY_OBJ_NAME, row, 1);
  _START_DROP_DOWN(UPPER_FREQUENCY_UNITS_OBJ, row, 2);
  _ADD_DROP_DOWN_ITEM("mHz");
  _ADD_DROP_DOWN_ITEM("Hz");
  _ADD_DROP_DOWN_ITEM("kHz");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Lower frequency limit: ", row, 0);
  _INSERT_TEXT_INPUT(LOWER_FREQUENCY_DEFAULT, LOWER_FREQUENCY_OBJ_NAME, row, 1);
  _START_DROP_DOWN(LOWER_FREQUENCY_UNITS_OBJ, row, 2);
  _ADD_DROP_DOWN_ITEM("mHz");
  _ADD_DROP_DOWN_ITEM("Hz");
  _ADD_DROP_DOWN_ITEM("kHz");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Steps per decade: ", row, 0);
  _INSERT_TEXT_INPUT(STEPS_PER_DEC_DEFAULT, STEPS_PER_DEC_OBJ_NAME, row, 1);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("DC Bias potential: ", row, 0);
  _INSERT_TEXT_INPUT(DC_BIAS_POTENTIAL_DEFAULT, DC_BIAS_POTENTIAL_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
  _START_DROP_DOWN_EXT(DC_BIAS_VS_OCP_OBJ_NAME, row, 1, 1, -1);
  _ADD_DROP_DOWN_ITEM("reference");
  _ADD_DROP_DOWN_ITEM("open circuit");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("AC amplitude: ", row, 0);
  _INSERT_TEXT_INPUT(AC_AMPLITUDE_DEFAULT, AC_AMPLITUDE_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("mV", row, 2);

  _SET_ROW_STRETCH(++row, 1);
  _SET_COL_STRETCH(2, 1);

  USER_INPUT_END();
}
NodesData EISPotentiostatic::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double upperFreq = inputs[UPPER_FREQUENCY_OBJ_NAME].toDouble();
  double lowerFreq = inputs[LOWER_FREQUENCY_OBJ_NAME].toDouble();
  double stepsPerDec = inputs[STEPS_PER_DEC_OBJ_NAME].toDouble();
  double biasVoltage = inputs[DC_BIAS_POTENTIAL_OBJ_NAME].toDouble();
  bool biasVsOCP = inputs[DC_BIAS_VS_OCP_OBJ_NAME].toString().contains("open circuit");
  double acAmp = inputs[AC_AMPLITUDE_OBJ_NAME].toDouble();

  upperFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[UPPER_FREQUENCY_UNITS_OBJ].toString());
  lowerFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[LOWER_FREQUENCY_UNITS_OBJ].toString());

  QList<qreal> frequencyList = ExperimentCalcHelperClass::calculateFrequencyList(lowerFreq, upperFreq, stepsPerDec);

  exp.isHead = exp.isTail = false;
  exp.nodeType = DCNODE_POINT_POT;
  exp.tMin = exp.tMax = 2 * SECONDS;
  exp.currentRangeMode = AUTORANGE;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 4);
  exp.DCPoint_pot.VPointUserInput = biasVoltage;
  exp.DCPoint_pot.VPointVsOCP = biasVsOCP;
  exp.DCPoint_pot.dIdtMin = 0;
  exp.DCPoint_pot.Imax = MAX_CURRENT;
  exp.DCPoint_pot.Imin = 0;
  PUSH_NEW_NODE_DATA();

  for (int i = 0; i < frequencyList.count(); i++)
  {
    exp.isHead = false;
    exp.isTail = false;
    exp.nodeType = FRA_NODE_POT;
    exp.tMin = 0;
    exp.tMax = 0xffffffffffffffff;
    exp.currentRangeMode = AUTORANGE;
    ExperimentCalcHelperClass::calcACSamplingParams(&calData, &exp, acAmp);
    exp.FRA_galv_node.IBias = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.FRA_galv_node.IRange, biasVoltage);
    PUSH_NEW_NODE_DATA();
  }
  NODES_DATA_END();
}