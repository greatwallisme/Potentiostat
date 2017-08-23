#include "OpenCircuit.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>


#define TOP_WIDGET_NAME			"open-circuit-potential"

#define SAMPLING_INT_OBJ_NAME "sampling-interval"
#define SAMPLING_INT_UNITS_OBJ_NAME "sampling-interval-units"
#define DURATION_OBJ_NAME "experiment-duration"
#define DURATION_UNITS_OBJ_NAME "experiment-duration-units"

#define SAMPLING_INT_DEFAULT 1
#define DURATION_DEFAULT 60

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Elapsed time (s)"
#define PLOT_VAR_ELAPSED_TIME_HR      "Elapsed time (hr)"
#define PLOT_VAR_EWE					"Working electrode (V)"
#define PLOT_VAR_ECE					"Counter electrode (V)"

QString OpenCircuit::GetShortName() const {
	return "Open Circuit";
}
QString OpenCircuit::GetFullName() const {
	return "Open circuit potential logging";
}
QString OpenCircuit::GetDescription() const {
  return "This experiment observes the <b>open circuit potential</b> of the working electrode for a specified period of time.";
}
QStringList OpenCircuit::GetCategory() const {
  return QStringList() <<
    "Basic voltammetry";
}
ExperimentTypeList OpenCircuit::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap OpenCircuit::GetImage() const {
	return QPixmap(":/Experiments/OpenCircuitPotential");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* OpenCircuit::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;

	_INSERT_RIGHT_ALIGN_COMMENT("Sampling interval = ", row, 0);
	_INSERT_TEXT_INPUT(SAMPLING_INT_DEFAULT, SAMPLING_INT_OBJ_NAME, row, 1);
  _START_DROP_DOWN(SAMPLING_INT_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();

  ++row;
  _INSERT_RIGHT_ALIGN_COMMENT("Experiment duration = ", row, 0);
  _INSERT_TEXT_INPUT(DURATION_DEFAULT, DURATION_OBJ_NAME, row, 1);
  _START_DROP_DOWN(DURATION_UNITS_OBJ_NAME, row, 2);
  _ADD_DROP_DOWN_ITEM("s");
  _ADD_DROP_DOWN_ITEM("min");
  _ADD_DROP_DOWN_ITEM("hr");
  _END_DROP_DOWN();

	_SET_ROW_STRETCH(++row, 1);
	_SET_COL_STRETCH(3, 1);

	USER_INPUT_END();
}
NodesData OpenCircuit::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

  double samplingInterval;
  QString samplingIntervalUnits;
  double duration;
  QString durationUnits;

  GET_TEXT_INPUT_VALUE_DOUBLE(samplingInterval, SAMPLING_INT_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(samplingIntervalUnits, SAMPLING_INT_UNITS_OBJ_NAME);
  GET_TEXT_INPUT_VALUE_DOUBLE(duration, DURATION_OBJ_NAME);
  GET_SELECTED_DROP_DOWN(durationUnits, DURATION_UNITS_OBJ_NAME);

  samplingInterval *= ExperimentCalcHelperClass::GetUnitsMultiplier(samplingIntervalUnits);
  duration *= ExperimentCalcHelperClass::GetUnitsMultiplier(durationUnits);

  exp.isHead = false;
  exp.isTail = false;
  exp.nodeType = DCNODE_OCP;
  exp.tMin = 0;
  exp.tMax = duration * SECONDS;
  ExperimentCalcHelperClass::GetSamplingParams_staticDAC(hwVersion.hwModel, &exp, samplingInterval);
  
  exp.DCocp.dVdtMin = 0;
  exp.DCocp.Vmax = 32767;
  exp.DCocp.Vmin = -32768;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList OpenCircuit::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
		ret <<
			PLOT_VAR_ELAPSED_TIME_HR <<
			PLOT_VAR_TIMESTAMP_NORMALIZED;
	}

	return ret;
}
QStringList OpenCircuit::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_DC) {
    ret <<
      PLOT_VAR_EWE <<
      PLOT_VAR_ECE;
	}

	return ret;
}
void OpenCircuit::PUSH_NEW_DC_DATA_DEFINITION{
	static QMap<DataMap*, qreal> timestampOffset;
	qreal timestamp = (qreal)expData.timestamp / SECONDS;
  ProcessedDCData processedDCdata = ExperimentCalcHelperClass::ProcessDCDataPoint(&calData, expData);

	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP, timestamp);
	PUSH_BACK_DATA(PLOT_VAR_EWE, processedDCdata.EWE);
	PUSH_BACK_DATA(PLOT_VAR_ECE, processedDCdata.ECE);

	if (!timestampOffset.contains(&container)) {
		timestampOffset[&container] = timestamp;
	}
	PUSH_BACK_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED, timestamp - timestampOffset[&container]);
  PUSH_BACK_DATA(PLOT_VAR_ELAPSED_TIME_HR, (timestamp - timestampOffset[&container]) / 3600);
}
void OpenCircuit::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);

	SAVE_DATA_HEADER_END();
}

void OpenCircuit::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_ECE);
	
	SAVE_DATA_END();
}