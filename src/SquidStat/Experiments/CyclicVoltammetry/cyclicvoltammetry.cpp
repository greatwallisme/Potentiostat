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

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Timestamp (normalized)"
#define PLOT_VAR_EWE					"Ewe"
#define PLOT_VAR_CURRENT				"Current"
#define PLOT_VAR_ECE					"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL		"Integral d(Current)/d(time)"

QString CyclicVoltammetry::GetShortName() const {
	return "Basic Cyclic Voltammetry";
}
QString CyclicVoltammetry::GetFullName() const {
	return "Basic Cyclic Voltammetry";
}
QString CyclicVoltammetry::GetDescription() const {
	return "This experiment sweeps the potential of the working electrode back and forth between <b>upper potential</b> and <b>lower potential</b> at a constant <b>scan rate dE/dT</b> for a specified number of <b>cycles</b>.";
}
QStringList CyclicVoltammetry::GetCategory() const {
	return QStringList() <<
		"Basic voltammetry";

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
	exp.tMax = 2e8;
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
	exp.tMin = 1e7;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	getSlewParameters(dEdt/1000, &exp);
	exp.DCSweep_pot.VStartUserInput = (int)(startVoltage * 3276.8);
	exp.DCSweep_pot.VStartVsOCP = false;
	exp.DCSweep_pot.VEndUserInput = (int)(upperVoltage * 3276.8);
	exp.DCSweep_pot.VEndVsOCP = false;
	exp.DCSweep_pot.Imax = 32767;
	exp.DCSweep_pot.IRangeMax = RANGE0;
	exp.DCSweep_pot.Imin = 0;
	exp.DCSweep_pot.IRangeMin = RANGE7;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = true;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 1e7;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	getSlewParameters(dEdt / 1000, &exp);
	exp.DCSweep_pot.VStartUserInput = (int)(upperVoltage * 3276.8);
	exp.DCSweep_pot.VStartVsOCP = false;
	exp.DCSweep_pot.VEndUserInput = (int)(lowerVoltage * 3276.8);
	exp.DCSweep_pot.VEndVsOCP = false;
	exp.DCSweep_pot.Imax = 32767;
	exp.DCSweep_pot.IRangeMax = RANGE0;
	exp.DCSweep_pot.Imin = 0;
	exp.DCSweep_pot.IRangeMin = RANGE7;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = true;
	exp.branchHeadIndex = 2;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 1e7;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	getSlewParameters(dEdt / 1000, &exp);
	exp.DCSweep_pot.VStartUserInput = (int)(lowerVoltage * 3276.8);
	exp.DCSweep_pot.VStartVsOCP = false;
	exp.DCSweep_pot.VEndUserInput = (int)(upperVoltage * 3276.8);
	exp.DCSweep_pot.VEndVsOCP = false;
	exp.DCSweep_pot.Imax = 32767;
	exp.DCSweep_pot.IRangeMax = RANGE0;
	exp.DCSweep_pot.Imin = 0;
	exp.DCSweep_pot.IRangeMin = RANGE7;
	exp.MaxPlays = cycles;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList CyclicVoltammetry::GetXAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_TIMESTAMP <<
		PLOT_VAR_TIMESTAMP_NORMALIZED <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT;
}
QStringList CyclicVoltammetry::GetYAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT <<
		PLOT_VAR_ECE <<
		PLOT_VAR_CURRENT_INTEGRAL;
}
void CyclicVoltammetry::PushNewData(const ExperimentalData &expData, DataMap &container, const CalibrationData&) const {
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / 100000000UL;

	if (container[PLOT_VAR_CURRENT_INTEGRAL].isEmpty()) {
		container[PLOT_VAR_CURRENT_INTEGRAL].append(expData.ADCrawData.current / timestamp);
	}
	else {
		qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].last();
		newVal += (container[PLOT_VAR_CURRENT].last() + expData.ADCrawData.current) * (timestamp + container[PLOT_VAR_TIMESTAMP].last()) / 2.;
		container[PLOT_VAR_CURRENT_INTEGRAL].append(newVal);
	}

	container[PLOT_VAR_TIMESTAMP].append(timestamp);
	container[PLOT_VAR_EWE].append(expData.ADCrawData.ewe);
	container[PLOT_VAR_ECE].append(expData.ADCrawData.ece);
	container[PLOT_VAR_CURRENT].append(expData.ADCrawData.current);

	if (!timestampOffset.contains(&container)) {
		timestampOffset[&container] = timestamp;
	}
	container[PLOT_VAR_TIMESTAMP_NORMALIZED].append(timestamp - timestampOffset[&container]);
}
void CyclicVoltammetry::SaveDataHeader(QFile &saveFile) const {
	QString toWrite;
	toWrite += QString("\"%1\";").arg(QString(PLOT_VAR_TIMESTAMP).replace("\"", "\"\""));
	toWrite += QString("\"%1\";").arg(QString(PLOT_VAR_TIMESTAMP_NORMALIZED).replace("\"", "\"\""));
	toWrite += QString("\"%1\";").arg(QString(PLOT_VAR_EWE).replace("\"", "\"\""));
	toWrite += QString("\"%1\";").arg(QString(PLOT_VAR_CURRENT).replace("\"", "\"\""));
	toWrite += QString("\"%1\";").arg(QString(PLOT_VAR_ECE).replace("\"", "\"\""));
	toWrite += QString("\"%1\"\n").arg(QString(PLOT_VAR_CURRENT_INTEGRAL).replace("\"", "\"\""));

	saveFile.write(toWrite.toLatin1());
	saveFile.flush();


	QString str;
	toWrite.clear();
	str = GetXAxisParameters().contains(PLOT_VAR_TIMESTAMP) ? "X" : "";
	str += GetYAxisParameters().contains(PLOT_VAR_TIMESTAMP) ? "Y" : "";
	toWrite += QString("\"%1\";").arg(str);
	str = GetXAxisParameters().contains(PLOT_VAR_TIMESTAMP_NORMALIZED) ? "X" : "";
	str += GetYAxisParameters().contains(PLOT_VAR_TIMESTAMP_NORMALIZED) ? "Y" : "";
	toWrite += QString("\"%1\";").arg(str);
	str = GetXAxisParameters().contains(PLOT_VAR_EWE) ? "X" : "";
	str += GetYAxisParameters().contains(PLOT_VAR_EWE) ? "Y" : "";
	toWrite += QString("\"%1\";").arg(str);
	str = GetXAxisParameters().contains(PLOT_VAR_CURRENT) ? "X" : "";
	str += GetYAxisParameters().contains(PLOT_VAR_CURRENT) ? "Y" : "";
	toWrite += QString("\"%1\";").arg(str);
	str = GetXAxisParameters().contains(PLOT_VAR_ECE) ? "X" : "";
	str += GetYAxisParameters().contains(PLOT_VAR_ECE) ? "Y" : "";
	toWrite += QString("\"%1\";").arg(str);
	str = GetXAxisParameters().contains(PLOT_VAR_CURRENT_INTEGRAL) ? "X" : "";
	str += GetYAxisParameters().contains(PLOT_VAR_CURRENT_INTEGRAL) ? "Y" : "";
	toWrite += QString("\"%1\"\n").arg(str);

	saveFile.write(toWrite.toLatin1());
	saveFile.flush();
}

void CyclicVoltammetry::SaveData(QFile &saveFile, const DataMap &container) const {
	static QChar decimalPoint = QLocale().decimalPoint();

	QString toWrite;
	toWrite += QString("%1;").arg(container[PLOT_VAR_TIMESTAMP].last(), 0, 'e').replace(QChar('.'), decimalPoint);
	toWrite += QString("%1;").arg(container[PLOT_VAR_TIMESTAMP_NORMALIZED].last(), 0, 'e').replace(QChar('.'), decimalPoint);
	toWrite += QString("%1;").arg(container[PLOT_VAR_EWE].last(), 0, 'e').replace(QChar('.'), decimalPoint);
	toWrite += QString("%1;").arg(container[PLOT_VAR_CURRENT].last(), 0, 'e').replace(QChar('.'), decimalPoint);
	toWrite += QString("%1;").arg(container[PLOT_VAR_ECE].last(), 0, 'e').replace(QChar('.'), decimalPoint);
	toWrite += QString("%1\n").arg(container[PLOT_VAR_CURRENT_INTEGRAL].last(), 0, 'e').replace(QChar('.'), decimalPoint);

	saveFile.write(toWrite.toLatin1());
	saveFile.flush();
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

	pNode->samplingParams.ADCTimerDiv = 0;
	int timerDiv = 1;
	pNode->samplingParams.ADCTimerPeriod = dt;
	while (pNode->samplingParams.ADCTimerPeriod > 2147483648)
	{
		pNode->samplingParams.ADCTimerDiv++;
		timerDiv <<= 1;
		pNode->samplingParams.ADCTimerPeriod = dt / timerDiv;
	} 
}