#pragma once

#include <QObject>

#include "ExternalStructures.h"
#include "SerialCommunicator.h"

class InstrumentOperator : public QObject {
	Q_OBJECT

public:
	InstrumentOperator(const InstrumentInfo&, QObject *parent = 0);
	~InstrumentOperator();

public slots:
	void RequestCalibrationData();
	void StartExperiment();

private slots:
	void ResponseReceived(ResponseID resp, quint8 channel, const QByteArray &data);

signals:
	void CalibrationDataReceived(const CalibrationData&);

private:
	SerialCommunicator *_communicator;
};