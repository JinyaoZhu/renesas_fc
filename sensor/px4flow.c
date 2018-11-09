#include "px4flow.h"
#include "global.h"



uint8_t PX4_FLOW_FrameUpdate(PX4_FLOW_I2C_FRAME *p)
{
  uint8_t status;
  uint8_t frame_buf[22] = {0};
  status = IIC2_Read(PX4_FLOW_I2C_ADDR<<1,0x00,frame_buf,22);
  p->frame_count = (frame_buf[1]<<8)+frame_buf[0];
  p->pixel_flow_x_sum = (frame_buf[3]<<8)+frame_buf[2];
  p->pixel_flow_y_sum = (frame_buf[5]<<8)+frame_buf[4];
  p->flow_comp_m_x = (frame_buf[7]<<8)+frame_buf[6];
  p->flow_comp_m_y = (frame_buf[9]<<8)+frame_buf[8];
  p->qual = (frame_buf[11]<<8)+frame_buf[10];
  p->gyro_x_rate = (frame_buf[13]<<8)+frame_buf[12];
  p->gyro_y_rate = (frame_buf[15]<<8)+frame_buf[14];
  p->gyro_z_rate = (frame_buf[17]<<8)+frame_buf[16];
  p->gyro_range = frame_buf[18];
  p->sonar_timestamp = frame_buf[19];
  p->ground_distance = (frame_buf[21]<<8)+frame_buf[20];
  return status;
}








