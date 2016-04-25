#ifndef __sys_conf_h
#define __sys_conf_h

//系统配置如下:
/***************************************************************************************************************/

//设置debug模式，屏蔽掉后调试信息就不会输出
#define DEBUG_EN

//设置睡眠模式(低功耗)，屏蔽掉后为正常模式
#define	MODE_SLEEP_EN

//设置系统是否带有振动检测功能,屏蔽掉后不带振动检测输出
//#define	DETECT_VIBRATE_EN

//设置NRL24L01为“SND”端 OR “RCV”端
#define CONF_NRF24L01_SND	1u	//SND：1	RCV：0
#if CONF_NRF24L01_SND
#define MODULE_ROLE		"This is CONF_NRF24L01 send module!  "
#else
#define MODULE_ROLE		"This is CONF_NRF24L01 receive module!  "
#endif

//游戏规则设定
#define RULE_NUM_MAX		10u		//定义规则组数
#define COMPTB_ANGLE 		10u		//定义兼容度数
#define SET_MIN_ANGLE		8u		//设置模式允许的最小角度

//Data storing location	of flash
#define STORE_SECTOR		15u			//flash sector number 15 (with 4k size ) is used for storing data.
#define	WRITE_LENGTH		512u		//the length of copying data from ram to flash
#define	SECTOR_ADDR 		0x0000F000	//the start address of sector number 15	is 0x000F000
#define RETRY_SAVE_TIMES	2u 		//当存储失败时，尝试再次存储的次数

//others
#define SCAN_TIME			10u
#define OUTPUT_BUF			128u	//定义串口输出缓冲区大小

//定义游戏使用的轴X，Y，Z，ALL
/*
#ifdef	axis_X
#elif	axis_Y
#elif	axis_Z
#else	axis_ALL
#endif
*/

#endif
