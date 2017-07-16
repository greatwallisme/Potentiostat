#pragma once

#include <QMap>
#include <QMetaType>
#include <ExternalStructures.h>
#include <AbstractExperiment.h>

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QPixmap>
#include <QVariant>

typedef QMap<QString, QVariant> UserInput;

class AbstractBuilderElement {
public:
	AbstractBuilderElement() {};

	virtual QString GetFullName() const = 0;
	virtual QStringList GetCategory() const = 0;
	virtual QPixmap GetImage() const = 0;
	virtual ExperimentType GetType() const = 0;

	virtual QWidget* CreateUserInput(UserInput&) const = 0;
	//virtual UserInput GetUserInput(QWidget*) const = 0;
	virtual NodesData GetNodesData(const UserInput&, const CalibrationData&, const HardwareVersion&) const = 0;
};

Q_DECLARE_METATYPE(AbstractBuilderElement*)
Q_DECLARE_METATYPE(const AbstractBuilderElement*)
