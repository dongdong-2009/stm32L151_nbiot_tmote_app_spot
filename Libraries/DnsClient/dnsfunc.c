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












































/********************************************** END OF FLEE **********************************************/
