#pragma once

#include <QObject>
#include <QUuid>

class AbstractExperimentTrigger : public QObject {
public:
	AbstractExperimentTrigger(QObject *parent = 0) : QObject(parent) {}
	virtual void StopCurrentExperiment() = 0;
};

class ExperimentTrigger : public AbstractExperimentTrigger {
	Q_OBJECT

public:
	ExperimentTrigger(QObject *parent = 0) : AbstractExperimentTrigger(parent) {}

	void SetUuid(const QUuid &id) { _id = id; }

	void StopCurrentExperiment() { emit StopExperiment(_id); }

signals:
	void StopExperiment(const QUuid&);

private:
	QUuid _id;
};