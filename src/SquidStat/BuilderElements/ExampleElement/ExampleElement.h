#pragma once

#include <AbstractBuilderElement.h>

class ExampleElement : public AbstractBuilderElement {
public:
	QString GetFullName() const;
	QStringList GetCategory() const;
	QPixmap GetImage() const;

	QWidget* CreateUserInput(UserInput&) const;
	NodesData GetNodesData(const UserInput&, const CalibrationData&, const HardwareVersion&) const;
};