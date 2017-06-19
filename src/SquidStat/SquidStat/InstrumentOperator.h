#pragma once

#include <QObject>

#include "ExternalStructures.h"
#include "SerialCommunicator.h"
#include "ExperimentReader.h"

class InstrumentOperator : public QObject {
	Q_OBJECT

public:
	InstrumentOperator(const InstrumentInfo&, QObject *parent = 0);
	~InstrumentOperator();

public slots:
	void RequestCalibrationData();
	void RequestHardwareVersion();
	//void StartExperiment(QVector<ExperimentNode_t>, quint8 channel = 0);
	void StartExperiment(const QByteArray &, quint8 channel = 0);
	void StopExperiment(quint8 channel = 0);

private slots:
	void ResponseReceived(ResponseID resp, quint8 channel, const QByteArray &data);

signals:
	void CalibrationDataReceived(const CalibrationData&);
	void HardwareVersionReceived(const HardwareVersion&);
	void ExperimentalDataReceived(quint8 channel, const ExperimentalData&);
	void ExperimentCompleted();

private:
	SerialCommunicator *_communicator;
};