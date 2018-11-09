#include "ekf_att.h"

#define w (0.0005f)
#define v (0.06f)

MAT_3X3_TYPE F;
MAT_3X3_TYPE L;
MAT_3X3_TYPE K;

MAT_3X3_TYPE I = {1, 0, 0,
                  0, 1, 0,
                  0, 0, 1};

MAT_3X3_TYPE Q = {w, 0, 0,
                  0, w, 0,
                  0, 0, w};

MAT_3X3_TYPE R = {v, 0, 0,
                  0, v, 0,
                  0, 0, v};

MAT_3X3_TYPE P = {100, 0, 0,
                  0, 100, 0,
                  0, 0, 100};

/* Matrix operation */

/*
************************************************************
*                Mat_copy_3x3()
* brief : 
************************************************************
*/
void Mat_copy_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a)
{
  result->m11 = a->m11;
  result->m12 = a->m12;
  result->m13 = a->m13;

  result->m21 = a->m21;
  result->m22 = a->m22;
  result->m23 = a->m23;

  result->m31 = a->m31;
  result->m32 = a->m32;
  result->m33 = a->m33;
}

/*
************************************************************
*                Mat_copy_3x1()
* brief : 
************************************************************
*/
void Mat_copy_3x1(MAT_3X1_TYPE *result, MAT_3X1_TYPE *a)
{
  result->m11 = a->m11;
  result->m21 = a->m21;
  result->m31 = a->m31;
}

/*
************************************************************
*                Mat_mul_3x3_3x3()
* brief : 
************************************************************
*/
void Mat_mul_3x3_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a, MAT_3X3_TYPE *b)
{
  MAT_3X3_TYPE tmp;

  tmp.m11 = a->m11 * b->m11 + a->m12 * b->m21 + a->m13 * b->m31;
  tmp.m12 = a->m11 * b->m12 + a->m12 * b->m22 + a->m13 * b->m32;
  tmp.m13 = a->m11 * b->m13 + a->m12 * b->m23 + a->m13 * b->m33;

  tmp.m21 = a->m21 * b->m11 + a->m22 * b->m21 + a->m23 * b->m31;
  tmp.m22 = a->m21 * b->m12 + a->m22 * b->m22 + a->m23 * b->m32;
  tmp.m23 = a->m21 * b->m13 + a->m22 * b->m23 + a->m23 * b->m33;

  tmp.m31 = a->m31 * b->m11 + a->m32 * b->m21 + a->m33 * b->m31;
  tmp.m32 = a->m31 * b->m12 + a->m32 * b->m22 + a->m33 * b->m32;
  tmp.m33 = a->m31 * b->m13 + a->m32 * b->m23 + a->m33 * b->m33;

  Mat_copy_3x3(result, &tmp);
}

/*
************************************************************
*                Mat_mul_3x3_3x1()
* brief : 
************************************************************
*/
void Mat_mul_3x3_3x1(MAT_3X1_TYPE *result, MAT_3X3_TYPE *a, MAT_3X1_TYPE *b)
{
  MAT_3X1_TYPE tmp;
  tmp.m11 = a->m11 * b->m11 + a->m12 * b->m21 + a->m13 * b->m31;
  tmp.m21 = a->m21 * b->m11 + a->m22 * b->m21 + a->m23 * b->m31;
  tmp.m31 = a->m31 * b->m11 + a->m32 * b->m21 + a->m33 * b->m31;
  Mat_copy_3x1(result, &tmp);
}

/*
************************************************************
*                Mat_add_3x3()
* brief : 
************************************************************
*/
void Mat_add_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a, MAT_3X3_TYPE *b)
{
  MAT_3X3_TYPE tmp;

  tmp.m11 = a->m11 + b->m11;
  tmp.m12 = a->m12 + b->m12;
  tmp.m13 = a->m13 + b->m13;

  tmp.m21 = a->m21 + b->m21;
  tmp.m22 = a->m22 + b->m22;
  tmp.m23 = a->m23 + b->m23;

  tmp.m31 = a->m31 + b->m31;
  tmp.m32 = a->m32 + b->m32;
  tmp.m33 = a->m33 + b->m33;

  Mat_copy_3x3(result, &tmp);
}

/*
************************************************************
*                Mat_add_3x1()
* brief : 
************************************************************
*/
void Mat_add_3x1(MAT_3X1_TYPE *result, MAT_3X1_TYPE *a, MAT_3X1_TYPE *b)
{
  MAT_3X1_TYPE tmp;
  tmp.m11 = a->m11 + b->m11;
  tmp.m21 = a->m21 + b->m21;
  tmp.m31 = a->m31 + b->m31;
  Mat_copy_3x1(result, &tmp);
}

/*
************************************************************
*                Mat_sub_3x3()
* brief : 
************************************************************
*/
void Mat_sub_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a, MAT_3X3_TYPE *b)
{
  MAT_3X3_TYPE tmp;
  tmp.m11 = a->m11 - b->m11;
  tmp.m12 = a->m12 - b->m12;
  tmp.m13 = a->m13 - b->m13;

  tmp.m21 = a->m21 - b->m21;
  tmp.m22 = a->m22 - b->m22;
  tmp.m23 = a->m23 - b->m23;

  tmp.m31 = a->m31 - b->m31;
  tmp.m32 = a->m32 - b->m32;
  tmp.m33 = a->m33 - b->m33;
  Mat_copy_3x3(result, &tmp);
}

/*
************************************************************
*                Mat_sub_3x1()
* brief : 
************************************************************
*/
void Mat_sub_3x1(MAT_3X1_TYPE *result, MAT_3X1_TYPE *a, MAT_3X1_TYPE *b)
{
  MAT_3X1_TYPE tmp;
  tmp.m11 = a->m11 - b->m11;
  tmp.m21 = a->m21 - b->m21;
  tmp.m31 = a->m31 - b->m31;
  Mat_copy_3x1(result, &tmp);
}

/*
************************************************************
*                Mat_transpose()
* brief : 
************************************************************
*/
void Mat_transpose(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a)
{
  MAT_3X3_TYPE tmp;
  tmp.m11 = a->m11;
  tmp.m12 = a->m21;
  tmp.m13 = a->m31;
  tmp.m21 = a->m12;
  tmp.m22 = a->m22;
  tmp.m23 = a->m32;
  tmp.m31 = a->m13;
  tmp.m32 = a->m23;
  tmp.m33 = a->m33;
  Mat_copy_3x3(result, &tmp);
}

/*
************************************************************
*                Mat_inv_3x3()
* brief : 
************************************************************
*/
void Mat_inv_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a)
{
  MAT_3X3_TYPE tmp;

  float a_m22_a_m33 = a->m22 * a->m33;
  float a_m12_a_m33 = a->m12 * a->m33;
  float a_m12_a_m23 = a->m12 * a->m23;
  float a_m21_a_m33 = a->m21 * a->m33;
  float a_m11_a_m33 = a->m11 * a->m33;
  float a_m11_a_m23 = a->m11 * a->m23;
  float a_m21_a_m32 = a->m21 * a->m32;
  float a_m11_a_m32 = a->m11 * a->m32;
  float a_m11_a_m22 = a->m11 * a->m22;
  float a_m22_a_m31 = a->m22 * a->m31;

  float a_m11_a_m22_a_m33 = a->m11 * a_m22_a_m33;
  float a_m11_a_m23_a_m32 = a_m11_a_m23 * a->m32;
  float a_m12_a_m21_a_m33 = a->m12 * a_m21_a_m33;
  float a_m12_a_m23_a_m31 = a_m12_a_m23 * a->m31;
  float a_m13_a_m21_a_m32 = a->m13 * a_m21_a_m32;
  float a_m13_a_m22_a_m31 = a->m13 * a_m22_a_m31;

  tmp.m11 = (a_m22_a_m33 - a->m23 * a->m32) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  tmp.m12 = -(a_m12_a_m33 - a->m13 * a->m32) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  tmp.m13 = (a_m12_a_m23 - a->m13 * a->m22) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  tmp.m21 = -(a_m21_a_m33 - a->m23 * a->m31) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  tmp.m22 = (a_m11_a_m33 - a->m13 * a->m31) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  tmp.m23 = -(a_m11_a_m23 - a->m13 * a->m21) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  tmp.m31 = (a_m21_a_m32 - a_m22_a_m31) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  tmp.m32 = -(a_m11_a_m32 - a->m12 * a->m31) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  tmp.m33 = (a_m11_a_m22 - a->m12 * a->m21) / (a_m11_a_m22_a_m33 - a_m11_a_m23_a_m32 - a_m12_a_m21_a_m33 + a_m12_a_m23_a_m31 + a_m13_a_m21_a_m32 - a_m13_a_m22_a_m31);
  Mat_copy_3x3(result, &tmp);
}

/*
************************************************************
*                  ekf_att_est()
* brief : extended kalman filter
************************************************************
*/
void ekf_att_est(MAT_3X1_TYPE *x, MAT_3X1_TYPE *u, MAT_3X1_TYPE *y, float dt)
{
  MAT_3X3_TYPE mat3X3_tmp1;
  MAT_3X3_TYPE mat3X3_tmp2;
  MAT_3X3_TYPE F_trans;
  MAT_3X3_TYPE L_trans;

  MAT_3X1_TYPE mat3X1_tmp1;
  MAT_3X1_TYPE x_tmp;

  float u1 = u->m11;
  float u2 = u->m21;
  float u3 = u->m31;

  Mat_copy_3x1(&x_tmp, x);

  F.m11 = 1;
  F.m12 = u3 * dt;
  F.m13 = -u2 * dt;
  F.m21 = -u3 * dt;
  F.m22 = 1;
  F.m23 = u1 * dt;
  F.m31 = u2 * dt;
  F.m32 = -u1 * dt;
  F.m33 = 1;

  L.m11 = 0;
  L.m12 = -(x->m31) * dt;
  L.m13 = (x->m21) * dt;
  L.m21 = (x->m31) * dt;
  L.m22 = 0;
  L.m23 = -(x->m11) * dt;
  L.m31 = -(x->m21) * dt;
  L.m32 = (x->m11) * dt;
  L.m33 = 0;

  Mat_transpose(&F_trans, &F);
  Mat_transpose(&L_trans, &L);

  /* P = F*P*F' + L*Q*L' */
  Mat_mul_3x3_3x3(&mat3X3_tmp1, &F, &P);
  Mat_mul_3x3_3x3(&mat3X3_tmp1, &mat3X3_tmp1, &F_trans);
  Mat_mul_3x3_3x3(&mat3X3_tmp2, &L, &Q);
  Mat_mul_3x3_3x3(&mat3X3_tmp2, &mat3X3_tmp2, &L_trans);
  Mat_add_3x3(&P, &mat3X3_tmp1, &mat3X3_tmp2);

  /* prio estimate */
  x_tmp.m11 = (x->m11) + u3 * (x->m21) * dt - u2 * (x->m31) * dt;
  x_tmp.m21 = -u3 * (x->m11) * dt + (x->m21) + u1 * (x->m31) * dt;
  x_tmp.m31 = u2 * (x->m11) * dt - u1 * (x->m21) * dt + (x->m31);

  /* kalman gain K = P*H'/(H*P*H'+M*R*M') */
  Mat_add_3x3(&mat3X3_tmp1, &P, &R);
  Mat_inv_3x3(&mat3X3_tmp1, &mat3X3_tmp1);
  Mat_mul_3x3_3x3(&K, &P, &mat3X3_tmp1);

  /* x = x + K*(y - H*x) */
  Mat_sub_3x1(&mat3X1_tmp1, y, &x_tmp);
  Mat_mul_3x3_3x1(&mat3X1_tmp1, &K, &mat3X1_tmp1);
  Mat_add_3x1(x, &x_tmp, &mat3X1_tmp1);

  /* P = (I - K*H)*P */
  Mat_sub_3x3(&mat3X3_tmp1, &I, &K);
  Mat_mul_3x3_3x3(&P, &mat3X3_tmp1, &P);
}
