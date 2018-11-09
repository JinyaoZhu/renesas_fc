#ifndef __EKF_ATT_H__
#define __EKF_ATT_H__

typedef struct
{
	float m11;
	float m12;
	float m13;
	float m21;
	float m22;
	float m23;
	float m31;
	float m32;
	float m33;
} MAT_3X3_TYPE;

typedef struct
{
	float m11;
	float m21;
	float m31;
} MAT_3X1_TYPE;

void Mat_copy_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a);

void Mat_copy_3x1(MAT_3X1_TYPE *result, MAT_3X1_TYPE *a);

void Mat_mul_3x3_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a, MAT_3X3_TYPE *b);

void Mat_mul_3x3_3x1(MAT_3X1_TYPE *result, MAT_3X3_TYPE *a, MAT_3X1_TYPE *b);

void Mat_add_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a, MAT_3X3_TYPE *b);

void Mat_add_3x1(MAT_3X1_TYPE *result, MAT_3X1_TYPE *a, MAT_3X1_TYPE *b);

void Mat_sub_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a, MAT_3X3_TYPE *b);

void Mat_sub_3x1(MAT_3X1_TYPE *result, MAT_3X1_TYPE *a, MAT_3X1_TYPE *b);

void Mat_transpose(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a);

void Mat_inv_3x3(MAT_3X3_TYPE *result, MAT_3X3_TYPE *a);

void ekf_att_est(MAT_3X1_TYPE *x, MAT_3X1_TYPE *u, MAT_3X1_TYPE *y, float dt);

#endif /* __EKF_ATT_H__ */
