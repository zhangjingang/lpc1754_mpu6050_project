
#include "UART0.h"
#include "LPC17xx.h" 
#include <stdio.h>		//sprintf

#define UART_BPS    115200                                              /* ����ͨ�Ų�����               */ 

/*********************************************************************************************************
** Function name:       uartInit
** Descriptions:        ���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ�飬������Ϊ115200
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0Init (void)
{
    uint16_t usFdiv;
    LPC_PINCON->PINSEL0 |= (0x01 << 4)|(0x01 << 6); 

    LPC_SC->PCONP   = LPC_SC->PCONP|0x08;                               /* �򿪴���0����                */
    LPC_UART0->LCR  = 0x83;                                             /* �������ò�����               */
    usFdiv = (SystemFrequency/ 4 / 16) / UART_BPS;                       /* ���ò�����                   */
	LPC_UART0->DLM  = usFdiv / 256;
    LPC_UART0->DLL  = usFdiv % 256; 
    LPC_UART0->LCR  = 0x03;                                             /* ����������                   */
    LPC_UART0->FCR  = 0x06;
}

/*********************************************************************************************************
** Function name:      uart0GetByte
** Descriptions:       �Ӵ��ڽ���1�ֽ����ݣ�ʹ�ò�ѯ��ʽ����
** input parameters:   ��
** output parameters:  ��
** Returned value:     ucRcvData:   ���յ�������
*********************************************************************************************************/
uint8_t uart0GetByte (void)
{
    uint8_t ucRcvData;
    
    while ((LPC_UART0->LSR & 0x01) == 0);                               /* �ȴ����ձ�־��λ             */
    ucRcvData = LPC_UART0->RBR;                                         /* ��ȡ����                     */
    return (ucRcvData);
    
}

/*********************************************************************************************************
** Function name:       uart0GetStr
** Descriptions:        ���ڽ����ַ���
** input parameters:    puiStr:  ָ��������������ָ��
**                      ulNum:   �������ݵĸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0GetStr (uint8_t *puiStr, uint32_t ulNum)
{
    for (; ulNum > 0; ulNum--) {
        *puiStr++ =  uart0GetByte ();
    }  
}

/*********************************************************************************************************
** Function name:       uart0SendByte
** Descriptions:        �򴮿ڷ����ӽ����ݣ����ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
** input parameters:    ucDat:   Ҫ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0SendByte (uint8_t ucDat)
{
    LPC_UART0->THR = ucDat;                                             /* д������                     */
    while ((LPC_UART0->LSR & 0x40) == 0);                               /* �ȴ����ݷ������             */
}

/*********************************************************************************************************
** Function name:       uart0SendStr
** Descriptions:        �򴮿ڷ����ַ���
** input parameters:    puiStr:  Ҫ���͵��ַ���ָ��
** output parameters:   ��
** Returned value:      ��
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
