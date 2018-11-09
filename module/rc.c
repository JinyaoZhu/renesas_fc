#include "bsp.h"
#include "rc.h"
#include "global.h"

#define MAX_PITCH_RAD (20 * DEG2RAD)
#define MAX_ROLL_RAD (20 * DEG2RAD)

extern volatile uint16_t ch1_width;
extern volatile uint16_t ch2_width;
extern volatile uint16_t ch3_width;
extern volatile uint16_t ch4_width;
extern volatile uint16_t ch5_width;
extern volatile uint16_t ch6_width;

uint8_t rc_GetCommand(RC_DataType *p)
{
  uint8_t status;

  if (1)
  {
    p->ch1 = Constraint_f(((int16_t)ch1_width - 1999) / 1999.0f - 0.01f, 0, 1);
    p->ch2 = Constraint_f(((int16_t)ch2_width - 1999) / 1999.0f - 0.01f, 0, 1);
    p->ch3 = Constraint_f(((int16_t)ch3_width - 1999) / 1999.0f - 0.01f, 0, 1);
    p->ch4 = Constraint_f(((int16_t)ch4_width - 1999) / 1999.0f - 0.01f, 0, 1);
    p->ch5 = Constraint_f(((int16_t)ch5_width - 1999) / 1999.0f - 0.01f, 0, 1);
    p->ch6 = Constraint_f(((int16_t)ch6_width - 1999) / 1999.0f - 0.01f, 0, 1);

    p->thrust = p->ch3;
    p->yaw = p->ch4 - 0.5f;
    p->pitch = (p->ch2 - 0.5f);
    p->roll = (p->ch1 - 0.5f);
    p->aux1 = p->ch5;
    p->aux2 = p->ch6;
  }
  else
  {
    p->yaw = 0;
    p->pitch = 0;
    p->roll = 0;
  }

  return status;
}

/*
******************************************************************
*                     rc_GetTarget()
* Description : Convert rc channel data to target quaternion and euler
******************************************************************
*/
void rc_GetTargetEuler(RC_DataType *p, EULER *target_e)
{
  float roll_in, pitch_in, yaw_in;
  float thrust_in;

  thrust_in = p->thrust;
  yaw_in = p->yaw;

  /* get target roll and pitch */
  pitch_in = (p->pitch) * MAX_PITCH_RAD * 2;
  roll_in = (p->roll) * MAX_ROLL_RAD * 2;

  if ((fabs(yaw_in) > 0.05f) && (thrust_in > 0.20f))
  {
    target_e->yaw = WarpToPI(target_e->yaw - yaw_in * 0.10f);
  }

  target_e->pitch = pitch_in;
  target_e->roll = roll_in;
}
