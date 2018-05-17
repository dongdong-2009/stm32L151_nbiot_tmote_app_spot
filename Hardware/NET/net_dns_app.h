#ifndef __NET_DNS_APP_H
#define   __NET_DNS_APP_H

#include "sys.h"
#include "dnsconfig.h"
#include "dnsfunc.h"
#include "dnstransport.h"
#include "nbiotconfig.h"
#include "nbiotfunc.h"
#include "nbiottransport.h"
#include "mqttsnconfig.h"
#include "mqttsnfunc.h"
#include "mqttsntransport.h"
#include "net_mqttsn_message_operate.h"
#include "net_coap_message_operate.h"
#include "platform_map.h"
#include "platform_config.h"

#define DNS_DEBUG_LOG_RF_PRINT											//定义开启RF输出DEBUG信息

#define DNS_NBIOT_BAND_CHINA_TELECOM		ChinaTelecom
#define DNS_NBIOT_BAND_CHINA_MOBILE		ChinaMobile
#define DNS_NBIOT_BAND_CHINA_UNICOM		ChinaUnicom
#define DNS_NBIOT_BAND					DNS_NBIOT_BAND_CHINA_MOBILE

void NET_DNS_APP_PollExecution(DNS_ClientsTypeDef* pClient);					//DNS逻辑处理
void NET_DNS_NBIOT_Event_StopMode(DNS_ClientsTypeDef* pClient);					//停止模式
void NET_DNS_NBIOT_Event_HardwareReboot(DNS_ClientsTypeDef* pClient);				//硬件重启
void NET_DNS_NBIOT_Event_ModuleCheck(DNS_ClientsTypeDef* pClient);				//模块检测
void NET_DNS_NBIOT_Event_ParameterConfig(DNS_ClientsTypeDef* pClient);			//参数配置
void NET_DNS_NBIOT_Event_SimICCIDCheck(DNS_ClientsTypeDef* pClient);				//Sim卡检测
void NET_DNS_NBIOT_Event_MiscEquipConfig(DNS_ClientsTypeDef* pClient);			//其他配置
void NET_DNS_NBIOT_Event_AttachCheck(DNS_ClientsTypeDef* pClient);				//注网检查
void NET_DNS_NBIOT_Event_AttachExecute(DNS_ClientsTypeDef* pClient);				//注网进行
void NET_DNS_NBIOT_Event_AttachInquire(DNS_ClientsTypeDef* pClient);				//注网查询
void NET_DNS_NBIOT_Event_PatameterCheckOut(DNS_ClientsTypeDef* pClient);			//参数检出

void NET_DNS_APP_ProcessExecution(DNS_ClientsTypeDef* pClient);					//DNS协议逻辑处理







#endif
