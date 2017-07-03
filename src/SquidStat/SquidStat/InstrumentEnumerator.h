#pragma once

#include <QThread>

#include "InstrumentStructures.h"

class InstrumentEnumerator : public QThread {
	Q_OBJECT

public:
	InstrumentEnumerator();

protected:
	void run();

signals:
	void RemoveDisconnectedInstruments(InstrumentList);
	void AddNewInstruments(InstrumentList);

private:
	//InstrumentList FindInstruments();
	//InstrumentList FindInstrumentsActive();

	InstrumentList instruments;
};