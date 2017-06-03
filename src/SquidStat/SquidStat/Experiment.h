#pragma once

class QString;
class QByteArray;
class QPixmap;
class QWidget;

#include <QVariant>

class Experiment {
public:
	Experiment() {};

	virtual QString GetShortName() const = 0;
	virtual QString GetFullName() const = 0;
	virtual QString GetDescription() const = 0;
	virtual QString GetCategory() const = 0;
	virtual QPixmap GetImage() const = 0;

	virtual QWidget* CreateUserInput() const = 0;
	virtual QByteArray GetNodesData(QWidget*) const = 0;
};

Q_DECLARE_METATYPE(Experiment*)
Q_DECLARE_METATYPE(const Experiment*)

class ExperimentFactory : public QObject {
public:
	virtual Experiment* CreateExperiment(const QVariant& = QVariant()) = 0;
};

Q_DECLARE_INTERFACE(ExperimentFactory, "squidstat.ExperimentFactory")