#ifndef _CCD_CAMERA_H
#define _CCD_CAMERA_H

#include "bsp.h"

#define TSL1401_CLK      		P2.5  	/*P2_bit.no1*/
#define TSL1401_SI       		P2.6  	/*P2_bt.no2*/
#define TSL1401_CLK_DDR                 PM2.5  	/*PM2_bit.no1*/
#define TSL1401_SI_DDR                  PM2.6 	/*PM2_bit.no2*/



void ccd_init(void);
void ImageCapture(void);
float CalculateIntegrationTime(void);
void StartIntegration(void) ;
void print_pixel(void);
int16_t find_black_line(void);
void imageFilter(void);
void imageBin(void);
int16_t findPosition(void);
void imageFindMinMax(void);

#endif /* _CCD_CAMERA_H */
