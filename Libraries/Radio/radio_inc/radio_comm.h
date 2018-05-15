#ifndef __RADIO_COMM_H
#define   __RADIO_COMM_H

#include "sys.h"

#define RADIO_CTS_TIMEOUT				10000

unsigned char Radio_Comm_GetResp(unsigned char byteCount, unsigned char* pData);				//Gets a command response from the radio chip
void Radio_Comm_SendCmd(unsigned char byteCount, unsigned char* pData);						//Sends a command to the radio chip
unsigned char Radio_Comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData, uint8_t respByteCount, uint8_t* pRespData);

void Radio_Comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);
void Radio_Comm_WriteData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);

unsigned char Radio_Comm_PollCTS(void);													//Waits for CTS to be high
void Radio_Comm_ClearCTS(void);														//Clears the CTS state variable

#endif
