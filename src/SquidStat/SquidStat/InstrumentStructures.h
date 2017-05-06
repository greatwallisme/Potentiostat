#pragma once

#include <QList>

struct InstrumentInfo {
	QString portName;
	QString serial;
};

typedef QList<InstrumentInfo> InstrumentList;
