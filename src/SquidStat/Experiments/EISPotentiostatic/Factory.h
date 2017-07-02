#pragma once

#include <ExperimentFactoryInterface.h>

class Factory : public QObject, public ExperimentFactoryInterface {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "ExperimentFactoryInterface")
	Q_INTERFACES(ExperimentFactoryInterface)

public:
	AbstractExperiment* CreateExperiment(const QVariant& = QVariant());
};