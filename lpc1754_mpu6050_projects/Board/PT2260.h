#ifndef PT2260_H
#define PT2260_H
 
#include "LPC17xx.h" 


/******************************************************************
LPC1754--PT2260Ó²¼þÁ¬½Ó£º
RF_A0: P2.1
RF_A1: P2.2
RF_A2: P2.3
RF_A3: P2.4
RF_A4: P2.5
RF_A5: P2.6
RF_A6: P2.7
RF_A7: P2.8
RF_D0: P0.7
RF_D1: P0.8
RF_D2: P0.9
RF_D3: P2.0
*******************************************************************/
#define PIN_RF_A0       (1<<1)
#define PIN_RF_A1       (1<<2)
#define PIN_RF_A2       (1<<3)
#define PIN_RF_A3       (1<<4)
#define PIN_RF_A4       (1<<5)
#define PIN_RF_A5       (1<<6)
#define PIN_RF_A6       (1<<7)
#define PIN_RF_A7       (1<<8)
#define PIN_RF_D0       (1<<7)
#define PIN_RF_D1       (1<<8)
#define PIN_RF_D2       (1<<9)
#define PIN_RF_D3       (1<<0)

#define RF_A0(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_A0):(LPC_GPIO2->FIOCLR |= PIN_RF_A0))
#define RF_A1(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_A1):(LPC_GPIO2->FIOCLR |= PIN_RF_A1))
#define RF_A2(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_A2):(LPC_GPIO2->FIOCLR |= PIN_RF_A2))
#define RF_A3(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_A3):(LPC_GPIO2->FIOCLR |= PIN_RF_A3))
#define RF_A4(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_A4):(LPC_GPIO2->FIOCLR |= PIN_RF_A4))
#define RF_A5(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_A5):(LPC_GPIO2->FIOCLR |= PIN_RF_A5))
#define RF_A6(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_A6):(LPC_GPIO2->FIOCLR |= PIN_RF_A6))
#define RF_A7(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_A7):(LPC_GPIO2->FIOCLR |= PIN_RF_A7))
#define RF_D0(x)    ((x)?(LPC_GPIO0->FIOSET |= PIN_RF_D0):(LPC_GPIO0->FIOCLR |= PIN_RF_D0))
#define RF_D1(x)    ((x)?(LPC_GPIO0->FIOSET |= PIN_RF_D1):(LPC_GPIO0->FIOCLR |= PIN_RF_D1))
#define RF_D2(x)    ((x)?(LPC_GPIO0->FIOSET |= PIN_RF_D2):(LPC_GPIO0->FIOCLR |= PIN_RF_D2))
#define RF_D3(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_RF_D3):(LPC_GPIO2->FIOCLR |= PIN_RF_D3))

void PT2260Init(void);
void PT2260SendBtye(uint8_t addr, uint8_t dat);

#endif
