#ifndef __RADIO_RF_APP_H
#define   __RADIO_RF_APP_H

#include "sys.h"

#define HEART_INTERVAL					30								//默认心跳间隔时间

/* RF 开辟缓存大小 */
#define RF_BUFFER_SIZE					256
#define RF_PRINTF_BUFFER_SIZE				256

#define SIZE_INFRAME_Q					2
#define MRFI_RX_METRICS_SIZE				2

#define FI_AVAILABLE					0
#define FI_INUSE_UNTIL_DEL				1

#define TRF_MSG_VERSION					0x01

#define TRF_MSG_REBOOT					24
#define TRF_MSG_HEART					27

#define TRF_MSG_DEFAULT					49
#define TRF_MSG_SENSITIVITY				50
#define TRF_MSG_WORKMODE					51
#define TRF_MSG_INITBACKGROUND			52
#define TRF_MSG_BASICINFO				53
#define TRF_MSG_UPGRADE					54
#define TRF_MSG_STATUS					55
#define TRF_MSG_HEART_BOOTLOADER			56
#define TRF_MSG_RFHEART_INTERVAL			57
#define TRF_MSG_DEBUG_INFO				58
#define TRF_MSG_DEBUG_STATUS				58
#define TRF_MSG_REMOTE_SERVER				59
#define TRF_MSG_DYNAMICINFO				60
#define TRF_MSG_JSON					61
#define TRF_MSG_GENERAL_CMD				62

enum trfStatus
{
	TRF_SUCCESS,
	TRF_NO_PAYLOAD,
	TRF_BAD_PROTOCAL,
	TRF_BAD_MSGTYPE,
	TRF_NOT_FORME
};

typedef struct
{
	uint8_t							frame[255];
	uint8_t							rxMetrics[MRFI_RX_METRICS_SIZE];
}mrfiPacket_t;

typedef struct
{
	volatile uint8_t					fi_usage;
	uint32_t							orderStamp;
	mrfiPacket_t						mrfiPkt;
}frameInfo_t;

typedef __packed struct _trfmsg_h
{
	uint32_t							destSN;
	uint8_t							version;  
	uint8_t							type;
}trf_msghead_s;

typedef __packed struct _trf_msg
{
	trf_msghead_s						head;
	char								pData[1];
}trf_msg_s;

typedef __packed struct _trf_heartbeat
{
	trf_msghead_s						head;
	uint8_t							major_softver;
	uint8_t							sub_softver;
	uint8_t							DevType;
	uint8_t							sub_hardver;
	uint8_t							workmode:2;
	uint8_t							status:3;
}trf_heartbeat_s;

typedef __packed struct _trf_defaultrsp
{
	trf_msghead_s						head;
	uint8_t							ret;
}trf_defaultrsp_s;

typedef __packed struct _tmote_beat_interval
{
	trf_msghead_s						head;
	uint16_t							seconds;
}tmote_beat_interval_s;

typedef __packed struct _tmote_sensitivity
{
	trf_msghead_s						head;
	uint8_t							sensitivity;
}tmote_sensitivity_s;

typedef __packed struct _tmote_work_mode
{
	trf_msghead_s						head;
	uint8_t							mode;
}tmote_work_mode_s;

typedef __packed struct _tmote_status
{
	trf_msghead_s						head;
	uint32_t							time;
	uint16_t							count;
	uint8_t							status;
}tmote_status_s;

typedef __packed struct _tmote_debug_status
{
	trf_msghead_s						head;
	uint32_t							time;
	uint16_t							count;
	uint8_t							status;
	int16_t							mag_x;
	int16_t							mag_y;
	int16_t							mag_z;
	uint16_t							mag_diff;
	uint8_t							radar_distance;
	uint8_t							radar_strength;
	uint8_t							radar_covercount;
	uint8_t							radar_diff;
}tmote_debug_status_s;

typedef __packed struct _tmote_basic_info
{
	trf_msghead_s						head;
	uint8_t							info[240];
}tmote_basic_info_s;

typedef __packed struct _tmote_general_cmd
{
	trf_msghead_s						head;
	char								buf[16];
}tmote_general_cmd_s;

void Radio_Rf_BeforeSleep(void);
void Radio_Rf_Data_Handle_Isr(mrfiPacket_t* rfpacket);
char Radio_Rf_Receive(uint8_t *outmsg, uint8_t *len);
char Radio_Rf_Send(uint8_t *inmsg, uint8_t len);
char Radio_Rf_Operate_Recvmsg(uint8_t *inmsg, uint8_t len);

void Radio_Trf_App_Task(void);
void Radio_Trf_Cfg_Buildframe(uint8_t *inmsg, uint8_t pkttype, uint8_t pktnum, uint32_t sn, uint8_t *outsend, uint8_t len);
uint8_t Radio_Trf_Xmit_Get_Pktnum(void);
void Radio_Trf_Default_Resp(uint8_t ret, uint8_t type);

uint8_t Radio_Trf_Get_Workmode(void);
void Radio_Trf_Set_Workmode(uint8_t val);

void Radio_Trf_Xmit_Heartbeat(void);
void Radio_Trf_Do_Heartbeat(void);

void Radio_Trf_Debug_Printf(const char *fmt, ...);
void Radio_Trf_Printf(const char *fmt, ...);

#endif
