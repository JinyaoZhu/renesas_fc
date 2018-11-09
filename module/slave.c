#include "slave.h"

extern volatile uint16_t g_csi20_rx_end;

uint8_t xorhash(uint8_t * data, uint16_t datalen)
{
  uint8_t tmp = 0;
  while (datalen--)
    tmp ^= *data++;
  if (tmp == 0)
    return DEF_OK;
  else
    return DEF_FAIL;
}

/* Get slave Renesas MCU data */
uint8_t Slave_GetData(SLAVE_DTAT_TYPE* p)
{
  uint8_t rx_buf[20];
  union {
    float f;
    uint8_t b[4];
  } sonar;
  g_csi20_rx_end = 0;
  P5.5 = 0; /* CSI20 Chip select */
  R_CSI20_Receive(rx_buf, 12);
  while (!g_csi20_rx_end);

  if (xorhash(rx_buf, 12) == DEF_OK) {
    p->ly    =  rx_buf[0];
    p->ly2   =  rx_buf[1];
    p->lx   =  rx_buf[2];
    p->lx2  =  rx_buf[3];
    p->by    =  rx_buf[4];
    p->bx    =  rx_buf[5];
    p->qual  =  rx_buf[6];
    sonar.b[0] = rx_buf[7];
    sonar.b[1] = rx_buf[8];
    sonar.b[2] = rx_buf[9];
    sonar.b[3] = rx_buf[10];
    p->sonar = sonar.f;
    return DEF_OK;
  }
  else {
    g_csi20_rx_end = 0;
    P5.5 = 0; /* CSI20 Chip select */
    R_CSI20_Receive(rx_buf, 12);
    while (!g_csi20_rx_end);
    if (xorhash(rx_buf, 12) == DEF_OK) {
      p->ly    =  rx_buf[0];
      p->ly2   =  rx_buf[1];
      p->lx   =  rx_buf[2];
      p->lx2  =  rx_buf[3];
      p->by    =  rx_buf[4];
      p->bx    =  rx_buf[5];
      p->qual  =  rx_buf[6];
      sonar.b[0] = rx_buf[7];
      sonar.b[1] = rx_buf[8];
      sonar.b[2] = rx_buf[9];
      sonar.b[3] = rx_buf[10];
      p->sonar = sonar.f;
      return DEF_OK;
    }
    else
      return DEF_FAIL;
  }
}