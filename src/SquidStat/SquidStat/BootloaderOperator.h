#pragma once

#include "HidCommunicator.h"
#include "HexLoader.h"

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
	void FlashErased();
	void FlashProgramed();

public slots:
	void RequestBootloaderInfo();
	void RequestFirmwareCrc(const HexCrc&);
	void EraseFlash();
	void ProgramFlash(const QByteArray&);
	void JumpToApplication();

private slots:
	void DataReceived(const QByteArray&);

private:
	HidCommunicator *hc;
};