#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "LPC17xx.h"

// Set the low power consumption
#ifdef	MODE_SLEEP_EN
#define CPU_IDLE()	vicSysPconSet(0)	// Enter into normal sleep mode
#else
#define CPU_IDLE() //none
#endif


// Struct of timer, which the members must be volatile, or there will be a problem when optimazing the C compiler 
typedef struct
{
	volatile uint32_t count;	// counter
	volatile uint8_t flag;		// flag of timer timing up
}SOFT_TMR;

extern uint32_t	SystemFrequency;   
extern void vicSysPconSet (uint16_t usPconMode);

void SysTickInit(void);
void BspDelayMS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);
void get_ms(unsigned long  *time);
#endif

