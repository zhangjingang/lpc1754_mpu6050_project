#ifndef MPU6050_H
#define MPU6050_H

#include "sys_conf.h"
#include "LPC17xx.h" 

//ָ��ĳһ����һ�βɼ��������е�����
struct axis_attr 
{
	int16_t gyro;						//gyro value
	int16_t accel;					//accel vaule
	int16_t currentAngle;				//current angle value
	int16_t lastAngle;				//last angle value
	uint64_t currentTimestamp;	//current timestramp
	uint64_t lastTimestamp;  	//last timestramp
	int32_t rotateAngle; 				//��ת�ĽǶ�	
	int16_t rotateDirect;				//��ת����	1:����	-1������	0:��ת	
	int16_t rotateSpeed;				//���ٶ�  ��/��
};

#define ABS(x) ((x) > 0 ? (x) : -(x))

//!!! Global variables, used in main.c and mpu6050_user.c
extern struct axis_attr mpuDat[3];//[0],[1],[2]�ֱ����X(Pitch)��Y(Roll)��Z(Yaw)��
extern uint8_t passFlag[3];
extern uint8_t buf[OUTPUT_BUF];
extern uint8_t gLen; 
extern int16_t DestRule[3][RULE_NUM_MAX][2];
extern uint16_t ruleGroupNum;
extern int16_t matchedRotateAngle[3]; //record the last matched angle in set-mode
extern uint8_t matchStage[3];
extern uint8_t isNewStage;


void MPU6050Init(void);
void HandleMPU6050Data(struct axis_attr *mpu6050dat);
void HandleMPU6050Data2(struct axis_attr *mpu6050dat);
void HandleMPU6050Data3(struct axis_attr *mpu6050dat);
void SoftReset(void);

#endif
