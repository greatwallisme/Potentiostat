#ifndef COM_PACKET_H
#define COM_PACKET_H

#pragma pack(push, 1)

/*typedef struct
{
	PCcommand_t command;
	uint8_t channelNum;
	uint16_t dataLength;
}FramelessComPacketHeader_t;*/

typedef struct
{
	uint16_t frame = 0xEEFF;
	Notifications_t returnCode;
	uint8_t channelNum;
	uint16_t dataLength;
}FramedComPacketHeader_t;

#pragma pack(pop)

#endif
