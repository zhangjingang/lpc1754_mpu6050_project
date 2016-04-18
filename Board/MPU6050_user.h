#ifndef MPU6050_H
#define MPU6050_H

#include "sys_conf.h"

//指定某一轴在一次采集中所具有的属性
struct axis_attr 
{
	short gyro;						//gyro value
	short accel;					//accel vaule
	short currentAngle;				//current angle value
	short lastAngle;				//last angle value
	unsigned long currentTimestamp;	//current timestramp
	unsigned long lastTimestamp;  	//last timestramp
	int rotateAngle; 				//旋转的角度	
	short rotateDirect;				//旋转方向	1:正向	-1：反向	0:不转	
	short rotateSpeed;				//角速度  度/秒
};

//!!! Global variables, used in main.c and mpu6050_user.c
extern struct axis_attr mpuDat[3];//[0],[1],[2]分别代码X(Pitch)，Y(Roll)，Z(Yaw)轴
extern unsigned char passFlag[3];
extern unsigned char buf[OUTPUT_BUF];
extern unsigned char gLen; 
extern short DestRule[3][RULE_NUM_MAX][2];
extern unsigned short ruleGroupNum;
extern short matchedRotateAngle[3]; //record the last matched angle in set-mode

extern void MPU6050Init(void);
extern void HandleMPU6050Data(struct axis_attr *mpu6050dat);
extern void HandleMPU6050Data2(struct axis_attr *mpu6050dat);
extern void SoftReset(void);

#endif
