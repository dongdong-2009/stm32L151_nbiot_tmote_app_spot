#ifndef __HAL_RTC_H
#define   __HAL_RTC_H

#include "sys.h"
#include <time.h>

#define RTCBulidTime		0											//RTC编译时间写入使能

extern RTC_HandleTypeDef	RTC_Handler;										//RTC句柄

u8 RTC_Init(void);														//RTC初始化

HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date);						//RTC日期设置
HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec);						//RTC时间设置

time_t RTC_TimeToStamp(u8 year, u8 month, u8 date, u8 hour, u8 min, u8 sec);		//时间转换为UNIX时间戳
time_t RTC_BulidTimeToStamp(void);											//编译时间转换为UNIX时间戳
time_t RTC_GetUnixTimeToStamp(void);										//获取当前UNIX时间戳
struct tm RTC_ConvUnixToCalendar(time_t t);									//转换UNIX时间戳为日历时间

uint8_t RTC_Time_GetCurrentHour(void);										//获取当前系统时间(时)
uint8_t RTC_Time_GetCurrentMinute(void);									//获取当前系统时间(分)
uint8_t RTC_Time_GetCurrentSecond(void);									//获取当前系统时间(秒)

#endif
