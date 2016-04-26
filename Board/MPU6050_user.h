#ifndef MPU6050_H
#define MPU6050_H

#include "sys_conf.h"
#include "LPC17xx.h" 

//指定某一轴在一次采集中所具有的属性
struct axis_attr 
{
	int16_t gyro;						//gyro value
	int16_t accel;					//accel vaule
	int16_t currentAngle;				//current angle value
	int16_t lastAngle;				//last angle value
	uint64_t currentTimestamp;	//current timestramp
	uint64_t lastTimestamp;  	//last timestramp
	int32_t rotateAngle; 				//旋转的角度	
	int16_t rotateDirect;				//旋转方向	1:正向	-1：反向	0:不转	
	int16_t rotateSpeed;				//角速度  度/秒
};

#define ABS(x) ((x) > 0 ? (x) : (-x))

//!!! Global variables, used in main.c and mpu6050_user.c
extern struct axis_attr mpuDat[3];//[0],[1],[2]分别代码X(Pitch)，Y(Roll)，Z(Yaw)轴
extern uint8_t buf[OUTPUT_BUF];
extern uint8_t gLen; 
extern int16_t DestRule[3][RULE_NUM_MAX][2];
extern uint16_t ruleGroupNum;
extern int16_t matchedRotateAngle[3]; //record the last matched angle in set-mode
extern uint8_t matchStage[3];


void MPU6050Init(void);
void HandleMPU6050Data(struct axis_attr *mpu6050dat);
void HandleMPU6050Data2(struct axis_attr *mpu6050dat);
void SoftReset(void);

#endif
