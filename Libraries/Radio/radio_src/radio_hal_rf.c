/**
  *********************************************************************************************************
  * @file    hal_rf.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-26
  * @brief   
  *********************************************************************************************************
  * @attention
  *					PB4	:		SDN
  *					PB10	:		nIRQ
  *					PA15	:		SPI1_NSS
  *					PA5	:		SPI1_SCK
  *					PA7	:		SPI1_MOSI
  *					PA6	:		SPI1_MISO
  *********************************************************************************************************
  */

#include "radio_hal_rf.h"
#include "radio_rf_app.h"
#include "radio.h"
#include "si446x_api_lib.h"
#include "si446x_cmd.h"
#include "si446x_defs.h"
#include "platform_config.h"
#include "delay.h"
#include "string.h"

#ifdef	SI4438_B1
#include "radio_config_Si4438_475_100k_B1_10dbm.h"
#else
#include "radio_config_Si4438_475_100k_C2_10dbm.h"
#endif

enum
{
	SEND_OVER,
	SENDING
};

enum Sendtype
{
	NO_WAIT,
	WAIT_OVER
};

SPI_HandleTypeDef SPI_Handler;											//SPI句柄

unsigned char RF_CHANNEL1 = 36;

const U8 Radio_Configuration_Data_Array[] = RADIO_CONFIGURATION_DATA_ARRAY;
uint8_t g_Pakcet_len;
uint8_t g_Datasending = SENDING;
uint8_t g_Wait_enable  = WAIT_OVER;

static char rf_inited = 0;
char trf_status = TRF_OK;

extern uint8_t *g_P_frame;
extern uint8_t  g_Send_num;
extern uint8_t  g_Rest_num;

/**********************************************************************************************************
 @Function			void Radio_Rf_Interface_Init(void)
 @Description			Radio_Rf_Interface_Init		: initialize the mcu's spi interface
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_Interface_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	SPI_Handler.Instance				= SPIx;
	
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == SYSTEMCLOCKMSI)
	SPI_Handler.Init.BaudRatePrescaler		= SPI_BAUDRATEPRESCALER_2;
#elif (SYSTEMCLOCK == SYSTEMCLOCKHSI)
	SPI_Handler.Init.BaudRatePrescaler		= SPI_BAUDRATEPRESCALER_8;
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	
	SPI_Handler.Init.Direction			= SPI_DIRECTION_2LINES;
	SPI_Handler.Init.CLKPhase			= SPI_PHASE_1EDGE;
	SPI_Handler.Init.CLKPolarity			= SPI_POLARITY_LOW;
	SPI_Handler.Init.CRCCalculation		= SPI_CRCCALCULATION_DISABLE;
	SPI_Handler.Init.CRCPolynomial		= 7;
	SPI_Handler.Init.DataSize			= SPI_DATASIZE_8BIT;
	SPI_Handler.Init.FirstBit			= SPI_FIRSTBIT_MSB;
	SPI_Handler.Init.NSS				= SPI_NSS_SOFT;
	SPI_Handler.Init.TIMode				= SPI_TIMODE_DISABLE;
	SPI_Handler.Init.Mode				= SPI_MODE_MASTER;
	
	if (HAL_SPI_Init(&SPI_Handler) != HAL_OK) {
		/* Initialization Error */
	}
	
	RF_SDN_GPIO_CLK_ENABLE();
	RF_nIRQ_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin					= RF_SDN_PIN;
	GPIO_Initure.Mode					= GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Speed					= GPIO_SPEED_HIGH;
	HAL_GPIO_Init(RF_SDN_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin					= RF_nIRQ_PIN;
	GPIO_Initure.Mode					= GPIO_MODE_IT_FALLING;
	GPIO_Initure.Pull					= GPIO_PULLUP;
	GPIO_Initure.Speed					= GPIO_SPEED_HIGH;
	HAL_GPIO_Init(RF_nIRQ_GPIO_PORT, &GPIO_Initure);
	
	/* Set EXTI lines 15 to 10 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(RF_IRQn, 2, 0);
	HAL_NVIC_DisableIRQ(RF_IRQn);
}

/**********************************************************************************************************
 @Function			void Radio_Rf_Interface_Deinit(void)
 @Description			Radio_Rf_Interface_Deinit	: Deinitialize the mcu's spi interface
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_Interface_Deinit(void)
{
	if (HAL_SPI_DeInit(&SPI_Handler) != HAL_OK) {
		/* Deinitialization Error */
	}
}

/**********************************************************************************************************
 @Function			void Radio_Rf_Sleep(void)
 @Description			Radio_Rf_Sleep				: set the radio to sleep mode
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_Sleep(void)
{
	Radio_Sleep();
}

/**********************************************************************************************************
 @Function			char Radio_Rf_is_Sleep(void)
 @Description			Radio_Rf_is_Sleep			: set the radio to sleep mode
 @Input				void
 @Return				void
**********************************************************************************************************/
char Radio_Rf_is_Sleep(void)
{
	return (RF_STATE_SLEEP == Radio_Get_State());
}

/**********************************************************************************************************
 @Function			char Radio_Rf_get_Status(void)
 @Description			Radio_Rf_get_Status			: get the radio error status
 @Input				void
 @Return				TRF_OK/TRF_ERROR
**********************************************************************************************************/
char Radio_Rf_get_Status(void)
{
#ifdef	RADIO_SI4438
	return trf_status;
#else
	return TRF_ERROR;
#endif
}

/**********************************************************************************************************
 @Function			void Radio_Rf_Interrupt_Init(void)
 @Description			Radio_Rf_Interrupt_Init		: Init the radio Interrupt
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_Interrupt_Init(void)
{
	HAL_NVIC_EnableIRQ(RF_IRQn);
}

/**********************************************************************************************************
 @Function			void Radio_Rf_Interrupt_Deinit(void)
 @Description			Radio_Rf_Interrupt_Deinit	: Deinit the radio Interrupt
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_Interrupt_Deinit(void)
{
	HAL_NVIC_DisableIRQ(RF_IRQn);
}

/**********************************************************************************************************
 @Function			char Radio_Rf_Init(void)
 @Description			Radio_Rf_Init			: initialize the radio chip
 @Input				void
 @Return				void
**********************************************************************************************************/
char Radio_Rf_Init(void)
{
	unsigned char i = 0;
	
	/* step 0 : */
	Radio_Rf_Interface_Init();
	
	/* step 1: reset chip */
	for (i = 0; i < 3; i++) {
		Radio_PowerUp();
		/* step 2: set up the rf chip */
		si446x_part_info();
		
		if (Si446xCmd.PART_INFO.PART != 0x4438) {
			trf_status = TRF_ERROR;
		}
		else {
			rf_inited = 1;
			trf_status = TRF_OK;
			break;
		}
	}
	
	if (trf_status == TRF_ERROR) {
		return TRF_ERROR;
	}
	
	while (SI446X_SUCCESS != si446x_configuration_init(Radio_Configuration_Data_Array)) {
		
		Radio_PowerUp();
	}
	
	/* step 3: Get the chip's Interrupt status/pending flags form the radio and clear flags if requested */
	si446x_get_int_status(0u, 0u, 0u);
	
	/* step 4: set the rf chip to rx state */
	Radio_StartRX(RF_CHANNEL1, 0);
	
	Radio_Rf_Interrupt_Init();
	
	return TRF_OK;
}

/**********************************************************************************************************
 @Function			char Radio_Rf_PrepareToTx(uint8_t* pPacket, uint8_t len)
 @Description			Radio_Rf_PrepareToTx		: send a wireless packet
 @Input				pPacket					: pointer of pPacket to be sent
					len						: length of pPacket to be sent
 @Return				void
**********************************************************************************************************/
char Radio_Rf_PrepareToTx(uint8_t* pPacket, uint8_t len)
{
	uint32_t s_count = len + 5;
	
	g_P_frame = pPacket;
	g_Datasending = SENDING;
	
	Radio_StartTx_Variable_Packet(RF_CHANNEL1, g_P_frame, len);
	
	g_Pakcet_len = len + 1;
	
	if (g_Wait_enable == WAIT_OVER) {
		while ((g_Datasending == SENDING) && (s_count--)) {
			Delay_US(150);
		}
		if (s_count == 0) {
			return TRF_ERROR;
		}
	}
	
	return TRF_OK;
}

void Radio_Rf_TxISR(void)
{
	if (g_Rest_num > RADIO_TX_ALMOST_EMPTY_THRESHOLD) {
		si446x_write_tx_fifo(RADIO_TX_ALMOST_EMPTY_THRESHOLD, g_P_frame);
		g_P_frame += RADIO_TX_ALMOST_EMPTY_THRESHOLD;
		g_Send_num += RADIO_TX_ALMOST_EMPTY_THRESHOLD;
		g_Rest_num = g_Pakcet_len - g_Send_num;
	}
	else if(g_Rest_num > 0) {
		si446x_write_tx_fifo(g_Rest_num, g_P_frame);
	}
}

void Radio_Rf_TxOverISR(void)
{
	g_Send_num = 0;
	g_Rest_num = 0;
	g_Datasending = SEND_OVER;
	si446x_set_property(0x12, 0x02, 0x11, 0x00, 255);
	Radio_StartRX(RF_CHANNEL1, 0);
}

check_t xm_CheckSum(uint8_t *recv_data)
{
	uint16_t check_sum = 0, i, len;
	
	len  = *recv_data;
	
	for (i = 1; i < len - 1; i++) {
		check_sum += recv_data[i];
	}
	
	*recv_data -= 2;
	
	if (check_sum == (recv_data[i]*0x100+recv_data[i+1]))
		return CHECK_SUCESS;
	else
		return CHECK_FAIL;
}

uint8_t		Recv_len;
uint8_t		g_LT_int;
uint8_t		current_num;
mrfiPacket_t	mrfiIncomingPacket;
/**********************************************************************************************************
 @Function			void Radio_Rf_ISR(void)
 @Description			Radio_Rf_ISR				: RF IRQ
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_ISR(void)
{
	uint8_t s_numOfRecvBytes = 0;
	uint8_t tmp;
	uint8_t* g_Recvlong = mrfiIncomingPacket.frame;
	
	if (rf_inited == 0) {
		return;
	}
	
	tmp = Radio_Check();
	if (tmp == SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT) {
		Radio_Rf_TxOverISR();
	}
	else if (tmp == SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_TX_FIFO_ALMOST_EMPTY_PEND_BIT) {
		Radio_Rf_TxISR();
	}
	else if (tmp == SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT) {
		s_numOfRecvBytes = Recv_len + 1 - current_num;
		
		if (!g_LT_int) {
			si446x_read_rx_fifo(1, &Recv_len);
			
			si446x_read_rx_fifo(Recv_len, &g_Recvlong[1]);
			
			g_Recvlong[0] = Recv_len;
		}
		else {
			si446x_read_rx_fifo(s_numOfRecvBytes, &g_Recvlong[current_num]);
			
			current_num += s_numOfRecvBytes;
		}
		
		g_Wait_enable = NO_WAIT;
		
		current_num = 0;
		g_LT_int = 0;
		
		si446x_set_property(0x12, 0x02, 0x11, 0x00, 255);
		Radio_StartRX(RF_CHANNEL1, 0);
		
		if (xm_CheckSum(g_Recvlong)) {
			Radio_Rf_Data_Handle_Isr(&mrfiIncomingPacket);
		}
		memset(g_Recvlong, 0x00, sizeof(mrfiIncomingPacket.frame));
		
		g_Wait_enable = WAIT_OVER;
	}
	else if (tmp == SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_RX_FIFO_ALMOST_FULL_BIT) {
		if (!g_LT_int) {
			si446x_read_rx_fifo(1, g_Recvlong);
			Recv_len = g_Recvlong[0];
			current_num = 0;
		}
		
		s_numOfRecvBytes = Recv_len + 1 - current_num;
		if (s_numOfRecvBytes >= RADIO_RX_ALMOST_FULL_THRESHOLD) {
			if (!g_LT_int) {
				si446x_read_rx_fifo(RADIO_RX_ALMOST_FULL_THRESHOLD - 1, &g_Recvlong[1]);
			}
			else {
				si446x_read_rx_fifo(RADIO_RX_ALMOST_FULL_THRESHOLD, &g_Recvlong[current_num]);
			}
			
			current_num += RADIO_RX_ALMOST_FULL_THRESHOLD;
			
			if (!g_LT_int) g_LT_int++;
		}
	}
}

/********************************************** END OF FLEE **********************************************/
