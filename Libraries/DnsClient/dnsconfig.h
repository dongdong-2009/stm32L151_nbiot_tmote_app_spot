#ifndef __DNS_CONFIG_H
#define   __DNS_CONFIG_H

#include "sys.h"
#include "nbiotconfig.h"

#define DNS_COMMAND_TIMEOUT_SEC			30
#define DNS_COMMAND_FAILURE_CNT			3

/* DNS 协议栈开辟缓存大小 */
#define DNS_BUFFER_SIZE					256
#define DNS_DATASTACK_SIZE				256
#define DNS_HOSTNAME_SIZE				30
#define DNS_HOSTIP_SIZE					17

#define DNS_ANALYSIS_DATA				3
#define DNS_ANALYSIS_HOSTNAME1			"movebroad.cn"
#define DNS_ANALYSIS_HOSTNAME2			"bilibili.com"
#define DNS_ANALYSIS_HOSTNAME3			"google.com"

typedef struct DNS_SocketNetTypeDef		DNS_SocketNetTypeDef;
typedef struct DNS_ClientsTypeDef			DNS_ClientsTypeDef;

/* DNS Status */
typedef enum
{
	DNS_OK       						= 0x00,
	DNS_ERROR    						= 0x01,
}DNS_StatusTypeDef;

/* DNS Header 12Byte */
typedef struct
{
	// 2 byte ID
	unsigned short						id;
	
	// 2 byte flag broken down with bitfields
	// BYTE 1 in REVERSE ORDER											//pos
	unsigned char						rd		:1;						//7
	unsigned char						tc		:1;						//6
	unsigned char						aa		:1;						//5
	unsigned char						opcode	:4;						//4-1
	unsigned char						qr		:1;						//0
	
	// BYTE 2 in REVERSE ORDER
	unsigned char						rcode	:4;						//15-12
	unsigned char						z		:3;						//11-9
	unsigned char						ra		:1;						//8
	
	// counts, each 2 bytes
	unsigned short						q_count;
	unsigned short						ans_count;
	unsigned short						auth_count;
	unsigned short						add_count;
}DNS_HeaderTypeDef;

/* DNS Question */
typedef struct
{
	unsigned short						qtype;
	unsigned short						qclass;
}DNS_QuestionTypeDef;

/* DNS Record */
typedef struct
{
	unsigned short						name;
	unsigned short						type;
	unsigned short						class;
	unsigned int						ttl;
	unsigned short						data_len;
	unsigned char*						rdata;
}DNS_RecordTypeDef;

/* DNS Query */
typedef struct
{
	unsigned char*						name;
	DNS_QuestionTypeDef*				ques;
}DNS_QueryTypeDef;

/* DNS Analysis Data */
typedef struct
{
	unsigned char*						hostnameAddr;
	unsigned char						hostname[DNS_HOSTNAME_SIZE];
	unsigned char						hostIP[DNS_HOSTIP_SIZE];
}DNS_AnalysisDataTypeDef;

/* DNS Socket */
struct DNS_SocketNetTypeDef
{
	int								SocketHandler;
	unsigned short						LocalPort;
	unsigned short						ServerPort;
	unsigned char						ServerHost[16];
	NBIOT_ClientsTypeDef*				NBIotStack;
	DNS_StatusTypeDef					(*Open)(DNS_SocketNetTypeDef*);
	DNS_StatusTypeDef					(*Close)(DNS_SocketNetTypeDef*);
	DNS_StatusTypeDef					(*Write)(DNS_SocketNetTypeDef*, const char*, u16);
	DNS_StatusTypeDef					(*Read)(DNS_SocketNetTypeDef*, char*, u16, int*, int*);
};

/* DNS Clients */
struct DNS_ClientsTypeDef
{
	unsigned char*						Sendbuf;
	unsigned char*						Recvbuf;
	size_t							Sendbuf_size;
	size_t							Recvbuf_size;
	short							Sendlen;
	short							Recvlen;
	unsigned char*						DataProcessStack;
	size_t							DataProcessStack_size;
	unsigned short						Command_Timeout_Sec;
	unsigned short						Command_Failure_Cnt;
	int								LetterCounter;
	
	DNS_AnalysisDataTypeDef				AnalysisData[DNS_ANALYSIS_DATA];
	
	
	
	DNS_SocketNetTypeDef*				SocketStack;
	
	
	
	
	
};

/* Application Programming Interface */
void DNS_Client_Init(DNS_ClientsTypeDef* pClient, DNS_SocketNetTypeDef* NetSock);					//DNS客户端初始化




















#endif
