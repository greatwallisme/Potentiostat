#pragma once

#include <QByteArray>
#include <QString>

namespace HexLoader {
	QByteArray ReadFile(const QString &fileName);
};