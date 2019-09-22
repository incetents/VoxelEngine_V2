// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <iostream>
#include <math.h>
#include <cmath>
#include <cfloat>
#include <windows.h>
#include <string>
#include <vector>
#include <assert.h>

#include "Random.h"

// Matrix Macros
#define MATRIX_INVERSE_EPSILON	1e-14f
#define MATRIX_EPSILON			1e-6f

#define Matrix2x2_Length 2
#define Matrix3x3_Length 3
#define Matrix4x4_Length 4

#define Matrix2x2_Size 4
#define Matrix3x3_Size 9
#define Matrix4x4_Size 16

// Size of an Arrayon the stack
#define ARRAY_SIZE(_array) (sizeof(_array) / sizeof((_array)[0]))

// Floating point comparisons
#define FUZZ 1e-12f
// #define FUZZ 1e-6f

// Math Macros
#define PI (std::atanf(1.0f) * 4.0f)
#define TWO_PI (2.0f*PI)
#define HALF_PI (0.5f*PI)
#define EULERS_E 2.71828182845904523536f
#define SQRT_TWO 1.41421356237309504880f
#define SQRT_THREE 1.73205080756887729352f
#define SQRT_1OVER2 0.70710678118654752440f
#define SQRT_1OVER3 0.57735026918962576450f
#define SQRT_HALF SQRT_1OVER2
#define SEC_TO_MS 1000.0f
#define MS_TO_SEC 0.001f
#define MAX_FLOAT 1e30f
#define FLOAT_EPSILON 1.192092896e-07f
#define DOUBLE_EPSILON 1.192092896e-07

// Macro Rotation Data
#define Rad_To_Deg(x) (x * (180.0f / PI))
#define Deg_To_Rad(x) (x * (PI / 180.0f))

// Macro Functions
#define MacroMax(a,b) (((a) > (b)) ? (a) : (b))
#define MacroMin(a,b) (((a) < (b)) ? (a) : (b))
#define MacroRound(x) (std::ceilf(value * (float)std::pow(10, x)) / (float)pow(10, x))
#define MacroClamp(value,low,high) (((value)<(low))?(low):(((value)>(high))?(high):(value)))
#define MacroClamp01(n) MacroClamp(n, 0, 1)
#define MacroSwap_UNSAFE(a, b)	((a)^=(b),(b)^=(a),(a)^=(b)) /* Doesn't work when a and b are the same object - assigns zero (0) to the object in that case */
#define MacroSwap(a, b)   ((&(a) == &(b)) ? (a) : ((a)^=(b),(b)^=(a),(a)^=(b))) /* checks that the addresses of a and b are different before XOR-ing */

namespace Vxl
{
	enum class Axis
	{
		NONE = -1,
		X = 0,
		Y = 1,
		Z = 2,
		ALL = 3
	};

	template<typename T>
	static T RoundToMultiple(T val, T multiple)
	{
		T remainder = val % multiple;
		if (remainder != 0)
		{
			val += multiple - remainder;
		}
		return val;
	}

	template<typename T>
	static bool Compare(T a, T b)
	{
		return (a == b);
	}
	template<>
	static bool Compare(float a, float b)
	{
		return (abs(a - b) < FLOAT_EPSILON);
	}
	template<>
	static bool Compare(double a, double b)
	{
		return (abs(a - b) < DOUBLE_EPSILON);
	}

	template<typename T>
	static T Round(T value, u_int decimals)
	{
		return std::ceilf(value * (float)std::pow(10, decimals)) / (float)std::pow(10, decimals);
	}

	template<typename T>
	static void Swap(T& a, T& b)
	{
		T c = a;
		a = b;
		b = c;
	}

	template<typename T>
	static T InverseSqrt(T value)
	{
		if (value == 0) return 0;
		long i;
		float x2, y;

		x2 = value * 0.5F;
		y = value;
		i = *(long *)&y;
		i = 0x5f3759df - (i >> 1);
		y = *(float *)&i;
		y = y * (1.5f - (x2 * y * y));   // 1st iteration

		return  y;
	}

	template<typename T>
	static T ToRadians(T value)
	{
		return Deg_To_Rad(value);
	}

	template<typename T>
	static T ToDegrees(T value)
	{
		return Rad_To_Deg(value);
	}

	template<typename T>
	// Puts any number into the [0->1] range
	static T Sigmoid(T value)
	{
		return (2.0f / (1.0f + pow(EULERS_E, -value))) - 1.0f;
	}

	// Puts any number into the [-1->+1] range
	template<typename T>
	static T SigmoidNormalized(T value)
	{
		return 1.0f / (1.0f + pow(EULERS_E, -value));
	}
}