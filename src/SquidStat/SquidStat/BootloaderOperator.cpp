#include "BootloaderOperator.h"

#include "Log.h"

enum Command : unsigned char {
	READ_BOOT_INFO = 1,
	ERASE_FLASH,
	PROGRAM_FLASH,
	READ_CRC,
	JMP_TO_APP
};

BootloaderOperator::BootloaderOperator(const QString &hidPath, QObject *parent) :
	QObject(parent)
{
	hc = new HidCommunicator(hidPath, this);

	connect(hc, &HidCommunicator::DataReceived, this, &BootloaderOperator::DataReceived);
}
void BootloaderOperator::DataReceived(const QByteArray &packet) {
	Command *cmd = (Command*)packet.data();

	switch (*cmd) {
		case READ_BOOT_INFO:
			if ( packet.size() == (sizeof(Command) + sizeof(BootloaderInfo)) ) {
				BootloaderInfo info;
				memcpy(&info, packet.data() + sizeof(Command), sizeof(BootloaderInfo));
				
				emit BootloaderInfoReceived(info);
			}
			else {
				LOG() << "Wrong packet length on READ_BOOT_INFO";
			}
			break;

		case READ_CRC:
			if (packet.size() == (sizeof(Command) + sizeof(uint16_t))) {
				uint16_t crc;
				memcpy(&crc, packet.data() + sizeof(Command), sizeof(uint16_t));
			
				emit FirmwareCrcReceived(crc);
			}
			else {
				LOG() << "Wrong packet length on READ_CRC";
			}
			break;

		case ERASE_FLASH:
			emit FlashErased();
			break;

		case PROGRAM_FLASH:
			emit FlashProgramed();
			break;

		default:
			LOG() << "Unknown response";
			break;
	}
}
void BootloaderOperator::RequestBootloaderInfo() {
	QByteArray dataToSend;
	dataToSend.push_back(READ_BOOT_INFO);

	hc->SendData(dataToSend);
}
void BootloaderOperator::RequestFirmwareCrc(const HexCrc &hexCrc) {
	QByteArray dataToSend;
	dataToSend.push_back(READ_CRC);

	struct {
		uint32_t startAddr;
		uint32_t len;
		uint16_t crc;
	} data;

	data.startAddr = hexCrc.start;
	data.len = hexCrc.length;
	data.crc = hexCrc.crc;

	dataToSend += QByteArray((char*)&data, sizeof(data));

	hc->SendData(dataToSend);
}
void BootloaderOperator::EraseFlash() {
	QByteArray dataToSend;
	dataToSend.push_back(ERASE_FLASH);

	hc->SendData(dataToSend);
}
void BootloaderOperator::JumpToApplication() {
	QByteArray dataToSend;
	dataToSend.push_back(JMP_TO_APP);

	hc->SendData(dataToSend);
}
void BootloaderOperator::ProgramFlash(const QByteArray &records) {
	QByteArray dataToSend;
	dataToSend.push_back(PROGRAM_FLASH);

	dataToSend += records;
	hc->SendData(dataToSend);
}