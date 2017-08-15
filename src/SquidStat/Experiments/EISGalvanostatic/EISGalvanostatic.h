#ifndef EIS_POTENTIOSTATIC_H
#define EIS_POTENTIOSTATIC_H

#include <AbstractExperiment.h>
#include <QtWidgets/QWidget>

class EISGalvanostatic : public AbstractExperiment {
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
	void PUSH_NEW_AC_DATA_DEFINITION;

	void SaveAcDataHeader(QFile&, const ExperimentNotes &notes) const;
	void SaveAcData(QFile&, const DataMap&) const;
private:
	int hardwareVersion; //TODO: this is a placeholder member
};

#endif // EIS_GALVANOSTATIC_H
