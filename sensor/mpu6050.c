#include "mpu6050.h"


#ifndef PI
#define PI     3.141592585f
#endif 

void MPU6050_SingleWrite(uint8_t reg, uint8_t val)
{
  IIC1_Write(MPU6050_SLAVE_ADDR, reg, &val, 1);
}

uint8_t MPU6050_SingleRead(uint8_t reg)
{
  uint8_t rx;
  IIC1_Read(MPU6050_SLAVE_ADDR, reg, &rx, 1);
  return rx;
}

uint8_t MPU6050_MultiRead(uint8_t reg, uint8_t *buf, uint8_t len)
{
  return IIC1_Read(MPU6050_SLAVE_ADDR, reg, buf, len);
}


void MPU6050_Print(char* str)
{
  BSP_Ser_Printf("%s",str);
}


/**************************************
* @brief : 锟斤拷始锟斤拷MPU6050
* @param : none
***************************************/
uint8_t MPU6050_Init(void)
{
  while (MPU6050_Check() == DEF_FAIL)
    MPU6050_Print("MPU6050 Init fail...");
  MPU6050_SingleWrite(PWR_MGMT_1, 0x80); /* Reset must wait ~50ms */
  MPU6050_Delay(200); /* after reset it turns to sleep mode */
  MPU6050_SingleWrite(PWR_MGMT_1, 0x03); /* wake up,PLL with Z axis gyroscope reference*/
  MPU6050_SingleWrite(USER_CTRL, 0x01); /* Reset signal path */
  MPU6050_Delay(200);
  MPU6050_SingleWrite(SMPLRT_DIV, 0x04); /* 1khz/(1+SMPLRT_DIV)*/
  MPU6050_SingleWrite(INT_PIN_CFG, 0x02); /* Config INT Pin PP,active high, the INT pin emits a 50us long pulse */
  MPU6050_SingleWrite(INT_ENABLE, 0x01); /* Enable data ready interrupt */
  MPU6050_SingleWrite(CONFIG, 0x04); /*锟斤拷通锟剿诧拷频锟绞ｏ拷0x06(5Hz) (0x04 20Hz) (0x03 42Hz) (0x05 10Hz) (0x02 98Hz)*/
  MPU6050_SingleWrite(GYRO_CONFIG, 0x18); /*锟斤拷锟斤拷锟斤拷锟皆检及锟斤拷锟斤拷锟斤拷围锟斤拷锟斤拷锟斤拷值锟斤拷0x18(锟斤拷锟皆检，2000deg/s) 0x08(500deg/s) 0x00(250deg/s) 0x10(1000deg/s)*/
  MPU6050_SingleWrite(ACCEL_CONFIG, 0x08); /* no self test,acc range +-4g*/
  MPU6050_Print("MPU6050 Init...\r\n");
  
//  BSP_SerPrintf("SMPLRT_DIV:%x\r\n", MPU6050_SingleRead(SMPLRT_DIV));
//  BSP_SerPrintf("INT_PIN_CFG:%x\r\n", MPU6050_SingleRead(INT_PIN_CFG));
//  BSP_SerPrintf("INT_ENABLE:%x\r\n", MPU6050_SingleRead(INT_ENABLE));
//  BSP_SerPrintf("CONFIG:%x\r\n", MPU6050_SingleRead(CONFIG));
//  BSP_SerPrintf("GYRO_CONFIG:%x\r\n", MPU6050_SingleRead(GYRO_CONFIG));
//  BSP_SerPrintf("ACCEL_CONFIG:%x\r\n", MPU6050_SingleRead(ACCEL_CONFIG));
//  
//  MPU6050_Delay(5000);
  return DEF_OK;
}
/*******************************************
*@name   : MPU6050_Check
*@brief  : 锟斤拷锟組PU6050锟角凤拷锟斤拷锟斤拷锟斤拷锟斤拷
*@param  : none
*@retval : none
********************************************/
uint8_t MPU6050_Check(void)
{
  uint8_t state;
  state = MPU6050_SingleRead(WHO_AM_I);
  if (state == 0X68)
    return DEF_OK;
  else
    return DEF_FAIL;
}

/********************************************
*@brief  : Get 6 Axis Data
*@param	:  -MPU6050_TYPE*
*@retval : 
*********************************************/
uint8_t MPU6050_Read(MPU6050_TYPE* MPU6050_Value)
{
  uint8_t tmp[14];
  /* acc data */
  
  if (MPU6050_MultiRead(ACCEL_XOUT_H, &tmp[0], 14) == DEF_FAIL)
  {
    //MPU6050_Print("MPU6050 error!!!\r\n");
    return DEF_FAIL;
  }
  
  MPU6050_Value->acc_x = ((tmp[0] << 8) + tmp[1]);
  
  MPU6050_Value->acc_y = ((tmp[2] << 8) + tmp[3]);
  
  MPU6050_Value->acc_z = ((tmp[4] << 8) + tmp[5]);
  /* gyro data */
  MPU6050_Value->gyro_x = ((tmp[8] << 8) + tmp[9]);
  
  MPU6050_Value->gyro_y = ((tmp[10] << 8) + tmp[11]);
  
  MPU6050_Value->gyro_z = ((tmp[12] << 8) + tmp[13]);
  
  MPU6050_Value->temperature = (tmp[6] << 8) + tmp[7];
  
  return DEF_OK;
}

/********************************************
*@brief  : Get 3 Axis Accelemeter data
*@param	:  -MPU6050_TYPE*
*@retval : 
*********************************************/
uint8_t MPU6050_ReadAcc(MPU6050_TYPE* MPU6050_Value)
{
  uint8_t tmp[6];
  /* acc data */
  if (MPU6050_MultiRead(ACCEL_XOUT_H, &tmp[0], 6) == DEF_FAIL)
  {
    return DEF_FAIL;
  }
  
  MPU6050_Value->acc_y = ((tmp[0] << 8) + tmp[1]);
  
  MPU6050_Value->acc_x = -((tmp[2] << 8) + tmp[3]);
  
  MPU6050_Value->acc_z = -((tmp[4] << 8) + tmp[5]);
  
  return DEF_OK;
}

/********************************************
*@brief  : Get 3 Axis Gyroscope data
*@param	:  -MPU6050_TYPE*
*@retval : 
*********************************************/
uint8_t MPU6050_ReadGyro(MPU6050_TYPE* MPU6050_Value)
{
  uint8_t tmp[6];
  /* acc data */
  if (MPU6050_MultiRead(GYRO_XOUT_H, &tmp[0], 6) == DEF_FAIL)
  {
    return DEF_FAIL;
  }
  
  MPU6050_Value->gyro_y = -((tmp[0] << 8) + tmp[1]);
  
  MPU6050_Value->gyro_x = ((tmp[2] << 8) + tmp[3]);
  
  MPU6050_Value->gyro_z = ((tmp[4] << 8) + tmp[5]);
  return DEF_OK;
}

