#ifndef __RADIO_HAL_RF_H
#define   __RADIO_HAL_RF_H

#include "sys.h"
#include "radio_compiler_defs.h"

/* Definition for SPIx clock resources */
#define SPIx							SPI1
#define SPIx_RCC_CLK_ENABLE()				__HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_RCC_CLK_DISABLE()			__HAL_RCC_SPI1_CLK_DISABLE()

#define SPIx_FORCE_RESET()				__HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()				__HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx GPIO resources */
#define SPIx_NSS_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_NSS_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()

/* Definition for SPIx Pins */
#define SPIx_NSS_GPIO_PORT				GPIOA
#define SPIx_NSS_PIN					GPIO_PIN_15
#define SPIx_SCK_GPIO_PORT				GPIOA
#define SPIx_SCK_PIN					GPIO_PIN_5
#define SPIx_SCK_AF						GPIO_AF5_SPI1
#define SPIx_MOSI_GPIO_PORT				GPIOA
#define SPIx_MOSI_PIN					GPIO_PIN_7
#define SPIx_MOSI_AF					GPIO_AF5_SPI1
#define SPIx_MISO_GPIO_PORT				GPIOA
#define SPIx_MISO_PIN					GPIO_PIN_6
#define SPIx_MISO_AF					GPIO_AF5_SPI1

/* Definition for RF GPIO resources */
#define RF_SDN_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_SDN_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOB_CLK_DISABLE()
#define RF_nIRQ_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_nIRQ_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOB_CLK_DISABLE()

/* Definition for RF Pins */
#define RF_SDN_GPIO_PORT					GPIOB
#define RF_SDN_PIN						GPIO_PIN_4
#define RF_nIRQ_GPIO_PORT				GPIOB
#define RF_nIRQ_PIN						GPIO_PIN_10
#define RF_nIRQ_Read()					HAL_GPIO_ReadPin(RF_nIRQ_GPIO_PORT, RF_nIRQ_PIN)

/* Definition for SPIx's NVIC */
#define RF_IRQn						EXTI15_10_IRQn

#define MMESH_CHECKSUM_SIZE				2

enum TRF_ERRCODE
{
	TRF_OK = 0,
	TRF_ERROR,
};

enum Protocol_type
{
	OPTIONAL_PRT,
	SIMPLICITI_PRT,
	XMESH_PRT,
	XMESHLITE_PRT,
	UPGRADE_PRT,
	XMESHCFG_PRT,
	RSVED6,
	RSVED7
};

typedef enum
{
	CHECK_FAIL,
	CHECK_SUCESS
}check_t;

extern SPI_HandleTypeDef SPI_Handler;										//SPI句柄
extern unsigned char RF_CHANNEL1;
extern char trf_status;

char Radio_Rf_Init(void);												//initialize the radio chip
char Radio_Rf_PrepareToTx(uint8_t* pPacket, uint8_t len);						//send a wireless packet
void Radio_Rf_ISR(void);													//RF IRQ
void Radio_Rf_Interface_Init(void);										//Initialize the mcu's spi interface
void Radio_Rf_Interface_Deinit(void);										//Deinitialize the mcu's spi interface
void Radio_Rf_Sleep(void);												//set the radio to sleep mode
char Radio_Rf_is_Sleep(void);												//check the radio to sleep mode
void Radio_Rf_Interrupt_Init(void);										//Init the radio Interrupt
void Radio_Rf_Interrupt_Deinit(void);										//Deinit the radio Interrupt
char Radio_Rf_get_Status(void);											//get the radio error status

#endif
