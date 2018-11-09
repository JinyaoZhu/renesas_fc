#include "cordic.h"
#include "math.h"

#define INV_2PI (0.159154943092f)
#define _2PI (6.2831853f)

#define QUAD1 (1)
#define QUAD2 (2)
#define QUAD3 (3)
#define QUAD4 (4)

#define NBITS (12) /* should below 15 */

static int16_t ArcTan[NBITS] = {0}; /* Rad * CordicBase */
static int16_t xInit;
static uint16_t CordicBase;
static float ConvertToReal; /* converte sin and cos value to virtual unit */
static float ConvertToRad;
static float ConvertToCAU; /* convert rad to Cordic angle unit */
static float MaxCAU;       /* Max Cordic angle unit */
static uint16_t Quad2Bondary;
static uint16_t Quad3Bondary;
static uint16_t Quad4Bondary;

/*
********************************************************
*                  CORDIC_Init
* Description :
********************************************************
*/

void CORDIC_Init(void)
{
  int16_t i; /* to index ArcTan[] */
  double f;  /* to calc initial x projection */
  long powr; /* powers of 2 up to 2^(2*(NBITS - 1)) */

  CordicBase = (uint16_t)1u << NBITS;
  Quad2Bondary = CordicBase << 1u;
  Quad3Bondary = Quad2Bondary + CordicBase;
  Quad4Bondary = Quad3Bondary + CordicBase;

  ConvertToReal = 1.0f / CordicBase;
  MaxCAU = CordicBase << 2u;
  ConvertToRad = PI / ((float)CordicBase * 2.0f);
  ConvertToCAU = MaxCAU / (2.0f * PI);
  powr = 1;
  for (i = 0; i < NBITS; i++)
  {
    ArcTan[i] = (int16_t)(atan(1.0f / powr) / (PI / 2.0f) * CordicBase + 0.5f);
    powr <<= 1u;
  }

  f = 1.0f;
  powr = 1;
  for (i = 0; i < NBITS; i++)
  {
    f = (f * (powr + 1)) / powr;
    powr <<= 2u;
  }
  f = 1.0f / sqrt(f);
  xInit = (int16_t)(CordicBase * f + 0.5f);
}

/*
**************************************************************
*                     CORDIC_SinCos
* Description :
**************************************************************
*/
void CORDIC_SinCos(uint16_t theta, int16_t *sin, int16_t *cos)
{
  int16_t quadrant; /* quadrant of incoming angle */
  int16_t z;        /* incoming angle move to 1st quadrant */
  uint16_t i;       /* to index rotation */
  int16_t x, y;     /* projections onto axes */
  int16_t x1, y1;   /* peojections of rotated vector */

  if (theta < CordicBase)
  {
    quadrant = QUAD1;
    z = (int16_t)theta;
  }
  else if (theta < Quad2Bondary)
  {
    quadrant = QUAD2;
    z = (int16_t)(Quad2Bondary - theta);
  }
  else if (theta < Quad3Bondary)
  {
    quadrant = QUAD3;
    z = (int16_t)(theta - Quad2Bondary);
  }
  else
  {
    quadrant = QUAD4;
    z = (int16_t)(Quad4Bondary - theta);
  }

  x = xInit;
  y = 0;

  /* Negate z, so same rotations taking angle z to  0 */
  z = -z;

  for (i = 0; i < NBITS; i++)
  {
    if (z < 0)
    {
      z += ArcTan[i];
      y1 = y + (x >> i);
      x1 = x - (y >> i);
    }
    else
    {
      z -= ArcTan[i];
      y1 = y - (x >> i);
      x1 = x + (y >> i);
    }

    x = x1;
    y = y1;
  }

  *cos = (quadrant == QUAD1 || quadrant == QUAD4) ? x : -x;
  *sin = (quadrant == QUAD1 || quadrant == QUAD2) ? y : -y;
}

/*
*****************************************************
*                  CORDIC_Atan2()
* Description : +- 0.000001 rad
*****************************************************
*/
float CORDIC_Atan2(float y_val, float x_val)
{
  int16_t quadrant = QUAD1;
  int16_t x = 0;
  int16_t y = 0;
  int16_t i = 0;
  int16_t x1, y1;
  int16_t angleSum = 0;

  if (x_val == 0.0f)
    x_val += 0.000000000001f;
  if (y_val == 0.0f)
    y_val += 0.000000000001f;

  x_val *= 256.0f;
  y_val *= 256.0f;

  if ((x_val > 0.0f) && (y_val >= 0.0f))
  {
    x = (int16_t)(x_val + 0.5f);
    y = (int16_t)(y_val + 0.5f);
    quadrant = QUAD1;
  }
  else if ((x_val <= 0.0f) && (y_val > 0.0f))
  {
    x = (int16_t)(-x_val + 0.5f);
    y = (int16_t)(y_val + 0.5f);
    quadrant = QUAD2;
  }
  else if ((x_val < 0.0f) && (y_val <= 0.0f))
  {
    x = (int16_t)(-x_val + 0.5f);
    y = (int16_t)(-y_val + 0.5f);
    quadrant = QUAD3;
  }
  else
  {
    x = (int16_t)(x_val + 0.5f);
    y = (int16_t)(-y_val + 0.5f);
    quadrant = QUAD4;
  }

  for (i = 0; i < NBITS; i++)
  {
    if (y > 0)
    {
      x1 = x + (y >> i);
      y1 = y - (x >> i);
      angleSum += ArcTan[i];
    }
    else
    {
      x1 = x - (y >> i);
      y1 = y + (x >> i);
      angleSum -= ArcTan[i];
    }

    x = x1;
    y = y1;
  }
  switch (quadrant)
  {
  case QUAD1:
    break;
  case QUAD2:
    angleSum = (int16_t)(0.5f * MaxCAU) - angleSum;
    break;
  case QUAD3:
    angleSum = -(int16_t)(0.5f * MaxCAU) + angleSum;
    break;
  case QUAD4:
    angleSum = -angleSum;
    break;
  default:
    break;
  }
  return (angleSum * ConvertToRad);
}

/*
************************************************
*               fast_sin()
* Description :  apprximation
************************************************
*/

float fast_sin(float x)
{
  int16_t cos, sin;
  uint16_t theta;

  while (x < 0.0f)
  {
    x += _2PI;
  }

  while (x > _2PI)
  {
    x -= _2PI;
  }

  theta = (uint16_t)(x * ConvertToCAU);

  LED = HIGH;
  CORDIC_SinCos(theta, &sin, &cos);
  LED = LOW;
  return (float)(sin * ConvertToReal);
}

/*
************************************************
*               fast_cos()
* Description :  apprximation
************************************************
*/
float fast_cos(float x)
{
  int16_t cos, sin;
  uint16_t theta;

  while (x < 0.0f)
  {
    x += _2PI;
  }

  while (x > _2PI)
  {
    x -= _2PI;
  }

  theta = (uint16_t)(x * ConvertToCAU + 0.5f);

  CORDIC_SinCos(theta, &sin, &cos);

  return (float)(cos * ConvertToReal);
}

void fast_SinCos(float x, float *sin, float *cos)
{
  int16_t cos_tmp, sin_tmp;
  uint16_t theta;

  while (x < 0.0f)
  {
    x += _2PI;
  }

  while (x > _2PI)
  {
    x -= _2PI;
  }

  theta = (uint16_t)(x * ConvertToCAU + 0.5f);

  CORDIC_SinCos(theta, &sin_tmp, &cos_tmp);

  *sin = (float)(sin_tmp * ConvertToReal);
  *cos = (float)(cos_tmp * ConvertToReal);
}

float fast_atan2(float y, float x)
{
  return CORDIC_Atan2(y, x);
}
