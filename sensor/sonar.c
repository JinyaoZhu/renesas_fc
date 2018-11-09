#include "sonar.h"
#include "algorithm.h"

extern volatile uint16_t sonar_get_flag;
extern volatile uint16_t sonar_width;


void Sonar_Delay10us(void)
{
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
}

float Sonar_GetDist(void)
{
  uint16_t time_out_cnt = 0;
  uint8_t time_out_flag = 0;
//  static float d_k1 = 0;
//  static float d_k2 = 0;
//  static float d_k3 = 0;
  static float distance = 0;
//  static uint16_t buf[5] = {0};
  
  while(!sonar_get_flag){
    time_out_cnt ++;
    if(time_out_cnt > 10000){
      time_out_flag = 1;/* time out */
      break;
    }
  }
  
  sonar_get_flag = 0; /* clear flag */
  /* tigger */
  SONAR_TRIG_PIN = HIGH;
  Sonar_Delay10us();
  SONAR_TRIG_PIN = LOW;
  
  if(time_out_flag == 1)
    return distance;
  else
     distance = 340.0*sonar_width*0.0000005f/2.0f; /* unit: m */
      
//  distance = 325.0*median_5(buf)*0.0000005f/2.0f; /* unit: m */
  
//  buf[0] = buf[1];
//  buf[1] = buf[2];
//  buf[2] = buf[3];
//  buf[3] = buf[4]; 
   /* Median Filter */
//  if(d_k1 <= d_k2){
//    if(d_k2 <= d_k3)
//      distance = d_k2;
//    else if(d_k1 < d_k3)
//      distance = d_k3;
//    else
//      distance = d_k1;
//  }
//  else{
//    if(d_k1 <= d_k3)
//      distance = d_k1;
//    else if(d_k2 < d_k3)
//      distance = d_k3;
//    else
//      distance = d_k2;
//  }
//  d_k1 = d_k2;
//  d_k2 = d_k3;
  return distance;
}