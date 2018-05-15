#ifndef __NBIOT_FUNC_H
#define   __NBIOT_FUNC_H

#include "sys.h"
#include "nbiotconfig.h"

NBIOT_StatusTypeDef NBIOT_Neul_NBxx_HardwarePoweroff(NBIOT_ClientsTypeDef* pClient);										//硬件断电
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_HardwareReboot(NBIOT_ClientsTypeDef* pClient, u32 rebootTimeoutMS);						//硬件重启NB模块
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SoftwareReboot(NBIOT_ClientsTypeDef* pClient, u32 rebootTimeoutMS);						//软件重启NB模块
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadManufacturer(NBIOT_ClientsTypeDef* pClient);									//检出制造商标识
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadManufacturerModel(NBIOT_ClientsTypeDef* pClient);								//检出制造商型号
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMEI(NBIOT_ClientsTypeDef* pClient);										//检出IMEI号
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMEISV(NBIOT_ClientsTypeDef* pClient);										//检出IMEISV号
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadRSSI(NBIOT_ClientsTypeDef* pClient);										//检出RSSI号
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadStatisticsCELL(NBIOT_ClientsTypeDef* pClient);								//检出基站连接参数
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadICCID(NBIOT_ClientsTypeDef* pClient);										//检出ICCID卡号
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMSI(NBIOT_ClientsTypeDef* pClient);										//检出IMSI号
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCGPADDR(NBIOT_ClientsTypeDef* pClient);										//检出CGPADDR地址
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCGDCONT(NBIOT_ClientsTypeDef* pClient);										//检出CGDCONT网址
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadDateTime(NBIOT_ClientsTypeDef* pClient);									//检出基站时间
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetAttachOrDetach(NBIOT_ClientsTypeDef* pClient, NBIOT_NetstateTypeDef attdet);			//设置终端入网退网
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadAttachOrDetach(NBIOT_ClientsTypeDef* pClient);								//查询终端入网退网
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetMinOrFullFunc(NBIOT_ClientsTypeDef* pClient, NBIOT_FunctionalityTypeDef minfull);		//设置设备功能
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadMinOrFullFunc(NBIOT_ClientsTypeDef* pClient);								//查询设备功能
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetNewMessageIndications(NBIOT_ClientsTypeDef* pClient, NBIOT_OpenOrCloseFuncTypeDef state);	//设置NNMI是否开启
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadNewMessageIndications(NBIOT_ClientsTypeDef* pClient);							//查询NNMI是否开启
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetSentMessageIndications(NBIOT_ClientsTypeDef* pClient, NBIOT_OpenOrCloseFuncTypeDef state);	//设置NSMI是否开启
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadSentMessageIndications(NBIOT_ClientsTypeDef* pClient);							//查询NSMI是否开启
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetSupportedBands(NBIOT_ClientsTypeDef* pClient, NBIOT_BandTypeDef bands);				//设置Band
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadSupportedBands(NBIOT_ClientsTypeDef* pClient);								//查询Band
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetCDPServer(NBIOT_ClientsTypeDef* pClient, const char *host, unsigned short port);			//设置CDP服务器
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCDPServer(NBIOT_ClientsTypeDef* pClient);									//查询CDP服务器
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetConfigUE(NBIOT_ClientsTypeDef* pClient, const char *ncmd, NBIOT_NConfigTypeDef state);	//配置NCONFIG模式
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadConfigUE(NBIOT_ClientsTypeDef* pClient);									//查询NCONFIG模式
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_QuerySendMessageCOAPPayload(NBIOT_ClientsTypeDef* pClient);							//COAP查询消息送达
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_QueryReadMessageCOAPPayload(NBIOT_ClientsTypeDef* pClient);							//COAP查询消息下行
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendCOAPPayload(NBIOT_ClientsTypeDef* pClient);										//COAP发送负载数据
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_ReadCOAPPayload(NBIOT_ClientsTypeDef* pClient);										//COAP读取负载数据
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CreateUDPSocket(NBIOT_ClientsTypeDef* pClient, u16 localport, u8 receivectl, int *socket);	//创建UDP链接
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CloseUDPSocket(NBIOT_ClientsTypeDef* pClient, int socket);								//关闭UDP连接
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendUDPPayload(NBIOT_ClientsTypeDef* pClient, int socket, const char *addr, u16 port);		//UDP发送负载数据
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendUDPPayloadFlag(NBIOT_ClientsTypeDef* pClient, int , const char *, u16 , const char *);	//UDP发送负载数据
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_ReadUDPPayload(NBIOT_ClientsTypeDef* pClient, int , int , int *, char *, u16 *);			//UDP读取负载数据

#endif
