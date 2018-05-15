/**
  *********************************************************************************************************
  * @file    hal_rtc.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-24
  * @brief   HAL RTC Device
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "hal_rtc.h"
#include "string.h"

RTC_HandleTypeDef RTC_Handler;											//RTC句柄

/**********************************************************************************************************
 @Function			u8 RTC_Init(void)
 @Description			RTC初始化
 @Input				void
 @Return				0 : 初始化成功
					2 : 进入初始化模式失败
**********************************************************************************************************/
u8 RTC_Init(void)
{
#if RTCBulidTime
	struct tm datetime;
#endif
	
	RTC_Handler.Instance = RTC;
	RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24;							//RTC设置为24小时格式
	RTC_Handler.Init.AsynchPrediv = 0x7F;									//RTC异步分频系数(1~0X7F)
	RTC_Handler.Init.SynchPrediv = 0xFF;									//RTC同步分频系数(0~7FFF)
	RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
	RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&RTC_Handler) != HAL_OK) return 2;
	
	if (HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR0) != 0X5050)				//是否第一次配置
	{
#if RTCBulidTime
		datetime = RTC_ConvUnixToCalendar(RTC_BulidTimeToStamp());				//获取系统编译时间
		RTC_Set_Date(datetime.tm_year, datetime.tm_mon, datetime.tm_mday);		//设置日期
		RTC_Set_Time(datetime.tm_hour, datetime.tm_min, datetime.tm_sec);		//设置时间
#else
		RTC_Set_Date(18, 01, 01);										//设置日期
		RTC_Set_Time(12, 00, 00);										//设置时间
#endif
		HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR0, 0X5050);				//标记已经初始化过了
	}
	
	return 0;
}

/**********************************************************************************************************
 @Function			HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec)
 @Description			RTC时间设置
 @Input				hour, min, sec : 小时, 分钟, 秒钟
 @Return				SUCEE(1) : 成功
					ERROR(0) : 进入初始化模式失败
**********************************************************************************************************/
HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_TimeStructure.Hours = hour;
	RTC_TimeStructure.Minutes = min;
	RTC_TimeStructure.Seconds = sec;
	RTC_TimeStructure.TimeFormat = (hour < 12)?RTC_HOURFORMAT12_AM:RTC_HOURFORMAT12_PM;
	RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
	
	return HAL_RTC_SetTime(&RTC_Handler, &RTC_TimeStructure, RTC_FORMAT_BIN);
}

/**********************************************************************************************************
 @Function			HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date)
 @Description			RTC日期设置
 @Input				year, month, date 	: 年(0~99), 月(1~12), 日(0~31)
 @Return				SUCEE(1) : 成功
					ERROR(0) : 进入初始化模式失败
**********************************************************************************************************/
HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date)
{
	u8 week;
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_DateStructure.Date = date;
	RTC_DateStructure.Month = month;
	RTC_DateStructure.Year = year;
	if (month == 1 || month == 2) {
		month += 12;
		year--;
	}
	week = (date+2*month+3*(month+1)/5+year+year/4-year/100+year/400+1)%7;
	RTC_DateStructure.WeekDay = (week == 0) ? 7 : week;
	
	return HAL_RTC_SetDate(&RTC_Handler, &RTC_DateStructure, RTC_FORMAT_BIN);
}

/**********************************************************************************************************
 @Function			time_t RTC_TimeToStamp(u8 year, u8 month, u8 date, u8 hour, u8 min, u8 sec)
 @Description			时间转换为UNIX时间戳
 @Input				year, month, date 	: 年(0~99), 月(1~12), 日(0~31)
					hour, min, sec 	: 小时, 分钟, 秒钟
 @Return				time_t			: 时间戳
**********************************************************************************************************/
time_t RTC_TimeToStamp(u8 year, u8 month, u8 date, u8 hour, u8 min, u8 sec)
{
	struct tm time;
	
	time.tm_year = 100 + year;											//1900年到当前年份差
	time.tm_mon = month - 1;												//0~11 表示 1~12月
	time.tm_mday = date;
	time.tm_hour = hour;
	time.tm_min = min;
	time.tm_sec = sec;
	
	return mktime(&time);
}

/**********************************************************************************************************
 @Function			time_t RTC_BulidTimeToStamp(void)
 @Description			编译时间转换为UNIX时间戳
 @Input				void
 @Return				time_t			: 时间戳
**********************************************************************************************************/
time_t RTC_BulidTimeToStamp(void)
{
	int year, month, date, hour, min, sec;
	char bulidData[12];
	char bulidTime[9];
	struct tm time;
	
	memset(bulidData, 0x0, sizeof(bulidData));
	memset(bulidTime, 0x0, sizeof(bulidTime));
	sprintf(bulidData, "%s", __DATE__);
	sprintf(bulidTime, "%s", __TIME__);
	
	if (!memcmp(&bulidData[0], "Jan", 3)) {
		month = 1;
	}
	else if (!memcmp(&bulidData[0], "Feb", 3)) {
		month = 2;
	}
	else if (!memcmp(&bulidData[0], "Mar", 3)) {
		month = 3;
	}
	else if (!memcmp(&bulidData[0], "Apr", 3)) {
		month = 4;
	}
	else if (!memcmp(&bulidData[0], "May", 3)) {
		month = 5;
	}
	else if (!memcmp(&bulidData[0], "Jun", 3)) {
		month = 6;
	}
	else if (!memcmp(&bulidData[0], "Jul", 3)) {
		month = 7;
	}
	else if (!memcmp(&bulidData[0], "Aug", 3)) {
		month = 8;
	}
	else if (!memcmp(&bulidData[0], "Sep", 3)) {
		month = 9;
	}
	else if (!memcmp(&bulidData[0], "Oct", 3)) {
		month = 10;
	}
	else if (!memcmp(&bulidData[0], "Nov", 3)) {
		month = 11;
	}
	else if (!memcmp(&bulidData[0], "Dec", 3)) {
		month = 12;
	}
	
	sscanf((const char *)&bulidData[4], "%d %d", &date, &year);
	sscanf((const char *)&bulidTime[0], "%d:%d:%d", &hour, &min, &sec);
	
	time.tm_year = year - 1900;
	time.tm_mon = month - 1;
	time.tm_mday = date;
	time.tm_hour = hour;
	time.tm_min = min;
	time.tm_sec = sec;
	
	return mktime(&time);
}

/**********************************************************************************************************
 @Function			struct tm RTC_ConvUnixToCalendar(time_t t)
 @Description			转换UNIX时间戳为日历时间
 @Input				UNIX时间戳
 @Return				struct tm
**********************************************************************************************************/
struct tm RTC_ConvUnixToCalendar(time_t t)
{
	struct tm *t_time;
	
	t_time = localtime(&t);
	t_time->tm_year -= 100;												//1900年标准转换至2000年标准
	t_time->tm_mon += 1;												//0~11标准转换至1~12标准
	
	return *t_time;
}

/**********************************************************************************************************
 @Function			time_t RTC_GetUnixTimeToStamp(void)
 @Description			获取当前UNIX时间戳
 @Input				void
 @Return				time_t			: 时间戳
**********************************************************************************************************/
time_t RTC_GetUnixTimeToStamp(void)
{
	RTC_TimeTypeDef RTC_Timestructure;
	RTC_DateTypeDef RTC_Datestructure;
	
	HAL_RTC_GetTime(&RTC_Handler, &RTC_Timestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_Handler, &RTC_Datestructure, RTC_FORMAT_BIN);
	
	return RTC_TimeToStamp(RTC_Datestructure.Year, RTC_Datestructure.Month, RTC_Datestructure.Date, RTC_Timestructure.Hours, RTC_Timestructure.Minutes, RTC_Timestructure.Seconds) - 8 * 3600;
}

/**********************************************************************************************************
 @Function			uint8_t RTC_Time_GetCurrentHour(void)
 @Description			获取当前系统时间(时)
 @Input				void
 @Return				uint8_t			: 时
**********************************************************************************************************/
uint8_t RTC_Time_GetCurrentHour(void)
{
	RTC_TimeTypeDef RTC_Timestructure;
	HAL_RTC_GetTime(&RTC_Handler, &RTC_Timestructure, RTC_FORMAT_BIN);
	return RTC_Timestructure.Hours;
}

/**********************************************************************************************************
 @Function			uint8_t RTC_Time_GetCurrentHour(void)
 @Description			获取当前系统时间(分)
 @Input				void
 @Return				uint8_t			: 分
**********************************************************************************************************/
uint8_t RTC_Time_GetCurrentMinute(void)
{
	RTC_TimeTypeDef RTC_Timestructure;
	HAL_RTC_GetTime(&RTC_Handler, &RTC_Timestructure, RTC_FORMAT_BIN);
	return RTC_Timestructure.Minutes;
}

/**********************************************************************************************************
 @Function			uint8_t RTC_Time_GetCurrentHour(void)
 @Description			获取当前系统时间(秒)
 @Input				void
 @Return				uint8_t			: 秒
**********************************************************************************************************/
uint8_t RTC_Time_GetCurrentSecond(void)
{
	RTC_TimeTypeDef RTC_Timestructure;
	HAL_RTC_GetTime(&RTC_Handler, &RTC_Timestructure, RTC_FORMAT_BIN);
	return RTC_Timestructure.Seconds;
}

/********************************************** END OF FLEE **********************************************/
