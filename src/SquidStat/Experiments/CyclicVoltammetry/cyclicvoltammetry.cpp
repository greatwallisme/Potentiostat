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
	return "Basic Cyclic Voltammetry";
}
QString CyclicVoltammetry::GetFullName() const {
	return "Basic Cyclic Voltammetry";
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
	_INSERT_RIGHT_ALIGN_COMMENT("Starting potential = ", row, 0);
	_INSERT_TEXT_INPUT(START_VOLTAGE_DEFAULT, START_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to", row, 0);
	_START_DROP_DOWN("Starting potential reference selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Upper scan limit = ", row, 0);
	_INSERT_TEXT_INPUT(UPPER_VOLTAGE_DEFAULT, UPPER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
	_START_DROP_DOWN("Upper potential reference selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Lower scan limit = ", row, 0);
	_INSERT_TEXT_INPUT(LOWER_VOLTAGE_DEFAULT, LOWER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
	_START_DROP_DOWN("Lower potential reference selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Scan rate (dE/dt) = ", row, 0);
	_INSERT_TEXT_INPUT(SCAN_RATE_DEFAULT, SCAN_RATE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mV/s", row, 2);
	
	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Repeats = ", row, 0);
	_INSERT_TEXT_INPUT(CYCLES_DEFAULT, CYCLES_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("", row, 2);
	
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
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e7;
	exp.tMax = 1e8;
	exp.samplingParams.ADCTimerDiv = 2;
	exp.samplingParams.ADCTimerPeriod = 200000;
	exp.samplingParams.ADCBufferSizeEven = 20;
	exp.samplingParams.ADCBufferSizeOdd = 20;
	exp.samplingParams.DACMultEven = 20;
	exp.samplingParams.DACMultOdd = 20;
	exp.samplingParams.PointsIgnored = 0;
	exp.DCPoint_pot.VPointUserInput = (int)(startVoltage * 3276.8);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.DCPoint_pot.Imax = 32767;
	exp.DCPoint_pot.IrangeMax = RANGE0;
	exp.DCPoint_pot.Imin = 0;
	exp.DCPoint_pot.IrangeMin = RANGE7;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 100000;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	getSlewParameters(dEdt/1000, &exp);
	exp.DCSweep_pot.VStartUserInput = (int)(startVoltage * 3276.8);
	exp.DCSweep_pot.VStartVsOCP = false;
	exp.DCSweep_pot.VEndUserInput = (int)(upperVoltage * 3276.8);
	exp.DCSweep_pot.VEndVsOCP = false;
	exp.DCSweep_pot.VStep = 1;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = true;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 100000;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	exp.DCSweep_pot.VStartUserInput = (int)(upperVoltage * 3276.8);
	exp.DCSweep_pot.VStartVsOCP = false;
	exp.DCSweep_pot.VEndUserInput = (int)(lowerVoltage * 3276.8);
	exp.DCSweep_pot.VEndVsOCP = false;
	exp.DCSweep_pot.VStep = 1;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = true;
	exp.branchHeadIndex = 2;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 100000;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	exp.DCSweep_pot.VStartUserInput = (int)(lowerVoltage * 3276.8);
	exp.DCSweep_pot.VStartVsOCP = false;
	exp.DCSweep_pot.VEndUserInput = (int)(upperVoltage * 3276.8);
	exp.DCSweep_pot.VEndVsOCP = false;
	exp.DCSweep_pot.VStep = 1;
	exp.MaxPlays = cycles;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

void CyclicVoltammetry::getSlewParameters(double dVdt, ExperimentNode_t * pNode) const
{
	/* This switch-case is a placeholder for calculating dt_min, which needs to be defined elsewhere*/
	int dt_min = 1;
	int HardwareVersion = 0;
	switch (HardwareVersion)
	{
		case 0:
			dt_min = 50000; //500 microseconds * 100 ticks/microsecond
			break;
		case 1:
			dt_min = 500;	//5 microseconds * 100 ticks/microsecond
			break;
		default:
			break;
	}
	pNode->samplingParams.ADCTimerDiv = 1;
	pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd = 1;

	/* 1) Minimize dt, maximize DACMult*/
	uint32_t dt;
	do
	{
		dt = (uint32_t)(1 / dVdt * 1e8 / 3276.8 / pNode->samplingParams.DACMultEven);		//3276.8 is a placeholder for cal->m_DAC,V
		if (dt / dt_min > 1)
		{
			pNode->samplingParams.DACMultEven <<= 1;
			pNode->samplingParams.DACMultOdd <<= 1;
		}
	} while (dt / dt_min > 1);

	/* 2) Increase VStep, if necessary */
	while (dt < dt_min)
	{
		pNode->DCSweep_pot.VStep++;
		dt = (uint32_t)(1 / dVdt * 1e8 / 3276.8 / pNode->samplingParams.DACMultEven * pNode->DCSweep_pot.VStep);
	}

	/* 3) Calculate ADCMult */
	pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.ADCBufferSizeOdd = pNode->samplingParams.DACMultEven;
	while (pNode->samplingParams.ADCBufferSizeEven * dt > 1e8)
	{
		pNode->samplingParams.ADCBufferSizeEven >>= 1;
		pNode->samplingParams.ADCBufferSizeOdd >>= 1;
	}
	if (pNode->samplingParams.ADCBufferSizeEven == pNode->samplingParams.DACMultEven)
		pNode->samplingParams.PointsIgnored = pNode->samplingParams.ADCBufferSizeEven / 2;

	pNode->samplingParams.ADCTimerDiv = 1;
	int timerDiv = 1;
	pNode->samplingParams.ADCTimerPeriod = dt;
	while (pNode->samplingParams.ADCTimerPeriod > 2147483648)
	{
		pNode->samplingParams.ADCTimerDiv++;
		timerDiv <<= 1;
		pNode->samplingParams.ADCTimerPeriod = dt / timerDiv;
	} 
}