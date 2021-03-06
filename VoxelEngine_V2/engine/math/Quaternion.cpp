// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Quaternion.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "MathCore.h"

#include <iostream>
#include <assert.h>

namespace Vxl
{
	// Identity Matrix
	const Quaternion Quaternion::_Identity = Quaternion(0, 0, 0, 1);

	// Constructors
	Quaternion::Quaternion()
	{
		*this = Quaternion::_Identity;
	}
	Quaternion::Quaternion(const float _x, const float _y, const float _z, const float _w) : x(_x), y(_y), z(_z), w(_w) {}
	Quaternion::Quaternion(float _radians, const Vector3& axis, bool _counterClockWise)
	{
		*this = Quaternion::AngleAxis(_radians, axis, _counterClockWise);
	}

	// Become default empty quaternion
	void Quaternion::Identity()
	{
		*this = _Identity;
	}

	// Turn euler rotation into Quaternion
	Quaternion Quaternion::ToQuaternion_ZYX(float _yaw, float _pitch, float _roll)
	{
		// XYZ
		Quaternion q;
		
		// EULER
		float angle;
		
		angle = _yaw * 0.5f;
		const float sr = sinf(angle);
		const float cr = cosf(angle);
		
		angle = _pitch * 0.5f;
		const float sp = sinf(angle);
		const float cp = cosf(angle);
		
		angle = _roll * 0.5f;
		const float sy = sinf(angle);
		const float cy = cosf(angle);
		
		const float cpcy = cp * cy;
		const float spcy = sp * cy;
		const float cpsy = cp * sy;
		const float spsy = sp * sy;
		
		q.w = (float)(cr * cpcy + sr * spsy);
		q.x = (float)(sr * cpcy - cr * spsy);
		q.y = (float)(cr * spcy + sr * cpsy);
		q.z = (float)(cr * cpsy - sr * spcy);

		return q;// .Normalize();
	}

	Quaternion Quaternion::ToQuaternion_YXZ(float _yaw, float _pitch, float _roll)
	{
		auto QX = Quaternion::AngleAxis(_yaw, Vector3(1, 0, 0));
		auto QY = Quaternion::AngleAxis(_pitch, Vector3(0, 1, 0));
		auto QZ = Quaternion::AngleAxis(_roll, Vector3(0, 0, 1));
		
		return QY * QX * QZ;
	}
	// Turn Quaternion into euler rotation
	void Quaternion::ToEuler_ZYX(const Quaternion& q, float& _roll, float& _pitch, float& _yaw)
	{
		// Formula retrieved from wiki link:
		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		// x-axis
		float sinr = +2.0f * (q.w * q.x + q.y * q.z);
		float cosr = +1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		_pitch = atan2f(sinr, cosr);

		// y-axis
		float sinp = +2.0f * (q.w * q.y - q.z * q.x);
		if (fabs(sinp) >= 1)
			_yaw = copysignf(PI / 2, sinp); // use 90 degrees if out of range
		else
			_yaw = asinf(sinp);

		// z-axis
		float siny = +2.0f * (q.w * q.z + q.x * q.y);
		float cosy = +1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		_roll = atan2f(siny, cosy);
	}

	// Get Matrix (column major)
	Matrix3x3 Quaternion::GetMatrix3x3() const
	{
		Matrix3x3 matrix;

		matrix[0x0] = 1.0f - 2.0f*y*y - 2.0f*z*z;
		matrix[0x1] = 2.0f*x*y + 2.0f*z*w;
		matrix[0x2] = 2.0f*x*z - 2.0f*y*w;

		matrix[0x3] = 2.0f*x*y - 2.0f*z*w;
		matrix[0x4] = 1.0f - 2.0f*x*x - 2.0f*z*z;
		matrix[0x5] = 2.0f*z*y + 2.0f*x*w;

		matrix[0x6] = 2.0f*x*z + 2.0f*y*w;
		matrix[0x7] = 2.0f*z*y - 2.0f*x*w;
		matrix[0x8] = 1.0f - 2.0f*x*x - 2.0f*y*y;

		return matrix;
	}
	Matrix4x4 Quaternion::GetMatrix4x4() const
	{
		return Quaternion::GetMatrix4x4(Vector3(0, 0, 0));
	}
	Matrix4x4 Quaternion::GetMatrix4x4(const Vector3& center) const
	{
		Matrix3x3 m = GetMatrix3x3();
		Matrix4x4 matrix(m);

		// Manually set position
		matrix[0x3] = center.x;
		matrix[0x7] = center.y;
		matrix[0xB] = center.z;

		return matrix;
	}


	// Create Angle Axis
	Quaternion Quaternion::AngleAxis(float _radians, const Vector3& axis, bool _counterClockWise)
	{
		float Half_Rotation = _radians * (_counterClockWise ? 0.5f : -0.5f);
		float SinHR = sinf(Half_Rotation);

		return Quaternion(
			SinHR * axis.x,
			SinHR * axis.y,
			SinHR * axis.z,
			cosf(Half_Rotation)
		);
	}

	// Haven't checked if formula copied has incorrect row/column major
	Quaternion Quaternion::AngleAxis(const Matrix3x3& matrix)
	{
		float w = sqrtf(1 + matrix[0] + matrix[4] + matrix[8]) / 2;
		float four_w = 4 * w;
		return Quaternion(
			(matrix[7] - matrix[5]) / four_w,
			(matrix[2] - matrix[6]) / four_w,
			(matrix[3] - matrix[1]) / four_w,
			w
		);
	}
	Quaternion Quaternion::AngleAxis(const Matrix4x4& matrix)
	{
		Matrix3x3 m(matrix);
		return AngleAxis(m);
	}

	// Inverse
	Quaternion& Quaternion::InverseSelf()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}
	Quaternion Quaternion::Inverse() const
	{
		return Quaternion(-x, -y, -z, w);
	}
	Quaternion Quaternion::Inverse(const Quaternion& q)
	{
		return q.Inverse();
	}

	// Normalize
	Quaternion& Quaternion::NormalizeSelf()
	{
		*this *= Vxl::InverseSqrt(x * x + y * y + z * z + w * w);
		return *this;
	}
	Quaternion Quaternion::Normalize()
	{
		Quaternion Q(*this);
		Q.NormalizeSelf();
		return Q;
	}
	Quaternion Quaternion::Normalize(const Quaternion& q)
	{
		Quaternion Q(q);
		Q.NormalizeSelf();
		return Q;
	}

	// Dot Product
	float Quaternion::Dot()
	{
		Vector4 v(x, y, z, w);
		return v.Dot();
	}
	float Quaternion::Dot(const Quaternion& q)
	{
		return Vector4(x, y, z, w).Dot(Vector4(q.x, q.y, q.z, q.w));
	}
	float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
	{
		return Vector4(q1.x, q1.y, q1.z, q1.w).Dot(Vector4(q2.x, q2.y, q2.z, q2.w));
	}

	// Operator Overloads
	float Quaternion::operator[](const unsigned int index) const
	{
		VXL_ASSERT(index < 4, "Quaternion index out of range");
		return (&x)[index];
	}
	float& Quaternion::operator[](const unsigned int index)
	{
		VXL_ASSERT(index < 4, "Quaternion index out of range");
		return (&x)[index];
	}
	Quaternion Quaternion::operator-()
	{
		return Quaternion(-x, -y, -z, -w);
	}

	// Operator Overload (Math)
	Quaternion Quaternion::operator+(const Quaternion& q) const
	{
		return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
	}
	Quaternion Quaternion::operator-(const Quaternion& q) const
	{
		return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
	}
	Quaternion Quaternion::operator*(const Quaternion& R) const
	{
		Quaternion Q;

		Q.w = (R.w * w) - (R.x * x) - (R.y * y) - (R.z * z);
		Q.x = (R.w * x) + (R.x * w) + (R.y * z) - (R.z * y);
		Q.y = (R.w * y) + (R.y * w) + (R.z * x) - (R.x * z);
		Q.z = (R.w * z) + (R.z * w) + (R.x * y) - (R.y * x);

		return Q;
	}
	Quaternion Quaternion::operator*(const float f) const
	{
		return Quaternion(x * f, y * f, z * f, w * f);
	}
	Vector3 operator*(const Quaternion& q, const Vector3& v)
	{
		// Extract the vector part of the quaternion
		Vector3 u(q.x, q.y, q.z);

		// Extract the scalar part of the quaternion
		float s = q.w;

		// Quaternion Formula
		Vector3 vprime = 2.0f * Vector3::Dot(u, v) * u
			+ (s*s - Vector3::Dot(u, u)) * v
			+ 2.0f * s * Vector3::Cross(u, v);

		return vprime;
	}
	// Operator Overload (Math Change)
	Quaternion& Quaternion::operator*=(const Quaternion& q)
	{
		*this = q * (*this);
		return *this;
	}
	Quaternion& Quaternion::operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	// Operator Comparison Overloading
	Quaternion& Quaternion::operator= (const Quaternion& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
		return *this;
	}
	bool Quaternion::operator== (const Quaternion& q) const
	{
		return (x == q.x && y == q.y && z == q.z && w == q.w);
	}
	bool Quaternion::operator!= (const Quaternion& q) const
	{
		return (x != q.x || y != q.y || z != q.z || w != q.w);
	}
}