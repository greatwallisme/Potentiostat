#include <HexLoader.h>

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

struct HexRecord {
	uint8_t length;
	uint16_t address;
	HexRecordType type;
	char data[0x100];
	uint8_t crc;
};

bool ReadHexRecord(const QString &line, HexRecord &rec) {
	bool ret = false;

	static QRegExp recordRx(":([0-9a-fA-F]{2}[0-9a-fA-F]{4}[0-9a-fA-F]{2}([0-9a-fA-F]{2})*[0-9a-fA-F]{2})");

	if (-1 != recordRx.indexIn(line)) {
		QByteArray lineData = QByteArray::fromHex(line.toLocal8Bit());
		
		if (lineData.size() != (5 + lineData[0])) {
			return ret;
		}

		char *ptr = lineData.data();

		rec.length = *ptr++;

		rec.address = *ptr++;
		rec.address <<= 8;
		rec.address += ((uint16_t)*ptr++) & 0x00ff;

		rec.type = (HexRecordType)*ptr++;

		if (rec.length) {
			memcpy(rec.data, ptr, rec.length);
			ptr += rec.length;
		}

		rec.crc = *ptr++;


		uint8_t crc = 0;
		for (int i = 0; i < (lineData.size() - 1); ++i) {
			crc += lineData[i];
		}
		crc = 0x00 - crc;

		if (crc != rec.crc) {
			return ret;
		}

		ret = true;
	}

	return ret;
}

QByteArray HexLoader::ReadFile(const QString &fileName) {
	QByteArray ret;

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

		HexRecord rec;
		if (!ReadHexRecord(line, rec)) {
			ret.clear();
			return ret;
		}

		switch (rec.type) {
			case HRT_DATA: {
				uint32_t address = addrOffset + rec.address;
				uint32_t minArraySize = address + rec.length;

				uint32_t curSize = ret.size();
				if (curSize < minArraySize) {
					ret.resize(minArraySize);
					memset(ret.data() + curSize, 0x00, minArraySize - curSize);
				}

				memcpy(ret.data() + address, rec.data, rec.length);
			} break;

			case HRT_END_OF_FILE:
				needToBreak = true;
				break;

			case HRT_EXTENDED_SEGMENT_ADDRESS:
				if (rec.length != 2) {
					ret.clear();
					return ret;
				}

				addrOffset = (uint32_t)rec.data[0];
				addrOffset <<= 8;
				addrOffset += (uint32_t)rec.data[1];
				addrOffset <<= 8;
				break;

			default:
				break;
		};
	}

	hexFile.close();

	return ret;
}