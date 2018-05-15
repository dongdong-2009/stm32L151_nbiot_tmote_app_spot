#ifndef __RADIO_COMPILER_DEFS_H
#define   __RADIO_COMPILER_DEFS_H

#include "sys.h"

#ifndef NULL
#define NULL							0
#endif

#define LSB							0
#define MSB							1

#define b0							0
#define b1							1
#define b2							2
#define b3							3

typedef bool							BIT;
typedef unsigned char					U8;
typedef unsigned int					U16;
typedef unsigned long					U32;

typedef signed char						S8;
typedef signed int						S16;
typedef signed long						S32;

typedef union UU16
{
	U16		U16;
	S16		S16;
	U8		U8[2];
	S8		S8[2];
}UU16;

typedef union UU32
{
    U32		U32;
    S32		S32;
    UU16		UU16[2];
    U16		U16[2];
    S16		S16[2];
    U8		U8[4];
    S8		S8[4];
}UU32;

#define BITS(bitArray, bitPos)  BIT bitArray ## bitPos
#define WRITE_TO_BIT_ARRAY(bitArray, byte)  bitArray ## 0 = byte & 0x01; \
                                            bitArray ## 1 = byte & 0x02; \
                                            bitArray ## 2 = byte & 0x04; \
                                            bitArray ## 3 = byte & 0x08; \
                                            bitArray ## 4 = byte & 0x10; \
                                            bitArray ## 5 = byte & 0x20; \
                                            bitArray ## 6 = byte & 0x40; \
                                            bitArray ## 7 = byte & 0x80;

#define READ_FROM_BIT_ARRAY(bitArray, byte) byte =  (bitArray ## 0) | \
                                                   ((bitArray ## 1) << 1) | \
                                                   ((bitArray ## 2) << 2) | \
                                                   ((bitArray ## 3) << 3) | \
                                                   ((bitArray ## 4) << 4) | \
                                                   ((bitArray ## 5) << 5) | \
                                                   ((bitArray ## 6) << 6) | \
                                                   ((bitArray ## 7) << 7);

#define NOP() __no_operation();

#endif
