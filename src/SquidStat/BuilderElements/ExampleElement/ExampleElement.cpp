#include "ExampleElement.h"

#include <ExternalStructures.h>

#define BUILDER_ELEMENTS_USER_INPUTS
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"example-element"

#define START_VOLTAGE_OBJ_NAME	"start-voltage"
#define END_VOLTAGE_OBJ_NAME	"end-voltage"
#define VOLTAGE_STEP_OBJ_NAME	"voltage-step"
#define REPEATS_OBJ_NAME		"repeats"

#define START_VOLTAGE_DEFAULT	0
#define END_VOLTAGE_DEFAULT		1024
#define VOLTAGE_STEP_DEFAULT	1
#define REPEATS_DEFAULT			3

QString ExampleElement::GetFullName() const {
	return "Example Element";
}
QStringList ExampleElement::GetCategory() const {
	return QStringList() <<
		"Example Category" <<
		"Example Category 2";
}
QPixmap ExampleElement::GetImage() const {
	return QPixmap(":/GUI/Resources/element.png");
}
ExperimentType ExampleElement::GetType() const {
	return ET_DC;
}
QWidget* ExampleElement::CreateUserInput(UserInput &inputs) const {
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
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("dV/dt = ", row, 0);
	_INSERT_TEXT_INPUT(VOLTAGE_STEP_DEFAULT, VOLTAGE_STEP_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Repeats = ", row, 0);
	_INSERT_TEXT_INPUT(REPEATS_DEFAULT, REPEATS_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("", row, 2);

	++row;
	_INSERT_VERTICAL_SPACING(row);
	
	//*
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test radio 1", row, 0);
	_START_RADIO_BUTTON_GROUP("Test radio 1 id");
	_INSERT_RADIO_BUTTON("Ref", row, 1);
	_INSERT_RADIO_BUTTON("Open circuit", ++row, 1);
	_END_RADIO_BUTTON_GROUP();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test radio 2", row, 0);
	_START_RADIO_BUTTON_GROUP("Test radio 2 id");
	_INSERT_RADIO_BUTTON("Ref", row, 1);
	_INSERT_RADIO_BUTTON("Open circuit", ++row, 1);
	_END_RADIO_BUTTON_GROUP();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test drop down", row, 0);
	_START_DROP_DOWN("Test drop down id", row, 1);
	_ADD_DROP_DOWN_ITEM("Item 1");
	_ADD_DROP_DOWN_ITEM("Item 2");
	_ADD_DROP_DOWN_ITEM("Item 3");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test radio 3", row, 0);
	_START_RADIO_BUTTON_GROUP_HORIZONTAL_LAYOUT("Test radio layout id", row, 1);
	_INSERT_RADIO_BUTTON_LAYOUT("R1");
	_INSERT_RADIO_BUTTON_LAYOUT("R2");
	_END_RADIO_BUTTON_GROUP_LAYOUT();
	//*/

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(2, 1);

	USER_INPUT_END();
}
NodesData ExampleElement::GetNodesData(const UserInput &inputs, const CalibrationData&, const HardwareVersion&) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 100000;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	exp.samplingParams.ADCTimerDiv = 3;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSizeEven = 20;
	exp.samplingParams.ADCBufferSizeOdd = 20;
	exp.samplingParams.PointsIgnored = 0;
	exp.samplingParams.DACMultEven = 5;
	exp.samplingParams.DACMultOdd = 5;
	exp.DCSweep_pot.VStartUserInput = inputs[START_VOLTAGE_OBJ_NAME].toInt();
	exp.DCSweep_pot.VStartVsOCP = false; //todo: get user input here
	exp.DCSweep_pot.VEndUserInput = inputs[END_VOLTAGE_OBJ_NAME].toInt();
	exp.DCSweep_pot.VEndVsOCP = false; //todo: get user input here
	exp.DCSweep_pot.VStep = 1;
  exp.DCSweep_pot.Imax = 1e10;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}