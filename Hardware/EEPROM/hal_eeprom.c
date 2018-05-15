/**
  *********************************************************************************************************
  * @file    hal_eeprom.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-11-01
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "hal_eeprom.h"

/**********************************************************************************************************
 @Function			static unsigned char FLASH_EEPROM_ReadByte_Private(unsigned int Address)
 @Description			FLASH_EEPROM_ReadByte_Private
 @Input				Address
 @Return				unsigned char
**********************************************************************************************************/
static unsigned char FLASH_EEPROM_ReadByte_Private(unsigned int Address)
{
	return *(__IO unsigned char*)(Address + EEPROM_BASE_ADDR);
}

/**********************************************************************************************************
 @Function			static HAL_StatusTypeDef FLASH_EEPROM_WriteByte_Private(unsigned int Address, unsigned char Data)
 @Description			FLASH_EEPROM_WriteByte_Private
 @Input				Address
					Data
 @Return				HAL_StatusTypeDef
**********************************************************************************************************/
static HAL_StatusTypeDef FLASH_EEPROM_WriteByte_Private(unsigned int Address, unsigned char Data)
{
	return HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, Address + EEPROM_BASE_ADDR, Data);
}

/**********************************************************************************************************
 @Function			unsigned char FLASH_EEPROM_ReadByte(unsigned int Address)
 @Description			FLASH_EEPROM_ReadByte
 @Input				Address
 @Return				unsigned char
**********************************************************************************************************/
unsigned char FLASH_EEPROM_ReadByte(unsigned int Address)
{
	return FLASH_EEPROM_ReadByte_Private(Address);
}

/**********************************************************************************************************
 @Function			void FLASH_EEPROM_WriteByte(unsigned int Address, unsigned char Data)
 @Description			FLASH_EEPROM_WriteByte
 @Input				Address
					Data
 @Return				void
**********************************************************************************************************/
void FLASH_EEPROM_WriteByte(unsigned int Address, unsigned char Data)
{
	EEPROM_ENTER_CRITICAL_SECTION();
	HAL_FLASHEx_DATAEEPROM_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
	FLASH_EEPROM_WriteByte_Private(Address, Data);
	
	HAL_FLASHEx_DATAEEPROM_Lock();
	EEPROM_EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			unsigned short FLASH_EEPROM_ReadHalfWord(unsigned int Address)
 @Description			FLASH_EEPROM_ReadHalfWord
 @Input				Address
 @Return				unsigned short
**********************************************************************************************************/
unsigned short FLASH_EEPROM_ReadHalfWord(unsigned int Address)
{
	unsigned short val = 0;
	
	*((unsigned char*)(&val) + 0) = FLASH_EEPROM_ReadByte_Private(Address + 0);
	*((unsigned char*)(&val) + 1) = FLASH_EEPROM_ReadByte_Private(Address + 1);
	
	return val;
}

/**********************************************************************************************************
 @Function			void FLASH_EEPROM_WriteHalfWord(unsigned int Address, unsigned short Data)
 @Description			FLASH_EEPROM_WriteHalfWord
 @Input				Address
					Data
 @Return				void
**********************************************************************************************************/
void FLASH_EEPROM_WriteHalfWord(unsigned int Address, unsigned short Data)
{
	EEPROM_ENTER_CRITICAL_SECTION();
	HAL_FLASHEx_DATAEEPROM_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
	FLASH_EEPROM_WriteByte_Private(Address, *((unsigned char*)(&Data)));
	FLASH_EEPROM_WriteByte_Private(Address + 1, *((unsigned char*)(&Data) + 1));
	
	HAL_FLASHEx_DATAEEPROM_Lock();
	EEPROM_EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			unsigned int FLASH_EEPROM_ReadWord(unsigned int Address)
 @Description			FLASH_EEPROM_ReadWord
 @Input				Address
 @Return				unsigned int
**********************************************************************************************************/
unsigned int FLASH_EEPROM_ReadWord(unsigned int Address)
{
	unsigned int val = 0;
	
	*((unsigned char*)(&val) + 0) = FLASH_EEPROM_ReadByte_Private(Address + 0);
	*((unsigned char*)(&val) + 1) = FLASH_EEPROM_ReadByte_Private(Address + 1);
	*((unsigned char*)(&val) + 2) = FLASH_EEPROM_ReadByte_Private(Address + 2);
	*((unsigned char*)(&val) + 3) = FLASH_EEPROM_ReadByte_Private(Address + 3);
	
	return val;
}

/**********************************************************************************************************
 @Function			void FLASH_EEPROM_WriteWord(unsigned int Address, unsigned int Data)
 @Description			FLASH_EEPROM_WriteWord
 @Input				Address
					Data
 @Return				void
**********************************************************************************************************/
void FLASH_EEPROM_WriteWord(unsigned int Address, unsigned int Data)
{
	EEPROM_ENTER_CRITICAL_SECTION();
	HAL_FLASHEx_DATAEEPROM_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
	FLASH_EEPROM_WriteByte_Private(Address + 0, *((unsigned char*)(&Data) + 0));
	FLASH_EEPROM_WriteByte_Private(Address + 1, *((unsigned char*)(&Data) + 1));
	FLASH_EEPROM_WriteByte_Private(Address + 2, *((unsigned char*)(&Data) + 2));
	FLASH_EEPROM_WriteByte_Private(Address + 3, *((unsigned char*)(&Data) + 3));
	
	HAL_FLASHEx_DATAEEPROM_Lock();
	EEPROM_EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			void FLASH_EEPROM_ReadBuffer(unsigned int Address, unsigned char* buf, unsigned short length)
 @Description			FLASH_EEPROM_ReadBuffer
 @Input				void
 @Return				addr   : 读出数据EEPROM偏移地址
					*buf   : 待读出数据地址
					length : 读出字节数
**********************************************************************************************************/
void FLASH_EEPROM_ReadBuffer(unsigned int Address, unsigned char* buf, unsigned short length)
{
	for (unsigned short i = 0; i < length; i++) {
		buf[i] = FLASH_EEPROM_ReadByte_Private(Address + i);
	}
}

/**********************************************************************************************************
 @Function			void FLASH_EEPROM_WriteBuffer(unsigned int Address, unsigned char* buf, unsigned short length)
 @Description			FLASH_EEPROM_WriteBuffer
 @Input				void
 @Return				addr   : 写入数据EEPROM偏移地址
					*buf   : 待写入数据地址
					length : 写入字节数
**********************************************************************************************************/
void FLASH_EEPROM_WriteBuffer(unsigned int Address, unsigned char* buf, unsigned short length)
{
	EEPROM_ENTER_CRITICAL_SECTION();
	HAL_FLASHEx_DATAEEPROM_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
	for (unsigned short i = 0; i < length; i++) {
		FLASH_EEPROM_WriteByte_Private(Address + i, *(buf + i));
	}
	
	HAL_FLASHEx_DATAEEPROM_Lock();
	EEPROM_EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			void EEPROM_EnterCriticalSection(void)
 @Description			进入关键操作,关闭中断,等待中断结束返回
 @Input				void
 @Return				void
**********************************************************************************************************/
void EEPROM_EnterCriticalSection(void)
{
	__disable_irq();
}

/**********************************************************************************************************
 @Function			void EEPROM_ExitCriticalSection(void)
 @Description			退出关键操作,开启中断
 @Input				void
 @Return				void
**********************************************************************************************************/
void EEPROM_ExitCriticalSection(void)
{
	__enable_irq();
}

/********************************************** END OF FLEE **********************************************/
