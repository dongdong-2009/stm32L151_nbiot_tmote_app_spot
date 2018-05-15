#ifndef __NET_COAP_APP_H
#define   __NET_COAP_APP_H

#include "sys.h"
#include "nbiotconfig.h"
#include "nbiotfunc.h"
#include "nbiottransport.h"
#include "net_coap_message_operate.h"
#include "platform_map.h"

#define COAP_DEBUG_LOG_RF_PRINT											//定义开启RF输出DEBUG信息

#define COAPCDPADDR_CHINA_TELECOM_FORMAL	"117.60.157.137"					//电信生产 : "117.60.157.137"
#define COAPCDPADDR_CHINA_TELECOM_TEST		"180.101.147.115"					//电信测试 : "180.101.147.115"
#define COAPCDPADDR						COAPCDPADDR_CHINA_TELECOM_FORMAL
#define COAPCDPPORT						5683

#define COAP_NBIOT_BAND_CHINA_TELECOM		ChinaTelecom
#define COAP_NBIOT_BAND_CHINA_MOBILE		ChinaMobile
#define COAP_NBIOT_BAND_CHINA_UNICOM		ChinaUnicom
#define COAP_NBIOT_BAND					COAP_NBIOT_BAND_CHINA_TELECOM

void NET_COAP_APP_PollExecution(NBIOT_ClientsTypeDef* pClient);					//COAP逻辑处理
void NET_COAP_NBIOT_Event_StopMode(NBIOT_ClientsTypeDef* pClient);				//停止模式
void NET_COAP_NBIOT_Event_HardwareReboot(NBIOT_ClientsTypeDef* pClient);			//硬件重启
void NET_COAP_NBIOT_Event_ModuleCheck(NBIOT_ClientsTypeDef* pClient);				//模块检测
void NET_COAP_NBIOT_Event_ParameterConfig(NBIOT_ClientsTypeDef* pClient);			//参数配置
void NET_COAP_NBIOT_Event_SimICCIDCheck(NBIOT_ClientsTypeDef* pClient);			//Sim卡检测
void NET_COAP_NBIOT_Event_FullFunctionality(NBIOT_ClientsTypeDef* pClient);		//完整功能
void NET_COAP_NBIOT_Event_MinimumFunctionality(NBIOT_ClientsTypeDef* pClient);		//最小功能
void NET_COAP_NBIOT_Event_CDPServerCheck(NBIOT_ClientsTypeDef* pClient);			//CDP服务器查询
void NET_COAP_NBIOT_Event_CDPServerConfig(NBIOT_ClientsTypeDef* pClient);			//CDP服务器配置
void NET_COAP_NBIOT_Event_MiscEquipConfig(NBIOT_ClientsTypeDef* pClient);			//其他配置
void NET_COAP_NBIOT_Event_AttachCheck(NBIOT_ClientsTypeDef* pClient);				//注网检查
void NET_COAP_NBIOT_Event_AttachExecute(NBIOT_ClientsTypeDef* pClient);			//注网进行
void NET_COAP_NBIOT_Event_AttachInquire(NBIOT_ClientsTypeDef* pClient);			//注网查询
void NET_COAP_NBIOT_Event_PatameterCheckOut(NBIOT_ClientsTypeDef* pClient);		//参数检出
void NET_COAP_NBIOT_Event_SendData(NBIOT_ClientsTypeDef* pClient);				//发送数据
void NET_COAP_NBIOT_Event_RecvData(NBIOT_ClientsTypeDef* pClient);				//接收数据
void NET_COAP_NBIOT_Event_ExecutDownlinkData(NBIOT_ClientsTypeDef* pClient);		//下行数据处理



#endif
