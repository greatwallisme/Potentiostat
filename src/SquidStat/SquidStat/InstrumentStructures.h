#pragma once

#include <QList>
#include <QMetaType>

#include <ExternalStructures.h>

struct InstrumentInfo {
	struct {
		QString name;
		QString serial;
	} port;
	CalibrationData calData;
	HardwareVersion hwVer;
	QString name;
};

Q_DECLARE_METATYPE(InstrumentInfo)

typedef QList<InstrumentInfo> InstrumentList;
