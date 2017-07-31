#pragma once

#include <AbstractExperiment.h>

class ExampleExperiment : public AbstractExperiment {
public:
	QString GetShortName() const;
	QString GetFullName() const;
	QString GetDescription() const;
	QStringList GetCategory() const;
	ExperimentTypeList GetTypes() const;
	QPixmap GetImage() const;

	QWidget* CreateUserInput() const;
	NodesData GetNodesData(QWidget*, const CalibrationData&, const HardwareVersion&) const;

	QStringList GetXAxisParameters(ExperimentType) const;
	QStringList GetYAxisParameters(ExperimentType) const;

	void PUSH_NEW_DC_DATA_DEFINITION;
	void SaveDcDataHeader(QFile&, const ExperimentNotes&) const;
	void SaveDcData(QFile&, const DataMap&) const;
};