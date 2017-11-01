#ifndef PHASE_CALIBRATION_H
#define PHASE_CALIBRATION_H

#include <AbstractExperiment.h>
#include <QtWidgets/QWidget>
#include <ExperimentCalcHelper.h>

class PhaseCalibration : public AbstractExperiment {
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
	void SaveAcDataHeader(QFile&, const ExperimentNotes&notes) const;
	void SaveAcData(QFile&, const DataMap&) const;
};

#endif // PHASE_CALIBRATION_H
