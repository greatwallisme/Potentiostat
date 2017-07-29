#pragma once

#include <AbstractExperiment.h>
#include <ExperimentCalcHelper.h>

class ConstantPower : public AbstractExperiment {
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

	void PushNewDcData(const ExperimentalDcData&, DataMap &, const CalibrationData&, const HardwareVersion&) const;
	void SaveDcDataHeader(QFile&, const ExperimentNotes&notes) const;
	void SaveDcData(QFile&, const DataMap&) const;
};