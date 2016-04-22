

#ifndef I2CINT_H
#define I2CINT_H

#include "LPC17xx.h"

#define ONE_BYTE_SUBA   1
#define TWO_BYTE_SUBA   2
#define X_ADD_8_SUBA    3


#define INT8U    uint8_t
#define INT16U   uint16_t
#define INT32U   uint32_t

#ifndef FALSE
#define FALSE    0
#endif

#ifndef TRUE
#define TRUE     1
#endif


extern  uint32_t       SystemFrequency;                                 /* Clock Variable               */ 

INT8U IRcvByte(INT8U sla, INT8U *dat);
INT8U ISendByte(INT8U sla, INT8U dat);
INT8U I2C_ReadNByte (INT8U sla, INT32U suba_type, INT32U suba, INT8U *s, INT32U num);
INT8U I2C_WriteNByte(INT8U sla, INT8U suba_type, INT32U suba, INT8U *s, INT32U num);
void  i2c1Init (INT32U ulFi2c);

int8_t i2cread(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
int8_t i2cwrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data);
#endif
