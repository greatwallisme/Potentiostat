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

#define PLOT_VAR_TIMESTAMP			"Timestamp"
#define PLOT_VAR_EWE				"Ewe"
#define PLOT_VAR_CURRENT			"Current"
#define PLOT_VAR_ECE				"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL	"Integral d(Current)/d(time)"

QString ExampleExperiment::GetShortName() const {
	return "Example Experiment";
}
QString ExampleExperiment::GetFullName() const {
	return "Linear Sweep Voltammetry";
}
QString ExampleExperiment::GetDescription() const {
	return "This experiment sweeps the <b>potential</b> of the working electrode from E1 to E2 at constant scan rate dE/dT";
}
QStringList ExampleExperiment::GetCategory() const {
	return QStringList() <<
		"Example Category" <<
		"Example Category 2";
}
QPixmap ExampleExperiment::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* ExampleExperiment::CreateUserInput() const {
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
		_INSERT_RADIO_BUTTON("Open circuit", row, 2);
	_END_RADIO_BUTTON_GROUP();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Test radio 2", row, 0);
	_START_RADIO_BUTTON_GROUP("Test radio 2 id");
		_INSERT_RADIO_BUTTON("Ref", row, 1);
		_INSERT_RADIO_BUTTON("Open circuit", row, 2);
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
	_INSERT_RIGHT_ALIGN_COMMENT("Test radio layout", row, 0);
	_START_RADIO_BUTTON_GROUP_HORIZONTAL_LAYOUT("Test radio layout id", row, 1);
		_INSERT_RADIO_BUTTON_LAYOUT("Ref");
		_INSERT_RADIO_BUTTON_LAYOUT("Open circuit");
	_END_RADIO_BUTTON_GROUP_LAYOUT();
	//*/
	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(3, 1);

	USER_INPUT_END();
}
QByteArray ExampleExperiment::GetNodesData(QWidget *wdg, const CalibrationData &calData) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);
	//*
	QString selectedRadio1;
	QString selectedRadio2;
	GET_SELECTED_RADIO(selectedRadio1, "Test radio 1 id");
	GET_SELECTED_RADIO(selectedRadio2, "Test radio 2 id");


	QString selectedDropDown;
	GET_SELECTED_DROP_DOWN(selectedDropDown, "Test drop down id");
	//*/

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

QStringList ExampleExperiment::GetXAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_TIMESTAMP <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT;
}
QStringList ExampleExperiment::GetYAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT << 
		PLOT_VAR_ECE <<
		PLOT_VAR_CURRENT_INTEGRAL;
}
void ExampleExperiment::PushNewData(const ExperimentalData &expData, DataMap &container) const {
	qreal timestamp = (qreal)expData.timestamp / 100000000UL;

	if (container[PLOT_VAR_CURRENT_INTEGRAL].isEmpty()) {
		container[PLOT_VAR_CURRENT_INTEGRAL].append(expData.adcData.current / timestamp);
	}
	else {
		qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].last();
		newVal += (container[PLOT_VAR_CURRENT].last() + expData.adcData.current) * (timestamp + container[PLOT_VAR_TIMESTAMP].last()) / 2.;
		container[PLOT_VAR_CURRENT_INTEGRAL].append(newVal);
	}

	container[PLOT_VAR_TIMESTAMP].append(timestamp);
	container[PLOT_VAR_EWE].append(expData.adcData.ewe);
	container[PLOT_VAR_ECE].append(expData.adcData.ece);
	container[PLOT_VAR_CURRENT].append(expData.adcData.current);
}
void ExampleExperiment::SaveDataHeader(QFile &saveFile) const {
	saveFile.write(QString("%1;%2;%3;%4;%5\n")
		.arg(PLOT_VAR_TIMESTAMP)
		.arg(PLOT_VAR_EWE)
		.arg(PLOT_VAR_CURRENT)
		.arg(PLOT_VAR_ECE)
		.arg(PLOT_VAR_CURRENT_INTEGRAL).toLatin1());
}
void ExampleExperiment::SaveData(QFile &saveFile, const DataMap &container) const {
	saveFile.write(QString("%1;%2;%3;%4;%5\n")
		.arg(container[PLOT_VAR_TIMESTAMP].last())
		.arg(container[PLOT_VAR_EWE].last())
		.arg(container[PLOT_VAR_CURRENT].last())
		.arg(container[PLOT_VAR_ECE].last())
		.arg(container[PLOT_VAR_CURRENT_INTEGRAL].last()).toLatin1());
	saveFile.flush();
}