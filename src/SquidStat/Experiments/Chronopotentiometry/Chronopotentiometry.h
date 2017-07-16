#ifndef CHRONOPOTENTIOMETRY_H
#define CHRONOPOTENTIOMETRY_H

#include <AbstractExperiment.h>
#include <QtWidgets/QWidget>

class Chronopotentiometry : public AbstractExperiment {
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

	void SaveDcDataHeader(QFile&) const;
	void SaveDcData(QFile&, const DataMap&) const;
private:
	int hardwareVersion; //TODO: this is a placeholder member
	void getSamplingParameters(double t_period, ExperimentNode_t * pNode) const;

};

#endif // CHRONOPOTENTIOMETRY_H
