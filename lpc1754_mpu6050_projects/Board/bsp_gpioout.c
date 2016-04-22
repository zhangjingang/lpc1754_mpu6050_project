
#include "LPC17xx.h" 
#include "bsp_gpioout.h"

/**********************************************************************
*	                                  
*硬件接口如下
*
*	蜂鸣器FMQ
*			FMQ----P1.19	高电平响
*
*********************************************************************/
/*接口定义*/

/*PIN*/
#define PIN_BEEP		1 << 19

void bsp_BeepOn(void)		{LPC_GPIO1->FIOSET |= PIN_BEEP;}
void bsp_BeepOff(void)		{LPC_GPIO1->FIOCLR |= PIN_BEEP;}
void bsp_BeepToggled(void)	{((LPC_GPIO1->FIOPIN)&PIN_BEEP)?(LPC_GPIO1->FIOCLR |= PIN_BEEP):(LPC_GPIO1->FIOSET |= PIN_BEEP);} 


/*
*********************************************************************************************************
*	函 数 名: bsp_InitBeep
*	功能说明: 初始化Beep
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitBeep(void)
{
	LPC_PINCON->PINSEL3 &= ~(0x03<<6); //bit6,7

	LPC_GPIO1->FIODIR   |= PIN_BEEP;

	LPC_GPIO1->FIOCLR 	|= PIN_BEEP;		
}

