#include "bmp180.h"

BMP180_CalParamStruct BMP180_CalStruct = {0};

int32_t B5 = 0;


/*
***************************************************************
*                  BMP180_SingleWrite()
***************************************************************
*/
uint8_t BMP180_SingleWrite(uint8_t reg, uint8_t data)
{
  return IIC1_Write(BMP180_SLAVE_ADDR, reg, &data, 1);
}

/*
***************************************************************
*                  BMP180_MultiRead()
***************************************************************
*/
uint8_t BMP180_MultiRead(uint8_t reg, uint8_t* buf, uint16_t len)
{
  return IIC1_Read(BMP180_SLAVE_ADDR, reg, buf, len);
}


void BMP180_DelayMs(uint32_t ms)
{

}

/*
***************************************************************
*                  BMP180_Init()
***************************************************************
*/
uint8_t BMP180_Init(void)
{
  if ((BMP180_ReadDevID() == 0x55) && (BMP180_GetCalParam() == DEF_OK))
  {
    return DEF_OK;
  }
  else return DEF_FAIL;
}

/*
********************************************************
*               BMP180_ReadDevID()
********************************************************
*/
uint8_t BMP180_ReadDevID(void)
{
  uint8_t id = 0;
  BMP180_MultiRead(BMP180_ID, &id, 1);
  return id;
}

/*
********************************************************
*              BMP180_GetCalParam()
* Description : Get Calibration Parameter
********************************************************
*/
uint8_t BMP180_GetCalParam(void)
{
  uint8_t bmp180_buf[22] = {0};
  BMP180_MultiRead(BMP180_CALIB00, &bmp180_buf[0], 22);
  BMP180_CalStruct.AC1 = (bmp180_buf[0] << 8) + bmp180_buf[1];
  BMP180_CalStruct.AC2 = (bmp180_buf[2] << 8) + bmp180_buf[3];
  BMP180_CalStruct.AC3 = (bmp180_buf[4] << 8) + bmp180_buf[5];
  BMP180_CalStruct.AC4 = (bmp180_buf[6] << 8) + bmp180_buf[7];
  BMP180_CalStruct.AC5 = (bmp180_buf[8] << 8) + bmp180_buf[9];
  BMP180_CalStruct.AC6 = (bmp180_buf[10] << 8) + bmp180_buf[11];
  BMP180_CalStruct.B1  = (bmp180_buf[12] << 8) + bmp180_buf[13];
  BMP180_CalStruct.B2  = (bmp180_buf[14] << 8) + bmp180_buf[15];
  BMP180_CalStruct.MB  = (bmp180_buf[16] << 8) + bmp180_buf[17];
  BMP180_CalStruct.MC  = (bmp180_buf[18] << 8) + bmp180_buf[19];
  BMP180_CalStruct.MD  = (bmp180_buf[20] << 8) + bmp180_buf[21];
//  BMP180_CalStruct.AC1 = 408;
//  BMP180_CalStruct.AC2 = -72;
//  BMP180_CalStruct.AC3 = -14383;
//  BMP180_CalStruct.AC4 = 32741;
//  BMP180_CalStruct.AC5 = 32757;
//  BMP180_CalStruct.AC6 = 23153;
//  BMP180_CalStruct.B1  = 6190;
//  BMP180_CalStruct.B2  = 4;
//  BMP180_CalStruct.MB  = -32767;
//  BMP180_CalStruct.MC  = -8711;
//  BMP180_CalStruct.MD  = 2868;

  if (BMP180_CalStruct.AC1 == 0x00 || BMP180_CalStruct.AC1 == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.AC2 == 0x00 || BMP180_CalStruct.AC2 == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.AC3 == 0x00 || BMP180_CalStruct.AC3 == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.AC4 == 0x00 || BMP180_CalStruct.AC4 == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.AC5 == 0x00 || BMP180_CalStruct.AC5 == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.AC6 == 0x00 || BMP180_CalStruct.AC6 == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.B1 == 0x00 || BMP180_CalStruct.B1 == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.B2 == 0x00 || BMP180_CalStruct.B2 == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.MB == 0x00 || BMP180_CalStruct.MB == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.MC == 0x00 || BMP180_CalStruct.MC == 0xff)
    return DEF_FAIL;
  if (BMP180_CalStruct.MD == 0x00 || BMP180_CalStruct.MD == 0xff)
    return DEF_FAIL;

  return DEF_OK;
}


/*
********************************************************
*              BMP180_GetUt()
* Description : Get Uncompensated temperature
********************************************************
*/
void BMP180_GetUt(BMP180_TYPE *pBMP180_Struct,uint8_t step)
{
  static uint8_t bmp180_buf[2];
  switch (step)
  {
    case 1:
      BMP180_SingleWrite(BMP180_CTRL_MEAS, 0x2E);
      break;
    //BMP180_DelayMs(5);
    case 2:
      BMP180_MultiRead(BMP180_MSB, &bmp180_buf[0], 2);
      pBMP180_Struct->ut = (bmp180_buf[0] << 8) + bmp180_buf[1];
      //pBMP180_Struct->ut = 27898;
      break;
    default : break;
  }
}


/*
********************************************************
*              BMP180_GetUp()
* Description : Get Uncompensated pressure
********************************************************
*/
void BMP180_GetUp(BMP180_TYPE *pBMP180_Struct,uint8_t step)
{
  static uint8_t bmp180_buf[3];
  int32_t b0,b1,b2;
  switch (step)
  {
    case 1:
      BMP180_SingleWrite(BMP180_CTRL_MEAS, 0x34 + (BMP180_OSS << 6));
      break;
    //BMP180_DelayMs(26);
    case 2:
      BMP180_MultiRead(BMP180_MSB, &bmp180_buf[0], 3);
      b0 = (int32_t)bmp180_buf[0]<<16;
      b1 = (int32_t)bmp180_buf[1]<<8;
      b2 = (int32_t)bmp180_buf[2];
      pBMP180_Struct->up = (int32_t)(b0+b1+b2)>>(8-BMP180_OSS);
      //pBMP180_Struct->up = 23843;
      break;
    default : break;
  }
}

/*
********************************************************
*              BMP180_GetTemperature()
* Description : Get compensated temperature
********************************************************
*/
void BMP180_CalTemperature(BMP180_TYPE *pBMP180_Struct)
{
  int32_t x1, x2;
  x1 = ((pBMP180_Struct->ut - BMP180_CalStruct.AC6) * BMP180_CalStruct.AC5)>>15;
  x2 = ((int32_t)BMP180_CalStruct.MC<<11) / (x1 + (int32_t)BMP180_CalStruct.MD);
  B5 = x1 + x2 - 1;
  pBMP180_Struct->temperature = ((B5 + 8) / 16)*0.1f;
}

/*
********************************************************
*              BMP180_CalPressure()
* Description : Get compensated pressure
********************************************************
*/
void BMP180_CalPressure(BMP180_TYPE *pBMP180_Struct)
{
  int32_t B6, B3, B4, B7;
  int32_t x1, x2, x3, p;
  B6 = B5 - 4000;
  x1 = (BMP180_CalStruct.B2 * (B6 * B6 / 4096)) / 2048;
  x2 = BMP180_CalStruct.AC2 * B6 / 2048;
  x3 = x1 + x2;
  B3 = (((BMP180_CalStruct.AC1*4 + x3) << BMP180_OSS) + 2)/4;
  x1 = BMP180_CalStruct.AC3 * B6 / 8192;
  x2 = (BMP180_CalStruct.B1 * (B6 * B6 / 4096)) / 65535;
  x3 = ((x1 + x2) + 2) / 4;
  B4 = BMP180_CalStruct.AC4 * (uint32_t)(x3 + 32768) / 32768;
  B7 = ((uint32_t)(pBMP180_Struct->up) - B3) * ((uint32_t)50000 >> BMP180_OSS);
  if (B7 < 0x8000000)
  {
    p = (B7 * 2) / B4;
  }
  else
  {
    p = (B7 / B4) * 2;
  }
  x1 = (p / 256) * (p / 256);
  x1 = (x1 * 3038) / 65536;
  x2 = (-7357 * p) / 65536;
  p = p + (x1 + x2 + 3791) / 16 + 1;
  pBMP180_Struct->pressure = p*0.01f;
}

/*
************************************************
*                 BMP180_Read()
* Description : Get compensated data from BMP180
************************************************
*/
void BMP180_Read(BMP180_TYPE *pBMP180_Struct)
{
  //BMP180_GetUt(pBMP180_Struct);
  //BMP180_GetUp(pBMP180_Struct);
  BMP180_CalTemperature(pBMP180_Struct);
  BMP180_CalPressure(pBMP180_Struct);
}
