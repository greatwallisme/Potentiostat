#include "OpenCircuitElement.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"open-circuit-element"

#define SAMPLING_INT_OBJ_NAME "sampling-interval"
#define SAMPLING_INT_UNITS_OBJ_NAME "sampling-interval-units"
#define DURATION_OBJ_NAME "experiment-duration"
#define DURATION_UNITS_OBJ_NAME "experiment-duration-units"
#define V_MAX_OBJ_NAME "maximum-voltage"
#define V_MIN_OBJ_NAME "minimum-voltage"
#define DVDT_MIN_OBJ_NAME "dvdt-minimum"

#define SAMPLING_INT_DEFAULT 1
#define DURATION_DEFAULT 60
#define V_MAX_DEFAULT 12
#define V_MIN_DEFAULT -12
#define DVDT_MIN_DEFAULT 0

QString OpenCircuitElement::GetFullName() const {
	return "Open Circuit";
}
QStringList OpenCircuitElement::GetCategory() const {
	return QStringList() <<
		"Basic voltammetry";
}
QPixmap OpenCircuitElement::GetImage() const {
	return QPixmap(":/GUI/OpenCircuit");
}
ExperimentType OpenCircuitElement::GetType() const {
	return ET_DC;
}
QWidget* OpenCircuitElement::CreateUserInput(UserInput &inputs) const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
  _INSERT_RIGHT_ALIGN_COMMENT("Sampling interval = ", row, 0);
  _INSERT_TEXT_INPUT(SAMPLING_INT_DEFAULT, SAMPLING_INT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(SAMPLING_INT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();

  ++row;
  _INSERT_CENTERED_COMMENT("<b>Ending Conditions</b>", row);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Experiment duration = ", row, 0);
  _INSERT_TEXT_INPUT(DURATION_DEFAULT, DURATION_OBJ_NAME, row, 1);
  _START_DROP_DOWN(DURATION_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Maximum voltage = ", row, 0);
  _INSERT_TEXT_INPUT(V_MAX_DEFAULT, V_MAX_OBJ_NAME, row, 1);
  _INSERT_RIGHT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Minimum voltage = ", row, 0);
  _INSERT_TEXT_INPUT(V_MIN_DEFAULT, V_MIN_OBJ_NAME, row, 1);
  _INSERT_RIGHT_ALIGN_COMMENT("V", row, 2);

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Minimum potential rate of change = ", row, 0);
  _INSERT_TEXT_INPUT(DVDT_MIN_DEFAULT, DVDT_MIN_OBJ_NAME, row, 1);
  _INSERT_RIGHT_ALIGN_COMMENT("mV/s", row, 2);

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(3, 1);

	USER_INPUT_END();
}
NodesData OpenCircuitElement::GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double samplingInterval = inputs[SAMPLING_INT_OBJ_NAME].toDouble();
  samplingInterval *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[SAMPLING_INT_UNITS_OBJ_NAME].toString());
  double duration = inputs[DURATION_OBJ_NAME].toDouble();
  duration *= ExperimentCalcHelperClass::GetUnitsMultiplier(inputs[DURATION_UNITS_OBJ_NAME].toString());
  double VMax = inputs[V_MAX_OBJ_NAME].toDouble();
  double VMin = inputs[V_MIN_OBJ_NAME].toDouble();
  double dVdtMin = inputs[DVDT_MIN_OBJ_NAME].toDouble();

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_OCP;
  exp.tMin = 0;
  exp.tMax = duration * SECONDS;
  exp.MaxPlays = 1;
  exp.currentRangeMode = AUTORANGE;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, samplingInterval);
  exp.DCocp.dVdtMin = (float) dVdtMin;
  exp.DCocp.Vmax = (float) VMax;
  exp.DCocp.Vmin = (float) VMin;
  PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}