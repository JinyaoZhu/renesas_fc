#ifndef __RC_H__
#define __RC_H__

#include "algorithm.h"

typedef struct
{
	float ch1; /* yaw */
	float ch2; /* thrust */
	float ch3; /* pitch */
	float ch4; /* roll */
	float ch5;
	float ch6;

	float thrust;
	float yaw;
	float pitch;
	float roll;
	float aux1;
	float aux2;
} RC_DataType;

uint8_t rc_GetCommand(RC_DataType *p);
void rc_GetTargetEuler(RC_DataType *p, EULER *target_e);

#endif /* __RC_H__ */
