#include "normalpulsevoltammetry.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Normal-Pulse-Voltammetry"

#define START_VOLTAGE_OBJ_NAME	"start-voltage"
#define START_V_VS_OCP_OBJ_NAME	"start-voltage-vs-ocp"
#define FINAL_VOLTAGE_OBJ_NAME	"final-voltage"
#define FINAL_V_VS_OCP_OBJ_NAME	"final-voltage-vs-ocp"
#define VOLTAGE_STEP_OBJ_NAME   "voltage-step"
#define PULSE_WIDTH_OBJ_NAME	"pulse-width"
#define PULSE_PERIOD_OBJ_NAME	"pulse-period"

#define START_VOLTAGE_DEFAULT	-0.5
#define FINAL_VOLTAGE_DEFAULT	0.5
#define VOLTAGE_STEP_DEFAULT	0.01
#define PULSE_WIDTH_DEFAULT		50
#define PULSE_PERIOD_DEFAULT	100

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Timestamp (normalized)"
#define PLOT_VAR_EWE					"Ewe"
#define PLOT_VAR_CURRENT				"Current"
#define PLOT_VAR_ECE					"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL		"Integral d(Current)/d(time)"

QString NormalPulseVoltammetry::GetShortName() const {
	return "Normal Pulse Voltammetry";
}
QString NormalPulseVoltammetry::GetFullName() const {
	return "Normal Pulse Voltammetry";
}
QString NormalPulseVoltammetry::GetDescription() const {
	return "This experiment holds the working electrode at a <b>baseline potential</b> during the <b>quiet time</b>, then applies a train of pulses, which increase in amplitude until the <b>final potential</b> is reached. The <b>voltage step</b> is the magnitude of this incremental increase. The <b>pulse width</b> is the amount of time between the rising and falling edge of a pulse. The <b>pulse period</b> is the amount of time between the beginning of one pulse and the beginning of the next.";
}
QStringList NormalPulseVoltammetry::GetCategory() const {
	return QStringList() <<
		"Basic voltammetry"<<
		"Pulse voltammetry";

}
ExperimentTypeList NormalPulseVoltammetry::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap NormalPulseVoltammetry::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* NormalPulseVoltammetry::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Baseline potential = ", row, 0);
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
NodesData NormalPulseVoltammetry::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
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

QStringList NormalPulseVoltammetry::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			PLOT_VAR_TIMESTAMP <<
			PLOT_VAR_TIMESTAMP_NORMALIZED <<
			PLOT_VAR_EWE <<
			PLOT_VAR_CURRENT;
	}

	return ret;
}
QStringList NormalPulseVoltammetry::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
		PLOT_VAR_EWE <<
		PLOT_VAR_CURRENT <<
		PLOT_VAR_ECE <<
		PLOT_VAR_CURRENT_INTEGRAL;
	}

	return ret;
}
void NormalPulseVoltammetry::PushNewDcData(const ExperimentalDcData &expData, DataMap &container, const CalibrationData&, const HardwareVersion &hwVersion) const {
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / 100000000UL;

	if (container[PLOT_VAR_CURRENT_INTEGRAL].data.isEmpty()) {
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, expData.ADCrawData.current / timestamp);
	}
	else {
		qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].data.last();
		newVal += (container[PLOT_VAR_CURRENT].data.last() + expData.ADCrawData.current) * (timestamp + container[PLOT_VAR_TIMESTAMP].data.last()) / 2.;
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, newVal);
	}

	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP, timestamp);
	PUSH_BACK_DATA(PLOT_VAR_EWE, expData.ADCrawData.ewe);
	PUSH_BACK_DATA(PLOT_VAR_ECE, expData.ADCrawData.ece);
	PUSH_BACK_DATA(PLOT_VAR_CURRENT, expData.ADCrawData.current);

	if (!timestampOffset.contains(&container)) {
		timestampOffset[&container] = timestamp;
	}
	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED, timestamp - timestampOffset[&container]);
}
void NormalPulseVoltammetry::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}

void NormalPulseVoltammetry::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_TIMESTAMP);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_END();
}
void NormalPulseVoltammetry::getSamplingParameters(quint32 t_period, quint32 t_pulsewidth, ExperimentNode_t * pNode) const
{
	//debugging: this algorithm doesn't work...
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
		pNode->samplingParams.DACMultOdd = pNode->samplingParams.DACMultEven * ((double)t_pulsewidth / (t_period - t_pulsewidth));
	}
	else
	{
		pNode->samplingParams.DACMultOdd = bufMult;
		pNode->samplingParams.DACMultEven = pNode->samplingParams.DACMultOdd * ((double)(t_period - t_pulsewidth) / t_pulsewidth);
	}
	pNode->samplingParams.PointsIgnored = bufMult / 2;

	/* 3) If dt is too small, then adjust pulse width and period */
	//TODO

	

	/* 3) Calculate ADCMult */
	pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.DACMultEven;
	pNode->samplingParams.ADCBufferSizeOdd = pNode->samplingParams.DACMultOdd;
}