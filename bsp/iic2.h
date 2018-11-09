#ifndef __IIC2_H__
#define __IIC2_H__

#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"

#define IIC2_SDA_PIN               P7.3
#define IIC2_SDA_PIN_INPUT         PM7.3
#define IIC2_SCL_PIN               P7.2
#define IIC2_SCL_PIN_INPUT         PM7.2

void    IIC2_Init(void);
uint8_t IIC2_Read(uint8_t  addr,uint8_t reg,uint8_t *rx_buf,uint16_t len);
uint8_t IIC2_Write(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len);

#endif /* IIC2_H */
