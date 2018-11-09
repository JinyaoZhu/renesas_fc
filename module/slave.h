#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "bsp.h"

typedef struct{
  uint8_t bx;
  uint8_t by;
  uint8_t ly;
  uint8_t ly2;
  uint8_t lx;
  uint8_t lx2;
  uint8_t qual;
  float   sonar;
}SLAVE_DTAT_TYPE;


uint8_t Slave_GetData(SLAVE_DTAT_TYPE* p);



#endif /* __SLAVE_H__ */