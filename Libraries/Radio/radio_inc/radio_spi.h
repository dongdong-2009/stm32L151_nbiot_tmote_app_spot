#ifndef __RADIO_SPI_H
#define   __RADIO_SPI_H

#include "sys.h"

unsigned char Radio_RF_ReadWriteByte_SPI1(unsigned char byteDataIn);								//读写SPI	Byte

void Radio_RF_WriteData_SPI1(unsigned short dataInLength, unsigned char* dataIn);					//写SPI Buffer
void Radio_RF_ReadData_SPI1(unsigned short dataOutLength, unsigned char* dataOut);					//读SPI Buffer

#endif
