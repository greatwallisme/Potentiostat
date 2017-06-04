#pragma once

#include <QVariant>

#include <AbstractExperiment.h>

class ExperimentFactoryInterface {
public:
	virtual ~ExperimentFactoryInterface() {}
	virtual AbstractExperiment* CreateExperiment(const QVariant& = QVariant()) = 0;
};

Q_DECLARE_INTERFACE(ExperimentFactoryInterface, "ExperimentFactoryInterface")
