#pragma once

#include <QString>
#include <QList>
#include <QByteArray>
#include <QVector>

#include "ExternalStructures.h"
#include "BuilderWidget.h"

struct CustomExperiment {
	QString name;
	QString fileName;
	QUuid id;
	BuilderContainer bc;
};

namespace ExperimentReader {
	CustomExperiment GenerateExperimentContainer(const QByteArray &jsonData);
};

namespace ExperimentWriter {
	QByteArray GenerateJsonObject(const CustomExperiment&);
};