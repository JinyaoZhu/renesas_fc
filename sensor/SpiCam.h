#ifndef __SPI_CAM_H__
#define __SPI_CAM_H__

#include "bsp.h"

#define ANDS3080_CS_PIN          P5.5



MD_STATUS SpiCam_SendRecevied(uint8_t * const tx_buf, uint16_t tx_num, uint8_t * const rx_buf);

void  pushpx(int val);
void  resetpx(void);

unsigned char isblock(unsigned char x, unsigned char y);
void findblock(void);
void findline(void);
uint8_t GetProductID(void);
void FrameCapture(void);

void SpiCam_Update(void);

unsigned char SpiCam_GetBx(void);
unsigned char SpiCam_GetQual(void);
unsigned char SpiCam_GetLy(void);

#endif /* __SPI_CAM_H__ */