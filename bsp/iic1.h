#ifndef __IIC1_H__
#define __IIC1_H__

#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"

#define IIC1_SDA_PIN               P7.1
#define IIC1_SDA_PIN_INPUT         PM7.1
#define IIC1_SCL_PIN               P7.0
#define IIC1_SCL_PIN_INPUT         PM7.0

void    IIC1_Init(void);
uint8_t IIC1_Read(uint8_t  addr,uint8_t reg,uint8_t *rx_buf,uint16_t len);
uint8_t IIC1_Write(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len);

#endif /* IIC1_H */
