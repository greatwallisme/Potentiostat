#pragma once

typedef enum {
	EDGE_SAMPLING,
	PERIODIC_SAMPLING,
	DYNAMIC_SAMPLING,
	PERIODIC_SAMPLING_STATIC_DAC
} DCSamplingMode_t;

#include <global_typedefs.h>
#include <PotentiostatStruct.h>
#include <ExperimentNodeStruct.h>
#include <BareMetalPeripherals/Baremetal_SignalGenStruct.h>

typedef enum {
	UR_HANDSHAKE = 65,
	UR_REPORT_STATUS,
	UR_SEND_CAL_DATA,
	UR_SET_OPMODE,
	UR_MANUAL_DC_SAMPLE,
	UR_V_SETPOINT,
	UR_I_SETPOINT,
	UR_SETUP_AC_SAMPLING,
	UR_RUN_FRA,
	UR_EXPERIMENTAL_DATA,

	USB_RESPONCE_LAST
} USBresponse_t;

typedef USBcommand_t CommandID;
typedef USBresponse_t ResponseID;

typedef cal_t CalibrationData;

#define COMMAND_FRAIMING_BYTES	0xFFEE
#define RESPONSE_FRAIMING_BYTES	0xFF

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