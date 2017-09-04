#include "ManualExperimentRunner.h"

#include "ExperimentUIHelper.h"

#include <QGroupBox>


#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Timestamp (normalized)"
#define PLOT_VAR_EWE					"Ewe"
#define PLOT_VAR_CURRENT				"Current"
#define PLOT_VAR_ECE					"Ece"
#define PLOT_VAR_CURRENT_INTEGRAL		"Integral d(Current)/d(time)"

#define TOP_WIDGET_NAME "manual-mode-experiment"

ManualExperimentRunner* ManualExperimentRunner::Instance() {
	static ManualExperimentRunner *ret = 0;

	if (ret == 0) {
		ret = new ManualExperimentRunner();
	}

	return ret;
}
ManualExperimentRunner::ManualExperimentRunner() {

}
QString ManualExperimentRunner::GetShortName() const {
	return "Manual experiment";
}
QString ManualExperimentRunner::GetFullName() const {
	return "Manual experiment";
}
QString ManualExperimentRunner::GetDescription() const {
	return QString();
}
QStringList ManualExperimentRunner::GetCategory() const {
	return QStringList();
}
ExperimentTypeList ManualExperimentRunner::GetTypes() const {
	return ExperimentTypeList() << ET_DC;
}
QPixmap ManualExperimentRunner::GetImage() const {
	return QPixmap();
}
QWidget* ManualExperimentRunner::CreateUserInput() const {
	auto *ret = WDG();
	OBJ_NAME(ret, TOP_WIDGET_NAME);
	auto *lay = NO_SPACING(NO_MARGIN(new QVBoxLayout(ret)));

	auto advOptionsGroup = OBJ_NAME(new QGroupBox("Advanced options (optional)"), "collapsible-group-box");
	advOptionsGroup->setCheckable(true);

	lay->addWidget(advOptionsGroup);

	return ret;
}
NodesData ManualExperimentRunner::GetNodesData(QWidget *wdg, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NODES_DATA_START(wdg, TOP_WIDGET_NAME);

	exp.isHead = true;
	exp.isTail = false;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 100000;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	exp.samplingParams.ADCTimerDiv = 3;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSizeEven = 20;
	exp.samplingParams.ADCBufferSizeOdd = 20;
	exp.samplingParams.PointsIgnored = 0;
	exp.samplingParams.DACMultEven = 5;
	exp.samplingParams.DACMultOdd = 5;
	exp.DCSweep_pot.VStartUserInput = 0;
	exp.DCSweep_pot.VStartVsOCP = false; //todo: get user input here
	exp.DCSweep_pot.VEndUserInput = 1024;
	exp.DCSweep_pot.VEndVsOCP = false; //todo: get user input here
	exp.DCSweep_pot.VStep = 1;
	exp.DCSweep_pot.Imax = 1e10;
	exp.MaxPlays = 3;
	PUSH_NEW_NODE_DATA();

	exp.isHead = false;
	exp.isTail = true;
	exp.nodeType = DCNODE_SWEEP_POT;
	exp.tMin = 100000;
	exp.tMax = 0xFFFFFFFFFFFFFFFF;
	exp.samplingParams.ADCTimerDiv = 2;
	exp.samplingParams.ADCTimerPeriod = 15625;
	exp.samplingParams.ADCBufferSizeEven = 20;
	exp.samplingParams.ADCBufferSizeOdd = 20;
	exp.samplingParams.PointsIgnored = 0;
	exp.samplingParams.DACMultEven = 10;
	exp.samplingParams.DACMultOdd = 10;
	exp.DCSweep_pot.VStartUserInput = 0;
	exp.DCSweep_pot.VStartVsOCP = false; //todo: get user input here
	exp.DCSweep_pot.VEndUserInput = 512;
	exp.DCSweep_pot.VEndVsOCP = false; //todo: get user input here
	exp.DCSweep_pot.VStep = 1;
	exp.DCSweep_pot.Imax = 1e10;
	exp.MaxPlays = 1;
	exp.branchHeadIndex = 0;
	PUSH_NEW_NODE_DATA();

	exp.nodeType = END_EXPERIMENT_NODE;
	PUSH_NEW_NODE_DATA();

	NODES_DATA_END();
}

QStringList ManualExperimentRunner::GetXAxisParameters(ExperimentType type) const {
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
QStringList ManualExperimentRunner::GetYAxisParameters(ExperimentType type) const {
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

void ManualExperimentRunner::PUSH_NEW_DC_DATA_DEFINITION{
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
void ManualExperimentRunner::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}
void ManualExperimentRunner::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_TIMESTAMP);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_END();
}
void ManualExperimentRunner::PUSH_NEW_AC_DATA_DEFINITION {
}
void ManualExperimentRunner::SaveAcDataHeader(QFile&, const ExperimentNotes &notes) const {
}
void ManualExperimentRunner::SaveAcData(QFile&, const DataMap&) const {
}