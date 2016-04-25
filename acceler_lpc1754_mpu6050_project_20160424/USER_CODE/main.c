#include <stdio.h>		//sprintf
#include "LPC17xx.h"
#include "I2CINT.h"
#include "bsp_gpioout.h"
#include "bsp_key.h"
#include "UART0.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "MPU6050_user.h"
#include "NRF24L01.h"
#include "PT2260.h"
#include "bsp_timer.h"
#include "IAP.h"
#include "math.h"    //Keil library (for asin, atan2, sqrt)
#include "sys_conf.h"


/********************************************************************************************************
 !!! Global Variables
*********************************************************************************************************/
struct axis_attr mpuDat[3];//[0],[1],[2] means X(Pitch), Y(Roll), Z(Yaw) axis separately
uint8_t passFlag[3];
uint8_t buf[OUTPUT_BUF];	//temporary output buffer	
uint8_t gLen;
int16_t matchedRotateAngle[3];//current matched angle of each stage of each asix
uint8_t matchStage[3];

uint16_t ruleGroupNum = 0;	//the Number of the game rule(NOTE:one group rule include two data, 
							//which one data indicates direction, and the other indicates angle)
int16_t DestRule[3][RULE_NUM_MAX][2];//game rule of every axis


/********************************************************************************************************
 Inner Variables
*********************************************************************************************************/
__align(4) static int16_t tmpSectorData[WRITE_LENGTH];//__align(4) means data in ram four bytes align


/********************************************************************************************************
Inner function declaration
*********************************************************************************************************/
static void Init(void);
static void AccessSettingMode(void);
static void AccessNormalMode(void);
static void SaveGameRule(int16_t *pdat, uint16_t len);
static void ReadGameRule(int16_t *pdat, uint16_t *len);

/*********************************************************************************************************
** Function name:       main
** Descriptions:        
** input parameters:    none
** output parameters:   none
** Returned value:      none
*********************************************************************************************************/
int main (void)
{
	uint8_t i = i;
	uint8_t len = len;

	Init();

#if CONF_NRF24L01_SND

	// setting-mode
	//check whether it entering into set-mode: generate key-long-set within 5 seconds
	bsp_StartTimer(1, 5000);
	while (0 == bsp_CheckTimer(1))
	{	
		if (bsp_GetKey() == KEY_LONG_SET)//check key-long-set 
		{
			for (i = 0; i < 6; i++)//beep-on 3 times with each 200ms
			{
				bsp_BeepToggled();
				BspDelayMS(200);
			}
				
			AccessSettingMode();// !!!never return.
		}
			
		CPU_IDLE();	//let CPU enter into low power mode	
	}

   //normal-mode
	AccessNormalMode();


#else

	//NRF24L01 receive data
	for(;;)
	{
		NRFSetRXMode();
		len = NRFGetData(buf);
        if (len > 0)
		{
			uart0SendStr(buf, len);
		}
	}
#endif


}

/*********************************************************************************************************
** Function name:       Init
** Descriptions:        
** input parameters:    none
** output parameters:   none
** Returned value:      none
*********************************************************************************************************/
static void Init (void)
{
    SystemInit();   	//init system frequency,96000000
	SysTickInit();		//SysTick init，1ms
	uart0Init();    	//115200 8n1
	uart_printf(MODULE_ROLE);
	uart_printf("SystemFrequency=[%dHz]  Init System ...\n", SystemFrequency);
	bsp_InitBeep();
	bsp_InitButton();
    i2c1Init(400000); 	//400KHZ
	NRF24L01Int();		//NRF24L01 and SPI init
	PT2260Init();
	MPU6050Init();		//MPU6050 init,runtime:1530ms
	ReadGameRule(&(DestRule[2][0][0]), &ruleGroupNum);//read game rules from flash.
	uart_printf("System Ready!\n");

	bsp_BeepOn();
	BspDelayMS(1000);
	bsp_BeepOff();
}

/*********************************************************************************************************
** Function name:       AccessSetMode
** Descriptions:        
** input parameters:    none
** output parameters:   none
** Returned value:      none
*********************************************************************************************************/
static void AccessSettingMode(void)
{
	uint8_t ii = ii;

	debug_printf("[%ldms] Enter into SET_MODE !\n",bsp_GetRunTime());

	bsp_StartTimer(1, 10000);//timer 1 used to monitor if the set-mode is complete (no rotation in ten seconds) 
	bsp_StartTimer(2, 2000); //timer 2 used to monitor if the one rule group is complete (no rotation in two seconds)

	while (0 == bsp_CheckTimer(1))
	{
		HandleMPU6050Data3(mpuDat); //rotate record and beep every 90 degree.

		if (0 != mpuDat[2].rotateDirect) //if angle changed, restart timer 1 and 2
		{
			bsp_StartTimer(1, 10000);	
			bsp_StartTimer(2, 2000);				
		}

		//if no rotation in two seconds,indicating a rule group is set complete. 
		if (1 == bsp_CheckTimer(2))
		{
			//if no rotatioin within 2 seconds,	do nothing.
			if (ABS(matchedRotateAngle[2]) <= SET_MIN_ANGLE)//BUG: matchedRotateAngle[2] maybe nagetive value
			{
				bsp_StartTimer(2, 2000);
				debug_printf("No rotatioin in 2 seconds.\n"); 
				continue;
			}

#if CONF_NRF24L01_SND
			gLen = sprintf((char*)buf, "Z-Axis:stage<%02d> Over, Angle:<%04d>!\n", matchStage[2], matchedRotateAngle[2]);
			debug_printf((char*)buf);
			NRFSndDate(buf, gLen);
#else
			debug_printf("Z-Axis:stage<%02d> Over, Angle:<%04d>!\n", matchStage[2], matchedRotateAngle[2]);
#endif

			//TODO:save one rule group data.including directoin and rotated angle.
			DestRule[2][matchStage[2]-1][0] = matchedRotateAngle[2] >= 0 ? 1 : -1;
			DestRule[2][matchStage[2]-1][1] = matchedRotateAngle[2];
			ruleGroupNum = matchStage[2]++;
			matchedRotateAngle[2] = 0;


			//最后一个rule达到指定角度后的情况处理
			if (matchStage[2] >= RULE_NUM_MAX)
			{
				debug_printf(" ERROR !!! game_rule_len > %d, Saving first %d data.\n", RULE_NUM_MAX, RULE_NUM_MAX);	
				matchStage[2] = 0;
				matchedRotateAngle[2] = 0;
			}

			//beep-on 3 times with each 50ms
			uart_printf("beep-on 3 times with each 50ms!\n");
		 	for (ii = 0; ii < 6; ii++) 
			{
				bsp_BeepToggled();
				BspDelayMS(50);
			}
		}//end:if (1 == bsp_CheckTimer(2))

		BspDelayMS(SCAN_TIME);

	}//end:while (0 == bsp_CheckTimer(1))

	//save the last game group info if there is no rotate within ten seconds
	if (matchedRotateAngle[2] > SET_MIN_ANGLE)
	{
		DestRule[2][ruleGroupNum][0] = matchedRotateAngle[2] > 0 ? -1 : 1;
		DestRule[2][ruleGroupNum][1] = matchedRotateAngle[2];
		ruleGroupNum++;	
	}

	//save game rules to flash.
	SaveGameRule(&(DestRule[2][0][0]), ruleGroupNum);

	//beep-on 3 times with each 200ms,indicating save data to flash success
	uart_printf("beep-on 3 times with each 200ms!\n");
 	for (ii = 0; ii < 6; ii++) 
	{
		bsp_BeepToggled();
		BspDelayMS(200);
	}

	//Delay 2s and soft-reset the system.
	BspDelayMS(2000);
	SoftReset();
}

/*********************************************************************************************************
** Function name:       AccessNormalMode
** Descriptions:        
** input parameters:    none
** output parameters:   none
** Returned value:      none
*********************************************************************************************************/
static void AccessNormalMode(void)
{
	uint8_t i = i;

	debug_printf("[%ldms] Enter into NORMAL_MODE !\n",bsp_GetRunTime());

	bsp_StartTimer(1, 2000);

	for(;;)
	{						
		HandleMPU6050Data2(mpuDat);
#if 0
		uart_printf(" Pitch=%04d\tRoll=%04d\tYaw=%04d\trunTime=%ldms\n", 
	   		mpuDat[0].currentAngle, mpuDat[1].currentAngle, mpuDat[2].currentAngle, bsp_GetRunTime());
#endif

		if ((0 != mpuDat[2].rotateDirect) && (0 == matchStage[2])) 
		{
			matchStage[2] = 1;;	
		}

		if (0 != mpuDat[2].rotateDirect)  
		{
			bsp_StartTimer(1, 2000);	
		}

		if (1 == bsp_CheckTimer(1))
		{
			if ((matchedRotateAngle[2] >= DestRule[2][matchStage[2]-1][1]) &&
				(matchedRotateAngle[2] <= DestRule[2][matchStage[2]-1][1] + COMPTB_ANGLE))
			{
#if CONF_NRF24L01_SND
			gLen = sprintf((char*)buf, " z-Axis:stage<%02d>Pass!\n", matchStage[2]);
			debug_printf((char*)buf);
			NRFSndDate(buf, gLen);
#else
			debug_printf(" z-Axis:stage<%02d>Pass!\n", matchStage[2]);
#endif		

			matchStage[2]++;
			}
			else
			{
#if CONF_NRF24L01_SND
					gLen = sprintf((char*)buf, " z-Axis:Stop=>[%d]\n", matchStage[2]);
					uart_printf((char*)buf);
					NRFSndDate(buf, gLen);
#else
					uart_printf(" z-Axis:Stop=>[%d]\n", matchStage[2]);
#endif	
				matchStage[2] = 0;				
			}


			//最后一个rule达到指定角度后的情况处理
			if (matchStage[2] >= ruleGroupNum)
			{
				matchStage[2] = 0;
				passFlag[2] = 1;	
			}

			matchedRotateAngle[2] = 0;

		}//end:if (1 == bsp_CheckTimer(2))


		for (i = 0; i < 3; i++)
		{
			if (passFlag[i] == 1)
			{
				passFlag[i] = 0;

				gLen = sprintf((char*)buf, "\t[%d]-Axis Trigger A Signal !\n", i);
				uart_printf((char*)buf);
				NRFSndDate(buf, gLen);

				for(;;)
				{
					bsp_BeepToggled();
					BspDelayMS(50);	
				}

				//return 1;//stop the App !!!*************************
			}		
		}

        BspDelayMS(SCAN_TIME);
    }
}

/*********************************************************************************************************
** Function name:       SaveGameRule
** Descriptions:        
** input parameters:    data pointer to be read, group length of the game rule
** output parameters:   none
** Returned value:      none
*********************************************************************************************************/
static void SaveGameRule(int16_t *pdat, uint16_t groupNum)
{
	uint32_t i,j;
	uint32_t tmpValue;
	uint8_t tryCount = 0; 

	//print the "len" of "*pdat"
#if 1
	debug_printf("Saving_game_groups=%d!  \n",groupNum);
	for (i = 0; i < groupNum; i++)
	{
		for (j = 0; j < 2; j++)
		{
			debug_printf("Sav_Dat[%d][%d]=%d \n", i, j, pdat[2*i+j]);//print out;				
		}
	}
#endif

//	__set_PRIMASK(1);  		//disable interrupt

	//first 2-byte saves the length of game rule
	tmpSectorData[0] = groupNum;//rule group number
	tmpSectorData[1] = 0;  //none

	//copy game rule data to array
    for (i = 0; i < groupNum*2; i++) 
	{                                         
		tmpSectorData[i+2] = *(pdat + i);
    }

	//fill the rest data of WRITE_LENGTH with 0X55FF
    for (i = groupNum*2; i < WRITE_LENGTH; i++) 
	{                                         
 		tmpSectorData[i+2] = 0x55FF;
    }

retry:
	//Calling IAP to write rule game data to flash
    SelSector(STORE_SECTOR, STORE_SECTOR);                          		// choose sector                     
    EraseSector(STORE_SECTOR, STORE_SECTOR);                     			// erase sector                     
    SelSector(STORE_SECTOR, STORE_SECTOR);                        			// choose sector                     
    RamToFlash(SECTOR_ADDR,(uint32_t)tmpSectorData,WRITE_LENGTH);    		// write data from ram to flash               
    tmpValue = Compare(SECTOR_ADDR,(uint32_t)tmpSectorData,WRITE_LENGTH/2);	// compare data                   

//	__set_PRIMASK(0);  		//enable interrupt

	//compareing, to make sure whether the saving data is right or wrong.
    if (tmpValue == 0) //saving data right													
	{                                                 
		debug_printf("[%ldms] Saving game rules success!\n",bsp_GetRunTime());
    }
	else //saving data wrong
	{
		if (tryCount < RETRY_SAVE_TIMES)
		{
			debug_printf("[%ldms] Saving game rules fail! Try times:%d \n",bsp_GetRunTime(),tryCount);
			tryCount++;
			goto retry;		
		}
		else
		{
			debug_printf("[%ldms] Saving game rules fail!\n",bsp_GetRunTime());
		}
	}

}

/*********************************************************************************************************
** Function name:       ReadGameRule
** Descriptions:        
** input parameters:    none
** output parameters:   data pointer, group length of the game rule
** Returned value:      none
*********************************************************************************************************/
static void ReadGameRule(int16_t *pdat, uint16_t *groupNum)
{
    uint8_t i, j;
	uint16_t groupLen;//the group length of game rule in flash.
	uint16_t addrCount;//the count of address storing the game rule data.
	uint32_t tmpValue;

	groupLen = *(volatile unsigned int *)SECTOR_ADDR; // 32bit data convert to 16bit. just saving last 16bit
	*groupNum = groupLen;
	debug_printf("Reading_game_groups=%d!  \n",groupLen);

	addrCount = (groupLen%2) ? (groupLen+1) : (groupLen);// address needs align by 4-bytes

	//read data from flash
	for (i = 0; i < addrCount; i++)//every address stores two group data.means four bytes.
	{
		tmpValue = *(volatile unsigned int *)(SECTOR_ADDR + 4*(i+1));//address is increased by every 4 bytes. skip four bytes(for using storing ruleGroupLen)

		for (j = 0; j < 2; j++)// there are two data(with 2 bytes each) in one address
		{
		 	pdat[2*i+j] = tmpValue >> 16*j;// read out every data(with 2 bytes each) 

			if (2*i+j < groupLen*2)
			{
				debug_printf("Rd_Dat[%d][%d]=%d \n", i, j, pdat[2*i+j]);//print out			
			}
		}
	}
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
