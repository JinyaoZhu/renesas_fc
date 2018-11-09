#include "bsp.h"
#include "control.h"
#include "motor.h"
#include "global.h"

#define ERROR_RATE_I_LIMIT (20.0f)

//static VECTOR KpAngle = {10, 10, 16};
//static VECTOR KpRate  = {0.025, 0.025, 0.08};
//static VECTOR KiRate  = {0.035, 0.035, 0.08};
//static VECTOR KdRate  = {0.0025, 0.0025, 0.00};

//const VECTOR KpAngle = {10, 10, 12};
//
//const VECTOR KpRate  = {0.026, 0.026,0.12};
//const VECTOR KiRate  = {0.0025, 0.0025, 0.0025};
//const VECTOR KdRate  = {0.0032, 0.0032, 0.00};

static VECTOR KpAngle = {11.5, 11.5, 15};

static VECTOR KpRate = {0.026, 0.026, 0.12};
static VECTOR KiRate = {0.025, 0.025, 0.004};
static VECTOR KdRate = {0.0026, 0.0026, 0.0040};

void CTRL_SetKpAngle(float param)
{
}

void CTRL_SetKpRate(float param)
{
}

/*
**************************************************
*               CTRL_PIDRegulator()
* Description : A PID regulator using quaternion
* Parameter   : q    --- attitude quaternion
*               gyro --- gyroscope rotate vector
*               m    --- vector of torqe
*               mode --- PID regulator on/off
**************************************************
*/
void CTRL_PIDRegulator(const EULER euler, const EULER euler_target, const VECTOR gyro,
                       const float Thrust, CTRL_DataType *control_data, const float dt, const uint8_t mode)
{
  VECTOR ErrorRate;
  VECTOR ErrorAngle;

  float thrust_compensation;

  static VECTOR LastErrorRate = {0};
  static VECTOR P_ErrorRate = {0};
  static VECTOR D_ErrorRate = {0};
  static VECTOR I_ErrorRate = {0};

  float ax, ay, az;
  float cos_a_bg = 0; /* rotation angle along rotation axis in horizonal plane */
  float Mx, My, Mz;
  float Fz = 0;

  cos_a_bg = g_AccEstBody.z * InvSqrt(g_AccEstBody.x * g_AccEstBody.x + g_AccEstBody.y * g_AccEstBody.y + g_AccEstBody.z * g_AccEstBody.z);

  //cos_a_bg = cos(euler.pitch)*cos(euler.roll);

  /* when a_bg is smaller than 60 degree */
  /* Thrust compensate .  Notice: too big Thrust will make output saturated*/
  if ((cos_a_bg > 0.5f) && (mode != 0))
    Fz = Thrust / cos_a_bg;
  else
    Fz = Thrust;

  ax = WarpToPI(euler_target.roll - euler.roll);
  ay = WarpToPI(euler_target.pitch - euler.pitch);
  az = WarpToPI(euler_target.yaw - euler.yaw);

  Constraint_f(Fz, 0, 0.8f);

  /* avoid gimbal lock */
  if (fabs(ay) > 60.0f * DEG2RAD)
  {
    ax = 0;
    az = 0;
  }

  /*************************PID Controller***************************************/
  /* Angle Controll */
  ErrorAngle.x = Constraint_f(ax, -45.0f * DEG2RAD, +45.0f * DEG2RAD);
  ErrorAngle.y = Constraint_f(ay, -45.0f * DEG2RAD, +45.0f * DEG2RAD);
  ErrorAngle.z = Constraint_f(az, -45.0f * DEG2RAD, +45.0f * DEG2RAD);

  /* Rate Controll */
  ErrorRate.x = KpAngle.x * ErrorAngle.x - gyro.x;
  ErrorRate.y = KpAngle.y * ErrorAngle.y - gyro.y;
  ErrorRate.z = KpAngle.z * ErrorAngle.z - gyro.z;

  P_ErrorRate.x = DLPF(ErrorRate.x, P_ErrorRate.x, 15.0f * HZ2RAD, dt);
  P_ErrorRate.y = DLPF(ErrorRate.y, P_ErrorRate.y, 15.0f * HZ2RAD, dt);
  P_ErrorRate.z = DLPF(ErrorRate.z, P_ErrorRate.z, 15.0f * HZ2RAD, dt);

  D_ErrorRate.x = DLPF((ErrorRate.x - LastErrorRate.x) / dt, D_ErrorRate.x, 15.0f * HZ2RAD, dt);
  D_ErrorRate.y = DLPF((ErrorRate.y - LastErrorRate.y) / dt, D_ErrorRate.y, 15.0f * HZ2RAD, dt);
  D_ErrorRate.z = DLPF((ErrorRate.z - LastErrorRate.z) / dt, D_ErrorRate.z, 15.0f * HZ2RAD, dt);
  LastErrorRate.x = ErrorRate.x;
  LastErrorRate.y = ErrorRate.y;
  LastErrorRate.z = ErrorRate.z;

  I_ErrorRate.x = Constraint_f(I_ErrorRate.x + dt * ErrorRate.x, -ERROR_RATE_I_LIMIT, ERROR_RATE_I_LIMIT);
  I_ErrorRate.y = Constraint_f(I_ErrorRate.y + dt * ErrorRate.y, -ERROR_RATE_I_LIMIT, ERROR_RATE_I_LIMIT);
  I_ErrorRate.z = Constraint_f(I_ErrorRate.z + dt * ErrorRate.z, -ERROR_RATE_I_LIMIT, ERROR_RATE_I_LIMIT);

  Mx = KpRate.x * P_ErrorRate.x + KiRate.x * I_ErrorRate.x + KdRate.x * D_ErrorRate.x;
  My = KpRate.y * P_ErrorRate.y + KiRate.y * I_ErrorRate.y + KdRate.y * D_ErrorRate.y;
  Mz = KpRate.z * P_ErrorRate.z + KiRate.z * I_ErrorRate.z + KdRate.z * D_ErrorRate.z;

  /***********************************************************************************/

  if (mode == 1) /* Enable PID */
  {
    control_data->Thrust = Fz;
    control_data->Mx = Mx;
    control_data->My = My;
    control_data->Mz = Mz;
  }
  else /* Dsiable PID */
  {
    I_ErrorRate.x = I_ErrorRate.y = I_ErrorRate.z = 0;
    control_data->Thrust = Fz;
    control_data->Mx = 0;
    control_data->My = 0;
    control_data->Mz = 0;
  }
}
