#ifndef __sys_conf_h
#define __sys_conf_h

//ϵͳ��������:
/***************************************************************************************************************/

//����debugģʽ�����ε��������Ϣ�Ͳ������
#define DEBUG_EN

//����˯��ģʽ(�͹���)�����ε���Ϊ����ģʽ
#define	MODE_SLEEP_EN

//����ϵͳ�Ƿ�����񶯼�⹦��,���ε��󲻴��񶯼�����
//#define	DETECT_VIBRATE_EN

//����NRL24L01Ϊ��SND���� OR ��RCV����
#define CONF_NRF24L01_SND	1u	//SND��1	RCV��0
#if CONF_NRF24L01_SND
#define MODULE_ROLE		"This is CONF_NRF24L01 send module!  "
#else
#define MODULE_ROLE		"This is CONF_NRF24L01 receive module!  "
#endif

//��Ϸ�����趨
#define RULE_NUM_MAX		10u		//�����������
#define COMPTB_ANGLE 		10u		//������ݶ���
#define SET_MIN_ANGLE		8u		//����ģʽ�������С�Ƕ�

//Data storing location	of flash
#define STORE_SECTOR		15u			//flash sector number 15 (with 4k size ) is used for storing data.
#define	WRITE_LENGTH		512u		//the length of copying data from ram to flash
#define	SECTOR_ADDR 		0x0000F000	//the start address of sector number 15	is 0x000F000
#define RETRY_SAVE_TIMES	2u 		//���洢ʧ��ʱ�������ٴδ洢�Ĵ���

//others
#define SCAN_TIME			10u
#define OUTPUT_BUF			128u	//���崮�������������С

//������Ϸʹ�õ���X��Y��Z��ALL
/*
#ifdef	axis_X
#elif	axis_Y
#elif	axis_Z
#else	axis_ALL
#endif
*/

#endif
