#include "EISGalvanostatic.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"eis-galvanostatic-element"

#define UPPER_FREQUENCY_OBJ_NAME "upper-frequency"
#define UPPER_FREQUENCY_UNITS_OBJ "upper-frequency-units"
#define LOWER_FREQUENCY_OBJ_NAME "lower-frequency"
#define LOWER_FREQUENCY_UNITS_OBJ "lower-frequency-units"
#define STEPS_PER_DEC_OBJ_NAME "steps-per-decade"
#define DC_BIAS_CURRENT_OBJ_NAME "dc-bias-current"
#define DC_BIAS_CURRENT_UNITS_OBJ_NAME "dc-bias-current-units"
#define AC_AMPLITUDE_OBJ_NAME "ac-amplitude"
#define AC_AMPLITUDE_UNITS_OBJ_NAME "ac-amplitude-units"

#define UPPER_FREQUENCY_DEFAULT 100
#define LOWER_FREQUENCY_DEFAULT 1
#define STEPS_PER_DEC_DEFAULT 5
#define DC_BIAS_CURRENT_DEFAULT 0
#define AC_AMPLITUDE_DEFAULT 1

QString EISGalvanostatic::GetFullName() const {
	return "EIS, Galvanostatic";
}
QStringList EISGalvanostatic::GetCategory() const {
	return QStringList() <<
		"Galvanostatic control" <<
		"Impedance methods";
}
QPixmap EISGalvanostatic::GetImage() const {
	return QPixmap(":/GUI/EISGalvanostatic");
}
ExperimentType EISGalvanostatic::GetType() const {
	return ET_AC;
}
QWidget* EISGalvanostatic::CreateUserInput(UserInput &inputs) const {
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
  _INSERT_RIGHT_ALIGN_COMMENT("DC Bias current: ", row, 0);
  _INSERT_TEXT_INPUT(DC_BIAS_CURRENT_DEFAULT, DC_BIAS_CURRENT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(DC_BIAS_CURRENT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("AC amplitude: ", row, 0);
  _INSERT_TEXT_INPUT(AC_AMPLITUDE_DEFAULT, AC_AMPLITUDE_OBJ_NAME, row, 1);
  _START_DROP_DOWN(AC_AMPLITUDE_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(2, 1);

	USER_INPUT_END();
}
NodesData EISGalvanostatic::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double upperFreq = inputs[UPPER_FREQUENCY_OBJ_NAME].toDouble();
  double lowerFreq = inputs[LOWER_FREQUENCY_OBJ_NAME].toDouble();
  double stepsPerDec = inputs[STEPS_PER_DEC_OBJ_NAME].toDouble();
  double biasCurrent = inputs[DC_BIAS_CURRENT_OBJ_NAME].toDouble();
  double acAmp = inputs[AC_AMPLITUDE_OBJ_NAME].toDouble();

  upperFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[UPPER_FREQUENCY_UNITS_OBJ].toString());
  lowerFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[LOWER_FREQUENCY_UNITS_OBJ].toString());
  biasCurrent *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[DC_BIAS_CURRENT_UNITS_OBJ_NAME].toString());
  acAmp *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[AC_AMPLITUDE_UNITS_OBJ_NAME].toString());

  currentRange_t DCcurrentRangeLimit = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, biasCurrent);
  currentRange_t ACcurrentRangeLimit = ExperimentCalcHelperClass::GetMinCurrentRange_DACac(&calData, acAmp);

  QList<qreal> frequencyList = ExperimentCalcHelperClass::calculateFrequencyList(lowerFreq, upperFreq, stepsPerDec);

  //todo: add galv_point node?

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_POINT_GALV;
  exp.currentRangeMode = (currentRange_t)MAX((int)DCcurrentRangeLimit, (int)ACcurrentRangeLimit);
  exp.tMin = exp.tMax = 2 * SECONDS;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 4);
  exp.DCPoint_galv.dVdtMin = 0;
  exp.DCPoint_galv.IPoint = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.currentRangeMode, biasCurrent);
  exp.DCPoint_galv.Vmax = 32767;
  exp.DCPoint_galv.Vmin = -32768;
  PUSH_NEW_NODE_DATA();

  for (int i = 0; i < frequencyList.count(); i++)
  {
    exp.isHead = false;
    exp.isTail = false;
    exp.nodeType = FRA_NODE_GALV;
    exp.tMin = 0;
    exp.tMax = 0xffffffffffffffff;
    exp.currentRangeMode = (currentRange_t)MAX((int)DCcurrentRangeLimit, (int)ACcurrentRangeLimit);
    ExperimentCalcHelperClass::calcACSamplingParams(&calData, &exp, acAmp);   //todo: make this for galvanostatic amplitude
    exp.FRA_galv_node.IRange = exp.currentRangeMode;
    exp.FRA_galv_node.IBias = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.FRA_galv_node.IRange, biasCurrent);
    PUSH_NEW_NODE_DATA();
  }

	NODES_DATA_END();
}