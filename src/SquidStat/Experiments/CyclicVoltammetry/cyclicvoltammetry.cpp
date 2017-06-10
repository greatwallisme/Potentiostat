#include "cyclicvoltammetry.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Cyclic-Voltammetry"

#define START_VOLTAGE_OBJ_NAME	"start-voltage"
#define START_V_VS_OCP_OBJ_NAME	"start-voltage-vs-ocp"
#define UPPER_VOLTAGE_OBJ_NAME	"upper-voltage"
#define UPPER_V_VS_OCP_OBJ_NAME	"upper-voltage-vs-ocp"
#define LOWER_VOLTAGE_OBJ_NAME  "lower-voltage"
#define LOWER_V_VS_OCP_OBJ_NAME	"lower-voltage-vs-ocp"
#define SCAN_RATE_OBJ_NAME		"scan-rate"
#define CYCLES_OBJ_NAME			"cycles"

#define START_VOLTAGE_DEFAULT	0
#define UPPER_VOLTAGE_DEFAULT	0.5
#define LOWER_VOLTAGE_DEFAULT	-0.5
#define SCAN_RATE_DEFAULT		1
#define CYCLES_DEFAULT			3


QString CyclicVoltammetry::GetShortName() const {
	return "Cyclic Voltammetry";
}
QString CyclicVoltammetry::GetFullName() const {
	return "Cyclic Voltammetry";
}
QString CyclicVoltammetry::GetDescription() const {
	return "This experiment sweeps the potential of the working electrode back and forth between <b>upper potential</b> and <b>lower potential</b> at a constant <b>scan rate dE/dT</b> for a specified number of <b>cycles</b>.";
}
QString CyclicVoltammetry::GetCategory() const {
	return "Basic voltammetry";
}
QPixmap CyclicVoltammetry::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* CyclicVoltammetry::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	/*
	_INSERT_RIGHT_ALIGN_COMMENT("Input mask ", row, 0);
	auto test = new QLineEdit();
	lay->addWidget(test, row, 1);
	test->setInputMask("00:00:00");

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Input mask + blank char ", row, 0);
	test = new QLineEdit();
	lay->addWidget(test, row, 1);
	test->setInputMask("00 \\d\\ays 00:00:00;_");

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("RegExp validator + placeholder ", row, 0);
	test = new QLineEdit();
	lay->addWidget(test, row, 1);
	test->setPlaceholderText("hh:mm:ss");
	test->setValidator(new QRegExpValidator(QRegExp("[0-9]{0,2}:[0-9]{0,2}:[0-9]{1,2}")));

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Int validator [0; 1024] ", row, 0);
	test = new QLineEdit();
	lay->addWidget(test, row, 1);
	test->setValidator(new QIntValidator(0, 1024));

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Double validator [-1.000; 1.000] ", row, 0);
	test = new QLineEdit();
	lay->addWidget(test, row, 1);
	auto validator = new QDoubleValidator(-1.0, 1.0, 3, test);
	validator->setNotation(QDoubleValidator::StandardNotation);
	test->setValidator(validator);

	++row;
	//*/
	_INSERT_RIGHT_ALIGN_COMMENT("Starting potential = ", row, 0);
	_INSERT_TEXT_INPUT(START_VOLTAGE_DEFAULT, START_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);
	_SET_ROW_STRETCH(row, 1);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to", row, 0);
	_START_DROP_DOWN("Starting potential reference selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();
	_SET_ROW_STRETCH(row, 2);

	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT(" ", row, 0);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Upper scan limit = ", row, 0);
	_INSERT_TEXT_INPUT(UPPER_VOLTAGE_DEFAULT, UPPER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);
	_SET_ROW_STRETCH(row, 1);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
	_START_DROP_DOWN("Upper potential reference selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();
	_SET_ROW_STRETCH(row, 2);

	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT(" ", row, 0);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Lower scan limit = ", row, 0);
	_INSERT_TEXT_INPUT(LOWER_VOLTAGE_DEFAULT, LOWER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);
	_SET_ROW_STRETCH(row, 1);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
	_START_DROP_DOWN("Lower potential reference selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();
	_SET_ROW_STRETCH(row, 2);

	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT(" ", row, 0);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Scan rate (dE/dt) = ", row, 0);
	_INSERT_TEXT_INPUT(SCAN_RATE_DEFAULT, SCAN_RATE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mV/s", row, 2);
	_SET_ROW_STRETCH(row, 1);
	
	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT(" ", row, 0);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Repeats = ", row, 0);
	_INSERT_TEXT_INPUT(CYCLES_DEFAULT, CYCLES_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("", row, 2);
	_SET_ROW_STRETCH(row, 1);
	/*
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
	//*/
	//_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	USER_INPUT_END();
}
QByteArray CyclicVoltammetry::GetNodesData(QWidget *wdg, const CalibrationData &calData) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);
	/*
	QString selectedRadio1;
	QString selectedRadio2;
	GET_SELECTED_RADIO(selectedRadio1, "Test radio 1 id");
	GET_SELECTED_RADIO(selectedRadio2, "Test radio 2 id");


	QString selectedDropDown;
	GET_SELECTED_DROP_DOWN(selectedDropDown, "Test drop down id");
	//*/

	double startVoltage;
	bool startVoltageVsOCP;
	double upperVoltage;
	bool upperVoltageVsOCP;
	double lowerVoltage;
	bool lowerVoltageVsOCP;
	double dEdt;
	qint32 cycles;
	GET_TEXT_INPUT_VALUE(startVoltage, START_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(upperVoltage, UPPER_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(lowerVoltage, LOWER_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(dEdt, SCAN_RATE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(cycles, CYCLES_OBJ_NAME);

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT;
	exp.tMin = 1e7;
	exp.tMax = LONG_MAX;
	exp.samplingParams.ADCTimerDiv = 2;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSize = 20;
	exp.samplingParams.DACMultiplier = 20;
	exp.DCPoint.VPoint = startVoltage;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP;
	exp.tMin = 100000;
	exp.tMax = LONG_MAX;
	exp.samplingParams.ADCTimerDiv = 2;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSize = 20;
	exp.samplingParams.DACMultiplier = 20;
	exp.DCSweep.VStart = startVoltage;
	exp.DCSweep.VEnd = upperVoltage;
	exp.DCSweep.dVdt = 1;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = true;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP;
	exp.tMin = 100000;
	exp.tMax = LONG_MAX;
	exp.samplingParams.ADCTimerDiv = 2;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSize = 20;
	exp.samplingParams.DACMultiplier = 20;
	exp.DCSweep.VStart = upperVoltage;
	exp.DCSweep.VEnd = lowerVoltage;
	exp.DCSweep.dVdt = -1;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = true;
	exp.nodeType = DCNODE_SWEEP;
	exp.tMin = 100000;
	exp.tMax = LONG_MAX;
	exp.samplingParams.ADCTimerDiv = 2;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSize = 20;
	exp.samplingParams.DACMultiplier = 20;
	exp.DCSweep.VStart = lowerVoltage;
	exp.DCSweep.VEnd = upperVoltage;
	exp.DCSweep.dVdt = 1;
	exp.MaxPlays = cycles;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}