// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Quaternion.h"
#include "Rotation.h"
#include "Vector3.h"
#include "Vector4.h"
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
	Quaternion::Quaternion(const Degrees& rotation, const Vector3& axis, RotationDirection direction)
	{
		*this = Quaternion::AngleAxis(rotation, axis, direction);
	}
	Quaternion::Quaternion(const Radians& rotation, const Vector3& axis, RotationDirection direction)
	{
		*this = Quaternion::AngleAxis(rotation, axis, direction);
	}

	// Become default empty quaternion
	void Quaternion::Identity()
	{
		*this = _Identity;
	}

	// Turn euler rotation into Quaternion
	Quaternion Quaternion::GetEuler(const Degrees& Deg_X, const Degrees& Deg_Y, const Degrees& Deg_Z)
	{
		return Quaternion::GetEuler(Radians(Deg_X), Radians(Deg_Y), Radians(Deg_Z));
	}
	Quaternion Quaternion::GetEuler(const Radians& Rad_X, const Radians& Rad_Y, const Radians& Rad_Z)
	{
		Quaternion q;

		// EULER
		float angle;

		angle = Rad_X.Get() * 0.5f;
		const float sr = sinf(angle);
		const float cr = cosf(angle);

		angle = Rad_Y.Get() * 0.5f;
		const float sp = sinf(angle);
		const float cp = cosf(angle);

		angle = Rad_Z.Get() * 0.5f;
		const float sy = sinf(angle);
		const float cy = cosf(angle);

		const float cpcy = cp * cy;
		const float spcy = sp * cy;
		const float cpsy = cp * sy;
		const float spsy = sp * sy;

		q.x = (float)(sr * cpcy - cr * spsy);
		q.y = (float)(cr * spcy + sr * cpsy);
		q.z = (float)(cr * cpsy - sr * spcy);
		q.w = (float)(cr * cpcy + sr * spsy);

		//std::cout << "TEST Q: " << q.x << " " << q.y << " " << q.z << " " << q.w << std::endl;

		return q.Normalize();
	}
	// Turn Quaternion into euler rotation
	void Quaternion::ToEuler(const Quaternion& q, Degrees& roll, Degrees& pitch, Degrees& yaw)
	{
		Radians r(0);
		Radians p(0);
		Radians y(0);

		Quaternion::ToEuler(q, r, p, y);

		roll = r.GetDegrees();
		pitch = p.GetDegrees();
		yaw = y.GetDegrees();
	}
	void Quaternion::ToEuler(const Quaternion& q, Radians& roll, Radians& pitch, Radians& yaw)
	{
		// Formula retrieved from wiki link:
		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		// x-axis
		float sinr = +2.0f * (q.w * q.x + q.y * q.z);
		float cosr = +1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		roll = atan2f(sinr, cosr);

		// y-axis
		float sinp = +2.0f * (q.w * q.y - q.z * q.x);
		if (fabs(sinp) >= 1)
			pitch = copysignf(PI / 2, sinp); // use 90 degrees if out of range
		else
			pitch = asinf(sinp);

		// z-axis
		float siny = +2.0f * (q.w * q.z + q.x * q.y);
		float cosy = +1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		yaw = atan2f(siny, cosy);
	}

	// Get Matrix
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
	Quaternion Quaternion::AngleAxis(const Degrees& rotation, const Vector3& axis, RotationDirection direction)
	{
		return Quaternion::AngleAxis(Radians(rotation), axis, direction);
	}
	Quaternion Quaternion::AngleAxis(const Radians& rotation, const Vector3& axis, RotationDirection direction)
	{
		float Half_Rotation = rotation.Get() * 0.5f;
		float SinHR = sinf(Half_Rotation);

		return Quaternion(
			SinHR * axis.x,
			SinHR * axis.y,
			SinHR * axis.z,
			cosf(Half_Rotation)
		);
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
	float Quaternion::operator[](const int index) const
	{
		assert((index >= 0) && (index < 4));
		return (&x)[index];
	}
	float& Quaternion::operator[](const int index)
	{
		assert((index >= 0) && (index < 4));
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