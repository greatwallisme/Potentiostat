#pragma once

#include <qglobal.h>

#include <global_typedefs.h>
#include <cal.h>
#include <ExperimentNode.h>
#include <DAC_ADC_Buffer_types.h>

typedef PCcommand_t CommandID;
typedef Notifications_t ResponseID;

typedef cal_t CalibrationData;
typedef legacy_cal_t LegacyCalibrationData;

typedef QList<QByteArray> NodesData;

#define COMMAND_FRAIMING_BYTES	FRAMING_WORD

#define MAX_CHANNEL_VALUE		4
#define MAX_DATA_LENGTH			8192

#pragma pack(push, 4)

struct HardwareVersion {
	union {
		struct {
			HardwareModel_t hwModel;
			char hwName[0];
		};
		uint8_t rawData[HW_DATA_LENGTH];
	};
};

typedef ExperimentalDataPoint_t ExperimentalDcData;

struct ProcessedDCData {
  double EWE;
  double ECE;
  double current;
};

struct CommandPacket {
	uint16_t frame;
	FramelessComPacketHeader_t hdr;
	char data[0];
};

typedef struct
{
	uint16_t frame = 0xFFEE;
	Notifications_t returnCode;
	uint8_t channelNum;
	uint16_t dataLength;
}FramedComPacketHeader_t;

struct ResponsePacket {
	FramedComPacketHeader_t hdr;
	char data[0];
};

namespace Manual {
	struct SamplingParams {
		uint8_t timerDiv;
		uint32_t timerPeriod;
		uint16_t ADCbufsize;
	};

	struct GalvanoSetpoint {
		int16_t g_setpoint;
		uint8_t range;
	};

	struct PotentioSetpoint {
		int16_t setpoint;
	};

	struct RangingMode {
		uint8_t currentRangeMode;
	};
}

#pragma pack(pop)