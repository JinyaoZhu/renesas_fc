#include "ir.h"

extern volatile uint16_t adc_get_flag;
extern volatile uint16_t adc_ch1_value;
extern volatile uint16_t adc_ch2_value;
extern volatile uint16_t adc_ch3_value;
extern volatile uint16_t adc_ch4_value;


void IR_GetDistance(float *ir1,float *ir2)
{  
  static const float conv_to_voltage = 5.0f/1023;
  static float d1 = 0;
  static float d2 = 0;
  adc_get_flag = 0;
  R_ADC_Start();
  while (!adc_get_flag);
  //d1 = 5.00f*(float)adc_ch2_value/1023;
  //d2 = 5.00f*(float)adc_ch3_value/1023;
  if(adc_ch2_value > 0)
    *ir1 = 0.6f/(conv_to_voltage*(float)adc_ch2_value);
  if(adc_ch3_value > 0)
    *ir2 = 0.6f/(conv_to_voltage*(float)adc_ch3_value);
}