#pragma once

#include <Experiment.h>

class ExampleExperiment : public Experiment {

	QString GetShortName() const;
	QString GetFullName() const;
	QString GetDescription() const;
	QString GetCategory() const;
	QPixmap GetImage() const;

	QWidget* CreateUserInput() const;
	QByteArray GetNodesData(QWidget*) const;
};