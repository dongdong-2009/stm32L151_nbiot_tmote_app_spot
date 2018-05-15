/**
  *********************************************************************************************************
  * @file    radio_hal.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-26
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "radio_hal.h"
#include "radio_hal_rf.h"
#include "radio_spi.h"

/**********************************************************************************************************
 @Function			void Radio_Hal_AssertShutdown(void)
 @Description			Radio_Hal_AssertShutdown			: 置位SDN
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Hal_AssertShutdown(void)
{
	HAL_GPIO_WritePin(RF_SDN_GPIO_PORT, RF_SDN_PIN, GPIO_PIN_SET);
}

/**********************************************************************************************************
 @Function			void Radio_Hal_DeassertShutdown(void)
 @Description			Radio_Hal_DeassertShutdown		: 清零SDN
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Hal_DeassertShutdown(void)
{
	HAL_GPIO_WritePin(RF_SDN_GPIO_PORT, RF_SDN_PIN, GPIO_PIN_RESET);
}

/**********************************************************************************************************
 @Function			void Radio_Hal_AssertNsel(void)
 @Description			Radio_Hal_AssertNsel			: 置位NSS
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Hal_AssertNsel(void)
{
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT, SPIx_NSS_PIN, GPIO_PIN_SET);
}

/**********************************************************************************************************
 @Function			void Radio_Hal_DeassertNsel(void)
 @Description			Radio_Hal_DeassertNsel			: 清零NSS
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Hal_DeassertNsel(void)
{
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT, SPIx_NSS_PIN, GPIO_PIN_RESET);
}

/**********************************************************************************************************
 @Function			unsigned char Radio_Hal_NirqLevel(void)
 @Description			Radio_Hal_NirqLevel				: 读取nIRQ电平
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char Radio_Hal_NirqLevel(void)
{
	return HAL_GPIO_ReadPin(RF_nIRQ_GPIO_PORT, RF_nIRQ_PIN);
}

/**********************************************************************************************************
 @Function			void Radio_Hal_SpiWriteByte(unsigned char byteToWrite)
 @Description			Radio_Hal_SpiWriteByte			: SPI写1Byte
 @Input				byteToWrite
 @Return				void
**********************************************************************************************************/
void Radio_Hal_SpiWriteByte(unsigned char byteToWrite)
{
	Radio_RF_ReadWriteByte_SPI1(byteToWrite);
}

/**********************************************************************************************************
 @Function			unsigned char Radio_Hal_SpiReadByte(void)
 @Description			Radio_Hal_SpiReadByte			: SPI读1Byte
 @Input				void
 @Return				byteToRead
**********************************************************************************************************/
unsigned char Radio_Hal_SpiReadByte(void)
{
	return Radio_RF_ReadWriteByte_SPI1(0xFF);
}

/**********************************************************************************************************
 @Function			void Radio_Hal_SpiWriteData(unsigned short dataInLength, unsigned char* dataIn)
 @Description			Radio_Hal_SpiWriteData			: SPI写buffer
 @Input				dataInLength
					dataIn
 @Return				void
**********************************************************************************************************/
void Radio_Hal_SpiWriteData(unsigned short dataInLength, unsigned char* dataIn)
{
	Radio_RF_WriteData_SPI1(dataInLength, dataIn);
}

/**********************************************************************************************************
 @Function			void Radio_Hal_SpiReadData(unsigned short dataOutLength, unsigned char* dataOut)
 @Description			Radio_Hal_SpiReadData			: SPI读buffer
 @Input				dataOutLength
					dataOut
 @Return				void
**********************************************************************************************************/
void Radio_Hal_SpiReadData(unsigned short dataOutLength, unsigned char* dataOut)
{
	while (dataOutLength--) {
		*dataOut++ = Radio_RF_ReadWriteByte_SPI1(0xFF);
	}
}

/********************************************** END OF FLEE **********************************************/
