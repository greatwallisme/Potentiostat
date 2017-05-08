#include "Log.h"

static QObject *logParent = 0;

void SetLogSignalEmitterParent(QObject *parent) {
	logParent = parent;
}

LogSignalEmitter* GetLogSignalEmitter() {
	static LogSignalEmitter *ret = 0;

	if (0 == ret) {
		if (logParent) {
			ret = new LogSignalEmitter(logParent);
		}
	}

	return ret;
}

void LogMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	LogSignalEmitter *log = GetLogSignalEmitter();

	if (log) {
		log->SendLogEmitter(msg);
	}
}

LogSignalEmitter::LogSignalEmitter(QObject *parent) :
	QObject(parent)
{
}
void LogSignalEmitter::SendLogEmitter(const QString &str) {
	emit SendLog(str);
}