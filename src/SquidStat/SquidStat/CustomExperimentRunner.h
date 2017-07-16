#pragma once

#include "AbstractExperiment.h"
#include "ExperimentReader.h"

class CustomExperimentRunner : public AbstractExperiment {
public:
	CustomExperimentRunner(const CustomExperiment&);

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
	void SaveDcDataHeader(QFile&) const;
	void SaveDcData(QFile&, const DataMap&) const;

	void PushNewAcData(const QByteArray&, DataMap &, const CalibrationData&, const HardwareVersion&) const;
	void SaveAcDataHeader(QFile&) const;
	void SaveAcData(QFile&, const DataMap&) const;

private:
	CustomExperiment _ce;
};