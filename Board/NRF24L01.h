
#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "LPC17xx.h"

#define	unchar	uint8_t

/******************************************************************
LPC1754--Rf24l01Ӳ�����ӣ�
CE	   P2.9
CSN	   P0.16	
SCK	   P0.15
MOSI   P0.18
MISO   P0.17
IRQ	   P0.22
*******************************************************************/
#define PIN_CE       (1<<9)
#define PIN_CSN      (1<<16)
#define PIN_SCK      (1<<15)
#define PIN_MOSI     (1<<18)
#define PIN_MISO     (1<<17)
#define PIN_IRQ      (1<<22)

#define CE(x)    ((x)?(LPC_GPIO2->FIOSET |= PIN_CE):(LPC_GPIO2->FIOCLR = PIN_CE))
#define CSN(x)   ((x)?(LPC_GPIO0->FIOSET |= PIN_CSN):(LPC_GPIO0->FIOCLR = PIN_CSN))
#define SCK(x)   ((x)?(LPC_GPIO0->FIOSET |= PIN_SCK):(LPC_GPIO0->FIOCLR = PIN_SCK))
#define MOSI(x)  ((x)?(LPC_GPIO0->FIOSET |= PIN_MOSI):(LPC_GPIO0->FIOCLR = PIN_MOSI))
#define MISO(x)  ((x)?(LPC_GPIO0->FIOSET |= PIN_MISO):(LPC_GPIO0->FIOCLR = PIN_MISO))
#define IRQ(x)   ((x)?(LPC_GPIO0->FIOSET |= PIN_IRQ):(LPC_GPIO0->FIOCLR = PIN_IRQ))

#define Select_NRF()     (LPC_GPIO0->FIOCLR = PIN_CSN)	  //NRF24L01 ѡ��
#define NotSelect_NRF()  (LPC_GPIO0->FIOSET |= PIN_CSN)	  //NRF24L01 ��ֹ

/*******************************************************/
#define TX_ADDR_WITDH 5	//���͵�ַ�������Ϊ5���ֽ�
#define RX_ADDR_WITDH 5	//���յ�ַ�������Ϊ5���ֽ�
//#define TX_DATA_WITDH 4//�������ݿ��1���ֽ�
//#define RX_DATA_WITDH 4//�������ݿ��1���ֽ�
/*******************����Ĵ���***************************/
#define  R_REGISTER      0x00//��ȡ���üĴ���
#define  W_REGISTER      0x20//д���üĴ���
#define  R_RX_PAYLOAD 	 0x61//��ȡRX��Ч����
#define  W_TX_PAYLOAD	 0xa0//дTX��Ч����
#define  FLUSH_TX		 0xe1//���TXFIFO�Ĵ���
#define  FLUSH_RX		 0xe2//���RXFIFO�Ĵ���
#define  REUSE_TX_PL     0xe3//����ʹ����һ����Ч����
#define  NOP             0xff//�ղ���

//add by jgs
#define  ACTIVE			 0x50//�����ض�����
#define  R_RX_PL_WID	 0x60//��ȡRX FIFO�е���Ч���ݿ��

/******************�Ĵ�����ַ****************************/
#define  CONFIG          0x00//���üĴ���
#define  EN_AA			 0x01//ʹ���Զ�Ӧ��
#define  EN_RXADDR       0x02//����ͨ��ʹ��0-5��ͨ��
#define  SETUP_AW        0x03//��������ͨ����ַ���3-5
#define  SETUP_RETR      0x04//�����Զ��ط�
#define  RF_CH           0x05//��Ƶͨ������
#define  RF_SETUP        0x06//��Ƶ�Ĵ���
#define  STATUS          0x07//״̬�Ĵ���
#define  OBSERVE_TX      0x08//���ͼ��Ĵ���
#define  CD              0x09//�ز�
#define  RX_ADDR_P0      0x0a//����ͨ��0���յ�ַ
#define  RX_ADDR_P1      0x0b//����ͨ��1���յ�ַ
#define  RX_ADDR_P2      0x0c//����ͨ��2���յ�ַ
#define  RX_ADDR_P3      0x0d//����ͨ��3���յ�ַ
#define  RX_ADDR_P4      0x0e//����ͨ��4���յ�ַ
#define  RX_ADDR_P5      0x0f//����ͨ��5���յ�ַ
#define  TX_ADDR         0x10//���͵�ַ
#define  RX_PW_P0        0x11//P0ͨ�����ݿ������
#define  RX_PW_P1        0x12//P1ͨ�����ݿ������
#define  RX_PW_P2        0x13//P2ͨ�����ݿ������
#define  RX_PW_P3        0x14//P3ͨ�����ݿ������
#define  RX_PW_P4        0x15//P4ͨ�����ݿ������
#define  RX_PW_P5        0x16//P5ͨ�����ݿ������
#define  FIFO_STATUS     0x17//FIFO״̬�Ĵ���

//add by jgs
#define  DYNPD		     0x1C//��̬ͨ�����ݿ������
#define  FEATURE	     0x1D//���ԼĴ���

/*******************��غ�������**************************/
unchar NRFSPI(unchar data);
unchar NRFReadReg(unchar RegAddr);
unchar NRFWriteReg(unchar RegAddr, unchar data);
unchar NRFReadRxDate(unchar RegAddr, unchar *RxData, unchar DataLen);
unchar NRFWriteTxDate(unchar RegAddr, unchar *TxData, unchar DataLen);
void config_24L01(void);
unchar CheckACK(void);
uint8_t NRFGetData(uint8_t *pdat);

void NRFSetTxMode(unchar *TxData, unchar len);
void NRFSndDate(unchar *pData, unchar len);//add by jgs
void NRFSetRXMode(void);
void NRF24L01Int(void);
void myDelay (uint32_t ulTime);

/*********************************************************/
#endif
