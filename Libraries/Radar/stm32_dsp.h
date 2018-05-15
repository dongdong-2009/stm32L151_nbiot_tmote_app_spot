#ifndef __STM32_DSP_H
#define   __STM32_DSP_H

#include "sys.h"

typedef struct {
	u16 *h;
	u32 nh;
}COEFS;

/* FIR 16-bit filter in assembly */
void fir_16by16_stm32(void *y, void *x, COEFS *c, uint32_t N);

/* PID controller in C, error computed outside the function */
u16  DoPID(uint16_t Error, uint16_t *Coeff);

/* Full PID in C, error computed inside the function */
u16  DoFullPID(uint16_t In, uint16_t Ref, uint16_t *Coeff);

/* PID controller in assembly, error computed outside the function */
u16  PID_stm32(uint16_t Error, uint16_t *Coeff);

/* Radix-4 complex FFT for STM32, in assembly */
/* 64 points*/
void cr4_fft_64_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);
/* 256 points */
void cr4_fft_256_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);
/* 1024 points */
void cr4_fft_1024_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);

/* IIR filter in assembly */
//void iirarma_stm32(void *y, void *x, uint16_t *h2, uint16_t *h1, uint32_t ny );

/* IIR filter in C */
//void iir_biquad_stm32(uint16_t *y, uint16_t *x, int16_t *IIRCoeff, uint16_t ny);

#endif
