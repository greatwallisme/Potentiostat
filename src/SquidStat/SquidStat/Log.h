#pragma once

#include <QDebug>
#include <QDateTime>

#define LOG()		qDebug().noquote() << QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) << "|" << __FILE__ << "|"