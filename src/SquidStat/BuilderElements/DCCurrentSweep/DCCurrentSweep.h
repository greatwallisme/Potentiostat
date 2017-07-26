#pragma once

#include <AbstractBuilderElement.h>
#include <ExperimentCalcHelper.h>

class DCCurrentSweep : public AbstractBuilderElement {
public:
	QString GetFullName() const;
	QStringList GetCategory() const;
	QPixmap GetImage() const;
	ExperimentType GetType() const;

	QWidget* CreateUserInput(UserInput&) const;
	NodesData GetNodesData(const UserInput&, const CalibrationData&, const HardwareVersion&) const;
};