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
#include <ComPacket.h>

typedef PCcommand_t CommandID;
typedef Notifications_t ResponseID;

typedef cal_t CalibrationData;

#define COMMAND_FRAIMING_BYTES	FRAMING_WORD

#define MAX_CHANNEL_VALUE		4
#define MAX_DATA_LENGTH			2048

#pragma pack(push, 4)

struct ExperimentalData {
	float x;
	float y;
};

struct CommandPacket {
	quint16 frame;
	FramelessComPacketHeader_t hdr;
	char data[0];
};

struct ResponsePacket {
	FramedComPacketHeader_t hdr;
	char data[0];
};

#pragma pack(pop)