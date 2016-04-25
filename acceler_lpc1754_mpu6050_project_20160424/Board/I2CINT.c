
#include "I2CINT.h"

                                                                        /* �������ں�I2C�жϴ�����Ϣ��  */
                                                                        /* ȫ�ֱ���                     */
volatile INT8U     I2C_sla;                                             /* I2C�����ӵ�ַ                */
volatile INT32U    I2C_suba;                                            /* I2C�����ڲ��ӵ�ַ            */
volatile INT8U     I2C_suba_num;                                        /* I2C�ӵ�ַ�ֽ���              */
volatile INT8U     *I2C_buf;                                            /* ���ݻ�����ָ��               */
volatile INT32U    I2C_num;                                             /* Ҫ��ȡ/д������ݸ���        */
volatile INT8U     I2C_end;                                             /* I2C���߽�����־������������  */
                                                                        /* ��1                          */
volatile INT8U     I2C_suba_en;                                         /*  �ӵ�ַ���ơ�
                                                                        0--�ӵ�ַ�Ѿ�������߲���Ҫ�ӵ�ַ
                                                                        1--��ȡ����
                                                                        2--д����
                                                                        */

/*********************************************************************************************************
** Function name:           Wait_I2c_End
** Descriptions:            �����ʱ���ṩ��I2C���ߵȴ���ʱʹ��
** input parameters:        dly        ��ʱ������ֵԽ����ʱԽ��
** Output parameters:       NONE
** Returned value:          NONE
*********************************************************************************************************/
INT8U  Wait_I2c_End(INT32U  dly)
{  INT32U  i;

    if( I2C_end==1 ) return (1);
    for(; dly>0; dly--)
    for(i=0; i<5000; i++)
    {
        if( I2C_end==1 ) return (1);
    }
    return (0);
}

/*********************************************************************************************************
** Function name:       ISendByte
** Descriptions:        �����ӵ�ַ��������1�ֽ����ݡ�
** input parameters:    sla     ������ַ
**                      dat     Ҫ���͵�����
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
INT8U  ISendByte(INT8U sla, INT8U dat)
{                                                                       /* ��������                     */
    I2C_sla     = sla;                                                  /* д������������ַ             */
    I2C_buf     = &dat;                                                 /* �����͵�����                 */
    I2C_num     = 1;                                                    /* ����1�ֽ�����                */
    I2C_suba_en = 0;                                                    /* ���ӵ�ַ                     */
    I2C_end     = 0;

    LPC_I2C1->I2CONCLR = 0x2C;
    LPC_I2C1->I2CONSET = 0x60;                                          /* ����Ϊ����������������       */
   
	return( Wait_I2c_End(20));

}
/*********************************************************************************************************
** Function name:       IRcvByte
** Descriptions:        �����ӵ�ַ������ȡ1�ֽ����ݡ�
** input parameters:    sla     ������ַ
**                      dat     Ҫ���͵�����
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
INT8U  IRcvByte(INT8U sla, INT8U *dat)
{                                                                       /* ��������                     */
    I2C_sla     = sla+1;                                                /* ��������������ַ             */
    I2C_buf     = dat;
    I2C_num     = 1;
    I2C_suba_en = 0;                                                    /* ���ӵ�ַ                     */
    I2C_end     = 0;

   LPC_I2C1->I2CONCLR = 0x2C;
    LPC_I2C1->I2CONSET = 0x60;                                          /* ����Ϊ����������������       */
   
	return( Wait_I2c_End(20));
   /*while(0==I2C_end);
   if(1==I2C_end) return(1);
     else return(0);*/

}
/*********************************************************************************************************
** Function name:       I2C_ReadNByte
** Descriptions:        �����ӵ�ַ���������ַ��ʼ��ȡN�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    2��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ���ݽ��ջ�����ָ��
**                      num         ��ȡ�ĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
INT8U I2C_ReadNByte (INT8U sla, INT32U suba_type, INT32U suba, INT8U *s, INT32U num)
{
    if (num > 0)                                                        /* �ж�num�����ĺϷ���          */
    {                                                                   /* ��������                     */
        if (suba_type == 1)
        {                                                               /* �ӵ�ַΪ���ֽ�               */
            I2C_sla         = sla + 1;                                  /* �������Ĵӵ�ַ��R=1          */
            I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�            */
        }
        if (suba_type == 2)
        {                                                               /* �ӵ�ַΪ2�ֽ�                */
            I2C_sla         = sla + 1;                                  /* �������Ĵӵ�ַ��R=1          */
            I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C_suba_num    = 2;                                        /* �����ӵ�ַΪ2�ֽ�            */
        }
        if (suba_type == 3)
        {                                                               /* �ӵ�ַ�ṹΪ8+X              */
            I2C_sla         = sla + ((suba >> 7 )& 0x0e) + 1;           /* �������Ĵӵ�ַ��R=1          */
            I2C_suba        = suba & 0x0ff;                             /* �����ӵ�ַ                   */
            I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ8+x              */
        }
        I2C_buf     = s;                                                /* ���ݽ��ջ�����ָ��           */
        I2C_num     = num;                                              /* Ҫ��ȡ�ĸ���                 */
        I2C_suba_en = 1;                                                /* ���ӵ�ַ��                   */
        I2C_end     = 0;

                                                                        /* ���STA,SI,AA��־λ          */
                                                                        /* ���STA,SI,AA��־λ          */
        LPC_I2C1->I2CONCLR = (1 << 2)|                                  /* AA                           */
                     (1 << 3)|                                          /* SI                           */
                     (1 << 5);                                          /* STA                          */

                                                                        /* ��λSTA,����I2C����          */
        LPC_I2C1->I2CONSET = (1 << 5)|                                  /* STA                          */
                     (1 << 6);                                          /* I2CEN                        */

                                                                        /* �ȴ�I2C�������              */
        return( Wait_I2c_End(200));

        /*while (I2C_end == 0)
        {

        }
        if (I2C_end == 1)
            return (TRUE);
        else
            return (FALSE);            */
    }
    return (FALSE);
}
/*********************************************************************************************************
** Function name:       I2C_WriteNByte
** Descriptions:        �����ӵ�ַ����д��N�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    2��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ��Ҫд������ݵ�ָ��
**                      num         ��Ҫд������ݵĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
INT8U I2C_WriteNByte(INT8U sla, INT8U suba_type, INT32U suba, INT8U *s, INT32U num)
{
    if (num > 0)                                                        /* �����ȡ�ĸ���Ϊ0���򷵻ش�  */                                                                    /* ��                           */
    {                                                                   /* ���ò���                     */
        if (suba_type == 1)
        {                                                               /* �ӵ�ַΪ���ֽ�               */
            I2C_sla         = sla;                                      /* �������Ĵӵ�ַ               */
            I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�            */
        }
        if (suba_type == 2)
        {                                                               /* �ӵ�ַΪ2�ֽ�                */
            I2C_sla         = sla;                                      /* �������Ĵӵ�ַ               */
            I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C_suba_num    = 2;                                        /* �����ӵ�ַΪ2�ֽ�            */
        }
        if (suba_type == 3)
        {                                                               /* �ӵ�ַ�ṹΪ8+X              */
            I2C_sla         = sla + ((suba >> 7 )& 0x0e);               /* �������Ĵӵ�ַ               */
            I2C_suba        = suba & 0x0ff;                             /* �����ӵ�ַ                   */
            I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ8+X              */
        }

        I2C_buf     = s;                                                /* ����                         */
        I2C_num     = num;                                              /* ���ݸ���                     */
        I2C_suba_en = 2;                                                /* ���ӵ�ַ��д����             */
        I2C_end     = 0;

                                                                        /* ���STA,SI,AA��־λ          */
		LPC_I2C1->I2CONCLR = (1 << 2)|                                  /* AA                           */
                             (1 << 3)|                                  /* SI                           */
                             (1 << 5);                                  /* STA                          */

                                                                        /* ��λSTA,����I2C����          */
        LPC_I2C1->I2CONSET = (1 << 5)|                                  /* STA                          */
                     (1 << 6);                                          /* I2CEN                        */                                             /* �ȴ�I2C�������              */
        return( Wait_I2c_End(200));
                /*while (I2C_end == 0)
        {
         //
        }
        if (I2C_end == 1)
            return (TRUE);
        else
            return (FALSE);            */
    }
    return (FALSE);
}
/*********************************************************************************************************
** Function name:       I2C1_IRQHandler
** Descriptions:        Ӳ��I2C�жϷ������
** input parameters:    ��
** Output parameters:   ��
** Returned value:      ע�⴦���ӵ�ַΪ2�ֽڵ������
*********************************************************************************************************/
void  I2C1_IRQHandler(void)
{   /*
     *��ȡI2C״̬�Ĵ���I2DAT,����ȫ�ֱ��������ý��в��������������־,����ж��߼�,�жϷ���
     */
    switch (LPC_I2C1->I2STAT & 0xF8)                                    /* ����״̬�������Ӧ�Ĵ���     */
    {   
     /* 
      *�ѷ�����ʼ����,�����ͺ������ն���,װ��SLA+W����SLA+R 
      */
        case 0x08:                                                      /* ָ���ӵ�ַ��                 */
        if(I2C_suba_en == 1) {                                          /* SLA+R                        */         
            LPC_I2C1->I2DAT = I2C_sla & 0xFE;                           /* ��д���ַ                   */
        }
        else                                                            /* SLA+W                        */
        {   LPC_I2C1->I2DAT  = I2C_sla;                                 /* ����ֱ�ӷ��ʹӻ���ַ         */
        }
                                                                        /* ����SIλ                     */
        LPC_I2C1->I2CONCLR = (1 << 3)|                                  /* SI                           */
                             (1 << 5);                                  /* STA                          */
        break;
                                                                        /*�ѷ����ظ���ʼ����            */
        case 0x10:                                                      /* �����ͺ������ն���           */
                                                                        /* װ��SLA+W����SLA+R           */
        LPC_I2C1->I2DAT = I2C_sla;                                      /* �������ߺ��ط��ӵ�ַ       */
        LPC_I2C1->I2CONCLR = 0x28;                                      /* ����SI,STA                   */
        break;

        case 0x18:
        case 0x28:                                                      /* �ѷ���I2DAT�е�����,�ѽ���ACK*/
        if (I2C_suba_en == 0)
        {
            if (I2C_num > 0)
            {   LPC_I2C1->I2DAT = *I2C_buf++;
                LPC_I2C1->I2CONCLR = 0x28;                              /* ����SI,STA                   */
                I2C_num--;
            }
            else                                                        /* û�����ݷ�����               */
            {                                                           /* ֹͣ����                     */
                LPC_I2C1->I2CONSET = (1 << 4);                          /* STO                          */
                LPC_I2C1->I2CONCLR = 0x28;                              /* ����SI,STA                   */
                I2C_end = 1;                                            /* �����Ѿ�ֹͣ                 */
            }
        }

        if(I2C_suba_en == 1)                                            /* ����ָ����ַ��,��������������*/
        {
            if (I2C_suba_num == 2)
            {   LPC_I2C1->I2DAT = ((I2C_suba >> 8) & 0xff);
                LPC_I2C1->I2CONCLR = 0x28;                              /* ����SI,STA                   */
                I2C_suba_num--;
                break;
            }

            if(I2C_suba_num == 1)
            {   LPC_I2C1->I2DAT = (I2C_suba & 0xff);
                LPC_I2C1->I2CONCLR = 0x28;                              /* ����SI,STA                   */
                I2C_suba_num--;
                break;
            }

            if (I2C_suba_num == 0)
            {
                LPC_I2C1->I2CONCLR = 0x08;
                LPC_I2C1->I2CONSET = 0x20;
                I2C_suba_en = 0;                                        /* �ӵ�ַ������                 */
                break;
            }
        }

        if (I2C_suba_en == 2)                                           /* ָ���ӵ�ַд,�ӵ�ַ��δָ��, */
                                                                        /* �����ӵ�ַ                 */
        {
            if (I2C_suba_num > 0)
            {    if (I2C_suba_num == 2)
                {    LPC_I2C1->I2DAT = ((I2C_suba >> 8) & 0xff);
                    LPC_I2C1->I2CONCLR  = 0x28;
                    I2C_suba_num--;
                    break;
                }
                if (I2C_suba_num == 1)
                {     LPC_I2C1->I2DAT    = (I2C_suba & 0xff);
                    LPC_I2C1->I2CONCLR = 0x28;
                    I2C_suba_num--;
                    I2C_suba_en  = 0;
                    break;
                }
            }
        }
        break;

        case 0x40:                                                      /* �ѷ���SLA+R,�ѽ���ACK        */
        if (I2C_num <= 1)                                               /* ��������һ���ֽ�           */
        {     LPC_I2C1->I2CONCLR  = 1 << 2;                             /* �´η��ͷ�Ӧ���ź�           */
        }
        else
        {    LPC_I2C1->I2CONSET = 1 << 2;                               /* �´η���Ӧ���ź�             */
        }
        LPC_I2C1->I2CONCLR = 0x28;                                      /* ����SI,STA                   */
        break;

        case 0x20:                                                      /* �ѷ���SLA+W,�ѽ��շ�Ӧ��     */
        case 0x30:                                                      /* �ѷ���I2DAT�е����ݣ��ѽ���  */
                                                                        /* ��Ӧ��                       */
        case 0x38:                                                      /* ��SLA+R/W�������ֽ��ж�ʧ��  */
                                                                        /* ��                           */
        case 0x48:                                                      /* �ѷ���SLA+R,�ѽ��շ�Ӧ��     */
        LPC_I2C1->I2CONCLR = 0x28;
        I2C_end = 0xFF;
        break;

        case 0x50:                                                      /* �ѽ��������ֽڣ��ѷ���ACK    */
        *I2C_buf++ = LPC_I2C1->I2DAT;
        I2C_num--;
        if (I2C_num == 1)                                               /* �������һ���ֽ�             */
        {    LPC_I2C1->I2CONCLR  = 0x2C;                                /* STA,SI,AA = 0                */
        }
        else
        {   LPC_I2C1->I2CONSET = 0x04;                                  /* AA=1                         */
            LPC_I2C1->I2CONCLR = 0x28;
        }
        break;

        case 0x58:                                                      /* �ѽ��������ֽڣ��ѷ��ط�Ӧ�� */
            *I2C_buf++ = LPC_I2C1->I2DAT;                               /* ��ȡ���һ�ֽ�����           */
            LPC_I2C1->I2CONSET = 0x10;                                  /* ��������                     */
            LPC_I2C1->I2CONCLR = 0x28;
            I2C_end = 1;
        break;

        default:
        break;
    }
}

/*********************************************************************************************************
** Function name:       i2c1Init
** Descriptions:        ��ģʽI2C��ʼ����������ʼ����ܽ������Լ�����IRQ�ж�
** input parameters:    ulFi2c:    ��ʼ��I2C�������ʣ����ֵΪ400K
** Output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void  i2c1Init (INT32U ulFi2c)
{                                                                       /* ����I2C1�ж����ȼ���ʹ���ж� */	
    LPC_PINCON->PINSEL0 |= (0x03 << 0);
    LPC_PINCON->PINSEL0 |= (0x03 << 2); 

    if (ulFi2c>400000) ulFi2c = 400000;
    LPC_I2C1->I2SCLH   = (SystemFrequency / 4 /ulFi2c + 1) / 2;         /* ����I2Cʱ��Ϊfi2c            */
    LPC_I2C1->I2SCLL   = (SystemFrequency / 4 /ulFi2c) / 2;
    LPC_I2C1->I2CONCLR = 0x2C;
    LPC_I2C1->I2CONSET = 0x40;                                          /* ʹ����I2C                    */
    NVIC_EnableIRQ(I2C1_IRQn);
    NVIC_SetPriority(I2C1_IRQn, 3);                                      /* ����I2C1�ж����ȼ���ʹ���ж� */
}


//jgs:Ϊ�˼���mpu6050��IIC����
int8_t i2cwrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
	if(I2C_WriteNByte(addr<<1, ONE_BYTE_SUBA, reg, data, len))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

//jgs:Ϊ�˼���mpu6050��IIC����
int8_t i2cread(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	if(I2C_ReadNByte(addr<<1, ONE_BYTE_SUBA, reg, buf, len))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

