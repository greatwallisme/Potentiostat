#include "EISPotentiostatic.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#include <ExperimentCalcHelper.h>

#define TOP_WIDGET_NAME			"EIS-Potentiostatic"

#define UPPER_FREQ_OBJ_NAME		"upper-frequency"
#define LOWER_FREQ_OBJ_NAME		"lower-frequency"
#define UPPER_FREQ_UNITS_OBJ_NAME "upper-frequency-units"
#define LOWER_FREQ_UNITS_OBJ_NAME "lower-frequency-units"
#define STEPS_PER_DEC_OBJ_NAME	"step-per-decade"
#define DC_BIAS_OBJ_NAME		"DC-bias"
#define DC_BIAS_VS_OCP_OBJ_NAME "DC-bias-vs-OCP"
#define AC_AMP_OBJ_NAME			"AC-amplitude"

#define UPPER_FREQ_DEFAULT		1000000		//(in Hz)
#define LOWER_FREQ_DEFAULT		0.001	//(in Hz)
#define STEPS_PER_DEC_DEFAULT	5
#define DC_BIAS_DEFAULT			0
#define AC_AMP_DEFAULT			10		//(in mA)

#define PLOT_VAR_IMPEDANCE				"|Z| (Ohms)"
#define PLOT_VAR_PHASE					"Phase"
#define PLOT_VAR_IMP_REAL				"Z\'(Ohms)"
#define PLOT_VAR_IMP_IMAG				"Z\"(Ohms)"
#define PLOT_VAR_NEG_IMP_IMAG			"-Z\"(Ohms)"
#define PLOT_VAR_FREQ					"Frequency (Hz)"
#define PLOT_VAR_ERROR        "Error (AU)"

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
	return ExperimentTypeList() << ET_AC;
}
QPixmap EISPotentiostatic::GetImage() const {
	return QPixmap(":/Experiments/EISPotentiostatic");
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
  _START_DROP_DOWN(UPPER_FREQ_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("kHz");
  _ADD_DROP_DOWN_ITEM("Hz");
  _ADD_DROP_DOWN_ITEM("mHz");
  _END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Lower frequency", row, 0);
	_INSERT_TEXT_INPUT(LOWER_FREQ_DEFAULT, LOWER_FREQ_OBJ_NAME, row, 1);
  _START_DROP_DOWN(LOWER_FREQ_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("kHz");
  _ADD_DROP_DOWN_ITEM("Hz");
  _ADD_DROP_DOWN_ITEM("mHz");
  _END_DROP_DOWN();

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
	_START_DROP_DOWN(DC_BIAS_VS_OCP_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("open circuit");
	_ADD_DROP_DOWN_ITEM("reference");
	_END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("AC excitation amplitude = ", row, 0);
	_INSERT_TEXT_INPUT(AC_AMP_DEFAULT, AC_AMP_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("mV", row, 2);
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	_SET_ROW_STRETCH(++row, 1);
	USER_INPUT_END();
}
NodesData EISPotentiostatic::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	double upperFreq;
	double lowerFreq;
  QString upperFreqUnits_str;
  QString lowerFreqUnits_str;
	double stepsPerDecade;
	double VBias;
	bool VBiasVsOCP = false;
	QString VBiasVsOCPStr;
  double amplitude;

	GET_TEXT_INPUT_VALUE_DOUBLE(upperFreq, UPPER_FREQ_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(lowerFreq, LOWER_FREQ_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(upperFreqUnits_str, UPPER_FREQ_UNITS_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(lowerFreqUnits_str, LOWER_FREQ_UNITS_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(stepsPerDecade, STEPS_PER_DEC_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(amplitude, AC_AMP_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(VBias, DC_BIAS_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(VBiasVsOCPStr, DC_BIAS_VS_OCP_OBJ_NAME);

	if (VBiasVsOCPStr.contains("open circuit"))
		VBiasVsOCP = true;

  upperFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(upperFreqUnits_str);
  lowerFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(lowerFreqUnits_str);

  QList<double> frequencyList = ExperimentCalcHelperClass::calculateFrequencyList(lowerFreq, upperFreq, stepsPerDecade);

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
  exp.tMin = exp.tMax = 2 * SECONDS;
  exp.currentRangeMode = AUTORANGE;
  exp.DCPoint_pot.Imax = MAX_CURRENT;
  exp.DCPoint_pot.Imin = 0;
  exp.DCPoint_pot.dIdtMin = 0;
  exp.DCPoint_pot.VPointUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, VBias);
  exp.DCPoint_pot.VPointVsOCP = VBiasVsOCP;
	exp.MaxPlays = 1;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 4);
	PUSH_NEW_NODE_DATA();

  for (int i = 0; i < frequencyList.length(); i++)
  {
    exp.isHead = false;
    exp.isTail = false;
    exp.nodeType = FRA_NODE_POT;
    exp.currentRangeMode = AUTORANGE;
    exp.FRA_pot_node.frequency = (float)frequencyList[i];
    exp.FRA_pot_node.VBiasUserInput = ExperimentCalcHelperClass::GetBINVoltageForDAC(&calData, VBias);
    exp.FRA_pot_node.VBiasVsOCP = VBiasVsOCP;
    exp.FRA_pot_node.amplitudeTarget = amplitude / 1000;
    ExperimentCalcHelperClass::calcACSamplingParams(&calData, &exp);
    exp.FRA_pot_node.firstTime = (i == 0);
    PUSH_NEW_NODE_DATA();
  }

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList EISPotentiostatic::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_AC) {
		ret <<
		PLOT_VAR_FREQ <<
		PLOT_VAR_IMP_REAL;
	}

	return ret;
}
QStringList EISPotentiostatic::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_AC) {
		ret <<
		PLOT_VAR_IMPEDANCE <<
		PLOT_VAR_PHASE <<
		PLOT_VAR_IMP_REAL <<
		PLOT_VAR_IMP_IMAG <<
		PLOT_VAR_NEG_IMP_IMAG <<
    PLOT_VAR_ERROR;
	}

	return ret;
}
void EISPotentiostatic::PUSH_NEW_AC_DATA_DEFINITION {
	ComplexDataPoint_t dataPoint;
	GET_COMPLEX_DATA_POINT(dataPoint, expDataRaw, &calData);
	
  //if (dataPoint.error < 2000)
  //{
    PUSH_BACK_DATA(PLOT_VAR_FREQ, dataPoint.frequency);
    PUSH_BACK_DATA(PLOT_VAR_IMPEDANCE, dataPoint.ImpedanceMag);
    PUSH_BACK_DATA(PLOT_VAR_PHASE, dataPoint.phase);
    PUSH_BACK_DATA(PLOT_VAR_IMP_REAL, dataPoint.ImpedanceReal);
    PUSH_BACK_DATA(PLOT_VAR_IMP_IMAG, dataPoint.ImpedanceImag);
    PUSH_BACK_DATA(PLOT_VAR_NEG_IMP_IMAG, -dataPoint.ImpedanceImag);
    PUSH_BACK_DATA(PLOT_VAR_ERROR, dataPoint.error);
  //}
}
void EISPotentiostatic::SaveAcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_AC_DATA_HEADER(PLOT_VAR_FREQ);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMPEDANCE);
	SAVE_AC_DATA_HEADER(PLOT_VAR_PHASE);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMP_REAL);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMP_IMAG);
	SAVE_AC_DATA_HEADER(PLOT_VAR_NEG_IMP_IMAG);
  SAVE_AC_DATA_HEADER(PLOT_VAR_ERROR);
	
	SAVE_DATA_HEADER_END();
}

void EISPotentiostatic::SaveAcData(QFile &saveFile, const DataMap &container) const {
  if(container[PLOT_VAR_ERROR].data.count() > 0 && container[PLOT_VAR_ERROR].data.last() < 2000)
  {
    SAVE_DATA_START();

    SAVE_DATA(PLOT_VAR_FREQ);
    SAVE_DATA(PLOT_VAR_IMPEDANCE);
    SAVE_DATA(PLOT_VAR_PHASE);
    SAVE_DATA(PLOT_VAR_IMP_REAL);
    SAVE_DATA(PLOT_VAR_IMP_IMAG);
    SAVE_DATA(PLOT_VAR_NEG_IMP_IMAG);
    SAVE_DATA(PLOT_VAR_ERROR);

    SAVE_DATA_END();
  }
}