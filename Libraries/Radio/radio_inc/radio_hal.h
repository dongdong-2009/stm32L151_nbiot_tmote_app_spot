#ifndef __RADIO_HAL_H
#define   __RADIO_HAL_H

#include "sys.h"

void Radio_Hal_AssertShutdown(void);										//置位SDN
void Radio_Hal_DeassertShutdown(void);										//清零SDN
void Radio_Hal_AssertNsel(void);											//置位NSS
void Radio_Hal_DeassertNsel(void);											//清零NSS
unsigned char Radio_Hal_NirqLevel(void);									//读取nIRQ电平

void Radio_Hal_SpiWriteByte(unsigned char byteToWrite);						//SPI写1Byte
unsigned char Radio_Hal_SpiReadByte(void);									//SPI读1Byte

void Radio_Hal_SpiWriteData(unsigned short dataInLength, unsigned char* dataIn);	//SPI写buffer
void Radio_Hal_SpiReadData(unsigned short dataOutLength, unsigned char* dataOut);	//SPI读buffer

#endif
