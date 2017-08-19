#pragma once

#include <QByteArray>
#include <QString>

typedef QList<QByteArray> HexRecords;

struct HexCrc{
	uint32_t start;
	uint32_t length;
	uint16_t crc;
};

namespace HexLoader {
	HexRecords ReadFile(const QString &fileName);
	HexCrc CalculateCrc(const HexRecords&);
};