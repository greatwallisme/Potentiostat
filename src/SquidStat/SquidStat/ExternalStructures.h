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


#define COMMAND_FRAIMING_BYTES	0xAAEE
#define RESPONSE_FRAIMING_BYTES	0xAA

#define MAX_CHANNEL_VALUE		4
#define MAX_DATA_LENGTH			2048

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

typedef struct {
	float m_DACdcP;   //DACdc V-to-bin slope, x > 0
	float m_DACdcN;   //DACdc V-to-bin slope, x < 0
	float b_DACdc;    //DACdc V-to-bin intercept
	float m_refP;     //Ref bin-to-V slope, x > 0
	float m_refN;     //Ref bin-to-V slope, x < 0
	float b_ref;      //Ref bin-to-V intercept
	float m_eweP;     //Ewe bin-to-V slope, x > 0
	float m_eweN;     //Ewe bin-to-V slope, x < 0
	float b_ewe;      //Ewe bin-to-V intercept
	float m_eceP;     //Ece bin-to-V slope, x > 0
	float m_eceN;     //Ece bin-to-V slope, x < 0
	float b_ece;      //Ece bin-to-V intercept
	float m_iP[8];    //Current bin-to-mA slope, x > 0
	float m_iN[8];    //Current bin-to-mA slope, x < 0
	float b_i[8];     //Current bin-to-mA intercept
	float m_DACac;    //DACac bin-to-mVAC slope
} cal_t;

typedef cal_t CalibrationData;

#pragma pack(pop)