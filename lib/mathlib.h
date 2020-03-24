#ifndef __MATHLIB_H__
#define __MATHLIB_H__


//-------------------------------------------------------------------------
//Mathematical Definitions
//-------------------------------------------------------------------------
#define PI2				(6.28318530717958647692528676654f) //PI��2�� 
#define PI				(3.14159265358979323846264338327f) //PI

#define POW2_15			32768L			// 2 ^ 15ֵ
#define POW2_16			65536L			// 2 ^ 16ֵ
#define POW2_23			8388608UL		// 2 ^ 23ֵ
#define POW2_24			16777216UL		// 2 ^ 24ֵ
#define POW2_27			134217728UL		// 2 ^ 27ֵ
#define POW2_28			268435456UL		// 2 ^ 28ֵ
#define POW2_31			2147483648UL	// 2 ^ 31ֵ


extern const float math_div10[];
extern const u32 math_pow10[];
extern const float math_sint[];

typedef struct
{
	float real;
	float imag;
} t_complex;



//-------------------------------------------------------------------------
//FixPoint
//-------------------------------------------------------------------------
typedef s32 fixpoint;
typedef s64 fixpoint_l;

typedef struct {
	fixpoint_l real;
	fixpoint_l imag;
} t_complex_fix_l;

typedef struct {
	fixpoint real;
	fixpoint imag;
} t_complex_fix;

#define EXP 			(13)//  32λ�ĺ�13λ��С������
#define FIX2DOUBLE(x)	(((double)(x)) / (1UL << EXP))
#define DOUBLE2FIX(x)	((fixpoint)(((double)(x)) * (1UL << EXP)))
#define FIX2FLOAT(x)	(((float)(x)) / (1UL << EXP))
#define FLOAT2FIX(x)	((fixpoint)(((float)(x)) * (1UL << EXP))) //����ת������
#define FIXP_EPSILON	(1.0 / (1UL << EXP))

u32 sqrtfix(u64 d, u32 N);


#endif




