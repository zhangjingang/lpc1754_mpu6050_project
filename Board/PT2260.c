#include "PT2260.h"

 
/******************************************************************
LPC1754--PT2260硬件连接：
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

/*********************************************************************************************************
** Function name:       PT2260Init
** Descriptions:        PT2260初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void PT2260Init(void)
{
    //初始化地址A0—A7 
    LPC_PINCON->PINSEL2 &= ~(PIN_RF_A0|PIN_RF_A1|PIN_RF_A2|PIN_RF_A3|	\
							PIN_RF_A4|PIN_RF_A5|PIN_RF_A6|PIN_RF_A7);		//功能：GPIO
    LPC_GPIO2->FIODIR   |=  (PIN_RF_A0|PIN_RF_A1|PIN_RF_A2|PIN_RF_A3|	\
							PIN_RF_A4|PIN_RF_A5|PIN_RF_A6|PIN_RF_A7);		//方向：输出
	LPC_GPIO2->FIOCLR 	|=  (PIN_RF_A0|PIN_RF_A1|PIN_RF_A2|PIN_RF_A3|	\
							PIN_RF_A4|PIN_RF_A5|PIN_RF_A6|PIN_RF_A7);		//电平：低 
	//初始化数据D0—D3 
	LPC_PINCON->PINSEL0 &= ~(PIN_RF_D0|PIN_RF_D1|PIN_RF_D2);	//功能：GPIO
	LPC_PINCON->PINSEL2 &= ~(PIN_RF_D3);
	LPC_GPIO0->FIODIR   |=  (PIN_RF_D0|PIN_RF_D1|PIN_RF_D2);	//方向：输出
	LPC_GPIO2->FIODIR   |=  (PIN_RF_D3);
	LPC_GPIO0->FIOCLR 	|=  (PIN_RF_D0|PIN_RF_D1|PIN_RF_D2);	//电平：低
	LPC_GPIO2->FIOCLR 	|=  (PIN_RF_D3);

}  

/*********************************************************************************************************
** Function name:       PT2260SendBtye
** Descriptions:        PT2260往指定地址发送一字节数据
** input parameters:    addr：地址	 dat：一字节数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

void PT2260SendBtye(uint8_t addr, uint8_t dat)
{
	(addr&(1<<0)) ? RF_A0(1) : RF_A0(0);
	(addr&(1<<1)) ? RF_A1(1) : RF_A1(0);
	(addr&(1<<2)) ? RF_A2(1) : RF_A2(0);
	(addr&(1<<3)) ? RF_A3(1) : RF_A3(0);
	(addr&(1<<4)) ? RF_A4(1) : RF_A4(0);
	(addr&(1<<5)) ? RF_A5(1) : RF_A5(0);
	(addr&(1<<6)) ? RF_A6(1) : RF_A6(0);
	(addr&(1<<7)) ? RF_A7(1) : RF_A7(0);

	(dat&(1<<0)) ? RF_D0(1) : RF_D0(0);
	(dat&(1<<1)) ? RF_D1(1) : RF_D1(0);
	(dat&(1<<2)) ? RF_D2(1) : RF_D2(0);
	(dat&(1<<3)) ? RF_D3(1) : RF_D3(0);
}
