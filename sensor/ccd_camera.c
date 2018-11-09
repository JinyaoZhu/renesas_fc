#include "ccd_camera.h"
#include "global.h"

#define PIXEL_DATA_LEN            (128)
#define PIXEL_DATA_FILTERED_LEN   (124)

uint8_t PixelData[PIXEL_DATA_LEN] = {0};

uint8_t PixelDataFiltered[PIXEL_DATA_FILTERED_LEN] = {0};

uint8_t PixelMaxValue,PixelMinValue;

extern volatile uint16_t adc_get_flag;
extern volatile uint16_t adc_ch1_value;

/* exposure time (ms) */
float IntegrationTime = 0.001;/* unit : second */

#define SamplingDelay()   {NOP();NOP();NOP();NOP();NOP();}

void ccd_init(void)
{
  TSL1401_CLK = 0;    //set output
  TSL1401_SI  = 0;    //set output
  TSL1401_SI  = 0;
  TSL1401_CLK = 0;
}

uint16_t PixelAverage(uint8_t len, uint8_t *data) {
  uint16_t i;
  uint16_t sum = 0;
  for (i = 0; i < len; i++) {
    sum = sum + *data++;
  }
  return (sum / len);
}

/* calculate adaptive exposure time */
float CalculateIntegrationTime(void)
{

  /* 128 pixels' average voltage x10 */
  int16_t PixelAverageVoltage;
  /* set target pixel voltage x10 */
  const int16_t TargetPixelAverageVoltage = 30;
  /* voltage error x10 */
  int16_t PixelAverageVoltageError = 0;
  /* allowed error x10 */
  const int16_t TargetPixelAverageVoltageAllowError = 2;

  PixelAverageVoltage = (int16_t)PixelAverage(PIXEL_DATA_LEN, PixelData) * 25 / 128;
  PixelAverageVoltageError = TargetPixelAverageVoltage - PixelAverageVoltage;

  if (PixelAverageVoltageError < -TargetPixelAverageVoltageAllowError)
    IntegrationTime -= 0.0002;

  if (PixelAverageVoltageError > TargetPixelAverageVoltageAllowError)
    IntegrationTime += 0.0002;

  if (IntegrationTime <= 0.0001)
    IntegrationTime = 0.0001;

  if (IntegrationTime >= 0.020)
    IntegrationTime = 0.020;

  return   IntegrationTime;
}

/*得到128个pixel数据，0--255*/
void ImageCapture(void)
{
  uint8_t i;
  uint16_t  temp_int;

  TSL1401_SI = 0;         /* SI  = 0 */
  TSL1401_SI = 1;         /* SI  = 1 */
  SamplingDelay();
  TSL1401_CLK = 1;        /* CLK = 1 */
  SamplingDelay();
  TSL1401_SI = 0;         /* SI  = 0 */
  SamplingDelay();
  TSL1401_CLK = 0;    /* CLK = 0 */
  //Delay 20us for sample the first pixel
//  for (i = 0; i < 50; i++) {      //少延时，pixel数据更加平滑
//    NOP(); NOP(); NOP(); NOP();
//    NOP(); NOP(); NOP(); NOP();
//    NOP(); NOP(); NOP(); NOP();
//    NOP(); NOP(); NOP(); NOP();
//  }
  //Sampling Pixel 1
  adc_get_flag = 0;
  R_ADC_Start_Conversion();
  while (!adc_get_flag);
  PixelData[0] = (uint8_t)(adc_ch1_value>>2);
  for (i = 0; i < (PIXEL_DATA_LEN-1); i++) {
    SamplingDelay();
    TSL1401_CLK = 1;    /* CLK = 1 */
    SamplingDelay();
    TSL1401_CLK = 0;    /* CLK = 0 */
    //Sampling Pixel 2~128
    adc_get_flag = 0;
    R_ADC_Start_Conversion();
    while (!adc_get_flag);
    PixelData[i+1] = (uint8_t)(adc_ch1_value>>2);
  }

  SamplingDelay();
  TSL1401_CLK = 1;        /* CLK = 1 */
  SamplingDelay();
  TSL1401_CLK = 0;        /* CLK = 0 */
}

/*开始曝光（积分）*/
void StartIntegration(void)
{
  uint8_t i;

  TSL1401_SI = 1;         /* SI  = 1 */
  SamplingDelay();
  TSL1401_CLK = 1;        /* CLK = 1 */
  SamplingDelay();
  TSL1401_SI = 0;         /* SI  = 0 */
  SamplingDelay();
  TSL1401_CLK = 0;        /* CLK = 0 */

  for (i = 0; i < (PIXEL_DATA_LEN-1); i++) {
    SamplingDelay();
    SamplingDelay();
    TSL1401_CLK = 1;    /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    TSL1401_CLK = 0;    /* CLK = 0 */
  }
  SamplingDelay();
  SamplingDelay();
  TSL1401_CLK = 1;        /* CLK = 1 */
  SamplingDelay();
  SamplingDelay();
  TSL1401_CLK = 0;        /* CLK = 0 */
}

/*转化并发送HEX数 */
//void SendHex(uint8_t hex) {
//  uint8_t temp;
//  uint8_t out;
//  temp = hex >> 4;
//  if(temp < 10) {
//  out = temp + '0';
//  } else {
//    out = temp - 10 + 'A';
//  }g_uart0_tx_done = 0;
//  R_UART0_Send(&out,1);
//  while(!g_uart0_tx_done);
//  temp = hex & 0x0F;
//  if(temp < 10) {
//    out = temp + '0';
//  } else {
//    out = temp - 10 + 'A';
//  }g_uart0_tx_done = 0;
//  R_UART0_Send(&out,1);
//  while(!g_uart0_tx_done);
//}
//
//
//void print_pixel()
//{
//  volatile uint8_t i = 0;
//  static char lines[11] = "*LD00000000";
//  g_uart0_tx_done = 0;
//  R_UART0_Send(lines,11);
//  while(!g_uart0_tx_done);
//  //R_printf("*LD0000");
//  for(i=0;i<128;i++)
//    SendHex(PixelData[i]);
//    //R_printf("%x",PixelData[i]);
//
//  i = '#';
//  g_uart0_tx_done = 0;
//  R_UART0_Send(&i,1);
//  while(!g_uart0_tx_done);
//}

void imageFilter(void)
{
  uint16_t i;
  uint16_t sum;
  for (i = 2; i <= (PIXEL_DATA_LEN - 3); i++) {
    sum = PixelData[i-2]+PixelData[i-1]+PixelData[i]+PixelData[i+1]+PixelData[i+2];
    PixelDataFiltered[i-2] = (uint8_t)(sum / 5);
  }
}

void imageFindMinMax(void)
{
  uint16_t i;
  PixelMaxValue = 0;
  PixelMinValue = 255;
  for(i = 0; i<PIXEL_DATA_FILTERED_LEN; i++){
	  
    if(PixelDataFiltered[i]>PixelMaxValue)
      PixelMaxValue = PixelDataFiltered[i];
    else if(PixelDataFiltered[i]<PixelMinValue)
      PixelMinValue = PixelDataFiltered[i];
  }
}

void imageBin(void)
{
  uint16_t i;
  float c = (float)(PixelMinValue + PixelMaxValue)/2 ;
  
  for (i = 0; i < PIXEL_DATA_FILTERED_LEN; i++) {
    if (PixelDataFiltered[i] < c)
      PixelDataFiltered[i] = 0;
    else
      PixelDataFiltered[i] = 255;
  }
}


int16_t findPosition(void)
{
  uint16_t i;
  uint16_t start_index = 0;
  uint16_t end_index = 0;
  uint16_t start_flag = 0;
  uint16_t trigger_cnt = 0;

  for (i = 0; i < PIXEL_DATA_FILTERED_LEN; i++) {
	  
    if (start_flag == 0) {
       if (PixelDataFiltered[i] == 0)
         start_flag = 1;
    }
    else{
      if (PixelDataFiltered[i] == 0){
        if(trigger_cnt >= 3)
          start_index = i-3;
	trigger_cnt++;
      }
      else{
	if(trigger_cnt < 3){
          trigger_cnt = 0;
	  start_flag = 0;
	}
	else{
	  end_index = i;
	  break;
	}
      }
    }
  }
  return (start_index + end_index)/2 - (PIXEL_DATA_FILTERED_LEN/2);
}

int16_t find_black_line()
{
  static int16_t pos_center = 128;
  int16_t pos_down, pos_up;
  int16_t i, j, gate;
  for (gate = 35; gate > 10; gate--) {
    for (i = 0; i < 128; i++) {
      if (*(PixelData + i) - * (PixelData + i + 2) > gate) {
        pos_down = i;
        for (j = i; j < 128; j++) {
          if (*(PixelData + i) - * (PixelData + i + 2) > gate)
            pos_down = i;
          if (*(PixelData + j + 2) - * (PixelData + j) > gate) {
            pos_up = j;
            pos_center = (pos_up + pos_down) / 2 + 1;
            return pos_center;
          }
        }
      }
    }
  }
  return pos_center;
}
