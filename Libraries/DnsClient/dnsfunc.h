#ifndef __DNS_FUNC_H
#define   __DNS_FUNC_H

#include "sys.h"
#include "dnsconfig.h"

unsigned long int htonl(unsigned long int h);															//本机字节序转网络字节序(u32)
unsigned long int ntohl(unsigned long int n);															//网络字节序转本机字节序(u32)
unsigned short int htons(unsigned short int h);															//本机字节序转网络字节序(u16)
unsigned short int ntohs(unsigned short int n);															//网络字节序转本机字节序(u16)

unsigned char* DNS_GetHostIP(DNS_ClientsTypeDef* pClient, unsigned char* hostname);								//获取转换IP地址

DNS_StatusTypeDef DNS_Seperate(DNS_ClientsTypeDef* pClient, unsigned char* name, unsigned char* host);				//将主机名格式化为DNS查询格式

DNS_StatusTypeDef DNS_ConvertIP(DNS_ClientsTypeDef* pClient, int ip, unsigned char* hostname);						//转换并填入IP地址

DNS_StatusTypeDef DNSSerialize_dnsDataStructure(DNS_ClientsTypeDef* pClient, unsigned char* hostname);				//序列化DNS数据结构体

DNS_StatusTypeDef DNSDeserialize_dnsDataStructure(DNS_ClientsTypeDef* pClient, unsigned char* hostname);				//反序列化DNS数据结构体

#endif
