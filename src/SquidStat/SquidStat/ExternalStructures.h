#pragma once

#pragma pack(push, 1)

typedef enum {
	HANDSHAKE = 65,
	REPORT_STATUS,
	SEND_CAL_DATA,
	SET_OPMODE,
	MANUAL_DC_SAMPLE,
	V_SETPOINT,
	I_SETPOINT,
	SETUP_AC_SAMPLING,
	RUN_FRA,

	USB_COMMAND_LAST
} USBcommand_t;

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

	USB_RESPONCE_LAST
} USBresponse_t;

typedef USBcommand_t CommandID;
typedef USBresponse_t ResponseID;


#define COMMAND_FRAIMING_BYTES	0xFFEE
#define RESPONSE_FRAIMING_BYTES	0xFF

#define MAX_CHANNEL_VALUE		4
#define MAX_DATA_LENGTH			0xFFFF

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