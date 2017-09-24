#include "CustomExperimentRunner.h"

#include "ExperimentUIHelper.h"

#include <ExternalStructures.h>
#include <ExperimentCalcHelper.h>

#include <QStringList>

#define TOP_WIDGET_NAME_PREFIX "custom-experiment-"


#define PLOT_VAR_TIMESTAMP				"Timestamp"
#define PLOT_VAR_TIMESTAMP_NORMALIZED	"Elapsed time (s)"
#define PLOT_VAR_ELAPSED_TIME_HR "Elapsed time (hr)"
#define PLOT_VAR_EWE					"Working electrode (V)"
#define PLOT_VAR_CURRENT				"Current (mA)"
#define PLOT_VAR_ECE					"Counter electrode (V)"
#define PLOT_VAR_CURRENT_INTEGRAL		"Cumulative charge (mAh)"

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

	//_INSERT_RIGHT_ALIGN_COMMENT("No parameters availabe", 0, 0);

	QPushButton *editPbt;

	auto buttonLay = new QVBoxLayout;
	buttonLay->addWidget(editPbt = OBJ_NAME(PBT("Edit Experiment"), "secondary-button"));
	buttonLay->addStretch(1);

	lay->addLayout(buttonLay, 1, 0);

	connect(editPbt, &QPushButton::clicked, this, &CustomExperimentRunner::EditButtonSlot);

	_SET_ROW_STRETCH(1, 1);
	_SET_COL_STRETCH(1, 1);

	USER_INPUT_END();
}
void CustomExperimentRunner::EditButtonSlot() {
	emit EditButtonClicked(_ce);
}
void TurnBranchIndexesIntoAbsolute(NodesData &nd) {
	for (int i = 0; i < nd.size(); ++i) {
		ExperimentNode_t *exp = (ExperimentNode_t*)nd[i].data();

		if (exp->isTail && (exp->MaxPlays > 1)) {
			int16_t *ind = (int16_t*)&exp->branchHeadIndex;
			
			if (*ind < 0) {
				*ind = i + *ind;
			}
		}
	}
}
void TurnBranchIndexesIntoRelative(NodesData &nd) {
	for (int i = 0; i < nd.size(); ++i) {
		ExperimentNode_t *exp = (ExperimentNode_t*)nd[i].data();

		if (exp->isTail && (exp->MaxPlays > 1)) {
			int16_t val = exp->branchHeadIndex;
			int16_t *ind = (int16_t*)&exp->branchHeadIndex;
			*ind = val - i;
		}
	}
}
void PushDummyNode(NodesData &nd, uint32_t repeats) {
	ExperimentNode_t *head = (ExperimentNode_t*)nd.first().data();
	head->isHead = true;
	head->isTail = false;

	ExperimentNode_t exp;
	memset(&exp, 0x00, sizeof(ExperimentNode_t));
	exp.nodeType = DUMMY_NODE;
	exp.isHead = false;
	exp.isTail = true;
	exp.MaxPlays = repeats;
	*((int16_t*)&exp.branchHeadIndex) = -nd.size();

	nd << QByteArray((char*)&exp, sizeof(ExperimentNode_t));
}
void SetupBranch(NodesData &nd, uint32_t repeats) {
	ExperimentNode_t *head = (ExperimentNode_t*)nd.first().data();
	head->isHead = true;
	head->isTail = false;

	ExperimentNode_t *tail = (ExperimentNode_t*)nd.last().data();
	tail->isHead = false;
	tail->isTail = true;
	tail->MaxPlays = repeats;
	*((int16_t*)&tail->branchHeadIndex) = 1 - nd.size();
}
uint32_t GetLastMaxPlays(const NodesData &nd) {
	const ExperimentNode_t *tail = (const ExperimentNode_t*)nd.last().data();
	return tail->MaxPlays;
}
void SetLastMaxPlays(NodesData &nd, uint32_t repeats) {
	ExperimentNode_t *tail = (ExperimentNode_t*)nd.last().data();
	tail->MaxPlays = repeats;
}
NodesData FillNodeData(const BuilderContainer &bc, const CalibrationData &calData, const HardwareVersion &hwVersion) {
	NodesData ret;

	if (bc.type == BuilderContainer::ELEMENT) {
		auto newData = bc.elem.ptr->GetNodesData(bc.elem.input, calData, hwVersion);
		
		if (newData.size()) {
			TurnBranchIndexesIntoRelative(newData);

			if (bc.repeats > 1) {
				if (GetLastMaxPlays(newData) > 1) {
					PushDummyNode(newData, bc.repeats);
				}
				else if (newData.size() == 1) {
					SetLastMaxPlays(newData, bc.repeats);
				}
				else {
					SetupBranch(newData, bc.repeats);
				}
			}

			ret = newData;
		}
	}
	else {
		NodesData newData;

		for (auto it = bc.elements.begin(); it != bc.elements.end(); ++it) {
			newData << FillNodeData(*it, calData, hwVersion);
		}

		if (newData.size()) {
			if (bc.repeats > 1) {
				if (GetLastMaxPlays(newData) > 1) {
					PushDummyNode(newData, bc.repeats);
				}
				else if (newData.size() == 1) {
					SetLastMaxPlays(newData, bc.repeats);
				}
				else {
					SetupBranch(newData, bc.repeats);
				}
			}

			ret = newData;
		}
	}

	return ret;
}
NodesData CustomExperimentRunner::GetNodesData(QWidget*, const CalibrationData &calData, const HardwareVersion &hwVersion) const {
	NodesData ret;

	ret = FillNodeData(_ce.bc, calData, hwVersion);

	ExperimentNode_t exp;
	memset(&exp, 0x00, sizeof(ExperimentNode_t));
	exp.nodeType = END_EXPERIMENT_NODE;

	ret << QByteArray((char*)&exp, sizeof(ExperimentNode_t));

	TurnBranchIndexesIntoAbsolute(ret);

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
			//PLOT_VAR_TIMESTAMP <<
			PLOT_VAR_TIMESTAMP_NORMALIZED <<
      PLOT_VAR_ELAPSED_TIME_HR <<
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

void CustomExperimentRunner::PUSH_NEW_DC_DATA_DEFINITION {
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
void CustomExperimentRunner::SaveDcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
	SAVE_DATA_HEADER_START();

	//SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP);
	SAVE_DC_DATA_HEADER(PLOT_VAR_TIMESTAMP_NORMALIZED);
  SAVE_DC_DATA_HEADER(PLOT_VAR_ELAPSED_TIME_HR);
	SAVE_DC_DATA_HEADER(PLOT_VAR_EWE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT);
	SAVE_DC_DATA_HEADER(PLOT_VAR_ECE);
	SAVE_DC_DATA_HEADER(PLOT_VAR_CURRENT_INTEGRAL);

	SAVE_DATA_HEADER_END();
}
void CustomExperimentRunner::SaveDcData(QFile &saveFile, const DataMap &container) const {
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

void CustomExperimentRunner::PUSH_NEW_AC_DATA_DEFINITION {
	ComplexDataPoint_t dataPoint;
	//GET_COMPLEX_DATA_POINT(dataPoint, expDataRaw, &calData);
  double numCycles = ExperimentCalcHelperClass::calcNumberOfCycles(*dataHeader);
  dataPoint = ExperimentCalcHelperClass::AnalyzeFRA(dataHeader->frequency, ACrawdata, numACBuffers, dataHeader->gainVoltage, dataHeader->gainCurrent, numCycles, &calData, dataHeader->IRange);

	PUSH_BACK_DATA(PLOT_VAR_FREQ, dataPoint.frequency);
	PUSH_BACK_DATA(PLOT_VAR_IMPEDANCE, dataPoint.ImpedanceMag);
	PUSH_BACK_DATA(PLOT_VAR_PHASE, dataPoint.phase);
	PUSH_BACK_DATA(PLOT_VAR_IMP_REAL, dataPoint.ImpedanceReal);
	PUSH_BACK_DATA(PLOT_VAR_IMP_IMAG, dataPoint.ImpedanceImag);
	PUSH_BACK_DATA(PLOT_VAR_NEG_IMP_IMAG, -dataPoint.ImpedanceImag);
}
void CustomExperimentRunner::SaveAcDataHeader(QFile &saveFile, const ExperimentNotes &notes) const {
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