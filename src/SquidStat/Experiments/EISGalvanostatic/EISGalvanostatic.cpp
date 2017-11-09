#include "EISGalvanostatic.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#include <ExperimentCalcHelper.h>

#define TOP_WIDGET_NAME			"EIS-Galvanostatic"

#define UPPER_FREQ_OBJ_NAME		"upper-frequency"
#define LOWER_FREQ_OBJ_NAME		"lower-frequency"
#define UPPER_FREQ_UNITS_OBJ_NAME "upper-frequency-units"
#define LOWER_FREQ_UNITS_OBJ_NAME "lower-frequency-units"
#define STEPS_PER_DEC_OBJ_NAME	"step-per-decade"
#define DC_BIAS_OBJ_NAME		"DC-bias-current"
#define DC_BIAS_UNITS_OBJ_NAME "DC-bias-units"
#define AC_AMP_OBJ_NAME			"AC-amplitude"
#define AC_AMP_UNITS_OBJ_NAME "AC-amplitude-units"

#define UPPER_FREQ_DEFAULT		1000		//(in kHz)
#define LOWER_FREQ_DEFAULT		1	//(in kHz)
#define STEPS_PER_DEC_DEFAULT	5
#define DC_BIAS_DEFAULT			0
#define AC_AMP_DEFAULT			0.1		//(in mA)

#define PLOT_VAR_IMPEDANCE				"|Z|"
#define PLOT_VAR_PHASE					"Phase (degrees)"
#define PLOT_VAR_ABS_PHASE      "|Phase| (degrees)"
#define PLOT_VAR_IMP_REAL				"Z\'"
#define PLOT_VAR_IMP_IMAG				"Z\""
#define PLOT_VAR_NEG_IMP_IMAG			"-Z\""
#define PLOT_VAR_FREQ					"Frequency"

QString EISGalvanostatic::GetShortName() const {
	return "EIS (Galvanostatic)";
}
QString EISGalvanostatic::GetFullName() const {
	return "EIS (Galvanostatic)";
}
QString EISGalvanostatic::GetDescription() const {
	return "This experiment records the complex impedance of the experimental cell in galvanostatic mode, starting from the <b>upper frequency</b> and sweeping downwards through the <b>lower frequency</b>, with a fixed number of frequency <b>steps per decade</b>. Important parameters include the <b>DC bias</b> and the <b>AC exitation amplitude</b>.";
}
QStringList EISGalvanostatic::GetCategory() const {
	return QStringList() <<
		"Impedance spectroscopy";

}
ExperimentTypeList EISGalvanostatic::GetTypes() const {
	return ExperimentTypeList() << ET_AC;
}
QPixmap EISGalvanostatic::GetImage() const {
	return QPixmap(":/Experiments/EISGalvanostatic");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* EISGalvanostatic::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Upper frequency: ", row, 0);
	_INSERT_TEXT_INPUT(UPPER_FREQ_DEFAULT, UPPER_FREQ_OBJ_NAME, row, 1);
  _START_DROP_DOWN(UPPER_FREQ_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("kHz");
  _ADD_DROP_DOWN_ITEM("Hz");
  _ADD_DROP_DOWN_ITEM("mHz");
  _END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Lower frequency: ", row, 0);
	_INSERT_TEXT_INPUT(LOWER_FREQ_DEFAULT, LOWER_FREQ_OBJ_NAME, row, 1);
  _START_DROP_DOWN(LOWER_FREQ_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("kHz");
  _ADD_DROP_DOWN_ITEM("Hz");
  _ADD_DROP_DOWN_ITEM("mHz");
  _END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Steps per decade: ", row, 0);
	_INSERT_TEXT_INPUT(STEPS_PER_DEC_DEFAULT, STEPS_PER_DEC_OBJ_NAME, row, 1);

	++row;
	_INSERT_VERTICAL_SPACING(row);
	
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("DC bias: ", row, 0);
	_INSERT_TEXT_INPUT(DC_BIAS_DEFAULT, DC_BIAS_OBJ_NAME, row, 1);
  _START_DROP_DOWN(DC_BIAS_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("AC excitation amplitude: ", row, 0);
	_INSERT_TEXT_INPUT(AC_AMP_DEFAULT, AC_AMP_OBJ_NAME, row, 1);
  _START_DROP_DOWN(AC_AMP_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	_SET_ROW_STRETCH(++row, 1);
	USER_INPUT_END();
}
NodesData EISGalvanostatic::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	double upperFreq;
	double lowerFreq;
  QString upperFreqUnits_str;
  QString lowerFreqUnits_str;
	double stepsPerDecade;
	double IBias;
  QString IBiasUnits_str;
  double amplitude;
  QString amplitudeUnits_str;

	GET_TEXT_INPUT_VALUE_DOUBLE(upperFreq, UPPER_FREQ_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(lowerFreq, LOWER_FREQ_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(upperFreqUnits_str, UPPER_FREQ_UNITS_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(lowerFreqUnits_str, LOWER_FREQ_UNITS_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(stepsPerDecade, STEPS_PER_DEC_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(amplitude, AC_AMP_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(amplitudeUnits_str, AC_AMP_UNITS_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(IBias, DC_BIAS_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(IBiasUnits_str, DC_BIAS_UNITS_OBJ_NAME);

  upperFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(upperFreqUnits_str);
  lowerFreq *= ExperimentCalcHelperClass::GetUnitsMultiplier(lowerFreqUnits_str);
  amplitude *= ExperimentCalcHelperClass::GetUnitsMultiplier(amplitudeUnits_str);
  IBias *= ExperimentCalcHelperClass::GetUnitsMultiplier(IBiasUnits_str);

  currentRange_t DCcurrentRangeLimit = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, IBias);
  currentRange_t ACcurrentRangeLimit = ExperimentCalcHelperClass::GetMinCurrentRange_DACac(&calData, amplitude);

  QList<double> frequencyList = ExperimentCalcHelperClass::calculateFrequencyList(lowerFreq, upperFreq, stepsPerDecade);

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_POINT_GALV;
  exp.currentRangeMode = exp.DCPoint_galv.Irange = (currentRange_t)MIN((int)DCcurrentRangeLimit, (int)ACcurrentRangeLimit);
  exp.tMin = exp.tMax = 2 * SECONDS;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, 4);
  exp.DCPoint_galv.dVdtMin = 0;
  exp.DCPoint_galv.IPoint = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.currentRangeMode, IBias);
  exp.DCPoint_galv.Vmax = MAX_VOLTAGE;
  exp.DCPoint_galv.Vmin = -MAX_VOLTAGE;
  PUSH_NEW_NODE_DATA();

  for (int i = 0; i < frequencyList.length(); i++)
  {
    exp.isHead = false;
    exp.isTail = false;
    exp.nodeType = FRA_NODE_GALV;
    exp.tMin = 0;
    exp.tMax = 0xffffffffffffffff;
    exp.currentRangeMode = (currentRange_t)MIN((int)DCcurrentRangeLimit, (int)ACcurrentRangeLimit);
    exp.ACsamplingParams.frequency = (float)frequencyList[i];
    ExperimentCalcHelperClass::calcACSamplingParams(&calData, &exp);
    exp.ACsamplingParams.amplitudeTarget = amplitude;
    exp.FRA_galv_node.IRange = exp.currentRangeMode;
    exp.FRA_galv_node.IBias = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.FRA_galv_node.IRange, IBias);
    exp.ACsamplingParams.firstTime = (i == 0);
    PUSH_NEW_NODE_DATA();
  }

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList EISGalvanostatic::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_AC) {
		ret <<
		PLOT_VAR_FREQ <<
		PLOT_VAR_IMP_REAL;
	}

	return ret;
}
QStringList EISGalvanostatic::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_AC) {
		ret <<
		PLOT_VAR_IMPEDANCE <<
		PLOT_VAR_PHASE <<
    PLOT_VAR_ABS_PHASE <<
		PLOT_VAR_IMP_REAL <<
		PLOT_VAR_IMP_IMAG <<
		PLOT_VAR_NEG_IMP_IMAG;
	}

	return ret;
}
void EISGalvanostatic::PUSH_NEW_AC_DATA_DEFINITION {
	ComplexDataPoint_t dataPoint;
	GET_COMPLEX_DATA_POINT(dataPoint);
	//double numCycles = ExperimentCalcHelperClass::estimatePeriod(*dataHeader);
	//dataPoint = ExperimentCalcHelperClass::AnalyzeFRA(dataHeader->frequency, ACrawdata, numACBuffers, dataHeader->gainVoltage, dataHeader->gainCurrent, numCycles, &calData, dataHeader->IRange);

	PUSH_BACK_DATA(PLOT_VAR_FREQ, dataPoint.frequency);
	PUSH_BACK_DATA(PLOT_VAR_IMPEDANCE, dataPoint.ImpedanceMag);
	PUSH_BACK_DATA(PLOT_VAR_PHASE, dataPoint.phase);
	PUSH_BACK_DATA(PLOT_VAR_ABS_PHASE, abs(dataPoint.phase));
	PUSH_BACK_DATA(PLOT_VAR_IMP_REAL, dataPoint.ImpedanceReal);
	PUSH_BACK_DATA(PLOT_VAR_IMP_IMAG, dataPoint.ImpedanceImag);
	PUSH_BACK_DATA(PLOT_VAR_NEG_IMP_IMAG, -dataPoint.ImpedanceImag);
}
void EISGalvanostatic::SaveAcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_AC_DATA_HEADER(PLOT_VAR_FREQ);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMPEDANCE);
	SAVE_AC_DATA_HEADER(PLOT_VAR_PHASE);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMP_REAL);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMP_IMAG);
	SAVE_AC_DATA_HEADER(PLOT_VAR_NEG_IMP_IMAG);
	
	SAVE_DATA_HEADER_END();
}

void EISGalvanostatic::SaveAcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_FREQ);
	SAVE_DATA(PLOT_VAR_IMPEDANCE);
	SAVE_DATA(PLOT_VAR_PHASE);
	SAVE_DATA(PLOT_VAR_IMP_REAL);
	SAVE_DATA(PLOT_VAR_IMP_IMAG);
	SAVE_DATA(PLOT_VAR_NEG_IMP_IMAG);

	SAVE_DATA_END();
}