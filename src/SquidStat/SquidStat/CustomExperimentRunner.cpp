#include "CustomExperimentRunner.h"

#include "ExperimentUIHelper.h"

#define TOP_WIDGET_NAME_PREFIX "custom-experiment-"

CustomExperimentRunner::CustomExperimentRunner(const CustomExperiment &ce) :
	_ce(ce)
{
}

QString CustomExperimentRunner::GetShortName() const {
	return _ce.name;
}
QString CustomExperimentRunner::GetFullName() const {
	return _ce.name;
}
QString CustomExperimentRunner::GetDescription() const {
	return "This is a custom built experiment";
}
QStringList CustomExperimentRunner::GetCategory() const {
	return QStringList() << "Custom";
}

void FillUniqueContainerTypes(const BuilderContainer &bc, ExperimentTypeList &types) {
	if (bc.type == BuilderContainer::ELEMENT) {
		if (!types.contains(bc.elem.ptr->GetType())) {
			types << bc.elem.ptr->GetType();
		}
		return;
	}

	for (auto it = bc.elements.begin(); it != bc.elements.end(); ++it) {
		FillUniqueContainerTypes(*it, types);
	}
}
ExperimentTypeList CustomExperimentRunner::GetTypes() const {
	ExperimentTypeList ret;
	
	FillUniqueContainerTypes(_ce.bc, ret);

	return ret;
}
QPixmap CustomExperimentRunner::GetImage() const {
	return QPixmap(":/GUI/Resources/experiment.png");
}

QWidget* CustomExperimentRunner::CreateUserInput() const {
	QString topWidgetName = TOP_WIDGET_NAME_PREFIX + _ce.fileName;
	USER_INPUT_START(topWidgetName);

	_INSERT_RIGHT_ALIGN_COMMENT("No parameters availabe", 0, 0);

	_SET_ROW_STRETCH(1, 1);
	_SET_COL_STRETCH(1, 1);

	USER_INPUT_END();
}
NodesData CustomExperimentRunner::GetNodesData(QWidget*, const CalibrationData&, const HardwareVersion&) const {
	return NodesData();
}

QStringList CustomExperimentRunner::GetXAxisParameters(ExperimentType) const {
	return QStringList();
}
QStringList CustomExperimentRunner::GetYAxisParameters(ExperimentType) const {
	return QStringList();
}

void CustomExperimentRunner::PushNewDcData(const ExperimentalDcData&, DataMap &, const CalibrationData&, const HardwareVersion&) const {

}
void CustomExperimentRunner::SaveDcDataHeader(QFile&) const {

}
void CustomExperimentRunner::SaveDcData(QFile&, const DataMap&) const {

}

void CustomExperimentRunner::PushNewAcData(const QByteArray&, DataMap &, const CalibrationData&, const HardwareVersion&) const {

}
void CustomExperimentRunner::SaveAcDataHeader(QFile&) const {

}
void CustomExperimentRunner::SaveAcData(QFile&, const DataMap&) const {

}