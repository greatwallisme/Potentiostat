#pragma once

#include <AbstractExperiment.h>

class ExampleExperiment : public AbstractExperiment {
public:
	QString GetShortName() const;
	QString GetFullName() const;
	QString GetDescription() const;
	QStringList GetCategory() const;
	QPixmap GetImage() const;

	QWidget* CreateUserInput() const;
	QByteArray GetNodesData(QWidget*, const CalibrationData&, const HardwareVersion&) const;

	QStringList GetXAxisParameters() const;
	QStringList GetYAxisParameters() const;
	void PushNewData(const ExperimentalData&, DataMap &, const CalibrationData&, const HardwareVersion&) const;

	void SaveDataHeader(QFile&) const;
	void SaveData(QFile&, const DataMap&) const;
};