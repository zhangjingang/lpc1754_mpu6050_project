#include "bsp_timer.h"
#include "LPC17xx.h"
#include "sys_conf.h"

#include "bsp_key.h" //for bsp_KeyPro()

#define TMR_COUNT	4				// the number of the timers. first timer is reserved for bsp_DelayMS(). 

SOFT_TMR g_Tmr[TMR_COUNT];			// soft timers. NOTE:g_Tmr[0] just only used by bsp_DelayMS().
volatile int32_t g_iRunTime = 0; 	// global run time.unit 1ms

static void bsp_SoftTimerDec(SOFT_TMR *_tmr);

/*********************************************************************************************************
** Function name:       vicSysPconSet 
** Descriptions:        set the power control mode
** input parameters:    usPconMode: 0 - enter into normal sleep mode
**                                  1 - enter into deep sleep mode
**                                  2 - enter into normal power-off mode
**                                  3 - enter into deep power-off mode
** output parameters:   none
** Returned value:      1:  success
**                      0:  fail
*********************************************************************************************************/
void vicSysPconSet (uint16_t usPconMode)
{
    if (usPconMode == 0){                                               // enter into normal sleep mode
        LPC_SC->PCON = 0x00;
        SCB->SCR  = 0x00; 
    }
    else {
        SCB->SCR  |= 0x04;
        if (usPconMode == 1){                                           // enter into deep sleep mode
            LPC_SC->PCON = 0x08;
        }
        if (usPconMode == 2){                                           // enter into normal power-off mode
        LPC_SC->PCON = 0x09;
        }
        if (usPconMode == 3){                                           // enter into deep power-off mode
        LPC_SC->PCON = 0x03;
        }
    } 
    __wfi();                                                            // waiting for waking up by interrupt                   
}

/*********************************************************************************************************
** Function name:       SysTickInit
** Descriptions:        System tick timer initialization. unit 1ms
** input parameters:    none
** output parameters:   none
** Returned value:      none
*********************************************************************************************************/
void SysTickInit(void)
{
    uint32_t ulMsTime = 1;//timing 1ms
	uint8_t i;
	
	// clear all software timer
	for (i = 0; i < TMR_COUNT; i++)
	{
		g_Tmr[i].count = 0;
		g_Tmr[i].flag = 0;
	}

    SysTick->CTRL   = (1 << 0) | (1 << 1) | (1 << 2);       		// Enable system tick timer and interrupt
    /*
     * (SystemFrequency / 1000000* ulMsTime - 1) use of us delay
     * (SystemFrequency / 1000* ulMsTime - 1) use of ms delay
     */
    SysTick->LOAD = (SystemFrequency/1000)*ulMsTime - 1;        	// set timing time	
}

/*
*********************************************************************************************************
** Function name:       SysTick_Handler
** Descriptions:        SysTick interrupt handler. called every 1 ms
** input parameters:    none
** output parameters:   none
** Returned value:      none
*********************************************************************************************************
*/

void SysTick_Handler(void)
{
	uint8_t i;
	static uint8_t s_count = 0;

	for (i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&g_Tmr[i]);
	}

	g_iRunTime++;	// global run time. increased every 1 millisecond 
	if (g_iRunTime == 0x7FFFFFFF)	//MAX record timer is 2^31 millisecond, about24.85 days
	{
		g_iRunTime = 0;
	}

	if (++s_count >= 10)
	{
		s_count = 0;
		
		//Check all keys. checking every 10 millisecond. and filtering of 40 millisecond for machine shakeing
		bsp_KeyPro();		// this function located in bsp_button.c/

	}
}

/*
*********************************************************************************************************
** Function name:       bsp_SoftTimerDec
** Descriptions:        decrease 1 for all timers every 1ms. MUST called by SysTick_Handler periodic
** input parameters:    _tmr : timer pointer
** output parameters:   none
** Returned value:      none
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->flag == 0)
	{
		if (_tmr->count > 0)
		{
			//set flag if the corresponding timer descrease to 1
			if (--_tmr->count == 0)
			{
				_tmr->flag = 1;
			}
		}
	}
}

/*
*********************************************************************************************************
** Function name:       BspDelayMS
** Descriptions:        Delay function of millisecond. 1 millisecond accuracy
** input parameters:    n : the length of delay. unit 1 millisecond. NOTE: n should greater than 2
** output parameters:   none
** Returned value:      none
*********************************************************************************************************
*/
void BspDelayMS(uint32_t n)
{
	//void program locked while n = 1.
	if (n <= 1)
	{
		n = 2;
	}

	__set_PRIMASK(1);  		// Disable interrupt
	g_Tmr[0].count = n;
	g_Tmr[0].flag = 0;
	__set_PRIMASK(0);  		// Enable interrupt

	while (1)
	{
		CPU_IDLE();	//enter into low power mode, reducing power consumption

		//waiting up to the delay time
		if (g_Tmr[0].flag == 1)
		{
			break;
		}
	}	
}

/*
*********************************************************************************************************
** Function name:       bsp_StartTimer
** Descriptions:        Start a timer, and set the period
** input parameters:    _id : timer ID.	range of [1, TMR_COUNT-1]. User must maintain the timers ID	for avoid conflicting.
							  ID = 0 is used by bsp_DelayMS().
**						_period : timing period, uint 1 millisecond
** output parameters:   none
** Returned value:      none
*********************************************************************************************************
*/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		return;
	}

	__set_PRIMASK(1);  		// Disable interrupt
	g_Tmr[_id].count = _period;
	g_Tmr[_id].flag = 0;
	__set_PRIMASK(0);  		// Enable interrupt
}

/*
*********************************************************************************************************
** Function name:       bsp_CheckTimer
** Descriptions:        check if the timer is out of time
** input parameters:    _id : timer ID.	range of [1, TMR_COUNT-1]. User must maintain the timers ID	for avoid conflicting
** output parameters:   none
** Returned value:      0: not up to the timing
						1: out of time
*********************************************************************************************************
*/
uint8_t bsp_CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (g_Tmr[_id].flag == 1)
	{
		g_Tmr[_id].flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}


/*
*********************************************************************************************************
** Function name:       bsp_GetRunTime
** Descriptions:        Get the run time of CPU, unit 1 millisecond
** input parameters:    none
** output parameters:   none
** Returned value:      Run time of CPU, unit 1 millisecond
*********************************************************************************************************
*/
int32_t bsp_GetRunTime(void)
{
	int runtime; 

	__set_PRIMASK(1);  		// Disable interrupt
	
	runtime = g_iRunTime;	//this variable is modifed in Systick interrupt service, thus it should be protect
		
	__set_PRIMASK(0);  		// Enable interrupt

	return runtime;
}

//jgs: compatible for get_ms using in the mpu6050
void get_ms(unsigned long *time)
{
	*time = bsp_GetRunTime();
}
