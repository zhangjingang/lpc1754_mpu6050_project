
#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "LPC17xx.h"

#define	unchar	uint8_t

/******************************************************************
LPC1754--Rf24l01硬件连接：
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

#define Select_NRF()     (LPC_GPIO0->FIOCLR = PIN_CSN)	  //NRF24L01 选择
#define NotSelect_NRF()  (LPC_GPIO0->FIOSET |= PIN_CSN)	  //NRF24L01 禁止

/*******************************************************/
#define TX_ADDR_WITDH 5	//发送地址宽度设置为5个字节
#define RX_ADDR_WITDH 5	//接收地址宽度设置为5个字节
//#define TX_DATA_WITDH 4//发送数据宽度1个字节
//#define RX_DATA_WITDH 4//接收数据宽度1个字节
/*******************命令寄存器***************************/
#define  R_REGISTER      0x00//读取配置寄存器
#define  W_REGISTER      0x20//写配置寄存器
#define  R_RX_PAYLOAD 	 0x61//读取RX有效数据
#define  W_TX_PAYLOAD	 0xa0//写TX有效数据
#define  FLUSH_TX		 0xe1//清除TXFIFO寄存器
#define  FLUSH_RX		 0xe2//清除RXFIFO寄存器
#define  REUSE_TX_PL     0xe3//重新使用上一包有效数据
#define  NOP             0xff//空操作

//add by jgs
#define  ACTIVE			 0x50//激活特定命令
#define  R_RX_PL_WID	 0x60//读取RX FIFO中的有效数据宽度

/******************寄存器地址****************************/
#define  CONFIG          0x00//配置寄存器
#define  EN_AA			 0x01//使能自动应答
#define  EN_RXADDR       0x02//接收通道使能0-5个通道
#define  SETUP_AW        0x03//设置数据通道地址宽度3-5
#define  SETUP_RETR      0x04//建立自动重发
#define  RF_CH           0x05//射频通道设置
#define  RF_SETUP        0x06//射频寄存器
#define  STATUS          0x07//状态寄存器
#define  OBSERVE_TX      0x08//发送检测寄存器
#define  CD              0x09//载波
#define  RX_ADDR_P0      0x0a//数据通道0接收地址
#define  RX_ADDR_P1      0x0b//数据通道1接收地址
#define  RX_ADDR_P2      0x0c//数据通道2接收地址
#define  RX_ADDR_P3      0x0d//数据通道3接收地址
#define  RX_ADDR_P4      0x0e//数据通道4接收地址
#define  RX_ADDR_P5      0x0f//数据通道5接收地址
#define  TX_ADDR         0x10//发送地址
#define  RX_PW_P0        0x11//P0通道数据宽度设置
#define  RX_PW_P1        0x12//P1通道数据宽度设置
#define  RX_PW_P2        0x13//P2通道数据宽度设置
#define  RX_PW_P3        0x14//P3通道数据宽度设置
#define  RX_PW_P4        0x15//P4通道数据宽度设置
#define  RX_PW_P5        0x16//P5通道数据宽度设置
#define  FIFO_STATUS     0x17//FIFO状态寄存器

//add by jgs
#define  DYNPD		     0x1C//动态通道数据宽度设置
#define  FEATURE	     0x1D//特性寄存器

/*******************相关函数声明**************************/
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
