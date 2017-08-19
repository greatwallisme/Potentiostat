#include "HexLoader.h"

#include "CrcCalculator.h"

#include <QFileInfo>
#include <QRegExp>

enum HexRecordType : uint8_t {
	HRT_DATA = 0,
	HRT_END_OF_FILE,
	HRT_EXTENDED_SEGMENT_ADDRESS,
	HRT_START_SEGMENT_ADDRESS,
	HRT_EXTENDED_LINEAR_ADDRESS,
	HRT_START_LINEAR_ADDRESS
};


struct RecordHeader {
	uint8_t length;
	uint8_t address[2];
	HexRecordType type;
	char data[0];
};

bool ReadHexRecord(const QString &line, QByteArray &rec) {
	bool ret = false;

	static QRegExp recordRx(":([0-9a-fA-F]{2}[0-9a-fA-F]{4}[0-9a-fA-F]{2}([0-9a-fA-F]{2})*[0-9a-fA-F]{2})");

	if (-1 == recordRx.indexIn(line)) {
		return ret;
	}

	QByteArray lineData = QByteArray::fromHex(line.toLocal8Bit());

	if (lineData.size() != (5 + lineData[0])) {
		return ret;
	}

	char *ptr = lineData.data();
	char *ptrEnd = ptr + lineData.size();

	uint8_t crc = 0;
	while (ptr != ptrEnd) {
		crc += *ptr++;
	}

	if (crc) {
		return ret;
	}

	rec = lineData;
	ret = true;

	return ret;
}

HexRecords HexLoader::ReadFile(const QString &fileName) {
	HexRecords ret;

	QFileInfo fi(fileName);

	if (!fi.isReadable()) {
		return ret;
	}

	QFile hexFile(fileName);

	if (!hexFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return ret;
	}

	uint32_t addrOffset = 0;
	bool needToBreak = false;
	while ( (!hexFile.atEnd()) && (!needToBreak) ) {
		QString line = hexFile.readLine();

		QByteArray rec;
		if (!ReadHexRecord(line, rec)) {
			ret.clear();
			return ret;
		}

		ret << rec;
	}

	hexFile.close();

	return ret;
}

HexCrc HexLoader::CalculateCrc(const HexRecords &hex) {
	HexCrc ret = {0xffffffff, 0, 0};

	uint32_t segAddr = 0;
	uint32_t linAddr = 0;

	QByteArray flash;

	for (auto it = hex.begin(); it != hex.end(); ++it) {
		RecordHeader *hdr = (RecordHeader*)it->data();

		bool needToBreak = false;
		switch (hdr->type) {
			case HRT_DATA: {
				uint32_t address = hdr->address[0];
				address <<= 8;
				address += ((uint32_t)hdr->address[1]) & 0x000000FF;
				address += segAddr + linAddr;

				if (address < ret.start) {
					ret.start = address;
				}

				uint32_t minArraySize = address + hdr->length;

				uint32_t curSize = flash.size();
				if (curSize < minArraySize) {
					flash.resize(minArraySize);
					memset(flash.data() + curSize, 0xFF, minArraySize - curSize);
				}

				memcpy(flash.data() + address, hdr->data, hdr->length);
			} break;
			
			case HRT_END_OF_FILE:
				needToBreak = true;
				break;

			case HRT_EXTENDED_SEGMENT_ADDRESS:
				linAddr = 0;

				segAddr  = ((uint32_t)hdr->data[0]) & 0x000000FF;
				segAddr <<= 8;
				segAddr += ((uint32_t)hdr->data[1]) & 0x000000FF;
				segAddr <<= 4;
				break;

			case HRT_EXTENDED_LINEAR_ADDRESS:
				segAddr = 0;

				linAddr = ((uint32_t)hdr->data[0]) & 0x000000FF;
				linAddr <<= 8;
				linAddr += ((uint32_t)hdr->data[1]) & 0x000000FF;
				linAddr <<= 16;
				break;
			
			default:
				break;
		}

		if (needToBreak) {
			break;
		}
	}

	ret.length = flash.size();

	ret.crc = CrcCalculator::Get16(flash.data(), flash.length());

	return ret;
}