#pragma once

#include <QObject>

#include "ExternalStructures.h"
#include "SerialCommunicator.h"
#include "ExperimentReader.h"
#include "AbstractExperiment.h"

class InstrumentOperator : public QObject {
	Q_OBJECT

public:
	InstrumentOperator(const InstrumentInfo&, QObject *parent = 0);
	~InstrumentOperator();

public slots:
	void StartExperiment(const NodesData&, quint8 channel = 0);
	void StopExperiment(quint8 channel = 0);
	void PauseExperiment(quint8 channel = 0);
	void ResumeExperiment(quint8 channel = 0);
	void SoftReset();

private slots:
	void ResponseReceived(ResponseID resp, quint8 channel, const QByteArray &data);

signals:
	void CalibrationDataReceived(const CalibrationData&);
	void HardwareVersionReceived(const HardwareVersion&);
	void ExperimentalDcDataReceived(quint8 channel, const ExperimentalDcData&);
	void ExperimentalAcDataReceived(quint8 channel, const QByteArray&);
	void ExperimentCompleted();
	void ExperimentPaused();
	void ExperimentResumed();
	void NodeDownloaded();

private:
	SerialCommunicator *_communicator;
};