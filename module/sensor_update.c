#include "sensor_update.h"
#include "global.h"

#define SensorUpdate_delayMs(ms) g_DelayMs(ms)

#define CAL_TH_X 0.20
#define CAL_TH_Y 0.20
#define CAL_TH_Z 0.10

float RotationMatrixAcc[3][3];
float RotationMatrixGyro[3][3];

const EULER acc_align_error = {0 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD};
const EULER gyro_align_error = {0 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD};

/* acceleartion without gain and offset */
/* FC2 Black and white */
const float AccMax_X = 9.7763;
const float AccMin_X = -9.8929;

const float AccMax_Y = 9.6681;
const float AccMin_Y = -9.8610;

const float AccMax_Z = 10.5568;
const float AccMin_Z = -9.2338;

/* FC1 white */
//const float AccMax_X =  9.7481;
//const float AccMin_X = -9.8965;
//
//const float AccMax_Y = 9.9329;
//const float AccMin_Y = -9.7113;
//
//const float AccMax_Z =  9.7152;
//const float AccMin_Z =  -10.004;

//const float AccMax_X =  G_VALUE;
//const float AccMin_X = -G_VALUE;
//
//const float AccMax_Y = G_VALUE;
//const float AccMin_Y = -G_VALUE;
//
//const float AccMax_Z =  G_VALUE;
//const float AccMin_Z =  -G_VALUE;

static float GyroOffsetX = 0;
static float GyroOffsetY = 0;
static float GyroOffsetZ = 0;

static float AccOffsetX = 0;
static float AccOffsetY = 0;
static float AccOffsetZ = 0;

static float AccGainX = 1;
static float AccGainY = 1;
static float AccGainZ = 1;

/**************Sensor xyz Gain**************/
extern float MAG_GAIN_X;
extern float MAG_GAIN_Y;
extern float MAG_GAIN_Z;

#define ACC_SCALE_X (G_VALUE / ACC_SENSITIVITY)
#define ACC_SCALE_Y (G_VALUE / ACC_SENSITIVITY)
#define ACC_SCALE_Z (G_VALUE / ACC_SENSITIVITY)

#define GYRO_SCALE_X (DEG2RAD / GYRO_SENSITIVITY)
#define GYRO_SCALE_Y (DEG2RAD / GYRO_SENSITIVITY)
#define GYRO_SCALE_Z (DEG2RAD / GYRO_SENSITIVITY)

/***********************************************/

/*
***********************************************
*            SensorUpdate_Init()
*Init all Sensors
***********************************************
*/
void SensorUpdate_Init(void)
{
  uint32_t i;
  uint32_t NUM_SAMPLES = 1000;
  VECTOR g_tmp = {0, 0, 0};
  VECTOR acc, gyro;
  VECTOR acc_gain;
  VECTOR acc_offset;

  BSP_Ser_Printf("Sensor Init\n\r");

  RotationMatrixAcc[0][0] = cos(acc_align_error.yaw) * cos(acc_align_error.pitch);
  RotationMatrixAcc[0][1] = cos(acc_align_error.yaw) * sin(acc_align_error.pitch) * sin(acc_align_error.roll) - sin(acc_align_error.yaw) * cos(acc_align_error.roll);
  RotationMatrixAcc[0][2] = cos(acc_align_error.yaw) * sin(acc_align_error.pitch) * cos(acc_align_error.roll) + sin(acc_align_error.yaw) * sin(acc_align_error.roll);
  RotationMatrixAcc[1][0] = sin(acc_align_error.yaw) * cos(acc_align_error.pitch);
  RotationMatrixAcc[1][1] = sin(acc_align_error.yaw) * sin(acc_align_error.pitch) * sin(acc_align_error.roll) + cos(acc_align_error.yaw) * cos(acc_align_error.roll);
  RotationMatrixAcc[1][2] = sin(acc_align_error.yaw) * sin(acc_align_error.pitch) * cos(acc_align_error.roll) - cos(acc_align_error.yaw) * sin(acc_align_error.roll);
  RotationMatrixAcc[2][0] = -sin(acc_align_error.pitch);
  RotationMatrixAcc[2][1] = cos(acc_align_error.pitch) * sin(acc_align_error.roll);
  RotationMatrixAcc[2][2] = cos(acc_align_error.pitch) * cos(acc_align_error.roll);

  RotationMatrixGyro[0][0] = cos(gyro_align_error.yaw) * cos(gyro_align_error.pitch);
  RotationMatrixGyro[0][1] = cos(gyro_align_error.yaw) * sin(gyro_align_error.pitch) * sin(gyro_align_error.roll) - sin(gyro_align_error.yaw) * cos(gyro_align_error.roll);
  RotationMatrixGyro[0][2] = cos(gyro_align_error.yaw) * sin(gyro_align_error.pitch) * cos(gyro_align_error.roll) + sin(gyro_align_error.yaw) * sin(gyro_align_error.roll);
  RotationMatrixGyro[1][0] = sin(gyro_align_error.yaw) * cos(gyro_align_error.pitch);
  RotationMatrixGyro[1][1] = sin(gyro_align_error.yaw) * sin(gyro_align_error.pitch) * sin(gyro_align_error.roll) + cos(gyro_align_error.yaw) * cos(gyro_align_error.roll);
  RotationMatrixGyro[1][2] = sin(gyro_align_error.yaw) * sin(gyro_align_error.pitch) * cos(gyro_align_error.roll) - cos(gyro_align_error.yaw) * sin(gyro_align_error.roll);
  RotationMatrixGyro[2][0] = -sin(gyro_align_error.pitch);
  RotationMatrixGyro[2][1] = cos(gyro_align_error.pitch) * sin(gyro_align_error.roll);
  RotationMatrixGyro[2][2] = cos(gyro_align_error.pitch) * cos(gyro_align_error.roll);

  acc_gain.x = (2.0f * G_VALUE) / (AccMax_X - AccMin_X);
  acc_gain.y = (2.0f * G_VALUE) / (AccMax_Y - AccMin_Y);
  acc_gain.z = (2.0f * G_VALUE) / (AccMax_Z - AccMin_Z);

  acc_offset.x = 0.5f * (AccMax_X + AccMin_X) * acc_gain.x;
  acc_offset.y = 0.5f * (AccMax_Y + AccMin_Y) * acc_gain.y;
  acc_offset.z = 0.5f * (AccMax_Z + AccMin_Z) * acc_gain.z;

  SensorUpdate_SetAccGain(acc_gain.x, acc_gain.y, acc_gain.z);

  SensorUpdate_SetAccOffset(acc_offset.x, acc_offset.y, acc_offset.z);

  SensorUpdate_SetGyroOffset(0, 0, 0);

  if (MPU6050_Init() == DEF_OK)
  {
    BSP_Ser_Printf("MPU6050 Init OK...\r\n");
  }
  else
    BSP_Ser_Printf("MPU6050 Init Fail...\r\n");

  do
  {
    BSP_Ser_Printf("Calibrating ...\r\n");

    g_tmp.x = 0;
    g_tmp.y = 0;
    g_tmp.z = 0;

    for (i = 0; i < NUM_SAMPLES; i++)
    {
      SensorUpdate_delayMs(1);
      SensorUpdate_GetGyroAccRaw(&gyro, &acc);
      VectAdd(&g_tmp, &g_tmp, &gyro);
    }

    VectScale(&g_tmp, 1.0f / (float)NUM_SAMPLES);
    SensorUpdate_SetGyroOffset(g_tmp.x, g_tmp.y, g_tmp.z);

    /* Calibrate check */
    g_tmp.x = 0;
    g_tmp.y = 0;
    g_tmp.z = 0;

    for (i = 0; i < NUM_SAMPLES; i++)
    {
      SensorUpdate_delayMs(1);
      SensorUpdate_GetGyroAccRaw(&gyro, &acc);
      g_tmp.x += gyro.x;
      g_tmp.y += gyro.y;
      g_tmp.z += gyro.z;
    }
    BSP_Ser_Printf("gyro error:x:%5.2f, y:%5.2f, z:%5.2f\r\n", g_tmp.x, g_tmp.y, g_tmp.z);
  } while ((fabs(g_tmp.x) > CAL_TH_X) || (fabs(g_tmp.y) > CAL_TH_Y) || (fabs(g_tmp.z) > CAL_TH_Z));

  /* Get Acc norm */
  for (i = 0; i < 200; i++)
  {
    SensorUpdate_delayMs(1);
    SensorUpdate_GetGyroAccRaw(&gyro, &acc);
    g_ACC_NORM += sqrt(acc.x * acc.x + acc.y * acc.y + acc.z * acc.z);
  }
  g_AccCurrentNorm = g_ACC_NORM /= 200.0f;

  BSP_Ser_Printf("Gyroscope Offset:x:%5.2f, y:%5.2f, z:%5.2f\r\n", GyroOffsetX, GyroOffsetY, GyroOffsetZ);
}

/*
*********************************************************************
*                  SensorUpdate_GetGyroAccRaw()
* Description : Get mpu6050 sensor data and convert to vector.
*********************************************************************
*/
uint8_t SensorUpdate_GetGyroAccRaw(VECTOR *g, VECTOR *a)
{
  uint8_t status;
  float gyro_x_tmp;
  float gyro_y_tmp;
  float gyro_z_tmp;
  float acc_x_tmp;
  float acc_y_tmp;
  float acc_z_tmp;

  MPU6050_TYPE mpu6050 = {0};

  status = MPU6050_Read(&mpu6050);

  if (status == DEF_OK)
  {
    //    acc_x_tmp = RotationMatrixAcc[0][0]*mpu6050.acc_x + RotationMatrixAcc[0][1]*mpu6050.acc_y +RotationMatrixAcc[0][2]*mpu6050.acc_z;
    //    acc_y_tmp = RotationMatrixAcc[1][0]*mpu6050.acc_x + RotationMatrixAcc[1][1]*mpu6050.acc_y +RotationMatrixAcc[1][2]*mpu6050.acc_z;
    //    acc_z_tmp = RotationMatrixAcc[2][0]*mpu6050.acc_x + RotationMatrixAcc[2][1]*mpu6050.acc_y +RotationMatrixAcc[2][2]*mpu6050.acc_z;

    //    gyro_x_tmp = RotationMatrixGyro[0][0]*mpu6050.gyro_x + RotationMatrixGyro[0][1]*mpu6050.gyro_y +RotationMatrixGyro[0][2]*mpu6050.gyro_z;
    //    gyro_y_tmp = RotationMatrixGyro[1][0]*mpu6050.gyro_x + RotationMatrixGyro[1][1]*mpu6050.gyro_y +RotationMatrixGyro[1][2]*mpu6050.gyro_z;
    //    gyro_z_tmp = RotationMatrixGyro[2][0]*mpu6050.gyro_x + RotationMatrixGyro[2][1]*mpu6050.gyro_y +RotationMatrixGyro[2][2]*mpu6050.gyro_z;

    acc_x_tmp = -mpu6050.acc_y;
    acc_y_tmp = mpu6050.acc_x;
    acc_z_tmp = -mpu6050.acc_z;

    gyro_x_tmp = mpu6050.gyro_y;
    gyro_y_tmp = -mpu6050.gyro_x;
    gyro_z_tmp = mpu6050.gyro_z;

    /* m/s^2 */
    a->x = AccGainX * ACC_SCALE_X * acc_x_tmp - AccOffsetX;
    a->y = AccGainY * ACC_SCALE_Y * acc_y_tmp - AccOffsetY;
    a->z = AccGainZ * ACC_SCALE_Z * acc_z_tmp - AccOffsetZ;

    /* Rad/s */
    g->x = GYRO_SCALE_X * gyro_x_tmp - GyroOffsetX;
    g->y = GYRO_SCALE_Y * gyro_y_tmp - GyroOffsetY;
    g->z = GYRO_SCALE_Z * gyro_z_tmp - GyroOffsetZ;
  }

  return status;
}

/*
*************************************************************
*           SensorUpdate_SetGyroOffset()
*************************************************************
*/
void SensorUpdate_SetGyroOffset(float x, float y, float z)
{
  GyroOffsetX += x;
  GyroOffsetY += y;
  GyroOffsetZ += z;
}

/*
*************************************************************
*           SensorUpdate_SetAccOffset()
*************************************************************
*/
void SensorUpdate_SetAccOffset(float x, float y, float z)
{
  AccOffsetX = x;
  AccOffsetY = y;
  AccOffsetZ = z;
}

/*
*************************************************************
*           SensorUpdate_SetAccGain()
*************************************************************
*/
void SensorUpdate_SetAccGain(float x, float y, float z)
{
  AccGainX = x;
  AccGainY = y;
  AccGainZ = z;
}
