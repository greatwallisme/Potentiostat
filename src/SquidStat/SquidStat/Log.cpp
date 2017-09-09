#include "Log.h"

#include <QRegExp>

static QObject *logParent = 0;

void SetLogSignalEmitterParent(QObject *parent) {
	logParent = parent;
}

LogSignalEmitter* GetLogSignalEmitter() {
	static LogSignalEmitter *ret = 0;

	if (0 == ret) {
		if (logParent) {
			ret = new LogSignalEmitter(logParent);
			ret->moveToThread(logParent->thread());
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
	connect(this, &LogSignalEmitter::MoveToNativeThread,
		this, &LogSignalEmitter::NativeThreadEmitter, Qt::QueuedConnection);
}
void LogSignalEmitter::SendLogEmitter(const QString &str) {
	emit MoveToNativeThread(str);
}
void LogSignalEmitter::NativeThreadEmitter(const QString &str) {
	emit SendLog(str);

	auto strs = str.split(" | ");

	auto header = strs.at(0);
	strs.removeAt(0);
	strs.removeAt(0);
	auto text = strs.join(" | ");

	emit SendLogExtended(header, text);
}