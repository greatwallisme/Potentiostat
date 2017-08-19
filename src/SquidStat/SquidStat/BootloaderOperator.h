#pragma once

#include "HidCommunicator.h"

struct BootloaderInfo {
	uint8_t major;
	uint8_t minor;
};

class BootloaderOperator : public QObject {
	Q_OBJECT

public:
	BootloaderOperator(const QString &hidPath, QObject *parent);

signals:
	void BootloaderInfoReceived(const BootloaderInfo&);
	void FirmwareCrcReceived(uint16_t);
	void FlashEraised();
	void FlashProgrammed();

public slots:
	void RequestBootloaderInfo();
	void RequestFirmwareCrc(uint32_t startAddr, uint32_t length);
	void EraseFlash();
	void ProgrammFlash(uint16_t address, const QByteArray&);
	void JumpToApplication();

private slots:
	void DataReceived(const QByteArray&);

private:
	HidCommunicator *hc;
};