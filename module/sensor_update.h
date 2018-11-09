#ifndef __SENSOR_UPDATE__
#define __SENSOR_UPDATE__

#include "algorithm.h"
#include "mpu6050.h"
#include "bmp180.h"
#include "bsp.h"

typedef struct
{
  VECTOR gyro;
  VECTOR acc;
} InertiaDataType;

#define G_VALUE 9.8f

void SensorUpdate_Init(void);

void SensorUpdate_SetAccOffset(float x, float y, float z);

void SensorUpdate_SetAccGain(float x, float y, float z);

void SensorUpdate_SetGyroOffset(float x, float y, float z);

uint8_t SensorUpdate_GetGyroAccRaw(VECTOR *g, VECTOR *a);

void Sensor_Update_GetOffsetData(void);

void Sensor_Update_StoreOffsetData(void);

#endif /* __SENSOR_UPDATE__ */
