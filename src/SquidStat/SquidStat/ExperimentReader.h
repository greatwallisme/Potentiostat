#pragma once

#include <QString>
#include <QList>
#include <QByteArray>
#include <QVector>

#include "ExternalStructures.h"
#include "BuilderWidget.h"

struct NodeContainer {
	enum Type : qint32 {
		NODE,
		SET
	};

	qint32 repetition;
	Type type;

	QList<NodeContainer> elements;
	ExperimentNode_t parameters;
};

struct ExperimentContainer {
	QString shortName;
	QString name;
	QString description;
	QString imagePath;
	NodeContainer nodes;
};

namespace ExperimentReader {
	ExperimentContainer GenerateExperimentContainer(const QByteArray &jsonData);
	QList<ExperimentNode_t*> GetNodeListForUserInput(ExperimentContainer&);
	QVector<ExperimentNode_t> GetNodeArrayForInstrument(ExperimentContainer&);
};

namespace ExperimentWriter {
	QByteArray GenerateJsonObject(const QString&, const BuilderContainer&);
};