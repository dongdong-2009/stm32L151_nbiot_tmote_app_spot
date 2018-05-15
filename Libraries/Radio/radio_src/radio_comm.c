/**
  *********************************************************************************************************
  * @file    radio_comm.c
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

#include "radio_comm.h"
#include "radio_hal.h"

unsigned char ctsWentHigh = 0;

/**********************************************************************************************************
 @Function			unsigned char Radio_Comm_GetResp(unsigned char byteCount, unsigned char* pData)
 @Description			Radio_Comm_GetResp				: Gets a command response from the radio chip
 @Input				byteCount						: Number of bytes to get from the radio chip
					pData						: Pointer to where to put the data
 @Return				CTS value
**********************************************************************************************************/
unsigned char Radio_Comm_GetResp(unsigned char byteCount, unsigned char* pData)
{
	unsigned char ctsVal = 0;
	unsigned int errCnt = RADIO_CTS_TIMEOUT;
	int loopCount;
	
	/* wait until radio IC is ready with the data */
	while (errCnt != 0) {
		Radio_Hal_DeassertNsel();
		Radio_Hal_SpiWriteByte(0x44);
		ctsVal = Radio_Hal_SpiReadByte();
		if (ctsVal == 0xFF) {
			if (byteCount) {
				Radio_Hal_SpiReadData(byteCount, pData);
			}
			Radio_Hal_AssertNsel();
			break;
		}
		Radio_Hal_AssertNsel();
		errCnt--;
		for (loopCount = 5; loopCount != 0; loopCount--);
	}
	
	if (errCnt == 0) {
		while (1){
		#ifdef RADIO_COMM_ERROR_CALLBACK
			RADIO_COMM_ERROR_CALLBACK();
		#else
			break;
		#endif
		}
	}
	
	if (ctsVal == 0xFF) {
		ctsWentHigh = 1;
	}
	
	return ctsVal;
}

/**********************************************************************************************************
 @Function			unsigned char Radio_Comm_PollCTS(void)
 @Description			Radio_Comm_PollCTS				: Waits for CTS to be high
 @Input				void
 @Return				CTS value
**********************************************************************************************************/
unsigned char Radio_Comm_PollCTS(void)
{
#ifdef RADIO_USER_CFG_USE_GPIO1_FOR_CTS
	while (!radio_hal_Gpio1Level()) {
		/* Wait...*/
	}
	ctsWentHigh = 1;
	return 0xFF;
#else
	return Radio_Comm_GetResp(0, 0);
#endif
}

/**********************************************************************************************************
 @Function			void Radio_Comm_ClearCTS(void)
 @Description			Radio_Comm_ClearCTS				: Clears the CTS state variable
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Comm_ClearCTS(void)
{
	ctsWentHigh = 0;
}

/**********************************************************************************************************
 @Function			void Radio_Comm_SendCmd(unsigned char byteCount, unsigned char* pData)
 @Description			Radio_Comm_SendCmd				: Sends a command to the radio chip
 @Input				byteCount						: Number of bytes in the command to send to the radio device
					pData						: Pointer to the command to send
 @Return				void
**********************************************************************************************************/
void Radio_Comm_SendCmd(unsigned char byteCount, unsigned char* pData)
{
	char count = 10;
	
	while (!ctsWentHigh) {
		Radio_Comm_PollCTS();
		count--;
		if (count == 0) {
			break;
		}
	}
	Radio_Hal_DeassertNsel();
	Radio_Hal_SpiWriteData(byteCount, pData);
	Radio_Hal_AssertNsel();
	ctsWentHigh = 0;
}

/**********************************************************************************************************
 @Function			void Radio_Comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
 @Description			Radio_Comm_ReadData				: Read
 @Input				cmd           					: Command ID
					pollCts						: Set to poll CTS
					byteCount						: Number of bytes to get from the radio chip
					pData						: Pointer to where to put the data
 @Return				void
**********************************************************************************************************/
void Radio_Comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
	char count = 10;
	
	if (pollCts) {
		while (!ctsWentHigh) {
			Radio_Comm_PollCTS();
			count--;
			if (count == 0) {
				break;
			}
		}
	}
	Radio_Hal_DeassertNsel();
	Radio_Hal_SpiWriteByte(cmd);
	Radio_Hal_SpiReadData(byteCount, pData);
	Radio_Hal_AssertNsel();
	ctsWentHigh = 0;
}

/**********************************************************************************************************
 @Function			void Radio_Comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
 @Description			Radio_Comm_ReadData				: Read
 @Input				cmd           					: Command ID
					pollCts						: Set to poll CTS
					byteCount						: Number of bytes to get from the radio chip
					pData						: Pointer to where to put the data
 @Return				void
**********************************************************************************************************/
void Radio_Comm_WriteData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
	char count = 10;
	
	if (pollCts) {
		while (!ctsWentHigh) {
			Radio_Comm_PollCTS();
			count--;
			if (count == 0) {
				break;
			}
		}
	}
	Radio_Hal_DeassertNsel();
	Radio_Hal_SpiWriteByte(cmd);
	Radio_Hal_SpiWriteData(byteCount, pData);
	Radio_Hal_AssertNsel();
	ctsWentHigh = 0;
}

/**********************************************************************************************************
 @Function			unsigned char Radio_Comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData, uint8_t respByteCount, uint8_t* pRespData)
 @Description			Radio_Comm_SendCmdGetResp		: Sends a command to the radio chip and gets a response
 @Input				cmdByteCount           			: Number of bytes in the command to send to the radio device
					pCmdData						: Pointer to the command data
					respByteCount					: Number of bytes in the response to fetch
					pRespData						: Pointer to where to put the response data
 @Return				CTS value
**********************************************************************************************************/
unsigned char Radio_Comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData, uint8_t respByteCount, uint8_t* pRespData)
{
	Radio_Comm_SendCmd(cmdByteCount, pCmdData);
	
	return Radio_Comm_GetResp(respByteCount, pRespData);
}

/********************************************** END OF FLEE **********************************************/
