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
					LOG() << "It it HANDSHAKE, sending response";
					communicator.SendResponse((ResponseID)UR_HANDSHAKE);
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
