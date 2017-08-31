#include "ManualExperimentRunner.h"

#include "ExperimentUIHelper.h"


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
	return QString();
}
QString ManualExperimentRunner::GetFullName() const {
	return QString();
}
QString ManualExperimentRunner::GetDescription() const {
	return QString();
}
QStringList ManualExperimentRunner::GetCategory() const {
	return QStringList();
}
ExperimentTypeList ManualExperimentRunner::GetTypes() const {
	return ExperimentTypeList();
}
QPixmap ManualExperimentRunner::GetImage() const {
	return QPixmap();
}
QWidget* ManualExperimentRunner::CreateUserInput() const {
	USER_INPUT_START(TOP_WIDGET_NAME);
	USER_INPUT_END();
}
NodesData ManualExperimentRunner::GetNodesData(QWidget*, const CalibrationData&, const HardwareVersion&) const {
	return NodesData();
}

QStringList ManualExperimentRunner::GetXAxisParameters(ExperimentType) const {
	return QStringList();
}
QStringList ManualExperimentRunner::GetYAxisParameters(ExperimentType) const {
	return QStringList();
}

void ManualExperimentRunner::PUSH_NEW_DC_DATA_DEFINITION {
}
void ManualExperimentRunner::SaveDcDataHeader(QFile&, const ExperimentNotes &notes) const {
}
void ManualExperimentRunner::SaveDcData(QFile&, const DataMap&) const {
}
void ManualExperimentRunner::PUSH_NEW_AC_DATA_DEFINITION {
}
void ManualExperimentRunner::SaveAcDataHeader(QFile&, const ExperimentNotes &notes) const {
}
void ManualExperimentRunner::SaveAcData(QFile&, const DataMap&) const {
}