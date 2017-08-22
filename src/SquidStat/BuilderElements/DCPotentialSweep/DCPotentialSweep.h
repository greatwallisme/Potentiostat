#pragma once

#include <AbstractBuilderElement.h>
#include <ExperimentCalcHelper.h>

class DCPotentialSweepElement : public AbstractBuilderElement {
public:
	QString GetFullName() const;
	QStringList GetCategory() const;
	QPixmap GetImage() const;
	ExperimentType GetType() const;

	QWidget* CreateUserInput(UserInput&) const;
  NodesData GetNodesData(const UserInput &inputs, const CalibrationData &calData, const HardwareVersion &hwVersion) const;
private:
  uint32_t numIgnoredPoints;
  uint32_t ignoredPointsCounter;
};