#include "Chronopotentiometry.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Chronopotentiometry"

#define I1_OBJECT_NAME			"current-1"
#define I1_UNITS_OBJ_NAME   "current-units-1"
#define T1_OBJECT_NAME			"current-1-time"
#define T1_UNITS_OBJ_NAME   "current-1-time-units"
//#define I2_OBJECT_NAME			"current-2"
//#define I2_UNITS_OBJ_NAME   "current-units-2"
//#define T2_OBJECT_NAME			"current-2-time"
//#define T2_UNITS_OBJ_NAME   "current-2-time-units"
//#define I3_OBJECT_NAME			"current-3"
//#define I3_UNITS_OBJ_NAME   "current-units-3"
//#define T3_OBJECT_NAME			"current-3-time"
//#define T3_UNITS_OBJ_NAME   "current-3-time-units"
//#define I4_OBJECT_NAME			"current-4"
//#define I4_UNITS_OBJ_NAME   "current-units-4"
//#define T4_OBJECT_NAME			"current-4-time"
//#define T4_UNITS_OBJ_NAME   "current-4-time-units"
#define SAMPLING_PERIOD_OBJ_NAME	"sampling-period"

#define I1_DEFAULT				1
#define T1_DEFAULT				10
//#define I2_DEFAULT				2
//#define T2_DEFAULT				0
//#define I3_DEFAULT				3
//#define T3_DEFAULT				0
//#define I4_DEFAULT				4
//#define T4_DEFAULT				0
#define SAMPLING_INT_DEFAULT	1

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Elapsed time (s)"
#define PLOT_VAR_ELAPSED_TIME_HR      "Elapsed time (hr)"
#define PLOT_VAR_EWE					"Working electrode (V)"
#define PLOT_VAR_CURRENT				"Current (mA)"
#define PLOT_VAR_ECE					"Counter electrode (V)"
#define PLOT_VAR_CURRENT_INTEGRAL		"Cumulative charge (mAh)"

QString Chronopotentiometry::GetShortName() const {
	return "Chronopotentiometry";
}
QString Chronopotentiometry::GetFullName() const {
	return "Chronopotentiometry";
}
QString Chronopotentiometry::GetDescription() const {
	return "This experiment applies a constant current to the cell while recording the working electrode potential.";
}
QStringList Chronopotentiometry::GetCategory() const {
  return QStringList() <<
    "Basic voltammetry";
}
ExperimentTypeList Chronopotentiometry::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap Chronopotentiometry::GetImage() const {
	return QPixmap(":/Experiments/Chronopotentiometry");
}

QWidget* Chronopotentiometry::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Current: ", row, 0);
	_INSERT_TEXT_INPUT(I1_DEFAULT, I1_OBJECT_NAME, row, 1);
  _START_DROP_DOWN(I1_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("mA");
  _ADD_DROP_DOWN_ITEM("uA");
  _ADD_DROP_DOWN_ITEM("nA");
  _END_DROP_DOWN();

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Duration: ", row, 0);
	_INSERT_TEXT_INPUT(T1_DEFAULT, T1_OBJECT_NAME, row, 1);
  _START_DROP_DOWN(T1_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();

	//++row;
	//_INSERT_VERTICAL_SPACING(row);

	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT("Current 2 = ", row, 0);
	//_INSERT_TEXT_INPUT(I2_DEFAULT, I2_OBJECT_NAME, row, 1);
 // _START_DROP_DOWN(I2_UNITS_OBJ_NAME, row, 2);
 // _ADD_DROP_DOWN_ITEM("mA");
 // _ADD_DROP_DOWN_ITEM("uA");
 // _ADD_DROP_DOWN_ITEM("nA");
 // _END_DROP_DOWN();

	////TODO: add hh:mm:ss format
	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT("Duration = ", row, 0);
	//_INSERT_TEXT_INPUT(T2_DEFAULT, T2_OBJECT_NAME, row, 1);
 // _START_DROP_DOWN(T2_UNITS_OBJ_NAME, row, 2);
 // _ADD_DROP_DOWN_ITEM("s");
 // _ADD_DROP_DOWN_ITEM("min");
 // _ADD_DROP_DOWN_ITEM("hr");
 // _END_DROP_DOWN();

	//++row;
	//_INSERT_VERTICAL_SPACING(row);

	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT("Current 3 = ", row, 0);
	//_INSERT_TEXT_INPUT(I3_DEFAULT, I3_OBJECT_NAME, row, 1);
 // _START_DROP_DOWN(I3_UNITS_OBJ_NAME, row, 2);
 // _ADD_DROP_DOWN_ITEM("mA");
 // _ADD_DROP_DOWN_ITEM("uA");
 // _ADD_DROP_DOWN_ITEM("nA");
 // _END_DROP_DOWN();

	////TODO: add hh:mm:ss format
	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT("Duration = ", row, 0);
	//_INSERT_TEXT_INPUT(T3_DEFAULT, T3_OBJECT_NAME, row, 1);
 // _START_DROP_DOWN(T3_UNITS_OBJ_NAME, row, 2);
 // _ADD_DROP_DOWN_ITEM("s");
 // _ADD_DROP_DOWN_ITEM("min");
 // _ADD_DROP_DOWN_ITEM("hr");
 // _END_DROP_DOWN();

	//++row;
	//_INSERT_VERTICAL_SPACING(row);

	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT("Current 4 = ", row, 0);
	//_INSERT_TEXT_INPUT(I4_DEFAULT, I4_OBJECT_NAME, row, 1);
 // _START_DROP_DOWN(I4_UNITS_OBJ_NAME, row, 2);
 // _ADD_DROP_DOWN_ITEM("mA");
 // _ADD_DROP_DOWN_ITEM("uA");
 // _ADD_DROP_DOWN_ITEM("nA");
 // _END_DROP_DOWN();

	////TODO: add hh:mm:ss format
	//++row;
	//_INSERT_RIGHT_ALIGN_COMMENT("Duration = ", row, 0);
	//_INSERT_TEXT_INPUT(T4_DEFAULT, T4_OBJECT_NAME, row, 1);
	//_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	//++row;
	//_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Sampling interval: ", row, 0);
	_INSERT_TEXT_INPUT(SAMPLING_INT_DEFAULT, SAMPLING_PERIOD_OBJ_NAME, row, 1);
  _INSERT_LEFT_ALIGN_COMMENT("s", row, 2);
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	_SET_ROW_STRETCH(++row, 1);
	USER_INPUT_END();
}
NodesData Chronopotentiometry::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	double i1, /*i2, i3, i4,*/ t1/*, t2, t3, t4*/;
  QString t1Units_str/*, t2Units_str, t3Units_str, t4Units_str*/;
	GET_TEXT_INPUT_VALUE_DOUBLE(i1, I1_OBJECT_NAME);
	/*GET_TEXT_INPUT_VALUE_DOUBLE(i2, I2_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(i3, I3_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(i4, I4_OBJECT_NAME);*/
	GET_TEXT_INPUT_VALUE_DOUBLE(t1, T1_OBJECT_NAME);
	//GET_TEXT_INPUT_VALUE_DOUBLE(t2, T2_OBJECT_NAME);
	//GET_TEXT_INPUT_VALUE_DOUBLE(t3, T3_OBJECT_NAME);
	//GET_TEXT_INPUT_VALUE_DOUBLE(t4, T4_OBJECT_NAME);
  GET_SELECTED_DROP_DOWN(t1Units_str, T1_UNITS_OBJ_NAME);
  //GET_SELECTED_DROP_DOWN(t2Units_str, T2_UNITS_OBJ_NAME);
  //GET_SELECTED_DROP_DOWN(t3Units_str, T3_UNITS_OBJ_NAME);
  //GET_SELECTED_DROP_DOWN(t4Units_str, T4_UNITS_OBJ_NAME);

	double dt;
	GET_TEXT_INPUT_VALUE_DOUBLE(dt, SAMPLING_PERIOD_OBJ_NAME);

	QString iUnits1, iUnits2, iUnits3, iUnits4;
	GET_SELECTED_DROP_DOWN(iUnits1, I1_UNITS_OBJ_NAME);
	//GET_SELECTED_DROP_DOWN(iUnits2, I2_UNITS_OBJ_NAME);
	//GET_SELECTED_DROP_DOWN(iUnits3, I3_UNITS_OBJ_NAME);
	//GET_SELECTED_DROP_DOWN(iUnits4, I4_UNITS_OBJ_NAME);

  i1 *= ExperimentCalcHelperClass::GetUnitsMultiplier(iUnits1);
  //i2 *= ExperimentCalcHelperClass::GetUnitsMultiplier(iUnits2);
  //i3 *= ExperimentCalcHelperClass::GetUnitsMultiplier(iUnits3);
  //i4 *= ExperimentCalcHelperClass::GetUnitsMultiplier(iUnits4);

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_GALV;
	exp.tMin = 0;
	exp.tMax = t1 * SECONDS * ExperimentCalcHelperClass::GetUnitsMultiplier(t1Units_str);
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, dt);
  exp.DCPoint_galv.Irange = exp.currentRangeMode = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, i1);
  exp.DCPoint_galv.IPoint = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, i1);
  exp.DCPoint_galv.Vmax = MAX_VOLTAGE;
  exp.DCPoint_galv.Vmin = -MAX_VOLTAGE;
  exp.DCPoint_galv.dVdtMin = 0;
	exp.MaxPlays = 1;
  if(exp.tMax != 0)
    PUSH_NEW_NODE_DATA();

 /* exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_POINT_GALV;
  exp.tMin = 0;
  exp.tMax = t2 * SECONDS * ExperimentCalcHelperClass::GetUnitsMultiplier(t2Units_str);
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, dt);
  exp.DCPoint_galv.Irange = exp.currentRangeMode = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, i2);
  exp.DCPoint_galv.IPoint = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, i2);
  exp.DCPoint_galv.Vmax = 32767;
  exp.DCPoint_galv.Vmin = -32768;
  exp.DCPoint_galv.dVdtMin = 0;
  exp.MaxPlays = 1;
  if (exp.tMax != 0)
    PUSH_NEW_NODE_DATA();

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_POINT_GALV;
  exp.tMin = 0;
  exp.tMax = t3 * SECONDS * ExperimentCalcHelperClass::GetUnitsMultiplier(t3Units_str);
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, dt);
  exp.DCPoint_galv.Irange = exp.currentRangeMode = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, i3);
  exp.DCPoint_galv.IPoint = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, i3);
  exp.DCPoint_galv.Vmax = MAX_VOLTAGE;
  exp.DCPoint_galv.Vmin = -MAX_VOLTAGE;
  exp.DCPoint_galv.dVdtMin = 0;
  exp.MaxPlays = 1;
  if (exp.tMax != 0)
    PUSH_NEW_NODE_DATA();

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_POINT_GALV;
  exp.tMin = 0;
  exp.tMax = t4 * SECONDS * ExperimentCalcHelperClass::GetUnitsMultiplier(t4Units_str);
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, dt);
  exp.DCPoint_galv.Irange = exp.currentRangeMode = ExperimentCalcHelperClass::GetMinCurrentRange(hwVersion.hwModel, &calData, i4);
  exp.DCPoint_galv.IPoint = ExperimentCalcHelperClass::GetBINCurrent(&calData, exp.DCPoint_galv.Irange, i4);
  exp.DCPoint_galv.Vmax = MAX_VOLTAGE;
  exp.DCPoint_galv.Vmin = -MAX_VOLTAGE;
  exp.DCPoint_galv.dVdtMin = 0;
  exp.MaxPlays = 1;
  if (exp.tMax != 0)
    PUSH_NEW_NODE_DATA();*/

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList Chronopotentiometry::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			//PLOT_VAR_TIMESTAMP <<
			PLOT_VAR_TIMESTAMP_NORMALIZED <<
      PLOT_VAR_ELAPSED_TIME_HR <<
			PLOT_VAR_EWE <<
			PLOT_VAR_CURRENT;
	}

	return ret;
}
QStringList Chronopotentiometry::GetYAxisParameters(ExperimentType type) const {
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
void Chronopotentiometry::PUSH_NEW_DC_DATA_DEFINITION {
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / SECONDS;
  ProcessedDCData processedData = ExperimentCalcHelperClass::ProcessDCDataPoint(&calData, expData);

	if (container[PLOT_VAR_CURRENT_INTEGRAL].data.isEmpty()) {
		PUSH_BACK_DATA(PLOT_VAR_CURRENT_INTEGRAL, 0);
	}
	else {
		qreal newVal = container[PLOT_VAR_CURRENT_INTEGRAL].data.last();
		newVal += (container[PLOT_VAR_CURRENT].data.last() + processedData.current) * (timestamp - container[PLOT_VAR_TIMESTAMP].data.last()) / 3600.0 / 2.;
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
void Chronopotentiometry::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}

void Chronopotentiometry::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_END();
}