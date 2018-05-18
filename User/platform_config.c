/**
  *********************************************************************************************************
  * @file    platform_config.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-26
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "platform_config.h"

bool BootUp				= true;										//BootUp

/* Work Mode */
bool DeviceIdleMode			= false;										//IDLE MODE
bool DeviceActivedMode		= true;										//ACTIVED MODE

/* Coap Packet */
COAP_PacketShortTypeDef		CoapShortStructure;								//Coap Short Packet
COAP_PacketLongTypeDef		CoapLongStructure;								//Coap Long Packet
COAP_PacketInfoTypeDef		CoapInfoStructure;								//Coap Info Packet

/* MqttSN Packet */
MQTTSN_StatusBasicTypeDef	MqttSNStatusBasicStructure;						//MqttSN Basic Status Packet
MQTTSN_StatusExtendTypeDef	MqttSNStatusExtendStructure;						//MqttSN Extend Status Packet
MQTTSN_InfoWorkTypeDef		MqttSNInfoWorkStructure;							//MqttSN Work Info Packet
MQTTSN_InfoBasicTypeDef		MqttSNInfoBasicStructure;						//MqttSN Basic Info Packet
MQTTSN_InfoDynamicTypeDef	MqttSNInfoDynamicStructure;						//MqttSN Dynamic Info Packet
MQTTSN_InfoRadarTypeDef		MqttSNInfoRadarStructure;						//MqttSN Radar Info Packet

/* NET Handler */
NET_NBIOT_ClientsTypeDef		NetNbiotClientHandler;							//NET NBIOT Clinet Handler
NBIOT_ATCmdTypeDef			NbiotATCmdHandler;								//NBIOT AT Cmd Handler
NBIOT_ClientsTypeDef		NbiotClientHandler;								//Coap Clinet Handler
MQTTSN_SocketNetTypeDef		MqttSNSocketNetHandler;							//MqttSN Net Handler
MQTTSN_ClientsTypeDef		MqttSNClientHandler;							//MqttSN Clinet Handler
DNS_SocketNetTypeDef		DNSSocketNetHandler;							//DNS Net Handler
DNS_ClientsTypeDef			DNSClientHandler;								//DNS Clinet Handler























/********************************************** END OF FLEE **********************************************/
