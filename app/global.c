#include "global.h"

VECTOR g_GyroRaw = {0,0,0};
VECTOR g_GyroFiltered = {0,0,0};
VECTOR g_AccRaw = {0,0,0};
VECTOR g_AccFiltered = {0,0,0};
float g_ACC_NORM = 9.8;
float g_AccCurrentNorm = 9.8;
float g_Thrust = 0;


VECTOR g_AccEstBody = {0,0,-9.8};
VECTOR g_MagEstBody = {0,1,0};

EULER g_Euler = {0,0,0};
EULER g_TargetEuler = {0,0,0};
EULER g_NavEuler = {0,0,0};
EULER g_UserEuler = {0,0,0};
EULER g_RCEuler = {0,0,0};
QUATERNION g_Quaternion = {1,0,0,0};
VECTOR g_AccRef = {0,0,-9.8};
VECTOR g_AccRefOffset = {0,0,0};
VECTOR g_VelRef = {0,0,0};
VECTOR g_PosRef = {0,0,0};
VECTOR g_TargetPos = {0,0,0};

float g_SonarAlt = 0;
float g_LastSonarAlt = 0;
float g_SonarVel = 0;

float g_CamOffset_x = 0;
float g_CamOffset_y = 0;


float g_AttLoopDt = 0;

CTRL_DataType g_Control; 
MOTOR_DataType g_Motor;
RC_DataType  g_RC;

uint8_t g_FlagArm = SET;
uint8_t g_FlagAttInit = RESET;
uint8_t g_FlagSonarUpdate = RESET;
uint8_t g_FlagHovering = RESET;
uint8_t g_FlagSetHeight = RESET;
uint8_t g_FlagTakeOff = RESET;
uint8_t g_FlagCamUpdate_Y = RESET;
uint8_t g_FlagCamUpdate_X = RESET;
uint8_t g_FlagEnableControlX = SET;
uint8_t g_FlagEnableControlY = SET;
uint8_t g_FlagFailSafe = RESET;


float g_Debug1 = 0;
float g_Debug2 = 0;
float g_Debug3 = 0;

uint32_t g_IMU_IIC_ErrorCnt = 0;
uint32_t g_OPTFLOW_IIC_ErrorCnt = 0;
uint32_t g_Slave_SPI_ErrorCnt = 0;


void g_DelayMs(uint16_t i)
{
  uint16_t j,k;
  for(k = 0; k<i; k++){
    j = 3600;
    while(j--);
  }
}

const float PERIOD_CNT = 0.5f/STAMPTIMER_MAX_CNT;

float g_Getdt(uint16_t* timestamp_old)
{
  uint16_t new,old;
  new = GetCounter7();
  old = *timestamp_old;
  *timestamp_old = new;
  if(new > old)
    return (float)(STAMPTIMER_MAX_CNT - new + old + 1)*PERIOD_CNT;
  else
    return (float)(old - new)*PERIOD_CNT;
}
