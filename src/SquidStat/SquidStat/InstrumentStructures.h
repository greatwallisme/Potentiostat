#pragma once

#include <QList>
#include <QMetaType>

struct InstrumentInfo {
	QString portName;
	QString serial;
};
Q_DECLARE_METATYPE(InstrumentInfo)

typedef QList<InstrumentInfo> InstrumentList;
