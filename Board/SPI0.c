#include "SPI0.h"

/*****************************************/
/* 函数功能：SPI0通信                    */
/* 说明：    发送一个字节，接收一个字节  */
/*****************************************/
uint8 SPI0_communication(uint8 TxData)
{		
	while(((LPC_SSP0->SR)&(1<<4))==(1<<4));//忙时等待,SR状态寄存器bit4 BSY:忙时为1
	LPC_SSP0->DR = TxData;                 //把要发送的数写入TxFIFO
	while(((LPC_SSP0->SR)&(1<<2))!=(1<<2));//等待接收完,SR状态寄存器bit2 RNE:接收FIFO非空为1
	return(LPC_SSP0->DR);		    	   //返回收到的数据
}


/*****************************************/
/* 函数功能：SPI0初始化                  */
/* 说明：    没有用SSEL0                 */
/*****************************************/
void SPI0_Init(void)
{
	uint8 i,Clear=Clear;//Clear=Clear:用这种语句形式解决编译产生的Waring:never used!	

	LPC_SYSCON->PRESETCTRL |= (0x1<<0);	   //禁止SSP0复位
	LPC_SYSCON->SYSAHBCLKCTRL |= (0x1<<11);//允许SSP0时钟 bit11
	LPC_SYSCON->SSP0CLKDIV = 0x01;		   //分频系数为1，使SPI0速率最大：48Mhz
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16); // 使能IOCON时钟(bit16)
	LPC_IOCON->SCK_LOC = 0x02;      //把SCK0复用到PIO0_6引脚
	LPC_IOCON->PIO0_6 &= ~0x07;
	LPC_IOCON->PIO0_6 |= 0x02;		//把PIO0_6设置为SSP CLK 
	LPC_IOCON->PIO0_8 &= ~0x07;	
	LPC_IOCON->PIO0_8 |= 0x01;		//把PIO0_8设置为SSP MISO 
	LPC_IOCON->PIO0_9 &= ~0x07;	
	LPC_IOCON->PIO0_9 |= 0x01;		//把PIO0_9设置为SSP MOSI 
	LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<16); // 禁能IOCON时钟(bit16)
	// 8位数据传输，SPI模式, CPOL = 0, CPHA = 0,空闲时CLK为0，第一个上升沿采集数据，SCR = 0
	LPC_SSP0->CR0 = 0x0107;	    
	// 预分频值（注意：必须为偶数 2~254） 
	LPC_SSP0->CPSR = 0x02;
	LPC_SSP0->CR1 &= ~(1<<0);//LBM=0:正常模式
	LPC_SSP0->CR1 &= ~(1<<2);//MS=0:主机模式 
	LPC_SSP0->CR1 |=  (1<<1);//SSE=1:使能SPI0
	//清空RxFIFO，LPC1114收发均有8帧FIFO,每帧可放置4~16位数据
	for ( i = 0; i < 8; i++ )
	{
	  	Clear = LPC_SSP0->DR;//读数据寄存器DR将清空RxFIFO
	} 		 		  
}


