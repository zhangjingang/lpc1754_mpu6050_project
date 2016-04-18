
#ifndef __BSP_GPIOOUT_H
#define __BSP_GPOIOUT_H

/* 供外部调用的函数声明 */

/*BEEP*/                         
void bsp_BeepOn(void);		
void bsp_BeepOff(void);		
void bsp_BeepToggled(void);

/*初始化beep*/
void bsp_InitBeep(void);

#endif


