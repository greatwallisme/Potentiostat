#pragma once

#include <QString>
#include <QList>
#include <QByteArray>

#include "ExternalStructures.h"

struct NodeContainer {
	enum Type : qint32 {
		NODE,
		SET
	};

	qint32 count;
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
	ExperimentContainer Generate(const QByteArray &jsonData);
};