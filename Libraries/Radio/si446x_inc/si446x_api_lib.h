#ifndef __SI446X_API_LIB_H
#define   __SI446X_API_LIB_H

#include "sys.h"
#include "radio_compiler_defs.h"

enum
{
	SI446X_SUCCESS,
	SI446X_NO_PATCH,
	SI446X_CTS_TIMEOUT,
	SI446X_PATCH_FAIL,
	SI446X_COMMAND_ERROR
};

extern union si446x_cmd_reply_union Si446xCmd;
extern unsigned char Pro2Cmd[16];

void si446x_reset(void);
void si446x_power_up(U8 BOOT_OPTIONS, U8 XTAL_OPTIONS, U32 XO_FREQ);

U8 si446x_configuration_init(const U8* pSetPropCmd);
void si446x_part_info(void);

void si446x_start_tx(uint8_t CHANNEL, uint8_t CONDITION, uint16_t TX_LEN);
void si446x_start_rx(uint8_t CHANNEL, uint8_t CONDITION, uint16_t RX_LEN, uint8_t NEXT_STATE1, uint8_t NEXT_STATE2, uint8_t NEXT_STATE3);

void si446x_get_int_status(uint8_t PH_CLR_PEND, uint8_t MODEM_CLR_PEND, uint8_t CHIP_CLR_PEND);
void si446x_gpio_pin_cfg(U8 GPIO0, U8 GPIO1, U8 GPIO2, U8 GPIO3, U8 NIRQ, U8 SDO, U8 GEN_CONFIG);

void si446x_set_property(uint8_t GROUP, uint8_t NUM_PROPS, uint8_t START_PROP, ...);
void si446x_change_state(uint8_t NEXT_STATE1);

void si446x_nop(void);

void si446x_fifo_info(uint8_t FIFO);
void si446x_write_tx_fifo(uint8_t numBytes, uint8_t* pTxData);
void si446x_read_rx_fifo(uint8_t numBytes, uint8_t* pRxData);

void si446x_get_property(U8 GROUP, U8 NUM_PROPS, U8 START_PROP);

void si446x_func_info(void);

void si446x_frr_a_read(U8 respByteCount);
void si446x_frr_b_read(U8 respByteCount);
void si446x_frr_c_read(U8 respByteCount);
void si446x_frr_d_read(U8 respByteCount);

void si446x_get_adc_reading(U8 ADC_EN);
void si446x_get_packet_info(U8 FIELD_NUMBER_MASK, U16 LEN, S16 DIFF_LEN);
void si446x_get_ph_status(U8 PH_CLR_PEND);
void si446x_get_modem_status(uint8_t MODEM_CLR_PEND);
void si446x_get_chip_status(U8 CHIP_CLR_PEND);

void si446x_ircal(U8 SEARCHING_STEP_SIZE, U8 SEARCHING_RSSI_AVG, U8 RX_CHAIN_SETTING1, U8 RX_CHAIN_SETTING2);
void si446x_ircal_manual(U8 IRCAL_AMP, U8 IRCAL_PH);

void si446x_request_device_state(void);

void si446x_tx_hop(U8 INTE, U8 FRAC2, U8 FRAC1, U8 FRAC0, U8 VCO_CNT1, U8 VCO_CNT0, U8 PLL_SETTLE_TIME1, U8 PLL_SETTLE_TIME0);
void si446x_rx_hop(U8 INTE, U8 FRAC2, U8 FRAC1, U8 FRAC0, U8 VCO_CNT1, U8 VCO_CNT0);

void si446x_start_tx_fast(void);
void si446x_start_rx_fast(void);

void si446x_get_int_status_fast_clear(void);
void si446x_get_int_status_fast_clear_read(void);

void si446x_gpio_pin_cfg_fast(void);

void si446x_get_ph_status_fast_clear(void);
void si446x_get_ph_status_fast_clear_read(void);

void si446x_get_modem_status_fast_clear(void);
void si446x_get_modem_status_fast_clear_read(void);

void si446x_get_chip_status_fast_clear(void);
void si446x_get_chip_status_fast_clear_read(void);

void si446x_fifo_info_fast_reset(U8 FIFO);
void si446x_fifo_info_fast_read(void);

#endif
