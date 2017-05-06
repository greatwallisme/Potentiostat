#include <QtCore/QCoreApplication>

#include "SerialCommunicator.h"

#include <Log.h>

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);

	static SerialCommunicator communicator;

	QObject::connect(&communicator, &SerialCommunicator::CommandReceived,
		[](CommandID comm, quint8 channel, const QByteArray &data) {
			LOG() << "Received command with id" << comm;
			switch (comm) {
				case HANDSHAKE:
					LOG() << "It is HANDSHAKE, sending response";
					communicator.SendResponse((ResponseID)UR_HANDSHAKE);
					break;

				case SEND_CAL_DATA:
					LOG() << "It is calibration data request, sending response";
					communicator.SendResponse((ResponseID)UR_SEND_CAL_DATA, 0, QByteArray(sizeof(CalibrationData), 0xAA));
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
