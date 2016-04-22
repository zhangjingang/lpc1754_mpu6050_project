
#include "NRF24L01.h"

unsigned char TxAddr[] = {0x34, 0x43, 0x10, 0x10, 0x01}; //发送地址
unsigned char RxAddr[] = {0x34, 0x43, 0x10, 0x10, 0x01}; //接收地址

unchar  sta;   //状态标志

/*********************************************************************************************************
** Function name:       myDelay
** Descriptions:        软件延时
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void myDelay (uint32_t ulTime)
{
    uint32_t i;

    i = 0;
    while (ulTime--) {
        for (i = 0; i < 5000; i++);
    }
}

/*****************SPI时序函数******************************************/
/*写一字节到nrf24L01*/
unchar NRFSPI(unsigned char byte)
{
  /* Write and Read a byte on SPI interface. */
    LPC_SPI->SPDR	= byte;
    while (0 == (LPC_SPI->SPSR & 0x80));	/* Wait for transfer to finish */               
	return (LPC_SPI->SPDR);					/* Return received value       */
}

/*****************SPI读寄存器一字节函数*********************************/
unchar NRFReadReg(unchar RegAddr)
{
    unchar BackDate;
    CSN(0); //启动时序
    NRFSPI(RegAddr);//写寄存器地址
    BackDate = NRFSPI(0x00); //写入读寄存器指令
    CSN(1);
    return(BackDate); //返回状态
}
/*****************SPI写寄存器一字节函数*********************************/
unchar NRFWriteReg(unchar RegAddr, unchar date)
{
    unchar BackDate;
    CSN(0); //启动时序
    BackDate = NRFSPI(RegAddr); //写入地址
    NRFSPI(date);//写入值
    CSN(1);
    return(BackDate);
}
/*****************SPI读取RXFIFO寄存器的值********************************/
unchar NRFReadRxDate(unchar RegAddr, unchar *RxData, unchar DataLen)
{
    //寄存器地址//读取数据存放变量//读取数据长度//用于接收
    unchar BackData, i;
    CSN(0); //启动时序
    BackData = NRFSPI(RegAddr); //写入要读取的寄存器地址
    for(i = 0; i < DataLen; i++) //读取数据
    {
        RxData[i] = NRFSPI(0);
    }
    CSN(1);
    return(BackData);
}
/*****************SPI写入TXFIFO寄存器的值**********************************/
unchar NRFWriteTxDate(unchar RegAddr, unchar *TxData, unchar DataLen)
{
    //寄存器地址//写入数据存放变量//读取数据长度//用于发送
    unchar BackData, i;
    CSN(0);
    BackData = NRFSPI(RegAddr); //写入要写入寄存器的地址
    for(i = 0; i < DataLen; i++) //写入数据
    {
        NRFSPI(*TxData++);
    }
    CSN(1);
    return(BackData);
}
/*****************NRF设置为发送模式并发送数据******************************/
//len 1-32
void NRFSetTxMode(unchar *TxData, unchar len)
{
    if (len > 32)	return;
		
	//发送模式
    CE(0);
    NRFWriteTxDate(W_REGISTER + TX_ADDR, TxAddr, TX_ADDR_WITDH); 	//写寄存器指令+发送地址使能指令+发送地址+地址宽度
    NRFWriteTxDate(W_REGISTER + RX_ADDR_P0, TxAddr, TX_ADDR_WITDH); //为了应答接收设备，接收通道0地址和发送地址相同
    NRFWriteTxDate(W_TX_PAYLOAD, TxData, len); //写入数据
    /******下面有关寄存器配置**************/
    NRFWriteReg(W_REGISTER + CONFIG, 0x0e);        		// RX_DR,TX_DS,MAX_RT中断不屏蔽；CRC使能，16位CRC校验，上电
    NRFWriteReg(W_REGISTER + EN_AA, 0x01);           	// 使能“数据管道0”自动应答
    NRFWriteReg(W_REGISTER + EN_RXADDR, 0x01);       	// 使能“数据管道0”接收地址
	NRFWriteReg(W_REGISTER + SETUP_AW, 0x03);			// 指定发送/接收地址宽度：5字节
    NRFWriteReg(W_REGISTER + SETUP_RETR, 0x0a); 		// 自动重发延时等待250us+86us，自动重发10次
    NRFWriteReg(W_REGISTER + RF_CH, 0x40);             	// 选择射频信道0x40，即"2400+0x40"[MHz]
    NRFWriteReg(W_REGISTER + RF_SETUP, 0x07);       	// 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益

	NRFWriteReg(W_REGISTER + FEATURE, 0x04);	//add by jgs: 使能动态数据宽度
	NRFWriteReg(W_REGISTER + DYNPD, 0x01);		//add by jgs：使能"数据管道0"为动态数据宽度

    CE(1);
    myDelay(50);//保持10us秒以上
}

//add by jgs for sending data>32
void NRFSndDate(unchar *pData, unchar len)
{
	uint8_t i;

	if (len <= 32)
	{
		NRFSetTxMode(pData, len);
		while(CheckACK());
	}
	else
	{
	 	for (i = 0; i < len / 32; i++)
		{
			NRFSetTxMode(pData, 32);
			pData += 32;
			while(CheckACK());
		}

		NRFSetTxMode(pData, len - 32*i);
		while(CheckACK());
	}	
}

/*****************NRF设置为接收模式并接收数据******************************/
//接收模式
void NRFSetRXMode(void)
{
    CE(0);
	NRFWriteTxDate(W_REGISTER + RX_ADDR_P0, RxAddr, RX_ADDR_WITDH); // 设置数据管道0的接收地址

    NRFWriteReg(W_REGISTER + CONFIG, 0x0f);        		// RX_DR,TX_DS,MAX_RT中断不屏蔽；CRC使能，16位CRC校验，上电，接收模式
    NRFWriteReg(W_REGISTER + EN_AA, 0x01);           	// 使能“数据管道0”自动应答
    NRFWriteReg(W_REGISTER + EN_RXADDR, 0x01);       	// 使能“数据管道0”接收地址
	NRFWriteReg(W_REGISTER + SETUP_AW, 0x03);			// 指定发送/接收地址宽度：5字节
    NRFWriteReg(W_REGISTER + RF_CH, 0x40);             	// 选择射频信道0x40，即"2400+0x40"[MHz]
//    NRFWriteReg(W_REGISTER + RX_PW_P0, TX_DATA_WITDH);	// 指定数据管道0接收的有效数据宽度：4
    NRFWriteReg(W_REGISTER + RF_SETUP, 0x07);       	// 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
	NRFWriteReg(W_REGISTER + FEATURE, 0x04);	//add by jgs: 使能动态数据宽度
	NRFWriteReg(W_REGISTER + DYNPD, 0x01);		//add by jgs：使能"数据管道0"为动态数据宽度
    CE(1);
    myDelay(50);//保持10us秒以上
}
/****************************检测应答信号******************************/
unchar CheckACK(void)
{
    //用于发射
    sta = NRFReadReg(R_REGISTER + STATUS);                // 返回状态寄存器
	if((sta&0x20)||(sta&0x10))//发送完毕中断
    {
        NRFWriteReg(W_REGISTER + STATUS, 0xff); // 清除TX_DS或MAX_RT中断标志
        CSN(0);
        NRFSPI(FLUSH_TX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！
        CSN(1);
        return(0);
    }
    else
        return(1);
}
/*************************接收数据*********************************************/ 
//接收完成返回接收数据长度，否则返回0
uint8_t NRFGetData(uint8_t *pdat)
{
	uint8_t len = 0;
    sta = NRFReadReg(R_REGISTER + STATUS); //发送数据后读取状态寄存器
    if(sta&0x40)				// 判断是否接收到数据
    {
        CE(0); //待机
		NRFWriteReg(ACTIVE, 0x73);	//add by jgs:激活
        len = NRFReadReg(R_RX_PL_WID);
		NRFReadRxDate(R_RX_PAYLOAD, pdat, len); //change by jgs: 从RXFIFO读取接收的数据
        NRFWriteReg(W_REGISTER + STATUS, 0xff); //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标
        CSN(0);
        NRFSPI(FLUSH_RX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！
        CSN(1);

		return len;
    }
	else
	{
		return 0;	
	}
}

/**********************************************************************************************/

static void RF24L01_Gpio_Init(void)
{
    //初始化P0.22为GPIO输入,用作IRQ 
    LPC_PINCON->PINSEL1 &= ~(0x03 << 12);  
    LPC_GPIO0->FIODIR   &= ~PIN_IRQ; 
	
    //初始化P2.9为GPIO输出,用作CEN,拉低 
    LPC_PINCON->PINSEL4 &= ~(0x03 << 18);  
    LPC_GPIO2->FIODIR   |=  PIN_CE;
	LPC_GPIO2->FIOCLR 	 =  PIN_CE; 	             
}
/*SPIX初始化*/
static void  SPIx_Init(void)
{ 
    // 设置硬件SPI的通讯脚 
    LPC_PINCON->PINSEL0  |= (0x03ul << 30);          	// 设置P0.15脚为SCK脚           
    LPC_PINCON->PINSEL1  |= (0x03ul << 2) |(0x03 << 4);	// 设置P0.17、P0.18管脚为SPI接口

    //初始化P0.16为GPIO输出,用作CS0 
    LPC_PINCON->PINSEL1 &= ~(0x03 << 0);  
    LPC_GPIO0->FIODIR   |=  PIN_CSN;

	LPC_SPI->SPCCR =	0x0C;		       		// 设置SPI时钟分频,该值要求>=8的偶数。(96MHz/4)/12=2MHz

    //设置SPI的工作方式 
	LPC_SPI->SPCR  = 	(0 << 2) |				// SPI控制器每次传输发送和接收8位数据。
				        (0 << 3) |				// CPHA = 0, 数据在SCK 的第一个时钟沿采样
				        (0 << 4) |				// CPOL = 0, SCK 为低有效
				        (1 << 5) |				// MSTR = 1, SPI 处于主模式
				        (0 << 6) |				// LSBF = 0, SPI 数据传输MSB (位7)在先
				        (0 << 7);				// SPIE = 0, SPI 中断被禁止	
}

void Config_RF24L01(void)
{
  RF24L01_Gpio_Init();//IO初始化
  NotSelect_NRF();//禁止SPI2 NRF24L01+的片选
  SPIx_Init();//SPI使能
}

/**********************NRF24L01初始化函数*******************************/
void NRF24L01Int(void)
{  
	Config_RF24L01();

    CE(0); //待机模式1
    CSN(1);
    SCK(0);
    IRQ(1);
}
