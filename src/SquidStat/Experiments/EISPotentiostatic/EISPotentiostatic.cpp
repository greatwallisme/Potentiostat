#include "EISPotentiostatic.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"EIS-Potentiostatic"

#define UPPER_FREQ_OBJ_NAME		"upper-frequency"
#define LOWER_FREQ_OBJ_NAME		"lower-frequency"
#define STEPS_PER_DEC_OBJ_NAME	"step-per-decade"
#define DC_BIAS_OBJ_NAME		"DC-bias"
#define AC_AMP_OBJ_NAME			"AC-amplitude"

#define UPPER_FREQ_DEFAULT		1000000		//(in Hz)
#define LOWER_FREQ_DEFAULT		0.001	//(in Hz)
#define STEPS_PER_DEC_DEFAULT	5
#define DC_BIAS_DEFAULT			0
#define AC_AMP_DEFAULT			10		//(in mA)


#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Timestamp (normalized)"
#define PLOT_VAR_EWE					"Ewe"
#define PLOT_VAR_CURRENT				"Current"
#define PLOT_VAR_ECE					"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL		"Integral d(Current)/d(time)"

#define PLOT_VAR_IMPEDANCE				"|Z|"
#define PLOT_VAR_PHASE					"Phase"
#define PLOT_VAR_IMP_REAL				"Z\'"
#define PLOT_VAR_IMP_IMAG				"Z\""
#define PLOT_VAR_NEG_IMP_IMAG			"-Z\""
#define PLOT_VAR_FREQ					"Frequency"

QString EISPotentiostatic::GetShortName() const {
	return "EIS (Potentiostatic)";
}
QString EISPotentiostatic::GetFullName() const {
	return "EIS (Potentiostatic)";
}
QString EISPotentiostatic::GetDescription() const {
	return "This experiment records the complex impedance of the experimental cell in potentiostatic mode, starting from the <b>upper frequency</b> and sweeping downwards through the <b>lower frequency</b>, with a fixed number of frequency <b>steps per decade</b>. Important parameters include the <b>DC bias</b> and the <b>AC exitation amplitude</b>.";
}
QStringList EISPotentiostatic::GetCategory() const {
	return QStringList() <<
		"Impedance spectroscopy";

}
ExperimentTypeList EISPotentiostatic::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap EISPotentiostatic::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* EISPotentiostatic::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Upper frequency", row, 0);
	_INSERT_TEXT_INPUT(UPPER_FREQ_DEFAULT, UPPER_FREQ_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("Hz", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Lower frequency", row, 0);
	_INSERT_TEXT_INPUT(LOWER_FREQ_DEFAULT, LOWER_FREQ_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("Hz", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Steps per decade", row, 0);
	_INSERT_TEXT_INPUT(STEPS_PER_DEC_DEFAULT, STEPS_PER_DEC_OBJ_NAME, row, 1);

	++row;
	_INSERT_VERTICAL_SPACING(row);
	
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("DC bias = ", row, 0);
	_INSERT_TEXT_INPUT(DC_BIAS_DEFAULT, DC_BIAS_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to ", row, 0);
	_START_DROP_DOWN("DC bias reference selection id", row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("AC excitation amplitude = ", row, 0);
	_INSERT_TEXT_INPUT(AC_AMP_DEFAULT, AC_AMP_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mV", row, 2);
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	USER_INPUT_END();
}
NodesData EISPotentiostatic::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);
	/*
	QString selectedRadio1;
	QString selectedRadio2;
	GET_SELECTED_RADIO(selectedRadio1, "Test radio 1 id");
	GET_SELECTED_RADIO(selectedRadio2, "Test radio 2 id");


	QString selectedDropDown;
	GET_SELECTED_DROP_DOWN(selectedDropDown, "Test drop down id");
	//*/

	double upperFreq;
	double lowerFreq;
	double stepsPerDecade;
	double VBias;
	bool VBiasVsOCP = false;
	QString VBiasVsOCPStr;
	
	GET_SELECTED_DROP_DOWN(VBiasVsOCPStr, "DC bias reference selection id");
	GET_TEXT_INPUT_VALUE_DOUBLE(upperFreq, UPPER_FREQ_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(lowerFreq, LOWER_FREQ_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(stepsPerDecade, STEPS_PER_DEC_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(VBias, DC_BIAS_OBJ_NAME);
	if (VBiasVsOCPStr.contains("open circuit"))
	{
		VBiasVsOCP = true;
	}

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e8;
			//exp.tMax = 0xffffffffffffffff;
	exp.tMax = 2e8;		//debugging
	exp.DCPoint_pot.Imax = 0xffff;
	exp.DCPoint_pot.Imin = 0;
	exp.DCPoint_pot.IrangeMax = RANGE0;
	exp.DCPoint_pot.IrangeMin = RANGE7;
	//TODO exp.DCPoint_pot.dVdtMax = 0.1;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e8;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	//getSamplingParameters(sampInterval, &exp);
	//exp.DCPoint_pot.IrangeMax = getCurrentRange(chargeFirst ? chgCurrent * 1.5 : dischgCurrent * 1.5, &calData, hwVersion.hwModel);
	//exp.DCPoint_pot.Imax = chargeFirst ? getCurrentBinary(exp.DCPoint_pot.IrangeMax, chgCurrent * 1.5, &calData) : getCurrentBinary(exp.DCPoint_pot.IrangeMax, chgCurrent * 1.5, &calData);
	//exp.DCPoint_pot.IrangeMin = getCurrentRange(chargeFirst ? minChgCurrent : minDischgCurrent, &calData, hwVersion.hwModel);
	//exp.DCPoint_pot.Imin = chargeFirst ? getCurrentBinary(exp.DCPoint_pot.IrangeMin, minChgCurrent, &calData) : getCurrentBinary(exp.DCPoint_pot.IrangeMin, minDischgCurrent, &calData);
	//exp.DCPoint_pot.VPointUserInput = getVoltageBinary(chargeFirst ? upperVoltage : lowerVoltage, &calData);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();
	
	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList EISPotentiostatic::GetXAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_FREQ <<
		PLOT_VAR_IMP_REAL;
}
QStringList EISPotentiostatic::GetYAxisParameters() const {
	return QStringList() <<
		PLOT_VAR_IMPEDANCE <<
		PLOT_VAR_PHASE <<
		PLOT_VAR_IMP_REAL <<
		PLOT_VAR_IMP_IMAG <<
		PLOT_VAR_NEG_IMP_IMAG;
}
void EISPotentiostatic::PushNewAcData(const QByteArray &expDataRaw, DataMap &container, const CalibrationData&, const HardwareVersion &hwVersion) const {
	ExperimentalAcData *expData = GET_DATA_PTR(expDataRaw);
	uint16_t expDataArraySize = GET_DATA_ARRAY_SIZE(expDataRaw);
	
	/*
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
	//*/
}
void EISPotentiostatic::SaveAcDataHeader(QFile &saveFile) const {
	SAVE_DATA_HEADER_START();

	/*
	SAVE_DATA_HEADER(PLOT_VAR_TIMESTAMP);
	SAVE_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);
	//*/

	SAVE_DATA_HEADER_END();
}

void EISPotentiostatic::SaveAcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	/*
	SAVE_DATA(PLOT_VAR_TIMESTAMP);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);
	//*/

	SAVE_DATA_END();
}