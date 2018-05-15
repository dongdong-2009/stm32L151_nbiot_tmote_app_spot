/**
  *********************************************************************************************************
  * @file    tmesh_xtea.h
  * @author  Movebroad -- Bell Zhong
  * @version V1.0
  * @date    2017-12-14
  * @brief   XTEA加密解密库
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#ifndef __TMESH_XTEA_H
#define   __TMESH_XTEA_H

#include "sys.h"

/**********************************************************************************************************
 @Function			void tmesh_securityInit(void)
 @Description			tmesh_securityInit					: XTEA加密解密库初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void tmesh_securityInit(void);

/**********************************************************************************************************
 @Function			void tmesh_decipher(uint8_t *msg, uint8_t len, uint32_t *cntStart)
 @Description			tmesh_decipher						: 解密
 @Input				msg		: the message to be encoded
					len		: the length of the message
					cntStart	: the counter to be encoded
 @Return				void
**********************************************************************************************************/
void tmesh_decipher(uint8_t *msg, uint8_t len, uint32_t *cntStart);

/**********************************************************************************************************
 @Function			void tmesh_encipher(uint8_t *msg, uint8_t len, uint32_t *cntStart)
 @Description			tmesh_encipher						: 加密
 @Input				msg		: the message to be decoded
					len		: the length of the message
					cntStart	: the counter to be decoded
 @Return				void
**********************************************************************************************************/
void tmesh_encipher(uint8_t *msg, uint8_t len, uint32_t *cntStart);

/**********************************************************************************************************
 @Function			void tmesh_securityMix(uint32_t sn)
 @Description			tmesh_securityMix					: 用SN设置密钥
 @Input				sn: the device's sn
 @Return				void
**********************************************************************************************************/
void tmesh_securityMix(uint32_t sn);

#endif
