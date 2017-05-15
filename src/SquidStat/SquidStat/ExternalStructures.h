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

typedef PCcommand_t CommandID;
typedef Notifications_t ResponseID;

typedef cal_t CalibrationData;

#define COMMAND_FRAIMING_BYTES	FRAMING_WORD
#define RESPONSE_FRAIMING_BYTES	FRAMING_UBYTE

#define MAX_CHANNEL_VALUE		4
#define MAX_DATA_LENGTH			2048

#pragma pack(push, 4)

struct ExperimentalData {
	float x;
	float y;
};

struct CommandPacket {
	quint16 fraiming;
	quint8 comm;
	quint8 channel;
	quint16 len;
	char data[0];
};

struct ResponsePacket {
	quint8 fraiming;
	quint8 comm;
	quint8 channel;
	quint16 len;
	char data[0];
};

#pragma pack(pop)