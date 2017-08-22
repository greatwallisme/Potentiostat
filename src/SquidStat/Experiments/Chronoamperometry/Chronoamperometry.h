#ifndef CHRONOAMPEROMETRY_H
#define CHRONOAMPEROMETRY_H

#include <AbstractExperiment.h>
#include <QtWidgets/QWidget>
#include <ExperimentCalcHelper.h>

class Chronoamperometry : public AbstractExperiment {
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

	void SaveDcDataHeader(QFile&, const ExperimentNotes&notes) const;
	void SaveDcData(QFile&, const DataMap&) const;
};

#endif // CHRONOAMPEROMETRY_H
