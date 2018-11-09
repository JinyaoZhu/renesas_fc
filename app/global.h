#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "bsp.h"
#include "algorithm.h"
#include "motor.h"
#include "control.h"
#include "rc.h"

extern VECTOR g_GyroRaw;
extern VECTOR g_GyroFiltered;
extern VECTOR g_AccRaw;
extern VECTOR g_AccFiltered;
extern float g_ACC_NORM;
extern float g_AccCurrentNorm;
extern float g_Thrust;

extern VECTOR g_AccEstBody;
extern VECTOR g_MagEstBody;

extern EULER g_Euler;
extern EULER g_TargetEuler;
extern EULER g_NavEuler;
extern EULER g_UserEuler;
extern EULER g_RCEuler;
extern QUATERNION g_Quaternion;

extern VECTOR g_AccRef;
extern VECTOR g_AccRefOffset;
extern VECTOR g_VelRef;
extern VECTOR g_PosRef;
extern VECTOR g_TargetPos;

extern float g_SonarAlt;

extern float g_CamOffset_x;
extern float g_CamOffset_y;

extern float g_AttLoopDt;

extern CTRL_DataType g_Control; 
extern MOTOR_DataType g_Motor;
extern RC_DataType  g_RC;

extern float g_Debug1;
extern float g_Debug2;
extern float g_Debug3;

extern uint32_t g_IMU_IIC_ErrorCnt;
extern uint32_t g_OPTFLOW_IIC_ErrorCnt;
extern uint32_t g_Slave_SPI_ErrorCnt;

extern uint8_t g_FlagArm;
extern uint8_t g_FlagAttInit;
extern uint8_t g_FlagSonarUpdate;
extern uint8_t g_FlagSetHeight;
extern uint8_t g_FlagHovering;
extern uint8_t g_FlagTakeOff;
extern uint8_t g_FlagCamUpdate_Y;
extern uint8_t g_FlagCamUpdate_X;
extern uint8_t g_FlagEnableControlX;
extern uint8_t g_FlagEnableControlY;
extern uint8_t g_FlagFailSafe;


void g_DelayMs(uint16_t i);

float g_Getdt(uint16_t* timestamp_old);


#endif /* __GLOBAL_H__ */
