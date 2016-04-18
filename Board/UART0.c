
#include "UART0.h"
#include "LPC17xx.h" 
#include <stdio.h>		//sprintf

#define UART_BPS    115200                                              /* 串口通信波特率               */ 

/*********************************************************************************************************
** Function name:       uartInit
** Descriptions:        串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为115200
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0Init (void)
{
    uint16_t usFdiv;
    LPC_PINCON->PINSEL0 |= (0x01 << 4)|(0x01 << 6); 

    LPC_SC->PCONP   = LPC_SC->PCONP|0x08;                               /* 打开串口0功能                */
    LPC_UART0->LCR  = 0x83;                                             /* 允许设置波特率               */
    usFdiv = (SystemFrequency/ 4 / 16) / UART_BPS;                       /* 设置波特率                   */
	LPC_UART0->DLM  = usFdiv / 256;
    LPC_UART0->DLL  = usFdiv % 256; 
    LPC_UART0->LCR  = 0x03;                                             /* 锁定波特率                   */
    LPC_UART0->FCR  = 0x06;
}

/*********************************************************************************************************
** Function name:      uart0GetByte
** Descriptions:       从串口接收1字节数据，使用查询方式接收
** input parameters:   无
** output parameters:  无
** Returned value:     ucRcvData:   接收到的数据
*********************************************************************************************************/
uint8_t uart0GetByte (void)
{
    uint8_t ucRcvData;
    
    while ((LPC_UART0->LSR & 0x01) == 0);                               /* 等待接收标志置位             */
    ucRcvData = LPC_UART0->RBR;                                         /* 读取数据                     */
    return (ucRcvData);
    
}

/*********************************************************************************************************
** Function name:       uart0GetStr
** Descriptions:        串口接收字符串
** input parameters:    puiStr:  指向接收数据数组的指针
**                      ulNum:   接收数据的个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0GetStr (uint8_t *puiStr, uint32_t ulNum)
{
    for (; ulNum > 0; ulNum--) {
        *puiStr++ =  uart0GetByte ();
    }  
}

/*********************************************************************************************************
** Function name:       uart0SendByte
** Descriptions:        向串口发送子节数据，并等待数据发送完成，使用查询方式
** input parameters:    ucDat:   要发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0SendByte (uint8_t ucDat)
{
    LPC_UART0->THR = ucDat;                                             /* 写入数据                     */
    while ((LPC_UART0->LSR & 0x40) == 0);                               /* 等待数据发送完毕             */
}

/*********************************************************************************************************
** Function name:       uart0SendStr
** Descriptions:        向串口发送字符串
** input parameters:    puiStr:  要发送的字符串指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0SendStr (uint8_t  const *puiStr, uint32_t len)
{
    while (len -- > 0) {
        uart0SendByte (*puiStr++);
    }
}

//void MyPrintf(fmt, ...)
//{
//	unsigned char tbuf[64];								
//	unsigned char length;								
//	length = sprintf((char*)tbuf, fmt, ## __VA_ARGS__);	
//	if (length <= 64)									
//	{													
//	 	uart0SendStr(tbuf, length);						
//	}													
//}
