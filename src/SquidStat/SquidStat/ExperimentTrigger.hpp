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
	void SetChannel(uint8_t channel) { _channel = channel; }

	void StopCurrentExperiment() { emit StopExperiment(_id); }
	void SwitchToNextFile(const QString &newName) { emit SwitchFile(_id, newName, _channel);  }

signals:
	void StopExperiment(const QUuid&);
	void SwitchFile(const QUuid&, const QString&, uint8_t);

private:
	QUuid _id;
	uint8_t _channel;
};