#ifndef __RADIO_H
#define   __RADIO_H

#include "sys.h"
#include "radio_compiler_defs.h"

#define		SI4438_B1
//#define		SI4438_C2

#define RF_STATE_SLEEP					0
#define RF_STATE_RX						1
#define RF_STATE_TX						2
#define RF_STATE_STANDBY					3

/*! Maximal packet length definition (FIFO size) */
#define RADIO_MAX_PACKET_LENGTH			64u
#define RADIO_TX_ALMOST_EMPTY_THRESHOLD		62u
#define RADIO_RX_ALMOST_FULL_THRESHOLD		62u
#define RADIO_MAX_LONG_PACKET_LENGTH		255u
#define RADIO_RX_FIFO_DATA_ARRAY			128u

void Radio_PowerUp(void);
unsigned char Radio_Check(void);
void Radio_StartRX(uint8_t channel, uint8_t packetLenght);
void Radio_StartRX_Tune(U8 channel, U8 packetLenght);
unsigned char Radio_StartTx_Variable_Packet(uint8_t channel, uint8_t* pioRadioPacket, uint8_t length);
void Radio_StartTx_dummy(uint8_t channel);
void Radio_Sleep(void);
char Radio_Get_State(void);
void Radio_StartTx(U8 channel, U8* pioRadioPacket, U8 len);

#endif
