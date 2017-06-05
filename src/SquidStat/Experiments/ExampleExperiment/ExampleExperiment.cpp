#include "ExampleExperiment.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"example-experiment"

#define START_VOLTAGE_OBJ_NAME	"start-voltage"
#define END_VOLTAGE_OBJ_NAME	"end-voltage"
#define VOLTAGE_STEP_OBJ_NAME	"voltage-step"
#define REPEATS_OBJ_NAME		"repeats"

#define START_VOLTAGE_DEFAULT	0
#define END_VOLTAGE_DEFAULT		1024
#define VOLTAGE_STEP_DEFAULT	1
#define REPEATS_DEFAULT			3


QString ExampleExperiment::GetShortName() const {
	return "Example Experiment";
}
QString ExampleExperiment::GetFullName() const {
	return "Linear Sweep Voltammetry";
}
QString ExampleExperiment::GetDescription() const {
	return "This experiment sweeps the <b>potential</b> of the working electrode from E1 to E2 at constant scan rate dE/dT";
}
QString ExampleExperiment::GetCategory() const {
	return "Example Category";
}
QPixmap ExampleExperiment::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
QWidget* ExampleExperiment::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Start Voltage = ", row, 0);
	_INSERT_TEXT_INPUT(START_VOLTAGE_DEFAULT, START_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("End Voltage = ", row, 0);
	_INSERT_TEXT_INPUT(END_VOLTAGE_DEFAULT, END_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("dV/dt = ", row, 0);
	_INSERT_TEXT_INPUT(VOLTAGE_STEP_DEFAULT, VOLTAGE_STEP_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Repeats = ", row, 0);
	_INSERT_TEXT_INPUT(REPEATS_DEFAULT, REPEATS_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test radio 1", row, 0);
	_START_RADIO_BUTTON_GROUP("Test radio 1 id");
		_INSERT_RADIO_BUTTON("Ref", row, 1);
		_INSERT_RADIO_BUTTON("Open circuit", row, 2);
	_END_RADIO_BUTTON_GROUP();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test radio 2", row, 0);
	_START_RADIO_BUTTON_GROUP("Test radio 2 id");
		_INSERT_RADIO_BUTTON("Ref", row, 1);
		_INSERT_RADIO_BUTTON("Open circuit", row, 2);
	_END_RADIO_BUTTON_GROUP();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test drop down", row, 0);
	_START_DROP_DOWN("Test drop down id", row, 1);
		_ADD_DROP_DOWN_ITEM("Item 1");
		_ADD_DROP_DOWN_ITEM("Item 2");
		_ADD_DROP_DOWN_ITEM("Item 3");
	_END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test radio layout", row, 0);
	_START_RADIO_BUTTON_GROUP_HORIZONTAL_LAYOUT("Test radio layout id", row, 1);
		_INSERT_RADIO_BUTTON_LAYOUT("Ref");
		_INSERT_RADIO_BUTTON_LAYOUT("Open circuit");
	_END_RADIO_BUTTON_GROUP_LAYOUT();

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(3, 1);

	USER_INPUT_END();
}
QByteArray ExampleExperiment::GetNodesData(QWidget *wdg) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	QString selectedRadio1;
	QString selectedRadio2;
	GET_SELECTED_RADIO(selectedRadio1, "Test radio 1 id");
	GET_SELECTED_RADIO(selectedRadio2, "Test radio 2 id");


	QString selectedDropDown;
	GET_SELECTED_DROP_DOWN(selectedDropDown, "Test drop down id");

	qint32 startVoltage;
	qint32 endVoltage;
	qint32 voltageStep;
	qint32 repeats;
	GET_TEXT_INPUT_VALUE(startVoltage, START_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(endVoltage, END_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(voltageStep, VOLTAGE_STEP_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(repeats, REPEATS_OBJ_NAME);

	exp.isHead = true;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP;
	exp.tMin = 100000;
	exp.tMax = 10000000000;
	exp.samplingParams.ADCTimerDiv = 2;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSize = 20;
	exp.samplingParams.DACMultiplier = 20;
	exp.DCSweep.VStart = startVoltage;
	exp.DCSweep.VEnd = endVoltage;
	exp.DCSweep.dVdt = voltageStep;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = true;
	exp.nodeType = DCNODE_SWEEP;
	exp.tMin = 100000;
	exp.tMax = 100000000;
	exp.samplingParams.ADCTimerDiv = 2;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSize = 20;
	exp.samplingParams.DACMultiplier = 20;
	exp.DCSweep.VStart = endVoltage;
	exp.DCSweep.VEnd = startVoltage;
	exp.DCSweep.dVdt = -voltageStep;
	exp.MaxPlays = repeats;
	exp.branchHeadIndex = 0;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}