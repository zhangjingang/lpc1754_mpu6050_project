
#include "NRF24L01.h"

unsigned char TxAddr[] = {0x34, 0x43, 0x10, 0x10, 0x01}; //���͵�ַ
unsigned char RxAddr[] = {0x34, 0x43, 0x10, 0x10, 0x01}; //���յ�ַ

unchar  sta;   //״̬��־

/*********************************************************************************************************
** Function name:       myDelay
** Descriptions:        �����ʱ
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void myDelay (uint32_t ulTime)
{
    uint32_t i;

    i = 0;
    while (ulTime--) {
        for (i = 0; i < 5000; i++);
    }
}

/*****************SPIʱ����******************************************/
/*дһ�ֽڵ�nrf24L01*/
unchar NRFSPI(unsigned char byte)
{
  /* Write and Read a byte on SPI interface. */
    LPC_SPI->SPDR	= byte;
    while (0 == (LPC_SPI->SPSR & 0x80));	/* Wait for transfer to finish */               
	return (LPC_SPI->SPDR);					/* Return received value       */
}

/*****************SPI���Ĵ���һ�ֽں���*********************************/
unchar NRFReadReg(unchar RegAddr)
{
    unchar BackDate;
    CSN(0); //����ʱ��
    NRFSPI(RegAddr);//д�Ĵ�����ַ
    BackDate = NRFSPI(0x00); //д����Ĵ���ָ��
    CSN(1);
    return(BackDate); //����״̬
}
/*****************SPIд�Ĵ���һ�ֽں���*********************************/
unchar NRFWriteReg(unchar RegAddr, unchar date)
{
    unchar BackDate;
    CSN(0); //����ʱ��
    BackDate = NRFSPI(RegAddr); //д���ַ
    NRFSPI(date);//д��ֵ
    CSN(1);
    return(BackDate);
}
/*****************SPI��ȡRXFIFO�Ĵ�����ֵ********************************/
unchar NRFReadRxDate(unchar RegAddr, unchar *RxData, unchar DataLen)
{
    //�Ĵ�����ַ//��ȡ���ݴ�ű���//��ȡ���ݳ���//���ڽ���
    unchar BackData, i;
    CSN(0); //����ʱ��
    BackData = NRFSPI(RegAddr); //д��Ҫ��ȡ�ļĴ�����ַ
    for(i = 0; i < DataLen; i++) //��ȡ����
    {
        RxData[i] = NRFSPI(0);
    }
    CSN(1);
    return(BackData);
}
/*****************SPIд��TXFIFO�Ĵ�����ֵ**********************************/
unchar NRFWriteTxDate(unchar RegAddr, unchar *TxData, unchar DataLen)
{
    //�Ĵ�����ַ//д�����ݴ�ű���//��ȡ���ݳ���//���ڷ���
    unchar BackData, i;
    CSN(0);
    BackData = NRFSPI(RegAddr); //д��Ҫд��Ĵ����ĵ�ַ
    for(i = 0; i < DataLen; i++) //д������
    {
        NRFSPI(*TxData++);
    }
    CSN(1);
    return(BackData);
}
/*****************NRF����Ϊ����ģʽ����������******************************/
//len 1-32
void NRFSetTxMode(unchar *TxData, unchar len)
{
    if (len > 32)	return;
		
	//����ģʽ
    CE(0);
    NRFWriteTxDate(W_REGISTER + TX_ADDR, TxAddr, TX_ADDR_WITDH); 	//д�Ĵ���ָ��+���͵�ַʹ��ָ��+���͵�ַ+��ַ���
    NRFWriteTxDate(W_REGISTER + RX_ADDR_P0, TxAddr, TX_ADDR_WITDH); //Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
    NRFWriteTxDate(W_TX_PAYLOAD, TxData, len); //д������
    /******�����йؼĴ�������**************/
    NRFWriteReg(W_REGISTER + CONFIG, 0x0e);        		// RX_DR,TX_DS,MAX_RT�жϲ����Σ�CRCʹ�ܣ�16λCRCУ�飬�ϵ�
    NRFWriteReg(W_REGISTER + EN_AA, 0x01);           	// ʹ�ܡ����ݹܵ�0���Զ�Ӧ��
    NRFWriteReg(W_REGISTER + EN_RXADDR, 0x01);       	// ʹ�ܡ����ݹܵ�0�����յ�ַ
	NRFWriteReg(W_REGISTER + SETUP_AW, 0x03);			// ָ������/���յ�ַ��ȣ�5�ֽ�
    NRFWriteReg(W_REGISTER + SETUP_RETR, 0x0a); 		// �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
    NRFWriteReg(W_REGISTER + RF_CH, 0x40);             	// ѡ����Ƶ�ŵ�0x40����"2400+0x40"[MHz]
    NRFWriteReg(W_REGISTER + RF_SETUP, 0x07);       	// ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������

	NRFWriteReg(W_REGISTER + FEATURE, 0x04);	//add by jgs: ʹ�ܶ�̬���ݿ��
	NRFWriteReg(W_REGISTER + DYNPD, 0x01);		//add by jgs��ʹ��"���ݹܵ�0"Ϊ��̬���ݿ��

    CE(1);
    myDelay(50);//����10us������
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

/*****************NRF����Ϊ����ģʽ����������******************************/
//����ģʽ
void NRFSetRXMode(void)
{
    CE(0);
	NRFWriteTxDate(W_REGISTER + RX_ADDR_P0, RxAddr, RX_ADDR_WITDH); // �������ݹܵ�0�Ľ��յ�ַ

    NRFWriteReg(W_REGISTER + CONFIG, 0x0f);        		// RX_DR,TX_DS,MAX_RT�жϲ����Σ�CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
    NRFWriteReg(W_REGISTER + EN_AA, 0x01);           	// ʹ�ܡ����ݹܵ�0���Զ�Ӧ��
    NRFWriteReg(W_REGISTER + EN_RXADDR, 0x01);       	// ʹ�ܡ����ݹܵ�0�����յ�ַ
	NRFWriteReg(W_REGISTER + SETUP_AW, 0x03);			// ָ������/���յ�ַ��ȣ�5�ֽ�
    NRFWriteReg(W_REGISTER + RF_CH, 0x40);             	// ѡ����Ƶ�ŵ�0x40����"2400+0x40"[MHz]
//    NRFWriteReg(W_REGISTER + RX_PW_P0, TX_DATA_WITDH);	// ָ�����ݹܵ�0���յ���Ч���ݿ�ȣ�4
    NRFWriteReg(W_REGISTER + RF_SETUP, 0x07);       	// ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
	NRFWriteReg(W_REGISTER + FEATURE, 0x04);	//add by jgs: ʹ�ܶ�̬���ݿ��
	NRFWriteReg(W_REGISTER + DYNPD, 0x01);		//add by jgs��ʹ��"���ݹܵ�0"Ϊ��̬���ݿ��
    CE(1);
    myDelay(50);//����10us������
}
/****************************���Ӧ���ź�******************************/
unchar CheckACK(void)
{
    //���ڷ���
    sta = NRFReadReg(R_REGISTER + STATUS);                // ����״̬�Ĵ���
	if((sta&0x20)||(sta&0x10))//��������ж�
    {
        NRFWriteReg(W_REGISTER + STATUS, 0xff); // ���TX_DS��MAX_RT�жϱ�־
        CSN(0);
        NRFSPI(FLUSH_TX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����
        CSN(1);
        return(0);
    }
    else
        return(1);
}
/*************************��������*********************************************/ 
//������ɷ��ؽ������ݳ��ȣ����򷵻�0
uint8_t NRFGetData(uint8_t *pdat)
{
	uint8_t len = 0;
    sta = NRFReadReg(R_REGISTER + STATUS); //�������ݺ��ȡ״̬�Ĵ���
    if(sta&0x40)				// �ж��Ƿ���յ�����
    {
        CE(0); //����
		NRFWriteReg(ACTIVE, 0x73);	//add by jgs:����
        len = NRFReadReg(R_RX_PL_WID);
		NRFReadRxDate(R_RX_PAYLOAD, pdat, len); //change by jgs: ��RXFIFO��ȡ���յ�����
        NRFWriteReg(W_REGISTER + STATUS, 0xff); //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�
        CSN(0);
        NRFSPI(FLUSH_RX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����
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
    //��ʼ��P0.22ΪGPIO����,����IRQ 
    LPC_PINCON->PINSEL1 &= ~(0x03 << 12);  
    LPC_GPIO0->FIODIR   &= ~PIN_IRQ; 
	
    //��ʼ��P2.9ΪGPIO���,����CEN,���� 
    LPC_PINCON->PINSEL4 &= ~(0x03 << 18);  
    LPC_GPIO2->FIODIR   |=  PIN_CE;
	LPC_GPIO2->FIOCLR 	 =  PIN_CE; 	             
}
/*SPIX��ʼ��*/
static void  SPIx_Init(void)
{ 
    // ����Ӳ��SPI��ͨѶ�� 
    LPC_PINCON->PINSEL0  |= (0x03ul << 30);          	// ����P0.15��ΪSCK��           
    LPC_PINCON->PINSEL1  |= (0x03ul << 2) |(0x03 << 4);	// ����P0.17��P0.18�ܽ�ΪSPI�ӿ�

    //��ʼ��P0.16ΪGPIO���,����CS0 
    LPC_PINCON->PINSEL1 &= ~(0x03 << 0);  
    LPC_GPIO0->FIODIR   |=  PIN_CSN;

	LPC_SPI->SPCCR =	0x0C;		       		// ����SPIʱ�ӷ�Ƶ,��ֵҪ��>=8��ż����(96MHz/4)/12=2MHz

    //����SPI�Ĺ�����ʽ 
	LPC_SPI->SPCR  = 	(0 << 2) |				// SPI������ÿ�δ��䷢�ͺͽ���8λ���ݡ�
				        (0 << 3) |				// CPHA = 0, ������SCK �ĵ�һ��ʱ���ز���
				        (0 << 4) |				// CPOL = 0, SCK Ϊ����Ч
				        (1 << 5) |				// MSTR = 1, SPI ������ģʽ
				        (0 << 6) |				// LSBF = 0, SPI ���ݴ���MSB (λ7)����
				        (0 << 7);				// SPIE = 0, SPI �жϱ���ֹ	
}

void Config_RF24L01(void)
{
  RF24L01_Gpio_Init();//IO��ʼ��
  NotSelect_NRF();//��ֹSPI2 NRF24L01+��Ƭѡ
  SPIx_Init();//SPIʹ��
}

/**********************NRF24L01��ʼ������*******************************/
void NRF24L01Int(void)
{  
	Config_RF24L01();

    CE(0); //����ģʽ1
    CSN(1);
    SCK(0);
    IRQ(1);
}
