#include "motor.h"
#include "global.h"

/*
*****************************************************************************
*                           Motor_SetPWM()
* Description : 
*****************************************************************************
*/
void Motor_SetPWM(CTRL_DataType *control_data, MOTOR_DataType *motor)
{
	float w1, w2, w3, w4; /* rotation speed */

	w1 = Constraint_f(control_data->Thrust + control_data->Mx - control_data->My - control_data->Mz, 0, 1);
	w2 = Constraint_f(control_data->Thrust + control_data->Mx + control_data->My + control_data->Mz, 0, 1);
	w3 = Constraint_f(control_data->Thrust - control_data->Mx + control_data->My - control_data->Mz, 0, 1);
	w4 = Constraint_f(control_data->Thrust - control_data->Mx - control_data->My + control_data->Mz, 0, 1);

	motor->motor1 = (uint16_t)(w1 * PWM_WIDTH + PWM_LOWEST + 0.5f);
	motor->motor2 = (uint16_t)(w2 * PWM_WIDTH + PWM_LOWEST + 0.5f);
	motor->motor3 = (uint16_t)(w3 * PWM_WIDTH + PWM_LOWEST + 0.5f);
	motor->motor4 = (uint16_t)(w4 * PWM_WIDTH + PWM_LOWEST + 0.5f);

	//Motor_Smooth(motor);
	Motor_Constraint(motor);
	BSP_PWM_Set(motor->motor1, motor->motor2, motor->motor3, motor->motor4);
}

/*
*****************************************************************************
*                        Motor_MinThrust()
* Description : 
*****************************************************************************
*/
void Motor_MinThrust(MOTOR_DataType *motor)
{
	motor->motor1 = PWM_LOWEST;
	motor->motor2 = PWM_LOWEST;
	motor->motor3 = PWM_LOWEST;
	motor->motor4 = PWM_LOWEST;

	BSP_PWM_Set(motor->motor1, motor->motor2, motor->motor3, motor->motor4);
}

/*
*****************************************************************************
*                        Motor_MaxThrust()
* Description : 
*****************************************************************************
*/
void Motor_MaxThrust(MOTOR_DataType *motor)
{
	motor->motor1 = PWM_HIGHEST;
	motor->motor2 = PWM_HIGHEST;
	motor->motor3 = PWM_HIGHEST;
	motor->motor4 = PWM_HIGHEST;

	BSP_PWM_Set(motor->motor1, motor->motor2, motor->motor3, motor->motor4);
}

/*
*****************************************************************************
*                        MotorConstraint()
* Description : 
*****************************************************************************
*/
void Motor_Constraint(MOTOR_DataType *motor)
{
	motor->motor1 = (uint16_t)Constraint_int(motor->motor1, PWM_LOWEST, PWM_HIGHEST);
	motor->motor2 = (uint16_t)Constraint_int(motor->motor2, PWM_LOWEST, PWM_HIGHEST);
	motor->motor3 = (uint16_t)Constraint_int(motor->motor3, PWM_LOWEST, PWM_HIGHEST);
	motor->motor4 = (uint16_t)Constraint_int(motor->motor4, PWM_LOWEST, PWM_HIGHEST);
}

/*
****************************************************************************
*                            MotorSmooth()
* Description : compensate the motor mechanics effect
****************************************************************************
*/
void Motor_Smooth(MOTOR_DataType *motor)
{
	static uint16_t last_motor1 = PWM_LOWEST;
	static uint16_t last_motor2 = PWM_LOWEST;
	static uint16_t last_motor3 = PWM_LOWEST;
	static uint16_t last_motor4 = PWM_LOWEST;

	if ((motor->motor1) > last_motor1)
		motor->motor1 = (last_motor1 + 1 * (motor->motor1)) / 2;
	else
		motor->motor1 = (motor->motor1) - (last_motor1 - (motor->motor1)) * 1;

	if ((motor->motor2) > last_motor2)
		motor->motor2 = (last_motor2 + 1 * (motor->motor2)) / 2;
	else
		motor->motor2 = (motor->motor2) - (last_motor2 - (motor->motor2)) * 1;

	if ((motor->motor3) > last_motor3)
		motor->motor3 = (last_motor3 + 1 * (motor->motor3)) / 2;
	else
		motor->motor3 = (motor->motor3) - (last_motor3 - (motor->motor3)) * 1;

	if ((motor->motor4) > last_motor4)
		motor->motor4 = (last_motor4 + 1 * (motor->motor4)) / 2;
	else
		motor->motor4 = (motor->motor4) - (last_motor4 - (motor->motor4)) * 1;

	last_motor1 = (motor->motor1);
	last_motor2 = (motor->motor2);
	last_motor3 = (motor->motor3);
	last_motor4 = (motor->motor4);
}
