#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "algorithm.h"

typedef struct
{
	float Thrust;
	float Mx;
	float My;
	float Mz;
} CTRL_DataType;

void CTRL_SetKpAngle(float param);

void CTRL_SetKpRate(float param);

void CTRL_PIDRegulator(const EULER euler, const EULER euler_target, const VECTOR gyro,
					   const float Thrust, CTRL_DataType *control_data, const float dt, const uint8_t mode);

#endif /* __CONTROL_H__ */
