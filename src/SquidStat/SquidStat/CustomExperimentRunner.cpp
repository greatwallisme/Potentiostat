#include "CustomExperimentRunner.h"

#include "ExperimentUIHelper.h"

#include <ExternalStructures.h>
#include <ExperimentCalcHelper.h>

#define TOP_WIDGET_NAME_PREFIX "custom-experiment-"


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
NodesData FillNodeData(const BuilderContainer &bc, const CalibrationData &calData, const HardwareVersion &hwVersion) {
	NodesData ret;

	if (bc.type == BuilderContainer::ELEMENT) {
		auto newData = bc.elem.ptr->GetNodesData(bc.elem.input, calData, hwVersion);
		
		//TODO: setup repeats for the element
		bc.repeats;

		ret = newData;
	}
	else {
		NodesData newData;

		for (auto it = bc.elements.begin(); it != bc.elements.end(); ++it) {
			newData << FillNodeData(*it, calData, hwVersion);
		}

		//TODO: setup repeats for the container
		bc.repeats;

		ret = newData;
	}

	return ret;
}
NodesData CustomExperimentRunner::GetNodesData(QWidget*, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NodesData ret;

	ret = FillNodeData(_ce.bc, calData, hwVersion);

	return ret;
}

QStringList CustomExperimentRunner::GetXAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_AC) {
		ret <<
			PLOT_VAR_FREQ <<
			PLOT_VAR_IMP_REAL;
	}
	if (type == ET_DC) {
		ret <<
			PLOT_VAR_TIMESTAMP <<
			PLOT_VAR_TIMESTAMP_NORMALIZED <<
			PLOT_VAR_EWE <<
			PLOT_VAR_CURRENT;
	}

	return ret;
}
QStringList CustomExperimentRunner::GetYAxisParameters(ExperimentType type) const {
	QStringList ret;

	if (type == ET_AC) {
		ret <<
			PLOT_VAR_IMPEDANCE <<
			PLOT_VAR_PHASE <<
			PLOT_VAR_IMP_REAL <<
			PLOT_VAR_IMP_IMAG <<
			PLOT_VAR_NEG_IMP_IMAG;
	}
	if (type == ET_DC) {
		ret <<
			PLOT_VAR_EWE <<
			PLOT_VAR_CURRENT <<
			PLOT_VAR_ECE <<
			PLOT_VAR_CURRENT_INTEGRAL;
	}

	return ret;
}

void CustomExperimentRunner::PushNewDcData(const ExperimentalDcData &expData, DataMap &container, const CalibrationData&, const HardwareVersion&) const {
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
void CustomExperimentRunner::SaveDcDataHeader(QFile &saveFile) const {
	SAVE_DATA_HEADER_START();

	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}
void CustomExperimentRunner::SaveDcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_TIMESTAMP);
	SAVE_DATA(PLOT_VAR_TIMESTAMP_NORMALIZED);
	SAVE_DATA(PLOT_VAR_EWE);
	SAVE_DATA(PLOT_VAR_CURRENT);
	SAVE_DATA(PLOT_VAR_ECE);
	SAVE_DATA(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_END();
}

void CustomExperimentRunner::PushNewAcData(const QByteArray &expDataRaw, DataMap &container, const CalibrationData&, const HardwareVersion &hwVersion) const {
	ComplexDataPoint_t dataPoint;
	GET_COMPLEX_DATA_POINT(dataPoint, expDataRaw);

	PUSH_BACK_DATA(PLOT_VAR_FREQ, dataPoint.frequency);
	PUSH_BACK_DATA(PLOT_VAR_IMPEDANCE, dataPoint.ImpedanceMag);
	PUSH_BACK_DATA(PLOT_VAR_PHASE, dataPoint.phase);
	PUSH_BACK_DATA(PLOT_VAR_IMP_REAL, dataPoint.ImpedanceReal);
	PUSH_BACK_DATA(PLOT_VAR_IMP_IMAG, dataPoint.ImpedanceImag);
	PUSH_BACK_DATA(PLOT_VAR_NEG_IMP_IMAG, -dataPoint.ImpedanceImag);
}
void CustomExperimentRunner::SaveAcDataHeader(QFile &saveFile) const {
	SAVE_DATA_HEADER_START();

	SAVE_AC_DATA_HEADER(PLOT_VAR_FREQ);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMPEDANCE);
	SAVE_AC_DATA_HEADER(PLOT_VAR_PHASE);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMP_REAL);
	SAVE_AC_DATA_HEADER(PLOT_VAR_IMP_IMAG);
	SAVE_AC_DATA_HEADER(PLOT_VAR_NEG_IMP_IMAG);

	SAVE_DATA_HEADER_END();
}
void CustomExperimentRunner::SaveAcData(QFile &saveFile, const DataMap &container) const {
	SAVE_DATA_START();

	SAVE_DATA(PLOT_VAR_FREQ);
	SAVE_DATA(PLOT_VAR_IMPEDANCE);
	SAVE_DATA(PLOT_VAR_PHASE);
	SAVE_DATA(PLOT_VAR_IMP_REAL);
	SAVE_DATA(PLOT_VAR_IMP_IMAG);
	SAVE_DATA(PLOT_VAR_NEG_IMP_IMAG);

	SAVE_DATA_END();
}