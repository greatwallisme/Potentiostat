#pragma once

typedef enum {
	EDGE_SAMPLING,
	PERIODIC_SAMPLING,
	DYNAMIC_SAMPLING,
	PERIODIC_SAMPLING_STATIC_DAC
} DCSamplingMode_t;

#include <global_typedefs.h>
#include <cal.h>
#include <ExperimentNode.h>
//#include <ComPacket.h>
#include <DAC_ADC_Buffer_types.h>

typedef PCcommand_t CommandID;
typedef Notifications_t ResponseID;

typedef cal_t CalibrationData;

#define COMMAND_FRAIMING_BYTES	FRAMING_WORD

#define MAX_CHANNEL_VALUE		4
#define MAX_DATA_LENGTH			2048

#pragma pack(push, 1)

struct HardwareVersion {
	union {
		HardwareModel_t hwModel;
		uint8_t rawData[HW_DATA_LENGTH];
	};
};

//*
typedef ExperimentalDataPoint_t ExperimentalData;
/*/
struct ExperimentalData {
	ADCdc_datastruct_t ADCrawData;
	currentRange_t currentRange;
	uint64_t timestamp;
};
//*/

struct CommandPacket {
	quint16 frame;
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

#pragma pack(pop)