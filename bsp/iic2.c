#include "iic2.h"

#define IIC2_DELAY() \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();             \
  NOP();

#define IIC2_DELAY_() \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();              \
  NOP();

void IIC2_Init(void)
{
  IIC2_SDA_PIN_INPUT = 0; //set output
  IIC2_SCL_PIN_INPUT = 0; //set output
  IIC2_SDA_PIN = 1;
  IIC2_SCL_PIN = 1;
}

/*******************************************************************
Start iic bus
********************************************************************/
void IIC2_Start(void)
{
  IIC2_SDA_PIN = 1; /* send start data signal */
  IIC2_SCL_PIN = 1;
  IIC2_DELAY();
  IIC2_SDA_PIN = 0; /* send start signal */
  IIC2_DELAY();
  IIC2_SCL_PIN = 0;
}
/*******************************************************************
Stop IIC Bus
********************************************************************/
void IIC2_Stop(void)
{
  IIC2_SDA_PIN = 0;
  IIC2_SCL_PIN = 1;
  IIC2_DELAY();
  IIC2_SDA_PIN = 1;
  IIC2_DELAY();
}

/*******************************************************************
Send one byte data or address
arguments   ��  a   0: IIC2_acknowledge
1: no IIC2_acknowledge
********************************************************************/
void IIC2_Ack(uint8_t a)
{
  IIC2_SCL_PIN = 0;
  if (a == 0)
    IIC2_SDA_PIN = 0;
  else
    IIC2_SDA_PIN = 1;
  IIC2_DELAY();
  IIC2_SCL_PIN = 1;
  IIC2_DELAY();
  IIC2_SCL_PIN = 0;
}

/*******************************************************************
Send one byte data or address
********************************************************************/
uint8_t IIC2_WriteByte(uint8_t ch)
{
  uint8_t BitCnt;
  uint8_t ack;
  IIC2_SCL_PIN = 0;

  if (ch & 0x80)
    IIC2_SDA_PIN = 1; //send the first bit of data.
  else
    IIC2_SDA_PIN = 0;

  IIC2_DELAY_();
  IIC2_SCL_PIN = 1;
  IIC2_DELAY();
  IIC2_SCL_PIN = 0;

  for (BitCnt = 0; BitCnt < 7; BitCnt++) //send the 7 other bit of data.
  {
    if ((ch <<= 1) & 0x80)
      IIC2_SDA_PIN = 1;
    else
      IIC2_SDA_PIN = 0;
    IIC2_DELAY_();
    IIC2_SCL_PIN = 1;
    IIC2_DELAY();
    IIC2_SCL_PIN = 0;
  }
  IIC2_SDA_PIN_INPUT = 1;
  IIC2_DELAY();
  IIC2_SCL_PIN = 1;
  IIC2_DELAY();

  if (IIC2_SDA_PIN == 1)
    ack = 0;
  else
    ack = 1;

  IIC2_SCL_PIN = 0;
  IIC2_SDA_PIN_INPUT = 0;
  return ack;
}

/*******************************************************************
receive one byte data or address
********************************************************************/
uint8_t IIC2_ReadByte(void)
{
  uint8_t retc = 0;
  uint8_t BitCnt;
  IIC2_SDA_PIN_INPUT = 1;

  for (BitCnt = 0; BitCnt < 8; BitCnt++)
  {
    IIC2_SCL_PIN = 0;
    IIC2_DELAY();
    IIC2_DELAY();
    IIC2_SCL_PIN = 1;
    retc <<= 1;
    if (IIC2_SDA_PIN == 1)
      retc = (uint8_t)(retc | 0x01);
  }
  IIC2_DELAY();
  IIC2_SCL_PIN = 0;
  IIC2_SDA_PIN_INPUT = 0;
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
uint8_t IIC2_Read(uint8_t addr, uint8_t reg, uint8_t *rx_buf, uint16_t len)
{
  uint8_t i;
  IIC2_SDA_PIN_INPUT = 0;
  IIC2_SCL_PIN_INPUT = 0;

  IIC2_Start();
  if (IIC2_WriteByte(addr) == 0)
  {
    IIC2_Stop();
    return DEF_FAIL;
  }
  if (IIC2_WriteByte(reg) == 0)
  {
    IIC2_Stop();
    return DEF_FAIL;
  }
  IIC2_Start();

  if (IIC2_WriteByte((uint8_t)(addr | 0x01)) == 0)
  {
    IIC2_Stop();
    return DEF_FAIL;
  }

  for (i = 0; i < len - 1; i++)
  {
    *rx_buf = IIC2_ReadByte();
    IIC2_Ack(0);
    rx_buf++;
  }

  *rx_buf = IIC2_ReadByte();
  IIC2_Ack(1);

  IIC2_Stop();
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
uint8_t IIC2_Write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
  uint8_t i;
  IIC2_SDA_PIN_INPUT = 0;
  IIC2_SCL_PIN_INPUT = 0;

  IIC2_Start();
  if (IIC2_WriteByte(addr) == 0)
  {
    IIC2_Stop();
    return DEF_FAIL; //no IIC2_ack ,quit.
  }
  if (IIC2_WriteByte(reg) == 0)
  {
    IIC2_Stop();
    return DEF_FAIL; //no IIC2_ack ,quit.
  }

  for (i = 0; i < len; i++)
  {
    if (IIC2_WriteByte(*data) == 0)
    {
      IIC2_Stop();
      return DEF_FAIL;
    }
    data++;
  }
  IIC2_Stop();
  return DEF_OK;
}
