#include <QtCore/QCoreApplication>

#include "SerialCommunicator.h"

#include <Log.h>

#include <QTimer>

#include <math.h>

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);

	static SerialCommunicator communicator;
	static QTimer dataTimer;
	static ExperimentalData expData;
	expData.x = 0;
	expData.y = sin(expData.x);

	QObject::connect(&dataTimer, &QTimer::timeout, [=]() {
		//LOG() << "Send experimental data portion";
		communicator.SendResponse((ResponseID)UR_EXPERIMENTAL_DATA, 0, QByteArray((char*)&expData, sizeof(ExperimentalData)));

		expData.x += 0.001;
		expData.y = sin(expData.x);
	});

	QObject::connect(&communicator, &SerialCommunicator::CommandReceived,
		[](CommandID comm, quint8 channel, const QByteArray &data) {
			LOG() << "Received command with id" << comm;
			switch (comm) {
				case HANDSHAKE:
					LOG() << "Handshake request, sending response";
					communicator.SendResponse((ResponseID)UR_HANDSHAKE);
					break;

				case SEND_CAL_DATA:
					LOG() << "Calibration data request, sending response";
					communicator.SendResponse((ResponseID)UR_SEND_CAL_DATA, 0, QByteArray(sizeof(CalibrationData), 0xAA));
					break;

				case START_EXPERIMENT:
					LOG() << "Start experiment request, start sending experiment data";
					dataTimer.setInterval(10);
					dataTimer.start();
					break;

				case STOP_EXPERIMENT:
					LOG() << "Stop experiment request";
					dataTimer.stop();
					break;

				default:
					LOG() << "Unhandled command. Skip.";
					break;
			}
		}
	);

	communicator.Start();

	return a.exec();
}
