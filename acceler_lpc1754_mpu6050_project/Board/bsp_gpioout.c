
#include "LPC17xx.h" 
#include "bsp_gpioout.h"

/**********************************************************************
*	                                  
*Ӳ���ӿ�����
*
*	������FMQ
*			FMQ----P1.19	�ߵ�ƽ��
*
*********************************************************************/
/*�ӿڶ���*/

/*PIN*/
#define PIN_BEEP		1 << 19

void bsp_BeepOn(void)		{LPC_GPIO1->FIOSET |= PIN_BEEP;}
void bsp_BeepOff(void)		{LPC_GPIO1->FIOCLR |= PIN_BEEP;}
void bsp_BeepToggled(void)	{((LPC_GPIO1->FIOPIN)&PIN_BEEP)?(LPC_GPIO1->FIOCLR |= PIN_BEEP):(LPC_GPIO1->FIOSET |= PIN_BEEP);} 


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitBeep
*	����˵��: ��ʼ��Beep
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitBeep(void)
{
	LPC_PINCON->PINSEL3 &= ~(0x03<<6); //bit6,7

	LPC_GPIO1->FIODIR   |= PIN_BEEP;

	LPC_GPIO1->FIOCLR 	|= PIN_BEEP;		
}

