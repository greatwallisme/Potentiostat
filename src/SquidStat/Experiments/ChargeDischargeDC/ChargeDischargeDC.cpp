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
#define SAMP_INTERVAL_OBJ_NAME	"sampling-interval"
#define REST_PERIOD_OBJ			"rest_period"
#define REST_PERIOD_INT_OBJ		"rest-period-interval"
#define CYCLES_OBJ_NAME			"cycles"

#define CHG_CURRENT_DEFAULT		100		//(in mA)
#define DISCHG_CURRENT_DEFAULT	-100	//(in mA)
#define MIN_CHG_CURRENT_DEFAULT	0.5
#define MIN_DISCHG_CURRENT_DEFAULT -0.5
#define UPPER_VOLTAGE_DEFAULT	3.6
#define LOWER_VOLTAGE_DEFAULT	2.7
#define SAMP_INTERVAL_DEFAULT	10
#define REST_PERIOD_DEFAULT		120
#define REST_PERIOD_INT_DEFAULT 1
#define CYCLES_DEFAULT			10

#define PLOT_VAR_DATETIME "Date/time"
#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Elapsed time (s)"
#define PLOT_VAR_ELAPSED_TIME_HR "Elapsed time (hr)"
#define PLOT_VAR_EWE					"Working electrode (V)"
#define PLOT_VAR_CURRENT				"Current (mA)"
#define PLOT_VAR_ECE					"Counter electrode (V)"
#define PLOT_VAR_CURRENT_INTEGRAL		"Cumulative charge (mAh)"

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
ExperimentTypeList ChargeDischargeDC::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap ChargeDischargeDC::GetImage() const {
	return QPixmap(":/Experiments/ChargeDischargeDC");
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
	_INSERT_RIGHT_ALIGN_COMMENT("Sampling interval = ", row, 0);
	_INSERT_TEXT_INPUT(SAMP_INTERVAL_DEFAULT, SAMP_INTERVAL_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Rest period duration = ", row, 0);
	_INSERT_TEXT_INPUT(REST_PERIOD_DEFAULT, REST_PERIOD_OBJ, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Rest period sampling interval = ", row, 0);
	_INSERT_TEXT_INPUT(REST_PERIOD_INT_DEFAULT, REST_PERIOD_INT_OBJ, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

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
NodesData ChargeDischargeDC::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	//TODO: what to do about cells hooked up backwards?
	//TODO: incorporate max charge/discharge capacity? Or incorporate this into another experiment?
	//TODO: make a const. power discharge node type
	//TODO: is there a constant voltage phase during discharge?

	bool chargeFirst= false;
	double upperVoltage;
	double lowerVoltage;
	double chgCurrent;
	double minChgCurrent;
	double dischgCurrent;
	double minDischgCurrent;
	double sampInterval;
	double restPeriodDuration;
	double restPeriodInterval;
	qint32 cycles;
	QString firstPhase;
	GET_SELECTED_DROP_DOWN(firstPhase, "Starting phase selection id");
	GET_TEXT_INPUT_VALUE_DOUBLE(upperVoltage, UPPER_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(lowerVoltage, LOWER_VOLTAGE_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(chgCurrent, CHG_CURRENT_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(dischgCurrent, DISCHG_CURRENT_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(minChgCurrent, MIN_CHG_CURRENT_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(minDischgCurrent, MIN_DISCHG_CURRENT_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(sampInterval, SAMP_INTERVAL_OBJ_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(restPeriodDuration, REST_PERIOD_OBJ);
	GET_TEXT_INPUT_VALUE_DOUBLE(restPeriodInterval, REST_PERIOD_INT_OBJ);
	GET_TEXT_INPUT_VALUE(cycles, CYCLES_OBJ_NAME);
	if (!firstPhase.contains("Discharge first"))
	{
		chargeFirst = true;
	}

	exp.isHead = true;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_GALV;
	exp.tMin = 1 * SECONDS;
	exp.tMax = 0xffffffffffffffff;
	ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, sampInterval);
	exp.DCPoint_galv.Irange = chargeFirst ? ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, chgCurrent) : ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, dischgCurrent);
	exp.DCPoint_galv.IPoint = chargeFirst ? ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, chgCurrent) : ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, dischgCurrent);
	exp.DCPoint_galv.Vmax = ExperimentCalcHelperClass::GetBINVoltage(&calData, upperVoltage);
	exp.DCPoint_galv.Vmin = ExperimentCalcHelperClass::GetBINVoltage(&calData, lowerVoltage);
  exp.DCPoint_galv.dVdtMin = 0;
  exp.currentRangeMode = exp.DCPoint_galv.Irange;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1 * SECONDS;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = AUTORANGE;
	ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, sampInterval);
  exp.DCPoint_pot.Imax = MAX_CURRENT;
	exp.DCPoint_pot.Imin = chargeFirst ? ABS(minChgCurrent) : ABS(minDischgCurrent);
  exp.DCPoint_pot.dIdtMin = 0;
	exp.DCPoint_pot.VPointUserInput = ExperimentCalcHelperClass::GetBINVoltage(&calData, chargeFirst ? upperVoltage : lowerVoltage);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = exp.isTail = false;
	exp.nodeType = DCNODE_OCP;
	exp.DCocp.Vmin = 0;
	exp.DCocp.Vmax = 0x7FFF;
  exp.DCocp.dVdtMin = 0;
	ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, restPeriodInterval);
	exp.tMin = restPeriodDuration * SECONDS;
	exp.tMax = restPeriodDuration * SECONDS;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = true;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_GALV;
	exp.tMin = 3e8;
	exp.tMax = 0xffffffffffffffff;
	ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, sampInterval);
	exp.DCPoint_galv.Irange = !chargeFirst ? ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, chgCurrent) : ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, dischgCurrent);
	exp.DCPoint_galv.IPoint = !chargeFirst ? ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, chgCurrent) : ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, dischgCurrent);
	exp.DCPoint_galv.Vmax = ExperimentCalcHelperClass::GetBINVoltage(&calData, upperVoltage);
	exp.DCPoint_galv.Vmin = ExperimentCalcHelperClass::GetBINVoltage(&calData, lowerVoltage);
  exp.DCPoint_galv.dVdtMin = 0;
	exp.MaxPlays = 1;
  exp.currentRangeMode = exp.DCPoint_galv.Irange;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 3e8;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
  exp.currentRangeMode = AUTORANGE;
	ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, sampInterval);
  exp.DCPoint_pot.Imax = MAX_CURRENT;
	exp.DCPoint_pot.Imin = !chargeFirst ?  ABS(minChgCurrent) : ABS(minDischgCurrent);
  exp.DCPoint_pot.dIdtMin = 0;
	exp.DCPoint_pot.VPointUserInput = ExperimentCalcHelperClass::GetBINVoltage(&calData,!chargeFirst ? upperVoltage : lowerVoltage);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = true;
	exp.nodeType = DCNODE_OCP;
  exp.DCocp.Vmin = 0;
  exp.DCocp.Vmax = 0x7FFF;
  exp.DCocp.dVdtMin = 0;
	ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, restPeriodInterval);
	exp.tMin = restPeriodDuration * SECONDS;
	exp.tMax = restPeriodDuration * SECONDS;
	exp.MaxPlays = cycles;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}
QStringList ChargeDischargeDC::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			PLOT_VAR_ELAPSED_TIME_HR <<
			PLOT_VAR_TIMESTAMP_NORMALIZED <<
      PLOT_VAR_ELAPSED_TIME_HR <<
			PLOT_VAR_EWE <<
			PLOT_VAR_CURRENT;
	}

	return ret;
}
QStringList ChargeDischargeDC::GetYAxisParameters(ExperimentType type) const {
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
void ChargeDischargeDC::PushNewDcData(const ExperimentalDcData &expData, DataMap &container, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / 100000000UL;
  ProcessedDCData processedData = ExperimentCalcHelperClass::ProcessDCDataPoint(&calData, expData);

	if (container[PLOT_VAR_CURRENT_INTEGRAL].data.isEmpty()) {
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, 0);
	}
	else {
		qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].data.last();
		newVal += (container[PLOT_VAR_CURRENT].data.last() + processedData.current) * (timestamp - container[PLOT_VAR_TIMESTAMP].data.last()) / 2. / 3600.0;
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, newVal);
	}

	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP, timestamp);
  PUSH_BACK_DATA(PLOT_VAR_EWE, processedData.EWE);
	PUSH_BACK_DATA(PLOT_VAR_ECE, processedData.ECE);
	PUSH_BACK_DATA(PLOT_VAR_CURRENT, processedData.current);

	if (!timestampOffset.contains(&container)) {
		timestampOffset[&container] = timestamp;
	}
	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED, timestamp - timestampOffset[&container]);
  PUSH_BACK_DATA(PLOT_VAR_ELAPSED_TIME_HR, (timestamp - timestampOffset[&container]) / 3600);
}
void ChargeDischargeDC::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	//SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP);
  SAVE_DC_DATA_HEADER(PLOT_VAR_DATETIME);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
  SAVE_DC_DATA_HEADER(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}

void ChargeDischargeDC::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	//SAVE_DATA(PLOT_VAR_TIMESTAMP);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
  SAVE_DATA(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_END();
}
