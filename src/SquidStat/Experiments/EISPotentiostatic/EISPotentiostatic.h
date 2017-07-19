#ifndef EIS_POTENTIOSTATIC_H
#define EIS_POTENTIOSTATIC_H

#include <AbstractExperiment.h>
#include <QtWidgets/QWidget>

class EISPotentiostatic : public AbstractExperiment {
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
	void PushNewAcData(const QByteArray&, DataMap &, const CalibrationData&, const HardwareVersion&) const;

	void SaveAcDataHeader(QFile&, const ExperimentNotes &notes) const;
	void SaveAcData(QFile&, const DataMap&) const;
private:
	int hardwareVersion; //TODO: this is a placeholder member
};

#endif // EIS_POTENTIOSTATIC_H
