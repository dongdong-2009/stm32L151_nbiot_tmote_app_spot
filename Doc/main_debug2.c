/**
  *********************************************************************************************************
  * @file    main.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-23
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "main.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stm32l1xx_config.h"
#include "platform_config.h"
#include "platform_map.h"
#include "hal_rtc.h"
#include "hal_iwdg.h"
#include "hal_beep.h"
#include "hal_switch.h"
#include "hal_vbat.h"
#include "hal_vptat.h"
#include "hal_temperature.h"
#include "hal_eeprom.h"
#include "hal_qmc5883l.h"
#include "hal_radar.h"
#include "net_coap_app.h"
#include "net_mqttsn_app.h"
#include "radio_hal_rf.h"
#include "radio_rf_app.h"
#include "tmesh_xtea.h"

/********************************************* DEBUG *****************************************************/
/* Debug Include File */
/* Debug Define */
#define NBIOTDEBUG		1
#define OTDEBUG		0
#define UARTDEBUG		0
/* Debug Variable */
unsigned int SendTimes = 0;
/* Debug Function */
int DebugMain(void);
/****************************************** Debug Ending *************************************************/

/**********************************************************************************************************
 @Function			int main(void)
 @Description			Main
 @Input				void
 @Return				int
**********************************************************************************************************/
int main(void)
{
	HAL_Init();																		//HAL库初始化
	
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == SYSTEMCLOCKMSI)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);													//设置时钟MSI->4.194MHz
	Delay_Init(4194);																	//延时初始化4.194MHz系统时钟
#elif (SYSTEMCLOCK == SYSTEMCLOCKHSI)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);											//设置时钟HSI->32MHz
	Delay_Init(32000);																	//延时初始化32MHz系统时钟
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	
	IWDG_Init(IWDG_PRESCALER_256, 0x0FFF);													//看门狗初始化,溢出时间28s
	RTC_Init();																		//RTC初始化
	
#ifdef MVB_SUBSN
	TCFG_EEPROM_Set_MAC_SN(MVB_SUBSN);														//写入MACSN
	TCFG_EEPROM_SetVender(MVB_BRAND);														//写入Verder
	TCFG_EEPROM_WriteConfigData();														//写入系统配置信息
#endif
	if (TCFG_EEPROM_CheckNewSNorBrand() == true) {											//检测新设备号或厂牌
		TCFG_EEPROM_WriteConfigData();													//写入系统配置信息
	}
	TCFG_EEPROM_SystemInfo_Init();														//系统运行信息初始化
	
	BEEP_CtrlRepeat_Extend(5, 50, 25);														//蜂鸣器
	
	LowPowerCtrlIO_Init();																//低功耗控制IO初始化
	ModulePowerReset_Init();																//模块电源复位
	PowerCtrlIO_Init();																	//电源控制IO初始化
	
	Uart1_Init(9700);																	//串口1初始化
	Uart2_Init(9600);																	//串口2初始化
	
	tmesh_securityInit();																//XTEA加密初始化
	Radio_Rf_Init();																	//SI4438初始化
	Radio_Trf_Xmit_Heartbeat();															//SI4438发送心跳包
	
#if OTDEBUG
	QMC5883L_Init();																	//QMC初始化
	Radar_Init();																		//雷达初始化
	Radar_InitBackGround();																//雷达背景初始化
#endif
	
#if NBIOTDEBUG
	NBIOT_Transport_Init(&NbiotATCmdHandler);												//NBIOT数据传输接口初始化
	NBIOT_Client_Init(&NbiotClientHandler, &NbiotATCmdHandler);									//NBIOT客户端初始化
	
	MQTTSN_Transport_Init(&MqttSNSocketNetHandler, &NbiotClientHandler, 4000, "106.14.142.169", 1884);	//MqttSN数据传输接口初始化
	MQTTSN_Client_Init(&MqttSNClientHandler, &MqttSNSocketNetHandler);							//MQTTSN客户端初始化
	
	CoapLongStructure.HeadPacket.DeviceSN = TCFG_EEPROM_Get_MAC_SN();
	CoapLongStructure.HeadPacket.DataLen = 0x00;
	CoapLongStructure.HeadPacket.ProtocolType = 0x00;
	CoapLongStructure.HeadPacket.Reserved1 = 0x00;
	CoapLongStructure.HeadPacket.ProtocolVersion = 0x00;
	CoapLongStructure.HeadPacket.Reserved2 = 0x00;
	CoapLongStructure.HeadPacket.PacketType = 0x05;
	CoapLongStructure.HeadPacket.PacketNumber = 0x00;
	CoapLongStructure.MsgPacket.DestSN = 0x00;
	CoapLongStructure.MsgPacket.Version = 0x01;
	CoapLongStructure.MsgPacket.Type = 0x3A;
	CoapLongStructure.MagneticX =  100;
	CoapLongStructure.MagneticY =  100;
	CoapLongStructure.MagneticZ =  100;
	CoapLongStructure.MagneticDiff = 300;
	CoapLongStructure.RadarDistance = 0;
	CoapLongStructure.RadarStrength = 0;
	CoapLongStructure.RadarCoverCount = 0;
	CoapLongStructure.RadarDiff = 0;
	
	MqttSNStatusExtendStructure.DeviceSN = TCFG_EEPROM_Get_MAC_SN();
	MqttSNStatusExtendStructure.MagX = 100;
	MqttSNStatusExtendStructure.MagY = 100;
	MqttSNStatusExtendStructure.MagZ = 100;
	MqttSNStatusExtendStructure.MagDiff = 300;
	MqttSNStatusExtendStructure.Distance = 0;
	MqttSNStatusExtendStructure.Strength = 0;
	MqttSNStatusExtendStructure.CoverCount = 0;
	MqttSNStatusExtendStructure.RadarDiff = 0;
#endif
	
	TCFG_EEPROM_SetBootCount(0);															//运行正常BootCount清0
	BEEP_CtrlRepeat_Extend(10, 50, 25);													//蜂鸣器
	Radio_Trf_Printf("Device Start MVBKK Reboot : %d ^_^", TCFG_SystemData.DeviceBootCount);			//启动信息
	
	while (1) {
		
		
		
		/* NBIot处理 */
#if NBIOTDEBUG
#if NETPROTOCAL == NETCOAP
		NET_COAP_APP_PollExecution(&NbiotClientHandler);
#elif NETPROTOCAL == NETMQTTSN
		NET_MQTTSN_APP_PollExecution(&MqttSNClientHandler);
#endif
#endif
		/* NBIot数据包写入 */
#if NBIOTDEBUG
		Uart2_PortSerialEnable(DISABLE, DISABLE);
		if (USART2_RX_STA & 0x8000) {
			USART2_RX_STA = 0;
			HAL_UART_Transmit(&UART2_Handler, (u8 *)"\r\nOK Data Enqueue Pack!\r\n", sizeof("\r\nOK Data Enqueue Pack!\r\n"), 0xFFFF);
			BEEP_CtrlRepeat(1, 500);
			BEEP_CtrlRepeat(1, 50);
			/* COAP DATA ENQUEUE */
			CoapLongStructure.SpotStatus = CoapLongStructure.SpotStatus ? 0 : 1;
			CoapLongStructure.SpotCount += 1;
			CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
			NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
			/* MQTTSN DATA ENQUEUE */
			MqttSNStatusExtendStructure.Status = MqttSNStatusExtendStructure.Status ? 0 : 1;
			MqttSNStatusExtendStructure.Count += 1;
			MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
			NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
		}
		Uart2_PortSerialEnable(ENABLE, DISABLE);
		
		SendTimes++;
		if ((SendTimes % 600) == 0) {
			HAL_UART_Transmit(&UART2_Handler, (u8 *)"\r\nOK Data Enqueue Pack!\r\n", sizeof("\r\nOK Data Enqueue Pack!\r\n"), 0xFFFF);
			BEEP_CtrlRepeat(1, 500);
			BEEP_CtrlRepeat(1, 50);
			/* COAP DATA ENQUEUE */
			CoapLongStructure.SpotStatus = CoapLongStructure.SpotStatus ? 0 : 1;
			CoapLongStructure.SpotCount += 1;
			CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
			NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
			/* MQTTSN DATA ENQUEUE */
			MqttSNStatusExtendStructure.Status = MqttSNStatusExtendStructure.Status ? 0 : 1;
			MqttSNStatusExtendStructure.Count += 1;
			MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
			NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
		}
#endif
		/* 模块测试 */
#if OTDEBUG
		if (Radar_GetDataPack(10) != TRADAR_OK) {									//获取雷达数据包
			printf("No Radar!!\n");
		}
		Radar_DataPackToDataStruct();												//将雷达数据提取到雷达结构体
		printf("NotargetNum : %d\n", radarDataStruct.NotargetNum);
		printf("CoverNum : %d\n", radarDataStruct.CoverNum);
		printf("DismagNum : %d\n", radarDataStruct.DismagNum);
		printf("DisVal : %d\n", radarDataStruct.DisVal);
		printf("MagVal : %d\n", radarDataStruct.MagVal);
		printf("Diff : %d\n\n", radarDataStruct.Diff);
		
		printf("%s\n", radarDataPack.RADARData[0]);
		printf("%s\n", radarDataPack.RADARData[1]);
		printf("%s\n", radarDataPack.RADARData[2]);
		printf("%s\n", radarDataPack.RADARData[3]);
		printf("%s\n", radarDataPack.RADARData[4]);
		printf("%s\n", radarDataPack.RADARData[5]);
		printf("%s\n", radarDataPack.RADARData[6]);
		printf("%s\n", radarDataPack.RADARData[7]);
		printf("%s\n", radarDataPack.RADARData[8]);
		printf("%s\n", radarDataPack.RADARData[9]);
		printf("\n");
		
		QMC5883L_Mode_Selection(QMC_MODE_CONTINOUS);
		QMC5883L_ReadData_Extend();
		QMC5883L_Mode_Selection(QMC_MODE_STANDBY);
		printf("X : %d\n", Qmc5883lData.X_Now);
		printf("Y : %d\n", Qmc5883lData.Y_Now);
		printf("Z : %d\n", Qmc5883lData.Z_Now);
		printf("\n");
		
		printf("VBAT : %d\n\n", VBAT_ADC_Read(1000));
		printf("TEMPERATURE : %d\n\n", TEMPERATURE_ADC_Read(1000));
		printf("Mercury : %d\n\n", Mercury_Read());
#endif
		/* 串口测试 */
#if UARTDEBUG
		Uart1_PortSerialEnable(DISABLE, DISABLE);
		Uart2_PortSerialEnable(DISABLE, DISABLE);
		
		if (USART1_RX_STA & 0x8000) {
			BEEP_CtrlRepeat(1, 50);
			HAL_UART_Transmit(&UART1_Handler, USART1_RX_BUF, USART1_RX_STA & 0X3FFF, 0xFFFF);
			USART1_RX_STA = 0;
		}
		if (USART2_RX_STA & 0x8000) {
			BEEP_CtrlRepeat(1, 50);
			HAL_UART_Transmit(&UART2_Handler, USART2_RX_BUF, USART2_RX_STA & 0X3FFF, 0xFFFF);
			USART2_RX_STA = 0;
		}
		
		Uart1_PortSerialEnable(ENABLE, DISABLE);
		Uart2_PortSerialEnable(ENABLE, DISABLE);
#endif
		/* 日常处理 */
		MainHandleRoutine();
		
		/* 小无线处理 */
		Radio_Trf_App_Task();
		
		/* 喂狗 */
		IWDG_Feed();
		
		Delay_MS(1000);
	}
}

/**********************************************************************************************************
 @Function			void MainMajorCycle(void)
 @Description			MainMajorCycle
 @Input				void
 @Return				void
**********************************************************************************************************/
void MainMajorCycle(void)
{
	/* NBIot数据包写入 */
#if NBIOTDEBUG
	Uart2_PortSerialEnable(DISABLE, DISABLE);
	if (USART2_RX_STA & 0x8000) {
		USART2_RX_STA = 0;
		HAL_UART_Transmit(&UART2_Handler, (u8 *)"\r\nOK Data Enqueue Pack!\r\n", sizeof("\r\nOK Data Enqueue Pack!\r\n"), 0xFFFF);
		BEEP_CtrlRepeat(1, 500);
		BEEP_CtrlRepeat(1, 50);
		/* COAP DATA ENQUEUE */
		CoapLongStructure.SpotStatus = CoapLongStructure.SpotStatus ? 0 : 1;
		CoapLongStructure.SpotCount += 1;
		CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
		/* MQTTSN DATA ENQUEUE */
		MqttSNStatusExtendStructure.Status = MqttSNStatusExtendStructure.Status ? 0 : 1;
		MqttSNStatusExtendStructure.Count += 1;
		MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
		NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
	}
	Uart2_PortSerialEnable(ENABLE, DISABLE);
	
	SendTimes++;
	if ((SendTimes % 600) == 0) {
		HAL_UART_Transmit(&UART2_Handler, (u8 *)"\r\nOK Data Enqueue Pack!\r\n", sizeof("\r\nOK Data Enqueue Pack!\r\n"), 0xFFFF);
		BEEP_CtrlRepeat(1, 500);
		BEEP_CtrlRepeat(1, 50);
		/* COAP DATA ENQUEUE */
		CoapLongStructure.SpotStatus = CoapLongStructure.SpotStatus ? 0 : 1;
		CoapLongStructure.SpotCount += 1;
		CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
		/* MQTTSN DATA ENQUEUE */
		MqttSNStatusExtendStructure.Status = MqttSNStatusExtendStructure.Status ? 0 : 1;
		MqttSNStatusExtendStructure.Count += 1;
		MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
		NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
	}
#endif
	
	/* 日常处理 */
	MainHandleRoutine();
	
	/* 小无线处理 */
	Radio_Trf_App_Task();
	
	/* 喂狗 */
	IWDG_Feed();
	
	Delay_MS(1000);
}

/**********************************************************************************************************
 @Function			void MainHandleRoutine(void)
 @Description			MainHandleRoutine
 @Input				void
 @Return				void
**********************************************************************************************************/
void MainHandleRoutine(void)
{
	if (Stm32_GetSecondTick() != SystemRunningTime.seconds) {
		
		
		SystemRunningTime.seconds = Stm32_GetSecondTick();
	}
	if ((Stm32_GetSecondTick() / 60) != SystemRunningTime.minutes) {
		
		
		SystemRunningTime.minutes = Stm32_GetSecondTick() / 60;
	}
	if ((Stm32_GetSecondTick() / 3600) != SystemRunningTime.hours) {
		
		
		SystemRunningTime.minutes = Stm32_GetSecondTick() / 3600;
	}
	if ((Stm32_GetSecondTick() / (24*3600)) != SystemRunningTime.days) {
		
		
		SystemRunningTime.minutes = Stm32_GetSecondTick() / (24*3600);
	}
}

/********************************************** END OF FLEE **********************************************/
