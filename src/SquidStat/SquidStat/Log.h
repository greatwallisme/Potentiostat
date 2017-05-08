#pragma once

#include <QObject>
#include <QDebug>
#include <QDateTime>

#define LOG()		qDebug().noquote() << QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) << "|" << __FILE__ << "|"

class LogSignalEmitter;

void LogMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

void SetLogSignalEmitterParent(QObject *parent);
LogSignalEmitter* GetLogSignalEmitter();

class LogSignalEmitter : public QObject {
	Q_OBJECT

public:
	LogSignalEmitter(QObject *parent = 0);

	void SendLogEmitter(const QString&);

signals:
	void SendLog(const QString&);
};