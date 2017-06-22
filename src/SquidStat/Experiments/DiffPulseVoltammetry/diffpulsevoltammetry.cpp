#include "diffpulsevoltammetry.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Normal-Pulse-Voltammetry"

#define START_VOLTAGE_OBJ_NAME	"start-voltage"
#define START_V_VS_OCP_OBJ_NAME	"start-voltage-vs-ocp"
#define FINAL_VOLTAGE_OBJ_NAME	"final-voltage"
#define FINAL_V_VS_OCP_OBJ_NAME	"final-voltage-vs-ocp"
#define VOLTAGE_STEP_OBJ_NAME   "voltage-step"
#define PULSE_HEIGHT_OBJ_NAME	"pulse-height"
#define PULSE_WIDTH_OBJ_NAME	"pulse-width"
#define PULSE_PERIOD_OBJ_NAME	"pulse-period"

#define START_VOLTAGE_DEFAULT	-0.5
#define FINAL_VOLTAGE_DEFAULT	0.5
#define VOLTAGE_STEP_DEFAULT	0.01
#define PULSE_HEIGHT_DEFAULT	0.1
#define PULSE_WIDTH_DEFAULT		50
#define PULSE_PERIOD_DEFAULT	100

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Timestamp (normalized)"
#define PLOT_VAR_EWE					"Ewe"
#define PLOT_VAR_CURRENT				"Current"
#define PLOT_VAR_ECE					"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL		"Integral d(Current)/d(time)"

QString DiffPulseVoltammetry::GetShortName() const {
	return "Differential Pulse Voltammetry";
}
QString DiffPulseVoltammetry::GetFullName() const {
	return "Differential Pulse Voltammetry";
}
QString DiffPulseVoltammetry::GetDescription() const {
	return "In this experiment, the working electrode holds at a <b>starting potential</b> during the <b>quiet time</b>.  then applies a train of pulses, which increase in amplitude until the <b>final potential</b> is reached. The <b>voltage step</b> is the magnitude of this incremental increase. The <b>pulse width</b> is the amount of time between the rising and falling edge of a pulse. The <b>pulse period</b> is the amount of time between the beginning of one pulse and the beginning of the next.";
}
QStringList DiffPulseVoltammetry::GetCategory() const {
	return QStringList() <<
		"Basic voltammetry"<<
		"Pulse voltammetry";

}
QPixmap DiffPulseVoltammetry::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* DiffPulseVoltammetry::CreateUserInput() const {
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
	_INSERT_RIGHT_ALIGN_COMMENT("Voltage step = ", row, 0);
	_INSERT_TEXT_INPUT(VOLTAGE_STEP_DEFAULT, VOLTAGE_STEP_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Final potential = ", row, 0);
	_INSERT_TEXT_INPUT(FINAL_VOLTAGE_DEFAULT, FINAL_VOLTAGE_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
	_START_DROP_DOWN("Final potential reference selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Pulse width = ", row, 0);
	_INSERT_TEXT_INPUT(PULSE_WIDTH_DEFAULT, PULSE_WIDTH_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("milliseconds", row, 2);
	
	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Pulse period = ", row, 0);
	_INSERT_TEXT_INPUT(PULSE_PERIOD_DEFAULT, PULSE_PERIOD_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("milliseconds", row, 2);
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	USER_INPUT_END();
}
QByteArray DiffPulseVoltammetry::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
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
	double VStep;
	double VFinal;
	bool VFinalVsOCP;
	double restTime = 1;
	quint32 pulseWidth;
	quint32 pulsePeriod;
	GET_TEXT_INPUT_VALUE_DOUBLE(startVoltage, START_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(VStep, VOLTAGE_STEP_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(VFinal, FINAL_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(pulseWidth, PULSE_WIDTH_OBJ_NAME);
	GET_TEXT_INPUT_VALUE(pulsePeriod, PULSE_PERIOD_OBJ_NAME);
	if (pulseWidth >= pulsePeriod)
		pulsePeriod += pulseWidth;

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e7;
	exp.tMax = restTime * 1e8;
	exp.samplingParams.ADCBufferSizeEven = exp.samplingParams.ADCBufferSizeOdd = 20;
	exp.samplingParams.DACMultEven = exp.samplingParams.DACMultOdd = 20;
	exp.samplingParams.ADCTimerPeriod = 50 * 1e5;
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
	exp.nodeType = DCNODE_NORMALPULSE_POT;
	exp.tMin = 10 * 1e5;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	getSamplingParameters(pulsePeriod, pulseWidth, &exp);
	exp.DCPulseNormal_pot.VBaselineUserInput = (int)(startVoltage * 3276.8);
	exp.DCPulseNormal_pot.VBaselineVsOCP = false;
	exp.DCPulseNormal_pot.VEndUserInput = (int)(VFinal * 3276.8);
	exp.DCPulseNormal_pot.VEndVsOCP = false;
	exp.DCPulseNormal_pot.VStep = (float)(VStep * 3276.8);
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList DiffPulseVoltammetry::GetXAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_TIMESTAMP <<
		PLOT_VAR_TIMESTAMP_NORMALIZED <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT;
}
QStringList DiffPulseVoltammetry::GetYAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT <<
		PLOT_VAR_ECE <<
		PLOT_VAR_CURRENT_INTEGRAL;
}
void DiffPulseVoltammetry::PushNewData(const ExperimentalData &expData, DataMap &container, const CalibrationData&, const HardwareVersion &hwVersion) const {
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
void DiffPulseVoltammetry::SaveDataHeader(QFile &saveFile) const {
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

void DiffPulseVoltammetry::SaveData(QFile &saveFile, const DataMap &container) const {
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
void DiffPulseVoltammetry::getSamplingParameters(quint32 t_period, quint32 t_pulsewidth, ExperimentNode_t * pNode) const
{

	//TODO: make sure that this doesn't calculate an ADCMult or DACMult too big for the hardware buffers

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
	pNode->samplingParams.ADCTimerDiv = 0;
	pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd = 1;
	pNode->DCSweep_pot.VStep = 1;

	/* 1) Take the lesser of (period - pulsewidth) and pulsewidth */
	double t_pulse;
	bool isEvenPeriodShorter;
	if (t_period - t_pulsewidth < t_pulsewidth)
	{
		t_pulse = t_period - t_pulsewidth;
		isEvenPeriodShorter = true;
	}
	else
	{
		t_pulse = t_pulsewidth;
		isEvenPeriodShorter = false;
	}
	uint16_t bufMult = 1;

	/* 2) Minimize dt */
	uint32_t dt;
	do
	{
		dt = t_pulse * 1e5 / bufMult;
		if (dt / dt_min > 1)
		{
			bufMult <<= 1;
		}
	} while (dt / dt_min > 1);
	pNode->samplingParams.ADCTimerPeriod = dt;

	if (isEvenPeriodShorter)
	{
		pNode->samplingParams.DACMultEven = bufMult;
		pNode->samplingParams.DACMultOdd = pNode->samplingParams.DACMultEven * (t_pulsewidth / (t_period - t_pulsewidth));
	}
	else
	{
		pNode->samplingParams.DACMultOdd = bufMult;
		pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd * ((t_period - t_pulsewidth) / t_pulsewidth);
	}
	pNode->samplingParams.PointsIgnored = bufMult / 2;

	/* 3) If dt is too small, then adjust pulse width and period */
	//TODO

	

	/* 3) Calculate ADCMult */
	pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.DACMultEven;
	pNode->samplingParams.ADCBufferSizeOdd = pNode->samplingParams.DACMultOdd;
}