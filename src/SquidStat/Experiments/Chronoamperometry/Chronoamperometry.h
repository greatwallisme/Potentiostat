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
	
	QStringList GetXAxisParameters() const;
	QStringList GetYAxisParameters() const;
	void PushNewDcData(const ExperimentalDcData&, DataMap &, const CalibrationData&, const HardwareVersion&) const;

	void SaveDcDataHeader(QFile&) const;
	void SaveDcData(QFile&, const DataMap&) const;
private:
	int hardwareVersion; //TODO: this is a placeholder member

};

#endif // CHRONOAMPEROMETRY_H