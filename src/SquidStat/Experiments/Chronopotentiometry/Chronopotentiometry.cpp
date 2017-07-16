#include "Chronopotentiometry.h"

#include <ExternalStructures.h>
#include <ExperimentUIHelper.h>

#define TOP_WIDGET_NAME			"Chronopotentiometry"

#define V1_OBJECT_NAME			"voltage-1"
#define V1_VS_OCP_OBJ_NAME		"voltage-1-vs-ocp"
#define T1_OBJECT_NAME			"voltage-1-time"
#define V2_OBJECT_NAME			"voltage-2"
#define V2_VS_OCP_OBJ_NAME		"voltage-2-vs-ocp"
#define T2_OBJECT_NAME			"voltage-2-time"
#define V3_OBJECT_NAME			"voltage-3"
#define V3_VS_OCP_OBJ_NAME		"voltage-3-vs-ocp"
#define T3_OBJECT_NAME			"voltage-3-time"
#define V4_OBJECT_NAME			"voltage-4"
#define V4_VS_OCP_OBJ_NAME		"voltage-4-vs-ocp"
#define T4_OBJECT_NAME			"voltage-4-time"
#define SAMPLING_PERIOD_OBJ_NAME	"sampling-period"

#define V1_DEFAULT				0.1
#define T1_DEFAULT				0
#define V2_DEFAULT				0.2
#define T2_DEFAULT				0
#define V3_DEFAULT				0.3
#define T3_DEFAULT				0
#define V4_DEFAULT				0.4
#define T4_DEFAULT				0
#define SAMPLING_INT_DEFAULT	1

#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Timestamp (normalized)"
#define PLOT_VAR_EWE					"Ewe"
#define PLOT_VAR_CURRENT				"Current"
#define PLOT_VAR_ECE					"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL		"Integral d(Current)/d(time)"

QString Chronopotentiometry::GetShortName() const {
	return "Chronopotentiometry";
}
QString Chronopotentiometry::GetFullName() const {
	return "Chronopotentiometry";
}
QString Chronopotentiometry::GetDescription() const {
	return "This experiment holds the working electrode at a constant potential while recording the current and charge passed. The working electrode is sequentially poised at up to four potentials for the specified duration of time";
}
QStringList Chronopotentiometry::GetCategory() const {
	return QStringList() <<
		"Basic voltammetry";

}
ExperimentTypeList Chronopotentiometry::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap Chronopotentiometry::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}
/*
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
//*/
QWidget* Chronopotentiometry::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);

	int row = 0;
	_INSERT_RIGHT_ALIGN_COMMENT("Potential 1 = ", row, 0);
	_INSERT_TEXT_INPUT(V1_DEFAULT, V1_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to", row, 0);
	_START_DROP_DOWN(V1_VS_OCP_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("reference");
	_ADD_DROP_DOWN_ITEM("open circuit");
	_END_DROP_DOWN();

	//TODO: add hh:mm:ss format
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Duration = ", row, 0);
	_INSERT_TEXT_INPUT(T1_DEFAULT, T1_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Potential 2 = ", row, 0);
	_INSERT_TEXT_INPUT(V2_DEFAULT, V2_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to", row, 0);
	_START_DROP_DOWN(V2_VS_OCP_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("reference");
	_ADD_DROP_DOWN_ITEM("open circuit");
	_END_DROP_DOWN();

	//TODO: add hh:mm:ss format
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Duration = ", row, 0);
	_INSERT_TEXT_INPUT(T2_DEFAULT, T2_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Potential 3 = ", row, 0);
	_INSERT_TEXT_INPUT(V3_DEFAULT, V3_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to", row, 0);
	_START_DROP_DOWN(V3_VS_OCP_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("reference");
	_ADD_DROP_DOWN_ITEM("open circuit");
	_END_DROP_DOWN();

	//TODO: add hh:mm:ss format
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Duration = ", row, 0);
	_INSERT_TEXT_INPUT(T3_DEFAULT, T3_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Potential 4 = ", row, 0);
	_INSERT_TEXT_INPUT(V4_DEFAULT, V4_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("V", row, 2);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("with respect to", row, 0);
	_START_DROP_DOWN(V4_VS_OCP_OBJ_NAME, row, 1);
	_ADD_DROP_DOWN_ITEM("reference");
	_ADD_DROP_DOWN_ITEM("open circuit");
	_END_DROP_DOWN();

	//TODO: add hh:mm:ss format
	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Duration = ", row, 0);
	_INSERT_TEXT_INPUT(T4_DEFAULT, T4_OBJECT_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);

	++row;
	_INSERT_VERTICAL_SPACING(row);

	++row;
	_INSERT_RIGHT_ALIGN_COMMENT("Sampling interval: ", row, 0);
	_INSERT_TEXT_INPUT(SAMPLING_INT_DEFAULT, SAMPLING_PERIOD_OBJ_NAME, row, 1);
	_INSERT_LEFT_ALIGN_COMMENT("s", row, 2);
	
	_SET_COL_STRETCH(3, 2);
	_SET_COL_STRETCH(1, 0);
	USER_INPUT_END();
}
NodesData Chronopotentiometry::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);
	/*
	QString selectedRadio1;
	QString selectedRadio2;
	GET_SELECTED_RADIO(selectedRadio1, "Test radio 1 id");
	GET_SELECTED_RADIO(selectedRadio2, "Test radio 2 id");


	QString selectedDropDown;
	GET_SELECTED_DROP_DOWN(selectedDropDown, "Test drop down id");
	//*/

	double v1, v2, v3, v4, t1, t2, t3, t4;
	GET_TEXT_INPUT_VALUE_DOUBLE(v1, V1_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(v2, V2_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(v3, V3_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(v4, V4_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(t1, T1_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(t2, T2_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(t3, T3_OBJECT_NAME);
	GET_TEXT_INPUT_VALUE_DOUBLE(t4, T4_OBJECT_NAME);

	double dt;
	GET_TEXT_INPUT_VALUE_DOUBLE(dt, SAMPLING_PERIOD_OBJ_NAME);

	QString ocp1, ocp2, ocp3, ocp4;
	bool _ocp1, _ocp2, _ocp3, _ocp4;
	GET_SELECTED_DROP_DOWN(ocp1, V1_VS_OCP_OBJ_NAME);
	GET_SELECTED_DROP_DOWN(ocp2, V2_VS_OCP_OBJ_NAME);
	GET_SELECTED_DROP_DOWN(ocp3, V3_VS_OCP_OBJ_NAME);
	GET_SELECTED_DROP_DOWN(ocp4, V4_VS_OCP_OBJ_NAME);
	_ocp1 = ocp1.contains("open circuit");
	_ocp2 = ocp2.contains("open circuit");
	_ocp3 = ocp3.contains("open circuit");
	_ocp4 = ocp4.contains("open circuit");

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e7;
	exp.tMax = t1 * 1e8;
	getSamplingParameters(dt, &exp);
	exp.DCPoint_pot.VPointUserInput = (int)(v1 * 3276.8);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.DCPoint_pot.Imax = 32767;
	exp.DCPoint_pot.IrangeMax = RANGE0;
	exp.DCPoint_pot.Imin = 0;
	exp.DCPoint_pot.IrangeMin = RANGE7;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e7;
	exp.tMax = t2 * 1e8;
	getSamplingParameters(dt, &exp);
	exp.DCPoint_pot.VPointUserInput = (int)(v2 * 3276.8);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.DCPoint_pot.Imax = 32767;
	exp.DCPoint_pot.IrangeMax = RANGE0;
	exp.DCPoint_pot.Imin = 0;
	exp.DCPoint_pot.IrangeMin = RANGE7;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e7;
	exp.tMax = t3 * 1e8;
	getSamplingParameters(dt, &exp);
	exp.DCPoint_pot.VPointUserInput = (int)(v3 * 3276.8);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.DCPoint_pot.Imax = 32767;
	exp.DCPoint_pot.IrangeMax = RANGE0;
	exp.DCPoint_pot.Imin = 0;
	exp.DCPoint_pot.IrangeMin = RANGE7;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = false;
	exp.nodeType = DCNODE_POINT_POT;
	exp.tMin = 1e7;
	exp.tMax = t4 * 1e8;
	getSamplingParameters(dt, &exp);
	exp.DCPoint_pot.VPointUserInput = (int)(v4 * 3276.8);
	exp.DCPoint_pot.VPointVsOCP = false;
	exp.DCPoint_pot.Imax = 32767;
	exp.DCPoint_pot.IrangeMax = RANGE0;
	exp.DCPoint_pot.Imin = 0;
	exp.DCPoint_pot.IrangeMin = RANGE7;
	exp.MaxPlays = 1;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList Chronopotentiometry::GetXAxisParameters(ExperimentType type) const {
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
void Chronopotentiometry::PushNewDcData(const ExperimentalDcData &expData, DataMap &container, const CalibrationData&, const HardwareVersion &hwVersion) const {
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
void Chronopotentiometry::SaveDcDataHeader(QFile &saveFile) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}

void Chronopotentiometry::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_TIMESTAMP);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_END();
}
void Chronopotentiometry::getSamplingParameters(double t_sample, ExperimentNode_t * pNode) const
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
	pNode->samplingParams.ADCBufferSizeEven = pNode->samplingParams.ADCBufferSizeOdd = 1;
	pNode->DCSweep_pot.VStep = 1;

	/* Minimize dt */		//todo: account for dt overflows
	uint32_t dt;
	do
	{
		dt = (uint32_t)(t_sample * 1.0e8 / pNode->samplingParams.ADCBufferSizeEven);
		if (dt / dt_min > 1)
		{
			pNode->samplingParams.ADCBufferSizeEven <<= 1;
		}
	} while (dt / dt_min > 1);
	pNode->samplingParams.ADCTimerPeriod = dt;
	pNode->samplingParams.ADCBufferSizeOdd = pNode->samplingParams.ADCBufferSizeEven;
	pNode->samplingParams.PointsIgnored = pNode->samplingParams.ADCBufferSizeEven / 2;
}