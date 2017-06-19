#ifndef CYCLICVOLTAMMETRY_H
#define CYCLICVOLTAMMETRY_H

#include <AbstractExperiment.h>
#include <QtWidgets/QWidget>

class CyclicVoltammetry : public AbstractExperiment {
public:
	QString GetShortName() const;
	QString GetFullName() const;
	QString GetDescription() const;
	QString GetCategory() const;
	QPixmap GetImage() const;

	QWidget* CreateUserInput() const;
	QByteArray GetNodesData(QWidget*, const CalibrationData&) const;
private:
	int HardwareVersion; //TODO: this is a placeholder member
	void getSlewParameters(double dVdt, ExperimentNode_t * pNode) const;

};

#endif // CYCLICVOLTAMMETRY_H
