#pragma once

#include <QObject>
#include <QTimer>

#include <QString>
#include <QByteArray>

#include <hidapi.h>

class HidCommunicator : public QObject {
	Q_OBJECT

public:
	HidCommunicator(const QString &path, QObject *parent);
	~HidCommunicator();

	static QString SearchForBootloaderHidPath();

signals:
	void DataReceived(const QByteArray&);

public slots:
	void SendData(const QByteArray&);

private slots:
	void TryToReceive();

private:
	hid_device *handler;
	QByteArray receivedData;

	QTimer *timer;
};