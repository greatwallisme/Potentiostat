#ifndef DIFFPULSEVOLTAMMETRY_H
#define DIFFPULSEVOLTAMMETRY_H

#include <AbstractExperiment.h>
#include <QtWidgets/QWidget>

class DiffPulseVoltammetry : public AbstractExperiment {
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
private:
	int hardwareVersion; //TODO: this is a placeholder member
	void getSamplingParameters(quint32 t_period, quint32 t_pulsewidth, ExperimentNode_t * pNode) const;

};

#endif // DIFFPULSEVOLTAMMETRY_H
