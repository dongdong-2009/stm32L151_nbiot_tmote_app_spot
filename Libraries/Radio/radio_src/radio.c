/**
  *********************************************************************************************************
  * @file    radio.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-29
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "radio.h"
#include "si446x_api_lib.h"
#include "si446x_cmd.h"

static char g_rf_state = RF_STATE_SLEEP;
unsigned char g_Rest_num = 0, g_Send_num = 0;
unsigned char *g_P_frame;

/**********************************************************************************************************
 @Function			void Radio_PowerUp(void)
 @Description			Radio_PowerUp				: Power up the Radio
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_PowerUp(void)
{
	U16 wDelay = 0;
	
	si446x_reset();
	
	for (; wDelay < 0x1000; wDelay++);
}

/**********************************************************************************************************
 @Function			unsigned char Radio_Check(void)
 @Description			Radio_Check				: Check if Packet sent IT flag or Packet Received IT is pending
 @Input				void
 @Return				SI4455_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT / SI4455_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT
**********************************************************************************************************/
unsigned char Radio_Check(void)
{
	/* Read ITs, clear pending ones */
	si446x_get_int_status(0u, 0u, 0u);
	
	if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_BIT) {
		/* State change to */
		si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_SLEEP);
		
		/* Reset FIFO */
		si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
		
		/* State change to */
		si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_RX);
	}
	
	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT) {
		
		return SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT;
	}
	
	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT) {
		
		return SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT;
	}
	
	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_CRC_ERROR_BIT) {
		/* Reset FIFO */
		si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
	}
	
	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_TX_FIFO_ALMOST_EMPTY_PEND_BIT) {
		
		return SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_TX_FIFO_ALMOST_EMPTY_PEND_BIT;
	}
	
	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_RX_FIFO_ALMOST_FULL_BIT) {
		
		return SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_RX_FIFO_ALMOST_FULL_BIT;
	}
	
	return 0;
}

/**********************************************************************************************************
 @Function			void Radio_StartRX(uint8_t channel, uint8_t packetLenght)
 @Description			Radio_StartRX				: Set Radio to RX mode
 @Input				channel					: Freq. Channel
					packetLength				: 0 Packet handler fields are used , nonzero: only Field1 is used
 @Return				void
**********************************************************************************************************/
void Radio_StartRX(uint8_t channel, uint8_t packetLenght)
{
	/* Read ITs, clear pending ones */
	si446x_get_int_status(0u, 0u, 0u);
	
	/* Reset the Rx Fifo */
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
	
	/* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
	si446x_start_rx(channel, 0u, packetLenght, SI446X_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE, 
									   SI446X_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_READY, 
									   SI446X_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX);
	
	g_rf_state = RF_STATE_RX;
}

/**********************************************************************************************************
 @Function			void Radio_StartRX_Tune(U8 channel, U8 packetLenght)
 @Description			Radio_StartRX_Tune			: Set Radio to RX mode
 @Input				channel					: Freq. Channel
					packetLength				: 0 Packet handler fields are used , nonzero: only Field1 is used
 @Return				void
**********************************************************************************************************/
void Radio_StartRX_Tune(U8 channel, U8 packetLenght)
{
	/* Read ITs, clear pending ones */
	si446x_get_int_status(0u, 0u, 0u);
	
	/* Reset the Rx Fifo */
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
	
	/* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
	si446x_start_rx(channel, 0u, packetLenght, SI446X_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE, 
									   SI446X_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_READY, 
									   SI446X_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX_TUNE);
}

/**********************************************************************************************************
 @Function			unsigned char Radio_StartTx_Variable_Packet(uint8_t channel, uint8_t* pioRadioPacket, uint8_t length)
 @Description			Radio_StartTx_Variable_Packet	: Set Radio to TX mode, variable packet length
 @Input				channel					: Freq. Channel
					packetLength				: Packet to be sent length of of the packet sent to TXFIFO
 @Return				void
**********************************************************************************************************/
unsigned char Radio_StartTx_Variable_Packet(uint8_t channel, uint8_t* pioRadioPacket, uint8_t length)
{
	uint8_t len_sent = 0;
	
	/* Leave RX state */
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY);
	
	/* Reset the Tx Fifo */
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);
	
	/* Read ITs, clear pending ones */
	si446x_get_int_status(0u, 0u, 0u);
	
	if(length >= RADIO_TX_ALMOST_EMPTY_THRESHOLD) {
		/* Data to be sent is more than the size of TX FIFO */
		si446x_write_tx_fifo(RADIO_TX_ALMOST_EMPTY_THRESHOLD, pioRadioPacket);
		len_sent = RADIO_TX_ALMOST_EMPTY_THRESHOLD;
		g_P_frame += RADIO_TX_ALMOST_EMPTY_THRESHOLD;
		g_Send_num = RADIO_TX_ALMOST_EMPTY_THRESHOLD;
		g_Rest_num = length+1 - RADIO_TX_ALMOST_EMPTY_THRESHOLD;
	}
	else {
		si446x_write_tx_fifo(length + 1, pioRadioPacket);
		len_sent = length + 1;
	}
	
	/* 设置发送数据长度 */
	si446x_set_property(0x12, 0x02, 0x11, 0x00, length);
	
	/* 发送数据 */
	si446x_start_tx(channel, 0x80, 0x00);
	
	g_rf_state = RF_STATE_TX;
	
	return len_sent;
}

/**********************************************************************************************************
 @Function			void Radio_StartTx_dummy(uint8_t channel)
 @Description			Radio_StartTx_dummy			: Set Radio to TX mode
 @Input				channel					: Freq. Channel
 @Return				void
**********************************************************************************************************/
void Radio_StartTx_dummy(uint8_t channel)
{
	/* Leave RX state */
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY);
	
	/* Reset the Tx Fifo */
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);
	
	/* Read ITs, clear pending ones */
	si446x_get_int_status(0u, 0u, 0u);
	
	si446x_write_tx_fifo(1, (uint8_t*)"0");
	
	/* 设置发送数据长度 */
	si446x_set_property(0x12, 0x02, 0x11, 0x00, 1);
	
	/* 发送数据 */
	si446x_start_tx(channel, 0x80, 0x00);
	
	g_rf_state = RF_STATE_TX;
}

/**********************************************************************************************************
 @Function			void Radio_Sleep(void)
 @Description			Radio_Sleep				: set the radio to sleep mode
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Sleep(void)
{
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_SLEEP);
	
	g_rf_state = RF_STATE_SLEEP;
}

/**********************************************************************************************************
 @Function			char Radio_Get_State(void)
 @Description			Radio_Get_State			: get the radio state
 @Input				void
 @Return				state
**********************************************************************************************************/
char Radio_Get_State(void)
{
	return g_rf_state;
}

/**********************************************************************************************************
 @Function			void Radio_StartTx(U8 channel, U8* pioRadioPacket, U8 len)
 @Description			Radio_StartTx				: start send long package data
 @Input				channel					: Freq. Channel
					packetLength				: Packet to be sent length of of the packet sent to TXFIFO
 @Return				void
**********************************************************************************************************/
void Radio_StartTx(U8 channel, U8* pioRadioPacket, U8 len)
{
	/* Reset TX FIFO */
  	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);
	
	/* Read ITs, clear pending ones */
	si446x_get_int_status(0u, 0u, 0u);
	
	if (len >= RADIO_TX_ALMOST_EMPTY_THRESHOLD) {
		/* Data to be sent is more than the size of TX FIFO */
		si446x_write_tx_fifo(RADIO_TX_ALMOST_EMPTY_THRESHOLD, pioRadioPacket);
	}
	else {
		si446x_write_tx_fifo(len + 1, pioRadioPacket);
	}
	
	/* 设置发送数据长度 */
	si446x_set_property(0x12, 0x02, 0x11, 0x00, len);
	
	/* 发送数据 */
	si446x_start_tx(channel, 0x80, 0x00);
	
	g_rf_state = RF_STATE_TX;
}

/********************************************** END OF FLEE **********************************************/
