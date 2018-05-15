/**
  *********************************************************************************************************
  * @file    radio_spi.c
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

#include "radio_spi.h"
#include "radio_hal_rf.h"

/**********************************************************************************************************
 @Function			unsigned char Radio_RF_ReadWriteByte_SPI1(unsigned char byteDataIn)
 @Description			Radio_RF_ReadWriteByte_SPI1		: 读写SPI Byte
 @Input				byteDataIn
 @Return				byteDataOut
**********************************************************************************************************/
unsigned char Radio_RF_ReadWriteByte_SPI1(unsigned char byteDataIn)
{
	unsigned char byteDataOut = 0;
	
	HAL_SPI_TransmitReceive(&SPI_Handler, &byteDataIn, &byteDataOut, 1, 100);
	
	return byteDataOut;
}

/**********************************************************************************************************
 @Function			void Radio_RF_WriteData_SPI1(unsigned short dataInLength, unsigned char* dataIn)
 @Description			Radio_RF_WriteData_SPI1			: 写SPI Buffer
 @Input				dataInLength
					dataIn
 @Return				void
**********************************************************************************************************/
void Radio_RF_WriteData_SPI1(unsigned short dataInLength, unsigned char* dataIn)
{
	HAL_SPI_Transmit(&SPI_Handler, dataIn, dataInLength, 100);
}

/**********************************************************************************************************
 @Function			void Radio_RF_ReadData_SPI1(unsigned short dataOutLength, unsigned char* dataOut)
 @Description			Radio_RF_ReadData_SPI1			: 读SPI Buffer
 @Input				dataOutLength
					dataOut
 @Return				void
**********************************************************************************************************/
void Radio_RF_ReadData_SPI1(unsigned short dataOutLength, unsigned char* dataOut)
{
	while (dataOutLength--) {
		*dataOut++ = Radio_RF_ReadWriteByte_SPI1(0xFF);
	}
}

/********************************************** END OF FLEE **********************************************/
