#include "iic1.h"

#define IIC1_DELAY()      NOP();NOP();NOP();\
                          NOP();NOP();NOP();\
                          NOP();NOP();NOP();

#define IIC1_DELAY_()       NOP();NOP();NOP();\
                            NOP();NOP();NOP();

void IIC1_Init(void)
{
  IIC1_SDA_PIN_INPUT = 0;  //set output
  IIC1_SCL_PIN_INPUT = 0;  //set output
  IIC1_SDA_PIN = 1;
  IIC1_SCL_PIN = 1;
}

/*******************************************************************
Start iic bus
********************************************************************/
void IIC1_Start(void)
{
  IIC1_SDA_PIN = 1; /* send start data signal */
  IIC1_SCL_PIN = 1;
  IIC1_DELAY();
  IIC1_SDA_PIN = 0;       /* send start signal */
  IIC1_DELAY();
  IIC1_SCL_PIN = 0;
}
/*******************************************************************
Stop IIC Bus
********************************************************************/
void IIC1_Stop(void)
{
  IIC1_SDA_PIN = 0;
  IIC1_SCL_PIN = 1;
  IIC1_DELAY();
  IIC1_SDA_PIN = 1;
  IIC1_DELAY();
}

/*******************************************************************
Send one byte data or address
arguments   ��  a   0: IIC1_acknowledge
1: no IIC1_acknowledge
********************************************************************/
void IIC1_Ack(uint8_t a)
{
  IIC1_SCL_PIN = 0;
  if (a == 0)
    IIC1_SDA_PIN = 0;
  else
    IIC1_SDA_PIN = 1;
  IIC1_DELAY();
  IIC1_SCL_PIN = 1;
  IIC1_DELAY();
  IIC1_SCL_PIN = 0;
}


/*******************************************************************
Send one byte data or address
********************************************************************/
uint8_t  IIC1_WriteByte(uint8_t ch)
{
  uint8_t BitCnt;
  uint8_t ack;
  IIC1_SCL_PIN = 0;

  if (ch & 0x80)
    IIC1_SDA_PIN = 1;      //send the first bit of data.
  else
    IIC1_SDA_PIN = 0;
    
  IIC1_DELAY_();
  IIC1_SCL_PIN = 1;
  IIC1_DELAY();
  IIC1_SCL_PIN = 0;

  for (BitCnt = 0; BitCnt < 7; BitCnt++) //send the 7 other bit of data.
  {
    if ((ch <<= 1) & 0x80)
      IIC1_SDA_PIN = 1;
    else
      IIC1_SDA_PIN = 0;
    IIC1_DELAY_();
    IIC1_SCL_PIN = 1;
    IIC1_DELAY();
    IIC1_SCL_PIN = 0;
  }
  IIC1_SDA_PIN_INPUT = 1;
  IIC1_DELAY();
  IIC1_SCL_PIN = 1;
  IIC1_DELAY();
  
  if (IIC1_SDA_PIN == 1)
    ack = 0;
  else
    ack = 1;
    
  IIC1_SCL_PIN = 0;
  IIC1_SDA_PIN_INPUT = 0;
  return ack;
}

/*******************************************************************
receive one byte data or address
********************************************************************/
uint8_t  IIC1_ReadByte(void)
{
  uint8_t retc = 0;
  uint8_t BitCnt;
  IIC1_SDA_PIN_INPUT = 1;
  
  for (BitCnt = 0; BitCnt < 8; BitCnt++)
  {
    IIC1_SCL_PIN = 0;
    IIC1_DELAY();
    IIC1_DELAY();
    IIC1_SCL_PIN = 1;
    retc <<= 1;
    if (IIC1_SDA_PIN == 1)
      retc = (uint8_t)(retc | 0x01);
  }
  IIC1_DELAY();
  IIC1_SCL_PIN = 0;
  IIC1_SDA_PIN_INPUT = 0;
  return (retc);
}


/*******************************************************************
read a serial of data by software_iic
arguments : addr  device address
reg   register address
rx_buf  receive buff
len   length of receive data
output    : is succeed?
********************************************************************/
uint8_t IIC1_Read(uint8_t  addr, uint8_t reg, uint8_t *rx_buf, uint16_t len)
{
  uint8_t  i;
  IIC1_SDA_PIN_INPUT = 0;
  IIC1_SCL_PIN_INPUT = 0;
  
  IIC1_Start();
  if (IIC1_WriteByte(addr) == 0){
    IIC1_Stop();
    return DEF_FAIL;
  }
  if (IIC1_WriteByte(reg) == 0){
    IIC1_Stop();
    return DEF_FAIL;
  }
  IIC1_Start();
  
  if (IIC1_WriteByte((uint8_t)(addr | 0x01)) == 0){
    IIC1_Stop();
    return DEF_FAIL;
  }
    
  for (i = 0; i < len - 1; i++)
  {
    *rx_buf = IIC1_ReadByte();
    IIC1_Ack(0);
    rx_buf ++;
  }
  
  *rx_buf = IIC1_ReadByte();
  IIC1_Ack(1);
  IIC1_Stop();
  return DEF_OK;
}


/*******************************************************************
write a serial of data by software_iic
arguments : addr  device address
reg   register address
data  need write data to register
len   length of receive data
output    : is succeed?
********************************************************************/
uint8_t IIC1_Write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
  uint8_t i;
  IIC1_SDA_PIN_INPUT = 0;
  IIC1_SCL_PIN_INPUT = 0;
  
  IIC1_Start();
  if (IIC1_WriteByte(addr) == 0){
    IIC1_Stop();
    return DEF_FAIL;      //no IIC1_ack ,quit.
  }
  if (IIC1_WriteByte(reg) == 0){
    IIC1_Stop();
    return DEF_FAIL;      //no IIC1_ack ,quit.
  }
    
  for (i = 0; i < len; i++)
  {
    if (IIC1_WriteByte(*data) == 0){
      IIC1_Stop();
      return DEF_FAIL;
    }
    data++;
  }
  IIC1_Stop();
  return DEF_OK;
}
