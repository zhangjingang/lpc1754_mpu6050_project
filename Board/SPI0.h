#ifndef SPI0_H
#define SPI0_H

#include "LPC17xx.h"  

#define uint8    uint8_t
#define uint16   uint16_t
#define uint32   uint32_t

void SPI0_Init(void);
uint8 SPI0_communication(uint8 TxData);

#endif
