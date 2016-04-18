#include "MPU6050_user.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "I2CINT.h"
#include "math.h"    //Keil library (for asin, atan2, sqrt)
#include <stdio.h>		//sprintf
#include "UART0.h"
#include "bsp_timer.h"
#include "bsp_gpioout.h"
#include "NRF24L01.h"
#include "sys_conf.h"


#define DEFAULT_MPU_HZ  (1000)	 /* Starting sampling rate. */

#define q30  1073741824.0f
float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;

#define ABS(x) ((x) > 0 ? (x) : -(x))

static unsigned char matchStage[3];//记录每个轴规则匹配阶段


/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from thei
 * driver(s).
 * TODO: The following matrices refer to the configuration on an internal test
 * board at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};


/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static  unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

static  unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */
    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;
    return scalar;
}

/*********************************************************************************************************
** Function name:       SoftReset
** Descriptions:        用于软件复位内核为ARM Cortex-M3处理器的芯片 add by jgs
** input parameters:    none
** output parameters:   无
** Returned value:      int
*********************************************************************************************************/
void SoftReset(void)
{
	__set_FAULTMASK(1); // 关闭所有中端
	NVIC_SystemReset();// 复位
}

/*********************************************************************************************************
** Function name:       run_self_test
** Descriptions:        
** input parameters:    none
** output parameters:   无
** Returned value:      int
*********************************************************************************************************/
static int run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x7) {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        dmp_set_accel_bias(accel);
    }
	else //add by jgs
	{
		return -1;
	}
	return 0;
}

/*********************************************************************************************************
** Function name:       GetMPU6050InitInfo
** Descriptions:        
** input parameters:    none
** output parameters:   无
** Returned value:      none
*********************************************************************************************************/
static void GetMPU6050InitInfo(void)
{
	unsigned short dmpOutRate;
	unsigned long pedometerStepCount;
	unsigned long pedometerWalkTime;
	unsigned char accelFullScaleRange;
	short rawAccelData[3];
	unsigned long timeStamp;
	unsigned short accelSensitivityScaleFactor;
	unsigned char dmpStatus;
	unsigned char fifoConfig;
	unsigned short gyroFullScaleRange;
	short rawGyroData[3];
	float gyroSensitivityScaleFactor;
	short intStatusReg;
	unsigned short DLPF_setting;
	unsigned char powerStatus;
	unsigned short samplingRate;
	long temperatureData;

//	dmp_set_fifo_rate(150);
	dmp_get_fifo_rate(&dmpOutRate);
	dmp_get_pedometer_step_count(&pedometerStepCount);	//val:0
	dmp_get_pedometer_walk_time(&pedometerWalkTime);	//val:0
	
	mpu_get_accel_fsr(&accelFullScaleRange);			//val:0x02
	mpu_get_accel_reg(rawAccelData, &timeStamp);	   	//val:0x0016,0xFFB8,0x491E	0
	mpu_get_accel_sens(&accelSensitivityScaleFactor);	//val:0x4000=16384
	mpu_get_dmp_state(&dmpStatus);						//val:0x01
	mpu_get_fifo_config(&fifoConfig); 					//val:0x78
	mpu_get_gyro_fsr(&gyroFullScaleRange);			   	//val:0x07D0=2000
	mpu_get_gyro_reg(rawGyroData, &timeStamp);		   	//val:0xFFD0,0xFFEF,0xFFBC	0
	mpu_get_gyro_sens(&gyroSensitivityScaleFactor);	 	//val:16.4
	mpu_get_int_status(&intStatusReg);				  	//val:0x0113/0x0011
	mpu_get_lpf(&DLPF_setting);						   	//val:0x0062
	mpu_get_power_state(&powerStatus);				   	//val:0x01
	mpu_get_sample_rate(&samplingRate);				   	//val:0xE7A0=59296/0x8114=33044
	mpu_get_temperature(&temperatureData, &timeStamp);	//val:0x00170181
}

/*********************************************************************************************************
** Function name:       DeteckVibrate
** Descriptions:        
** input parameters:    none
** output parameters:   无
** Returned value:      none
*********************************************************************************************************/
#ifdef	DETECT_VIBRATE_EN
#define	CAL_NUM		20 
static void DeteckVibrate(void)
{
	uint8_t i;
	uint32_t sum=0,sumX=0,sumY=0,sumZ=0;
	short accelDat[3];

	for(i=0; i<CAL_NUM; i++)
	{	
        mpu_get_accel_reg (accelDat, 0);

		accelDat[0]=accelDat[0]/16.384;
		accelDat[1]=accelDat[1]/16.384;
		accelDat[2]=accelDat[2]/16.384;

		sumX +=	ABS(accelDat[0]);
		sumY +=	ABS(accelDat[1]);
		sumZ +=	ABS(accelDat[2]);			
		}

		sumX /=	CAL_NUM;
		sumY /=	CAL_NUM;
		sumZ /=	CAL_NUM;
		sum = sqrt((sumX * sumX) + (sumY * sumY) + (sumZ * sumZ)) ;

		if (sum > 33)
		{
			uart_printf("==> Vibrate! (%03d)\n", sum);		
		}
}
#endif

/*********************************************************************************************************
** Function name:       UpdateMPU6050Data
** Descriptions:        
** input parameters:    struct axis_attr *mpu6050dat
** output parameters:   无
** Returned value:      none
*********************************************************************************************************/
static void UpdateMPU6050Data(struct axis_attr *mpu6050dat)
{
	unsigned long sensor_timestamp;
	short gyro[3], accel[3], sensors;
	unsigned char more;
	long quat[4];

	 dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more);	 
	 /* Gyro and accel data are written to the FIFO by the DMP in chip
     * frame and hardware units. This behavior is convenient because it
     * keeps the gyro and accel outputs of dmp_read_fifo and
     * mpu_read_fifo consistent.
     */

	mpu6050dat[0].currentTimestamp = sensor_timestamp;
	mpu6050dat[1].currentTimestamp = sensor_timestamp;
	mpu6050dat[2].currentTimestamp = sensor_timestamp;

	if (sensors & INV_XYZ_GYRO )
	{
		mpu6050dat[0].gyro = gyro[0];
		mpu6050dat[1].gyro = gyro[1];
		mpu6050dat[2].gyro = gyro[2];
	}

	if (sensors & INV_XYZ_ACCEL)
	{
		mpu6050dat[0].accel = accel[0];
		mpu6050dat[1].accel = accel[1];
		mpu6050dat[2].accel = accel[2];
	}

	if (sensors & INV_WXYZ_QUAT )
	{
		q0=quat[0] / q30;
		q1=quat[1] / q30;
		q2=quat[2] / q30;
		q3=quat[3] / q30;

		//直接计算结果赋值给short型
		mpu6050dat[0].currentAngle = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; 	// pitch
		mpu6050dat[1].currentAngle = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;	// roll
		mpu6050dat[2].currentAngle = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;	//yaw
	 }
}

/*********************************************************************************************************
** Function name:       CalIntervalAngle
** Descriptions:        
** input parameters:    short current_angle, short last_angle
** output parameters:   无
** Returned value:      short
*********************************************************************************************************/
static short CalIntervalAngle(short current_angle, short last_angle)
{
	short tmpAngle = 0;

	//跨越+-0度及+-180度时的情况处理
	if (((current_angle > 0) && (last_angle < 0)) ||
		(current_angle < 0) && (last_angle > 0))	
	{
		if (ABS(current_angle) < 90)	//跨越+-0度
		{
			tmpAngle = ABS(current_angle) + ABS(last_angle);
		}
		else 	//跨越+-180度
		{
			tmpAngle = 360 - ABS(current_angle) - ABS(last_angle);		
		}
	}
	else
	{
		tmpAngle = current_angle - last_angle;	
	}
	return tmpAngle;
}

/*********************************************************************************************************
** Function name:       HandleMPU6050Data2
** Descriptions:        used in set-mode
** input parameters:    axis_attr *mpu6050dat
** output parameters:   none
** Returned value:      none
*********************************************************************************************************/
void HandleMPU6050Data2(struct axis_attr *mpu6050dat)
{
	uint8_t i;
	int16_t tempAngle;

	UpdateMPU6050Data(mpu6050dat);

	//如果只检测Z(Yaw)轴，将i=2即可。注：由于X(Pitch)轴的角度范围是+-90。故当角度大于+-90度时计算有误。
	for (i = 2; i < 3; i++)
	{
		//计算角速度：度/秒
		mpu6050dat[i].rotateSpeed = ABS(CalIntervalAngle(mpu6050dat[i].currentAngle, mpu6050dat[i].lastAngle)) * 1000 /
			(mpu6050dat[i].currentTimestamp - mpu6050dat[i].lastTimestamp);
#if 0
		debug_printf(" rotateSpeed:<%06d>\n", mpu6050dat[i].rotateSpeed);
#endif
		//判断正反转
		if (mpu6050dat[i].currentAngle > mpu6050dat[i].lastAngle) 
		{
		 	mpu6050dat[i].rotateDirect = 1;
		}
		else if (mpu6050dat[i].currentAngle < mpu6050dat[i].lastAngle)
		{
		 	mpu6050dat[i].rotateDirect = -1;
		}
		else 
		{
		 	mpu6050dat[i].rotateDirect = 0;	
			goto end;	
		}

		//---匹配规则---

		//+匹配规则启动的条件
		if (matchStage[i] == 0)
		{
			DestRule[i][matchStage[i]][0] = mpu6050dat[i].rotateDirect;
			matchStage[i] = 1;	 
		}

		//+各个阶段的规则匹配处理
		if (matchStage[i] > 0)
		{
			if (mpu6050dat[i].rotateDirect == DestRule[i][matchStage[i]-1][0])//与规则同向旋转情况处理
			{
				tempAngle = CalIntervalAngle(mpu6050dat[i].currentAngle, mpu6050dat[i].lastAngle);
				matchedRotateAngle[i] += ABS(tempAngle);

#if CONF_NRF24L01_SND
				gLen = sprintf((char*)buf, " [%d]-Axis:matchStage<%d>, RotateAngle:<%04d> \n", i, matchStage[i], matchedRotateAngle[i]);
				debug_printf((char*)buf);
				NRFSndDate(buf, gLen);
#else
				debug_printf(" [%d]-Axis:matchStage<%d>, RotateAngle:<%04d> \n", i, matchStage[i], matchedRotateAngle[i]);
#endif
				if ((matchedRotateAngle[i] != 0 ) && (matchedRotateAngle[i] % 90 == 0))//beep every rotate 90 degree
				{
					bsp_BeepOn();
					BspDelayMS(100);
					bsp_BeepOff();
				} 	
			}
			else //与规则反向旋转情况处理
			{
				if (matchedRotateAngle[i] < SET_MIN_ANGLE)//do not meet the condition
				{
//					if (matchStage[i] == 1)
//					{
//						matchStage[i] = 0;
//						matchedRotateAngle[i] = 0;
//					}
//					else
//					{
//						mpu6050dat[i].rotateDirect = mpu6050dat[i].rotateDirect > 0 ? -1 : 1;
//						matchedRotateAngle[i] = DestRule[i][matchStage[i]-1-1][1] - matchedRotateAngle[i] + 
//							ABS(CalIntervalAngle(mpu6050dat[i].currentAngle, mpu6050dat[i].lastAngle));						
//					}
				}
				else	
				{
#if CONF_NRF24L01_SND
				gLen = sprintf((char*)buf, " [%d]-Axis:stage<%02d> Over!\n", i, matchStage[i]);
				debug_printf((char*)buf);
				NRFSndDate(buf, gLen);
#else
				debug_printf(" [%d]-Axis:stage<%02d> Over!\n", i, matchStage[i]);
#endif
					//save current group game rule
					DestRule[i][matchStage[i]-1][0] = mpu6050dat[i].rotateDirect > 0 ? -1 : 1;
					DestRule[i][matchStage[i]-1][1] = matchedRotateAngle[i];
					ruleGroupNum = matchStage[i]++;

					//set the direction of next group game rule, and clear the rotate angle. 
					DestRule[i][matchStage[i]-1][0] = mpu6050dat[i].rotateDirect;
					matchedRotateAngle[i] = ABS(CalIntervalAngle(mpu6050dat[i].currentAngle, mpu6050dat[i].lastAngle));	 				
				}
			}
		}

		//+最后一个rule达到指定角度后的情况处理
		if (matchStage[i] >= RULE_NUM_MAX)
		{
			debug_printf(" ERROR !!! game_rule_len > %d, Saving first %d data.\n", RULE_NUM_MAX, RULE_NUM_MAX);	
			matchStage[i] = 0;
			matchedRotateAngle[i] = 0;
		}

   end:
		//保存当前状态(到last变量中)
		mpu6050dat[i].lastAngle = mpu6050dat[i].currentAngle;
		mpu6050dat[i].lastTimestamp = mpu6050dat[i].currentTimestamp;
	}
		
}

/*********************************************************************************************************
** Function name:       HandleMPU6050Data
** Descriptions:        used in nomal-mode
** input parameters:    axis_attr *mpu6050dat
** output parameters:   none
** Returned value:      none
*********************************************************************************************************/
void HandleMPU6050Data(struct axis_attr *mpu6050dat)
{
	uint8_t i;
	int16_t tempAngle;

	UpdateMPU6050Data(mpu6050dat);

#ifdef	DETECT_VIBRATE_EN	
	DeteckVibrate();
#endif


	//如果只检测Z(Yaw)轴，将i=2即可。注：由于X(Pitch)轴的角度范围是+-90。故当角度大于+-90度时计算有误。
	for (i = 2; i < 3; i++)
	{
		//计算角速度：度/秒
		mpu6050dat[i].rotateSpeed = ABS(CalIntervalAngle(mpu6050dat[i].currentAngle, mpu6050dat[i].lastAngle)) * 1000 /
			(mpu6050dat[i].currentTimestamp - mpu6050dat[i].lastTimestamp);
#if 0
		debug_printf(" rotateSpeed:<%06d>\n", mpu6050dat[i].rotateSpeed);
#endif
		//判断正反转
		if (mpu6050dat[i].currentAngle > mpu6050dat[i].lastAngle) 
		{
		 	mpu6050dat[i].rotateDirect = 1;
		}
		else if (mpu6050dat[i].currentAngle < mpu6050dat[i].lastAngle)
		{
		 	mpu6050dat[i].rotateDirect = -1;
		}
		else 
		{
		 	mpu6050dat[i].rotateDirect = 0;	
			goto end;	
		}

		//---匹配规则---

		//+匹配规则启动的条件
		if ((mpu6050dat[i].rotateDirect == DestRule[i][0][0]) && (matchStage[i] == 0))
		{
			matchStage[i] = 1;	 
		}

		//+各个阶段的规则匹配处理
		if ((matchStage[i] > 0) && (matchStage[i] <= ruleGroupNum))
		{
			if (mpu6050dat[i].rotateDirect == DestRule[i][matchStage[i]-1][0])//与规则同向旋转情况处理
			{
				tempAngle = CalIntervalAngle(mpu6050dat[i].currentAngle, mpu6050dat[i].lastAngle);
				matchedRotateAngle[i] += ABS(tempAngle);
				
#if CONF_NRF24L01_SND
				gLen = sprintf((char*)buf, " [%d]-Axis:matchStage<%d>, RotateAngle:<%04d> \n", i, matchStage[i], matchedRotateAngle[i]);
				debug_printf((char*)buf);
				NRFSndDate(buf, gLen);
#else
				debug_printf(" [%d]-Axis:matchStage<%d>, RotateAngle:<%04d> \n", i, matchStage[i], matchedRotateAngle[i]);
#endif
			}
			else //与规则反向旋转情况处理
			{
				if ((matchedRotateAngle[i] >= DestRule[i][matchStage[i]-1][1]) && (matchedRotateAngle[i] < 
					DestRule[i][matchStage[i]-1][1] + COMPTB_ANGLE))//处于浮动角度范围内时的反转情况处理
				{			
#if CONF_NRF24L01_SND
					gLen = sprintf((char*)buf, " [%d]-Axis:stage<%02d>Pass!\n", i, matchStage[i]);
					debug_printf((char*)buf);
					NRFSndDate(buf, gLen);
#else
					debug_printf(" [%d]-Axis:stage<%02d>Pass!\n", i, matchStage[i]);
#endif
					matchStage[i]++;//本次规则匹配通过，进入下一个规则匹配
				}
				else
				{				
#if CONF_NRF24L01_SND
					gLen = sprintf((char*)buf, " [%d]-Axis:Stop=>[%d]\n", i, matchStage[i]);
					uart_printf((char*)buf);
					NRFSndDate(buf, gLen);
#else
					uart_printf(" [%d]-Axis:Stop=>[%d]\n", i, matchStage[i]);
#endif
					matchStage[i] = 0;//进入初始阶段											
				}
				matchedRotateAngle[i] = 0;
			}
		}

		//+最后一个rule达到指定角度后的情况处理
		if ((matchStage[i] >= ruleGroupNum) && (matchedRotateAngle[i] >= DestRule[i][matchStage[i]-1][1]))
		{
			matchStage[i] = 0;
			matchedRotateAngle[i] = 0;
			passFlag[i] = 1;	
		}

   end:
		//保存当前状态(到last变量中)
		mpu6050dat[i].lastAngle = mpu6050dat[i].currentAngle;
		mpu6050dat[i].lastTimestamp = mpu6050dat[i].currentTimestamp;
	}
		
}

/*********************************************************************************************************
** Function name:       MPU6050Init
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MPU6050Init(void)//耗时是由于提供的库函数中带有延时函数导致
{
	mpu_init();	//runtime:150ms
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);	//runtime:50ms
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);	//runtime:50ms
	mpu_set_sample_rate(DEFAULT_MPU_HZ);
	dmp_load_motion_driver_firmware();	//runtime:210ms
	dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
	dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
				DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
				DMP_FEATURE_GYRO_CAL);	//runtime:150ms
	dmp_set_fifo_rate(DEFAULT_MPU_HZ);

	if (run_self_test())	//runtime:870ms
	{
#if NRF24L01_SND	
		gLen = sprintf((char*)buf, "MPU6050 Self-Test Failed. Reset System!\n");
		uart_printf(buf);
		NRFSndDate(buf, gLen);
#else
		uart_printf("MPU6050 Self-Test Failed. Reset System!\n");		
#endif
        
		SoftReset();//jgs:软复位系统 
	}

	mpu_set_dmp_state(1);	//runtime:50ms

#if 1
	GetMPU6050InitInfo(); //for test
#endif
}
