#ifndef __RADIO_MSG_CONFIG_H
#define   __RADIO_MSG_CONFIG_H

#include "sys.h"

/* 配置包版本号, 第一版先不考虑密码, 后续新版本里增加安全功能, 根据版本号来区分 */
#define CONFIG_VERSION					0x01

/* 配置包种类长度, 这是为了兼容升级包的格式, 这个位就留下来吧 */
#define CONFIG_TYPE_SIZE					1

/****offset & bits of the xmeshlite protocol*/
//_MSK表示哪些位是需要的, _SFT表示偏移量

#define CFG_LEN_OS						0X00	//配置包的长度偏移
#define CFG_LEN_OS_MSK					0xFF
#define CFG_LEN_OS_SFT					0X00

//Protocol_Type(0‐2bit)
#define CFG_PRE_TYPE_OS					0x00	// offset:first Byte of preoperate offset
#define CFG_PRE_TYPE_OS_MSK				0x07	// 0000 0111
#define CFG_PRE_TYPE_OS_SFT				0x00	// 
//5‐7bit
#define CFG_PRE_VS_OS					0x00	// version
#define CFG_PRE_VS_OS_MSK				0xE0	// 1110 0000
#define CFG_PRE_VS_OS_SFT				0x05	// 0000 0101

#define CFG_HEAD_TYPE_OS					0x00	//
#define CFG_HEAD_TYPE_OS_MSK				0xF0	// 1111 0000
#define CFG_HEAD_TYPE_OS_SFT				0x04	// 0000 1000

#define CFG_PKTNUM_OS					0x01	// sequence num
#define CFG_PKTNUM_OS_MSK				0xFF	// 1111 1111
#define CFG_PKTNUM_OS_SFT				0x00	//

//数据包中各个字段所占的字节长度
#define CFG_FRAME_CRC_SIZE				0x02
#define CFG_FRAME_LEN_SIZE				0x01
#define CFG_FRAME_PRE_SIZE				0x01
#define CFG_FRAME_HEAD_SIZE				0x02
#define CFG_FRAME_SN_SIZE				0x04

//数据包中各个字段的偏移
#define CFG_FRAME_LEN_OFS				0x00
#define CFG_FRAME_PRE_OFS				(CFG_FRAME_LEN_OFS + CFG_FRAME_LEN_SIZE)	//0+1
#define CFG_FRAME_HEAD_OFS				(CFG_FRAME_PRE_OFS + CFG_FRAME_PRE_SIZE)	//1+1
#define CFG_FRAME_SN_OFS					(CFG_FRAME_HEAD_OFS + CFG_FRAME_HEAD_SIZE)	//1+1+2
#define CFG_FRAME_PAYLOAD_OFS				(CFG_FRAME_SN_OFS + CFG_FRAME_SN_SIZE)		//1+1+2+4


/***address of the first byte in each filed*/
#define CFG_P_FRAME_LEN(P)				(&((P)[CFG_FRAME_LEN_OFS]))   
#define CFG_P_FRAME_PRE(P)				(&((P)[CFG_FRAME_PRE_OFS]))
#define CFG_P_FRAME_HEAD(P)				(&((P)[CFG_FRAME_HEAD_OFS]))
#define CFG_P_FRAME_SN(P)				(&((P)[CFG_FRAME_SN_OFS]))
#define CFG_P_FRAME_PAYLOAD(P)			(&((P)[CFG_FRAME_PAYLOAD_OFS]))


#define CFG_GET_PAYLOAD_LEN(P)			(((P)[CFG_FRAME_LEN_OFS]) - (CFG_FRAME_PAYLOAD_OFS))
#define CFG_GET_FROM_FRAME(b,f)			((b)[f] & (f##_MSK))>>(f##_SFT)
#define CFG_PUT_INTO_FRAME(b,f,v)			do {(b)[f] = ((b)[f] & ~(f##_MSK)) | ((v)<<(f##_SFT)); } while(0)


//通过RF发送的配置包的最大长度
#define FR_CONFIG_BUFSIZE				256

enum
{
	CONFIG_REQUEST						= 0x00,
	CONFIG_RESPONSE					= 0x01,
	TMOTE_PLAIN_SET					= 0x02,								//	2 配置器发配置命令给终端
	TMOTE_PLAIN_GET					= 0x03,								//	3 配置器发配置命令给终端
	TMOTE_PLAIN_RSP					= 0x04,								//	4 终端应答命令给配置器
	TMOTE_PLAIN_PUB					= 0x05,								//	5 终端主动上报信息给配置器,主动上报心跳包,状态包等
	TMOTE_PLAIN_ACK					= 0x06,								//	6 配置器应答信息给终端
};

#endif
