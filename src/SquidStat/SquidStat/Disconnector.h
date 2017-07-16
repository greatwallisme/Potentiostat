#pragma once

#include <QObject>
#include <QList>

class Disconnector : public QObject {
public:
	Disconnector(QObject *parent) : QObject(parent) {}
	~Disconnector() {
		foreach(auto c, connections) {
			disconnect(c);
		}
	}

	Disconnector& operator <<(const QMetaObject::Connection &c) {
		connections << c;
		return *this;
	}

private:
	QList<QMetaObject::Connection> connections;
};