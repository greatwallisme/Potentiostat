#include "HidCommunicator.h"

#include <Log.h>

#define RX_TIMER_INTERVAL_MS	100

#define BOOTLOADER_VID		0x04d8
#define BOOTLOADER_PID		0x003c
#define MANUFACTURER_NAME	"Admiral Instruments"
#define PRODUCT_NAME		"Squidstat HID bootloader"

static const unsigned short crc_table[16] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};
unsigned short CalculateCrc(char *data, unsigned int len) {
	unsigned int i;
	unsigned short crc = 0;

	while (len--) {
		i = (crc >> 12) ^ (*data >> 4);
		crc = crc_table[i & 0x0F] ^ (crc << 4);
		i = (crc >> 12) ^ (*data >> 0);
		crc = crc_table[i & 0x0F] ^ (crc << 4);
		data++;
	}

	return (crc & 0xFFFF);
}

#define SOH 01
#define EOT 04
#define DLE 16

QString HidCommunicator::SearchForBootloaderHidPath() {
	hid_device_info *devs = 0;
	hid_device_info *curDev = 0;
	QString bootPath;
	
	if (hid_init()) {
		return bootPath;
	}

	devs = hid_enumerate(0x0, 0x0);
	curDev = devs;
	while (curDev) {
		if ((curDev->vendor_id == BOOTLOADER_VID) &&
			(curDev->product_id == BOOTLOADER_PID) &&
			(QString::fromWCharArray(curDev->manufacturer_string) == MANUFACTURER_NAME) &&
			(QString::fromWCharArray(curDev->product_string) == PRODUCT_NAME)) {

			bootPath = curDev->path;

			LOG() << "Bootloader found!";
			break;
		}

		curDev = curDev->next;
	}
	hid_free_enumeration(devs);

	hid_exit();
	
	return bootPath;
}
HidCommunicator::HidCommunicator(const QString &hidPath, QObject *parent) :
	QObject(parent),
	handler(0)
{
	if (!hid_init()) {
		if ( handler = hid_open_path(hidPath.toLocal8Bit().data()) ) {
			LOG() << "Start working with HID";
			hid_set_nonblocking(handler, 1);

			timer = new QTimer(this);
			timer->setInterval(RX_TIMER_INTERVAL_MS);
			connect(timer, &QTimer::timeout, this, &HidCommunicator::TryToReceive);

			timer->start();
		}
		else {
			LOG() << "Unable to open path " << hidPath;
			hid_exit();
		}
	}
	else {
		LOG() << "Unable to init HID";
	}
}
HidCommunicator::~HidCommunicator() {
	if (handler) {
		timer->stop();

		hid_close(handler);
		hid_exit();

		LOG() << "HID exited";
	}
}
void HidCommunicator::SendData(const QByteArray &data) {
	if (!handler) {
		LOG() << "HID is not initialized";
		return;
	}

	QByteArray dataToSend = data;

	/*
	dataToSend.push_back(cmd);
	
	switch (cmd) {
		case PROGRAM_FLASH:
			//if (ResetHexFilePtr)
			//{
			//	if (!HexManager.ResetHexFilePointer())
			//	{
			//		// Error in resetting the file pointer
			//		return false;
			//	}
			//}
			//HexRecLen = HexManager.GetNextHexRecord(&Buff[BuffLen], (sizeof(Buff) - 5));
			//if (HexRecLen == 0)
			//{
			//	//Not a valid hex file.
			//	return false;
			//}

			//BuffLen = BuffLen + HexRecLen;
			//while (totalRecords)
			//{
			//	HexRecLen = HexManager.GetNextHexRecord(&Buff[BuffLen], (sizeof(Buff) - 5));
			//	BuffLen = BuffLen + HexRecLen;
			//	totalRecords--;
			//}
			break;

	}
	//*/

	auto crc = CalculateCrc(dataToSend.data(), dataToSend.size());
	dataToSend.push_back((char)crc);
	dataToSend.push_back((char)(crc >> 8));
	
	for (int i = 0; i < dataToSend.size(); ++i) {
		switch (dataToSend[i]) {
			case EOT:
			case SOH:
			case DLE:
				dataToSend.insert(i++, (char)DLE);
				break;

			default:
				break;
		}
	}

	dataToSend.push_front((char)SOH);
	dataToSend.push_back((char)EOT);

	if (hid_write(handler, (unsigned char*)dataToSend.data(), dataToSend.size()) < 0) {
		LOG() << "Unable to write, error: " << QString::fromWCharArray(hid_error(handler));
	}

	LOG() << "Command sent";
}
void HidCommunicator::TryToReceive() {
	if (!handler) {
		LOG() << "HID is not initialized";
		return;
	}

	static QByteArray temp(0x1000, 0x00);

	auto res = hid_read(handler, (unsigned char*)temp.data(), temp.size());

	if (res < 0) {
		LOG() << "Unable to read, error: " << QString::fromWCharArray(hid_error(handler));
		return;
	}

	if (res > 0) {
		receivedData += QByteArray(temp.data(), res);
	}

	char *ptr = temp.data();
	char *tempEnd = ptr + temp.size();

	bool escape = false;
	while (ptr != tempEnd) {
		switch (*ptr) {
			case SOH:
				if (escape) {
					escape = false;
					receivedData += *ptr;
				}
				else {
					receivedData.clear();
				}
				break;

			case EOT:
				if (escape) {
					escape = false;
					receivedData += *ptr;
				}
				else {
					if (receivedData.size() > 1) {
						uint16_t crc = receivedData[receivedData.size() - 2] & 0x00FF;
						crc |= ((receivedData[receivedData.size() - 1] << 8) & 0xFF00);

						if (CalculateCrc(receivedData.data(), receivedData.size()-2) == crc) {
							LOG() << "Packet found";
							
							receivedData.remove(receivedData.size() - 2, 2);
							if (receivedData.size()) {
								emit DataReceived(receivedData);
							}
							else {
								LOG() << "Packet is zero-sized, skip";
							}
						}
						else {
							LOG() << "Wrong CRC, skip packet";
						}
					}
					else {
						LOG() << "";
					}
				}
				break;

			case DLE:
				if (escape) {
					escape = false;
					receivedData += *ptr;
				}
				else {
					escape = true;
				}
				break;

			default:
				escape = false;
				receivedData += *ptr;
				break;
		}
		ptr++;
	}
}
/*/
bool HidCommunicator::WriteHexData(QString &hidPath, QByteArray &data) {
	HidIniter hidIniter;

	bool ret = false;

	if (!hidIniter.IsInited()) {
		return ret;
	}

	auto handle = hid_open_path(hidPath.toLocal8Bit().data());

	if (!handle) {
		return ret;
	}

	hid_set_nonblocking(handle, 1);

	;

	hid_close(handle);

	return ret;
}
//*/