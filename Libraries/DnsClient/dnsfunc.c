/**
  *********************************************************************************************************
  * @file    dnsfunc.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-05-15
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "dnsfunc.h"
#include "string.h"
#include "stdlib.h"

#define BigLittleSwap16(A)	((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))
#define BigLittleSwap32(A)	((((uint32_t)(A) & 0xff000000) >> 24) | (((uint32_t)(A) & 0x00ff0000) >> 8) | (((uint32_t)(A) & 0x0000ff00) << 8) | (((uint32_t)(A) & 0x000000ff) << 24))

/**********************************************************************************************************
 @Function			static int checkCPUendian(void)
 @Description			checkCPUendian			: 本机大端返回1,小端返回0
 @Input				void
 @Return				int
**********************************************************************************************************/
static int checkCPUendian(void)
{
	union {
		unsigned long int i;
		unsigned char s[4];
	}c;
	
	c.i = 0x12345678;
	
	return (0x12 == c.s[0]);
}

/**********************************************************************************************************
 @Function			unsigned long int htonl(unsigned long int h)
 @Description			htonl				: 本机字节序转网络字节序(u32)
 @Input				unsigned long int
 @Return				unsigned long int
**********************************************************************************************************/
unsigned long int htonl(unsigned long int h)
{
	return checkCPUendian() ? h : BigLittleSwap32(h);
}

/**********************************************************************************************************
 @Function			unsigned long int ntohl(unsigned long int n)
 @Description			ntohl				: 网络字节序转本机字节序(u32)
 @Input				unsigned long int
 @Return				unsigned long int
**********************************************************************************************************/
unsigned long int ntohl(unsigned long int n)
{
	return checkCPUendian() ? n : BigLittleSwap32(n);
}

/**********************************************************************************************************
 @Function			unsigned short int htons(unsigned short int h)
 @Description			htons				: 本机字节序转网络字节序(u16)
 @Input				unsigned short int
 @Return				unsigned short int
**********************************************************************************************************/
unsigned short int htons(unsigned short int h)
{
	return checkCPUendian() ? h : BigLittleSwap16(h);
}

/**********************************************************************************************************
 @Function			unsigned short int ntohs(unsigned short int n)
 @Description			ntohl				: 网络字节序转本机字节序(u16)
 @Input				unsigned short int
 @Return				unsigned short int
**********************************************************************************************************/
unsigned short int ntohs(unsigned short int n)
{
	return checkCPUendian() ? n : BigLittleSwap16(n);
}

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNS_Seperate(DNS_ClientsTypeDef* pClient, unsigned char* name, unsigned char* host)
 @Description			DNS_Seperate			: 将主机名格式化为DNS查询格式
 @Input				pClient				: DNS客户端实例
					name					: DNS查询格式名
					host					: 查询主机名
 @Return				DNS_StatusTypeDef		: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNS_Seperate(DNS_ClientsTypeDef* pClient, unsigned char* name, unsigned char* host)
{
	DNS_StatusTypeDef DNSStatus = DNS_OK;
	char *tokens = strtok((char *)host, ".");
	
	while (1) {
		if (tokens == NULL) {
			break;
		}
		else {
			char *i;
			name[pClient->LetterCounter] = (unsigned char)strlen(tokens);
			pClient->LetterCounter++;
			for (i = tokens; *i != NULL; i++) {
				name[pClient->LetterCounter] = *i;
				pClient->LetterCounter++;
			}
		}
		tokens = strtok(NULL, ".");
	}
	
	name[pClient->LetterCounter] = '\0';
	
	return DNSStatus;
}

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNS_ConvertIP(DNS_ClientsTypeDef* pClient, int ip, unsigned char* hostname)
 @Description			DNS_ConvertIP					: 转换并填入IP地址
 @Input				pClient						: DNS客户端实例
					ip							: 待转换IP
					hostname						: DNS查询主机名
 @Return				DNS_StatusTypeDef				: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNS_ConvertIP(DNS_ClientsTypeDef* pClient, int ip, unsigned char* hostname)
{
	DNS_StatusTypeDef DNSStatus = DNS_ERROR;
	unsigned char bytes[4];
	
	bytes[0] = ip & 0xFF;
	bytes[1] = (ip >> 8) & 0xFF;
	bytes[2] = (ip >> 16) & 0xFF;
	bytes[3] = (ip >> 24) & 0xFF;
	
	for (unsigned char i = 0; i < DNS_ANALYSIS_DATA; i++) {
		if (strcmp((char *)pClient->AnalysisData[i].hostname, (char *)hostname) == 0) {
			memset((void*)pClient->AnalysisData[i].hostIP, 0, sizeof(pClient->AnalysisData[i].hostIP));
			sprintf((char *)pClient->AnalysisData[i].hostIP, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
			DNSStatus = DNS_OK;
		}
	}
	
	return DNSStatus;
}

/**********************************************************************************************************
 @Function			unsigned char* DNS_GetHostIP(DNS_ClientsTypeDef* pClient, unsigned char* hostname)
 @Description			DNS_GetHostIP					: 获取转换IP地址
 @Input				pClient						: DNS客户端实例
					hostname						: DNS查询主机名
 @Return				hospIP						: DNS解析IP
**********************************************************************************************************/
unsigned char* DNS_GetHostIP(DNS_ClientsTypeDef* pClient, unsigned char* hostname)
{
	for (unsigned char i = 0; i < DNS_ANALYSIS_DATA; i++) {
		if (strcmp((char *)pClient->AnalysisData[i].hostname, (char *)hostname) == 0) {
			
			return pClient->AnalysisData[i].hostIP;
		}
	}
	
	return pClient->AnalysisData[0].hostIP;
}

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNSSerialize_dnsDataStructure(DNS_ClientsTypeDef* pClient, unsigned char* hostname)
 @Description			DNSSerialize_dnsDataStructure		: 序列化DNS数据结构体
 @Input				pClient						: DNS客户端实例
					hostname						: DNS查询主机名
 @Return				DNS_StatusTypeDef				: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNSSerialize_dnsDataStructure(DNS_ClientsTypeDef* pClient, unsigned char* hostname)
{
	DNS_StatusTypeDef DNSStatus = DNS_OK;
	int length = 0;
	uint16_t ID_q;
	unsigned char* queryname;
	DNS_QuestionTypeDef* queryinfo;
	
	/* Clean Letter Counter */
	pClient->LetterCounter = 0;
	
	/* Set the dnsheader pointer to point at the beggining of the buffer */
	DNS_HeaderTypeDef *dns = (DNS_HeaderTypeDef *)pClient->DataProcessStack;
	memset((void*)pClient->DataProcessStack, 0, pClient->DataProcessStack_size);
	
	/* fill id and flag info */
	srand(HAL_GetTick());
	ID_q = (uint16_t)rand() % 65536 + 1;
	
	dns->id = ID_q;
	dns->qr = 0;
	dns->opcode = 0;
	dns->aa = 0;
	dns->tc = 0;
	dns->rd = 1;
	dns->ra = 0;
	dns->z = 0;
	dns->rcode = 0;
	
	dns->q_count = htons(1);
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = 0;
	
	/* given unsiged char queryname, make it point to the address of buf after the dnsheader */
	queryname = (unsigned char*)&pClient->DataProcessStack[sizeof(DNS_HeaderTypeDef)];
	DNS_Seperate(pClient, queryname, hostname);
	
	/* given unsigned char queryinfo, make it point to the address of buf after dnsheader and queryname (including the null byte) */
	queryinfo = (DNS_QuestionTypeDef*)&pClient->DataProcessStack[sizeof(DNS_HeaderTypeDef) + (strlen((char*)queryname) + 1)];
	queryinfo->qtype = htons(1);
	queryinfo->qclass = htons(1);
	
	length = sizeof(DNS_HeaderTypeDef) + strlen((char*)queryname) + sizeof(DNS_QuestionTypeDef) + 1;
	memset((void*)pClient->Sendbuf, 0, pClient->Sendbuf_size);
	memcpy(pClient->Sendbuf, pClient->DataProcessStack, length);
	pClient->Sendlen = length;
	
	return DNSStatus;
}

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNSDeserialize_dnsDataStructure(DNS_ClientsTypeDef* pClient, unsigned char* hostname)
 @Description			DNSDeserialize_dnsDataStructure	: 反序列化DNS数据结构体
 @Input				pClient						: DNS客户端实例
					hostname						: DNS查询主机名
 @Return				DNS_StatusTypeDef				: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNSDeserialize_dnsDataStructure(DNS_ClientsTypeDef* pClient, unsigned char* hostname)
{
	DNS_StatusTypeDef DNSStatus = DNS_ERROR;
	int offset = 0;
	int ans_length;
	uint16_t* d_length;
	DNS_RecordTypeDef* answer;
	
	offset = sizeof(DNS_HeaderTypeDef);
	offset += pClient->LetterCounter + 1;
	offset += sizeof(DNS_QuestionTypeDef);
	
	answer = (DNS_RecordTypeDef*)&pClient->Recvbuf[offset];
	
	d_length = (uint16_t*)&pClient->Recvbuf[offset + sizeof(uint16_t) * 3 + sizeof(uint32_t)];
	
	ans_length = sizeof(answer->name) + sizeof(answer->type) + sizeof(answer->class) + sizeof(answer->ttl) + sizeof(answer->data_len) + ntohs(*d_length);
	
	if (ntohs(answer->class) == 1 && ntohs(answer->type) == 1){
		answer = (DNS_RecordTypeDef*)&pClient->Recvbuf[offset - 4];
		DNSStatus = DNS_ConvertIP(pClient, (int)answer->rdata, hostname);
	}
	else {
		offset = offset + ans_length;
		answer = (DNS_RecordTypeDef*)&pClient->Recvbuf[offset];
	}
	
	return DNSStatus;
}

/********************************************** END OF FLEE **********************************************/
