#include "ChargeDischargeDC.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Charge-Discharge-DC"

#define CHG_CURRENT_OBJ_NAME	"charge-current"
#define DISCHG_CURRENT_OBJ_NAME	"discharge-current"
#define MIN_CHG_CURRENT_OBJ_NAME	"min-charge-current"
#define MIN_DISCHG_CURRENT_OBJ_NAME	"min-discharge-current"
#define UPPER_VOLTAGE_OBJ_NAME	"upper-voltage"
#define LOWER_VOLTAGE_OBJ_NAME  "lower-voltage"
#define CYCLES_OBJ_NAME			"cycles"

#define CHG_CURRENT_DEFAULT		100		//(in mA)
#define DISCHG_CURRENT_DEFAULT	-100	//(in mA)
#define MIN_CHG_CURRENT_DEFAULT	0.5
#define MIN_DISCHG_CURRENT_DEFAULT -0.5
#define UPPER_VOLTAGE_DEFAULT	3.6
#define LOWER_VOLTAGE_DEFAULT	2.7
#define CYCLES_DEFAULT			10

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Timestamp (normalized)"
#define PLOT_VAR_EWE					"Ewe"
#define PLOT_VAR_CURRENT				"Current"
#define PLOT_VAR_ECE					"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL		"Integral d(Current)/d(time)"

QString ChargeDischargeDC::GetShortName() const {
	return "Charge/Discharge";
}
QString ChargeDischargeDC::GetFullName() const {
	return "Charge/Discharge (DC only)";
}
QString ChargeDischargeDC::GetDescription() const {
	return "This experiment repeatedly charges and discharges the cell under test. For each charge and discharge phase, the cell undergoes a <b>constant current</b> segment and then a <b>constant voltage</b> segment.";
}
QStringList ChargeDischargeDC::GetCategory() const {
	return QStringList() <<
		"Energy storage";

}
QPixmap ChargeDischargeDC::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* ChargeDischargeDC::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Starting phase", row, 0);
	_START_DROP_DOWN("Starting phase selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("Charge first");
	_ADD_DROP_DOWN_ITEM("Discharge first");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);
	
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Charging current = ", row, 0);
	_INSERT_TEXT_INPUT(CHG_CURRENT_DEFAULT, CHG_CURRENT_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mA", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Upper voltage limit = ", row, 0);
	_INSERT_TEXT_INPUT(UPPER_VOLTAGE_DEFAULT, UPPER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Minimum charging current = ", row, 0);
	_INSERT_TEXT_INPUT(MIN_CHG_CURRENT_DEFAULT, MIN_CHG_CURRENT_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mA", row, 2);

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Discharging current = ", row, 0);
	_INSERT_TEXT_INPUT(DISCHG_CURRENT_DEFAULT, DISCHG_CURRENT_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mA", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Lower voltage limit = ", row, 0);
	_INSERT_TEXT_INPUT(LOWER_VOLTAGE_DEFAULT, LOWER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Minimum discharging current = ", row, 0);
	_INSERT_TEXT_INPUT(MIN_DISCHG_CURRENT_DEFAULT, MIN_DISCHG_CURRENT_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mA", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Upper scan limit = ", row, 0);
	_INSERT_TEXT_INPUT(UPPER_VOLTAGE_DEFAULT, UPPER_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Cycles = ", row, 0);
	_INSERT_TEXT_INPUT(CYCLES_DEFAULT, CYCLES_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("", row, 2);
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	USER_INPUT_END();
}
QByteArray ChargeDischargeDC::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);
	/*
	QString selectedRadio1;
	QString selectedRadio2;
	GET_SELECTED_RADIO(selectedRadio1, "Test radio 1 id");
	GET_SELECTED_RADIO(selectedRadio2, "Test radio 2 id");


	QString selectedDropDown;
	GET_SELECTED_DROP_DOWN(selectedDropDown, "Test drop down id");
	//*/

	//TODO: what to do about cells hooked up backwards?
	//TODO: incorporate max charge/discharge capacity? Or incorporate this into another experiment?

	bool chargeFirst;
	double upperVoltage;
	double lowerVoltage;
	double chgCurrent;
	double minChgCurrent;
	double dischgCurrent;
	double minDischgCurrent;
	qint32 cycles;
	QString firstPhase;
	GET_SELECTED_DROP_DOWN(firstPhase, "Starting phase selection id");
	GET_TEXT_INPUT_VALUE_DOUBLE(upperVoltage, UPPER_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(lowerVoltage, LOWER_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(chgCurrent, CHG_CURRENT_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(dischgCurrent, DISCHG_CURRENT_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(minChgCurrent, MIN_CHG_CURRENT_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(minDischgCurrent, MIN_DISCHG_CURRENT_OBJ_NAME);
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
	//exp.DCPoint_pot.VPointUserInput = (int)(startVoltage * 3276.8);
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
	//getSlewParameters(dEdt/1000, &exp);
	//exp.DCSweep_pot.VStartUserInput = (int)(startVoltage * 3276.8);
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
//	getSlewParameters(dEdt / 1000, &exp);
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
//	getSlewParameters(dEdt / 1000, &exp);
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

QStringList ChargeDischargeDC::GetXAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_TIMESTAMP <<
		PLOT_VAR_TIMESTAMP_NORMALIZED <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT;
}
QStringList ChargeDischargeDC::GetYAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT <<
		PLOT_VAR_ECE <<
		PLOT_VAR_CURRENT_INTEGRAL;
}
void ChargeDischargeDC::PushNewData(const ExperimentalData &expData, DataMap &container, const CalibrationData&, const HardwareVersion &hwVersion) const {
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
void ChargeDischargeDC::SaveDataHeader(QFile &saveFile) const {
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

void ChargeDischargeDC::SaveData(QFile &saveFile, const DataMap &container) const {
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
void ChargeDischargeDC::getSlewParameters(double dVdt, ExperimentNode_t * pNode) const
{
	
	//TODO: make sure that ADCMult and DACMult aren't too big for hardware buffers


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
	pNode->DCSweep_pot.VStep = 1;

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