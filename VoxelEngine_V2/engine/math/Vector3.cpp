// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once
#include "Precompiled.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3x3.h"
#include "MathCore.h"
#include "Lerp.h"

namespace Vxl
{
	const Vector3i Vector3i::ZERO = Vector3i(0, 0, 0);
	const Vector3i Vector3i::ONE = Vector3i(1, 1, 1);
	const Vector3i Vector3i::N_ONE = Vector3i(-1, -1, -1);
	const Vector3i Vector3i::UP = Vector3i(0, +1, 0);
	const Vector3i Vector3i::DOWN = Vector3i(0, -1, 0);
	const Vector3i Vector3i::FORWARD = Vector3i(0, 0, +1);
	const Vector3i Vector3i::BACK = Vector3i(0, 0, -1);
	const Vector3i Vector3i::LEFT = Vector3i(-1, 0, 0);
	const Vector3i Vector3i::RIGHT = Vector3i(+1, 0, 0);

	// Constructors
	Vector3i::Vector3i(void) : x(0), y(0), z(0) {}
	Vector3i::Vector3i(int all) : x(all), y(all), z(all) {}
	Vector3i::Vector3i(Vector2i v) : x(v.x), y(v.y), z(0) {}
	Vector3i::Vector3i(Vector4i v) : x(v.x), y(v.y), z(v.z) {}
	Vector3i::Vector3i(Radians all)
	{
		x = (int)all.Get();
		y = (int)all.Get();
		z = (int)all.Get();
	}
	Vector3i::Vector3i(Degrees all)
	{
		x = (int)all.Get();
		y = (int)all.Get();
		z = (int)all.Get();
	}
	Vector3i::Vector3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

	Vector3i::Vector3i(Radians _x, Radians _y, Radians _z)
	{
		x = (int)_x.Get();
		y = (int)_y.Get();
		z = (int)_z.Get();
	}
	Vector3i::Vector3i(Degrees _x, Degrees _y, Degrees _z)
	{
		x = (int)_x.Get();
		y = (int)_y.Get();
		z = (int)_z.Get();
	}
	Vector3i::Vector3i(Vector2i _v, int _z)
	{
		x = _v.x;
		y = _v.y;
		z = _z;
	}
	Vector3i::Vector3i(int _x, Vector2i _v)
	{
		x = _x;
		y = _v.x;
		z = _v.y;
	}

	// Compare
	bool Vector3i::Compare(const Vector3i& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}
	bool Vector3i::Compare(const Vector3i& v1, const Vector3i& v2)
	{
		return v1.Compare(v2);
	}

	// Special Operator Overloads
	int Vector3i::operator[](const int index) const
	{
		assert((index >= 0) && (index < 3));
		return (&x)[index];
	}
	int& Vector3i::operator[](const int index)
	{
		assert((index >= 0) && (index < 3));
		return (&x)[index];
	}
	Vector3i Vector3i::operator-() const
	{
		return Vector3i(-x, -y, -z);
	}

	// Basic Operator Overloading
	Vector3i Vector3i::operator+ (const Vector2i& v) const
	{
		return Vector3i(x + v.x, y + v.y, z);
	}
	Vector3i Vector3i::operator- (const Vector2i& v) const
	{
		return Vector3i(x - v.x, y - v.y, z);
	}
	Vector3i Vector3i::operator* (const Vector2i& v) const
	{
		return Vector3i(x * v.x, y * v.y, z);
	}
	Vector3i Vector3i::operator/ (const Vector2i& v) const
	{
		return Vector3i(x / v.x, y / v.y, z);
	}
	Vector3i& Vector3i::operator+= (const Vector2i& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector3i& Vector3i::operator-= (const Vector2i& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector3i& Vector3i::operator*= (const Vector2i& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector3i& Vector3i::operator/= (const Vector2i& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vector3i Vector3i::operator+ (const Vector3i& v) const
	{
		return Vector3i(x + v.x, y + v.y, z + v.z);
	}
	Vector3i Vector3i::operator- (const Vector3i& v) const
	{
		return Vector3i(x - v.x, y - v.y, z - v.z);
	}
	Vector3i Vector3i::operator* (const Vector3i& v) const
	{
		return Vector3i(x * v.x, y * v.y, z * v.z);
	}
	Vector3i Vector3i::operator/ (const Vector3i& v) const
	{
		return Vector3i(x / v.x, y / v.y, z / v.z);
	}
	Vector3i& Vector3i::operator+= (const Vector3i& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3i& Vector3i::operator-= (const Vector3i& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3i& Vector3i::operator*= (const Vector3i& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector3i& Vector3i::operator/= (const Vector3i& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vector3i Vector3i::operator+ (const Vector4i& v) const
	{
		return Vector3i(x + v.x, y + v.y, z + v.z);
	}
	Vector3i Vector3i::operator- (const Vector4i& v) const
	{
		return Vector3i(x - v.x, y - v.y, z - v.z);
	}
	Vector3i Vector3i::operator* (const Vector4i& v) const
	{
		return Vector3i(x * v.x, y * v.y, z * v.z);
	}
	Vector3i Vector3i::operator/ (const Vector4i& v) const
	{
		return Vector3i(x / v.x, y / v.y, z / v.z);
	}
	Vector3i& Vector3i::operator+= (const Vector4i& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3i& Vector3i::operator-= (const Vector4i& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3i& Vector3i::operator*= (const Vector4i& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector3i& Vector3i::operator/= (const Vector4i& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	// Operator Overloading with Float
	Vector3i operator+ (const Vector3i& v, const int f)
	{
		return Vector3i(v.x + f, v.y + f, v.z + f);
	}
	Vector3i operator- (const Vector3i& v, const int f)
	{
		return Vector3i(v.x - f, v.y - f, v.z - f);
	}
	Vector3i operator* (const Vector3i& v, const int f)
	{
		return Vector3i(v.x * f, v.y * f, v.z * f);
	}
	Vector3i operator/ (const Vector3i& v, const int f)
	{
		return Vector3i(v.x / f, v.y / f, v.z / f);
	}

	Vector3i operator+ (const int f, const Vector3i& v)
	{
		return v + f;
	}
	Vector3i operator- (const int f, const Vector3i& v)
	{
		return v - f;
	}
	Vector3i operator* (const int f, const Vector3i& v)
	{
		return v * f;
	}
	Vector3i operator/ (const int f, const Vector3i& v)
	{
		return v / f;
	}


	Vector3i& Vector3i::operator+= (int f)
	{
		x += f;
		y += f;
		z += f;
		return *this;
	}
	Vector3i& Vector3i::operator-= (int f)
	{
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}
	Vector3i& Vector3i::operator*= (int f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}
	Vector3i& Vector3i::operator/= (int f)
	{
		assert(f);
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	// Comparison Operator Overloading
	Vector3i& Vector3i::operator= (const Vector2i& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
	Vector3i& Vector3i::operator= (const Vector3i& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	Vector3i& Vector3i::operator= (const Vector4i& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	bool Vector3i::operator== (const Vector3i& v) const
	{
		return Compare(v);
	}
	bool Vector3i::operator!= (const Vector3i& v) const
	{
		return !Compare(v);
	}


	const Vector3 Vector3::ZERO = Vector3(0, 0, 0);
	const Vector3 Vector3::ONE = Vector3(1, 1, 1);
	const Vector3 Vector3::N_ONE = Vector3(-1, -1, -1);
	const Vector3 Vector3::HALF = Vector3(0.5f, 0.5f, 0.5f);
	const Vector3 Vector3::MINUS_HALF = Vector3(-0.5f, -0.5f, -0.5f);
	const Vector3 Vector3::QUARTER = Vector3(0.25f, 0.25f, 0.25f);
	const Vector3 Vector3::MINUS_QUARTER = Vector3(-0.25f, -0.25f, -0.25f);
	const Vector3 Vector3::UP = Vector3(0, +1, 0);
	const Vector3 Vector3::DOWN = Vector3(0, -1, 0);
	const Vector3 Vector3::FORWARD = Vector3(0, 0, +1);
	const Vector3 Vector3::BACK = Vector3(0, 0, -1);
	const Vector3 Vector3::LEFT = Vector3(-1, 0, 0);
	const Vector3 Vector3::RIGHT = Vector3(+1, 0, 0);

	// Constructors
	Vector3::Vector3(void) : x(0), y(0), z(0) {}
	Vector3::Vector3(float all) : x(all), y(all), z(all) {}
	Vector3::Vector3(Vector2 v) : x(v.x), y(v.y), z(0) {}
	Vector3::Vector3(Vector4 v) : x(v.x), y(v.y), z(v.z) {}
	Vector3::Vector3(Radians all)
	{
		x = all.Get();
		y = all.Get();
		z = all.Get();
	}
	Vector3::Vector3(Degrees all)
	{
		x = all.Get();
		y = all.Get();
		z = all.Get();
	}
	Vector3::Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	Vector3::Vector3(Radians _x, Radians _y, Radians _z)
	{
		x = _x.Get();
		y = _y.Get();
		z = _z.Get();
	}
	Vector3::Vector3(Degrees _x, Degrees _y, Degrees _z)
	{
		x = _x.Get();
		y = _y.Get();
		z = _z.Get();
	}
	Vector3::Vector3(Vector2 _v, float _z)
	{
		x = _v.x;
		y = _v.y;
		z = _z;
	}
	Vector3::Vector3(float _x, Vector2 _v)
	{
		x = _x;
		y = _v.x;
		z = _v.y;
	}

	// Cast
	Vector3::operator Vector2()
	{
		return Vector2(x, y);
	}
	Vector3::operator Vector4()
	{
		return Vector4(x, y, z, 0);
	}

	// Inverse
	Vector3 Vector3::Inverse() const
	{
		return Vector3(
			1.0f / x,
			1.0f / y,
			1.0f / z
		);
	}
	Vector3 Vector3::Inverse(const Vector3& v)
	{
		return v.Inverse();
	}

	// Compare
	bool Vector3::Compare(const Vector3& v) const
	{
		if (fabs(x - v.x) > FLOAT_EPSILON)
			return false;

		else if (fabs(y - v.y) > FLOAT_EPSILON)
			return false;

		else if (fabs(z - v.z) > FLOAT_EPSILON)
			return false;

		return true;
	}
	bool Vector3::Compare(const Vector3& v1, const Vector3& v2)
	{
		return v1.Compare(v2);
	}
	// Compare Fuzzy
	bool Vector3::CompareFuzzy(const Vector3& v) const
	{
		if (fabs(x - v.x) > 0.01f)
			return false;

		else if (fabs(y - v.y) > 0.01f)
			return false;

		else if (fabs(z - v.z) > 0.01f)
			return false;

		return true;
	}
	bool Vector3::CompareFuzzy(const Vector3& v1, const Vector3& v2)
	{
		return v1.CompareFuzzy(v2);
	}

	// Normalize Self (Returns Length)
	Vector3& Vector3::NormalizeSelf()
	{
		return (*this *= Vxl::InverseSqrt(x * x + y * y + z * z));
	}
	// Normalize
	Vector3 Vector3::Normalize() const
	{
		return Vector3(x, y, z) * Vxl::InverseSqrt(x * x + y * y + z * z);
	}
	Vector3 Vector3::NormalizeXY() const // Remove Z and normalize
	{
		return Vector3(x, y, 0) * Vxl::InverseSqrt(x * x + y * y);
	}
	Vector3 Vector3::NormalizeXZ() const // Remove Y and normalize
	{
		return Vector3(x, 0, z) * Vxl::InverseSqrt(x * x + z * z);
	}
	Vector3 Vector3::NormalizeYZ() const // Remove X and normalize
	{
		return Vector3(0, y, z) * Vxl::InverseSqrt(y * y + z * z);

	}
	Vector3 Vector3::Normalize(const Vector3& v)
	{
		return v.Normalize();
	}
	Vector3 Vector3::NormalizeXY(const Vector3& v)
	{
		return v.NormalizeXY();
	}
	Vector3 Vector3::NormalizeXZ(const Vector3& v)
	{
		return v.NormalizeXZ();
	}
	Vector3 Vector3::NormalizeYZ(const Vector3& v)
	{
		return v.NormalizeYZ();
	}
	// Magnitude
	float Vector3::Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	float Vector3::Magnitude(const Vector3& v)
	{
		return v.Magnitude();
	}
	// Length
	float Vector3::Length() const
	{
		return Magnitude();
	}
	float Vector3::Length(const Vector3& v)
	{
		return Magnitude(v);
	}
	float Vector3::Length(float x, float y, float z)
	{
		return Magnitude(Vector3(x, y, z));
	}
	// Length Squared
	float Vector3::LengthSqr() const
	{
		return Vector3::Dot();
	}
	float Vector3::LengthSqr(const Vector3& v)
	{
		return v.LengthSqr();
	}
	float Vector3::LengthSqr(float x, float y, float z)
	{
		return Vector3(x, y, z).Dot();
	}

	// Distance
	float Vector3::Distance(const Vector3& v)
	{
		return Length(*this - v);
	}
	float Vector3::Distance(const Vector3& v1, const Vector3& v2)
	{
		return Length(v1 - v2);
	}

	// Dot Product
	float Vector3::Dot() const
	{
		return (x * x + y * y + z * z);
	}
	float Vector3::Dot(const Vector3& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}
	float Vector3::Dot(const Vector3& v1, const Vector3& v2)
	{
		return v1.Dot(v2);
	}

	// Reflect Vector
	Vector3 Vector3::Reflect(const Vector3& Normal)
	{
		return Reflect(*this, Normal);
	}
	Vector3 Vector3::Reflect(const Vector3& Incident, const Vector3& Normal)
	{
		return Incident - 2 * (Incident.Dot(Normal)) * Normal;
	}
	// Refract Vector
	Vector3 Vector3::Refract(const Vector3& Normal, float index)
	{
		return Refract(*this, Normal, index);
	}
	Vector3 Vector3::Refract(const Vector3& Incident, const Vector3& Normal, float index)
	{
		float dot = Dot(Incident, Normal);
		float k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
			return (index * Incident - (index * dot + sqrt(k)) * Normal);

		return Vector3();
	}

	// Project Vector
	Vector3 Vector3::Project(const Vector3& v) const
	{
		// Formula:	(Dot / (b length * b length)) * b
		float D = Vector3::Dot(*this, v);
		float S = v.LengthSqr();

		return v * (D / S);
	}
	Vector3 Vector3::Project(const Vector3& v1, const Vector3& v2)
	{
		Vector3 Result = v1.Project(v2);
		return Result;
	}
	float Vector3::ProjectLength(const Vector3& v) const
	{
		float D = Vector3::Dot(*this, v);
		float L = v.Length();
		return D / L;
	}
	float Vector3::ProjectLength(const Vector3& v1, const Vector3& v2)
	{
		return v1.ProjectLength(v2);
	}

	// Cross Product
	Vector3 Vector3::Cross(const Vector3& v) const
	{
		return Vector3(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}
	Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2)
	{
		return v1.Cross(v2);
	}

	// Rotate
	Vector3 Vector3::RotateX(const Degrees& deg, RotationDirection rd) const
	{
		return RotateX(Radians(deg), rd);
	}
	Vector3 Vector3::RotateY(const Degrees& deg, RotationDirection rd) const
	{
		return RotateY(Radians(deg), rd);
	}
	Vector3 Vector3::RotateZ(const Degrees& deg, RotationDirection rd) const
	{
		return RotateZ(Radians(deg), rd);
	}
	Vector3 Vector3::Rotate(const Degrees& deg, const Vector3& direction, RotationDirection rd) const
	{
		return Rotate(Radians(deg), direction, rd);
	}
	// Rotate Radians
	Vector3 Vector3::RotateX(const Radians& rad, RotationDirection rd) const
	{
		float Val = rad.GetDegrees() * (float)rd;
		return Vector3(
			+x,
			+y * cosf(Val) - z * sinf(Val),
			+z * sinf(Val) + z * cosf(Val)
		);
	}
	Vector3 Vector3::RotateY(const Radians& rad, RotationDirection rd) const
	{
		float Val = rad.GetDegrees() * (float)rd;
		return Vector3(
			+x * cosf(Val) + z * sinf(Val),
			+y,
			-x * sinf(Val) + z * cosf(Val)
		);
	}
	Vector3 Vector3::RotateZ(const Radians& rad, RotationDirection rd) const
	{
		float Val = rad.GetDegrees() * (float)rd;
		return Vector3(
			+x * cosf(Val) - y * sinf(Val),
			+x * sinf(Val) + y * cosf(Val),
			+z
		);
	}
	Vector3 Vector3::Rotate(const Radians& rad, const Vector3& direction, RotationDirection rd) const
	{
		// TO DO
		return Vec3();
	}

	// Get Angle Degrees
	Degrees Vector3::GetAngleDegrees(const Vector3& v) const
	{
		return Degrees(GetAngleRadians(v));
	}
	Degrees Vector3::GetAngleDegrees(const Vector3& v1, const Vector3& v2)
	{
		return v1.GetAngleDegrees(v2);
	}

	// Get Angle Radians
	Radians Vector3::GetAngleRadians(const Vector3& v) const
	{
		return Radians(
			acosf(
				Dot(v) / (Magnitude() * v.Magnitude())
			)
		);
	}
	Radians Vector3::GetAngleRadians(const Vector3& v1, const Vector3& v2)
	{
		return v1.GetAngleRadians(v2);
	}

	// Random Vector3 Unit Vector
	Vector3 Vector3::GetRandomUnitVector()
	{
		float _x = Random::Range(-1.0f, 1.0f);
		float _y = Random::Range(-1.0f, 1.0f);
		float _z = Random::Range(-1.0f, 1.0f);

		return Vector3(_x, _y, _z).Normalize();
	}

	// Fix Denormals (Fix Numbers close to zero)
	bool Vector3::FixDenormals()
	{
		bool denormal = false;
		if (fabs(x) < 1e-30f) {
			x = 0.0f;
			denormal = true;
		}
		if (fabs(y) < 1e-30f) {
			y = 0.0f;
			denormal = true;
		}
		if (fabs(z) < 1e-30f) {
			z = 0.0f;
			denormal = true;
		}
		return denormal;
	}
	bool Vector3::FixDenormals(Vector3& v)
	{
		return v.FixDenormals();
	}

	// Resize
	Vector3 Vector3::Resize(float length)
	{
		// If length is zero, return empty vector
		if (!length)
			return Vector3();

		// Normalize then multiply length
		return Normalize() * length;

	}

	// Abs
	Vector3& Vector3::Abs()
	{
		x = abs(x);
		y = abs(y);
		z = abs(z);
		return *this;
	}
	Vector3 Vector3::Abs(const Vector3& v)
	{
		return Vector3(
			abs(v.x),
			abs(v.y),
			abs(v.z)
		);
	}
	// Min
	Vector3& Vector3::Min(const Vector3& v)
	{
		x = min(x, v.x);
		y = min(y, v.y);
		z = min(z, v.z);
		return *this;
	}
	Vector3 Vector3::Min(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(
			min(v1.x, v2.x),
			min(v1.y, v2.y),
			min(v1.z, v2.z)
		);
	}
	// Max
	Vector3& Vector3::Max(const Vector3& v)
	{
		x = max(x, v.x);
		y = max(y, v.y);
		z = max(z, v.z);
		return *this;
	}
	Vector3 Vector3::Max(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(
			max(v1.x, v2.x),
			max(v1.y, v2.y),
			max(v1.z, v2.z)
		);
	}
	// Clamp
	void Vector3::Clamp(const Vector3& lower, const Vector3& upper)
	{
		// Fix X Values
		x = MacroClamp(x, lower.x, upper.x);
		// Fix Y Values
		y = MacroClamp(y, lower.y, upper.y);
		// Fix Y Values
		z = MacroClamp(z, lower.z, upper.z);
	}
	Vector3 Vector3::Clamp(Vector3 v, const Vector3& min, const Vector3& max)
	{
		v.Clamp(min, max);
		return v;
	}

	// Snap to nearest integer value
	Vector3 Vector3::Snap()
	{
		return Vector3(
			floor(x + 0.5f),
			floor(y + 0.5f),
			floor(z + 0.5f)
		);
	}

	// Get Float pointer of values
	float* Vector3::GetStartPointer(void)
	{
		return &x;
	}
	float* Vector3::GetStartPointer(Vector3& v)
	{
		return &v.x;
	}

	// Vector to string
	std::string Vector3::ToString()
	{
		return std::to_string(x) + ' ' + std::to_string(y) + ' ' + std::to_string(z);
	}

	// Lerp
	Vector3 Vector3::Lerp(const Vector3& v, const float t)
	{
		return MacroLerp(*this, v, t);
	}
	Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t)
	{
		return MacroLerp(v1, v2, t);
	}
	// Slerp
	Vector3 Vector3::Slerp(const Vector3& v, const float t)
	{
		return Slerp(*this, v, t);
	}
	Vector3 Vector3::Slerp(const Vector3& v1, const Vector3& v2, float t)
	{
		float dot = Vector3::Dot(v1, v2);
		MacroClamp(dot, -1.0f, 1.0f);
		float theta = acosf(dot)*t;
		Vector3 RelativeVec = v2 - v1 * dot;
		RelativeVec.Normalize();

		return ((v1*cosf(theta)) + (RelativeVec*sinf(theta)));
	}
	// Nlerp
	Vector3 Vector3::Nlerp(const Vector3& v, const float t)
	{
		return Nlerp(*this, v, t);
	}
	Vector3 Vector3::Nlerp(const Vector3& v1, const Vector3& v2, float t)
	{
		return (MacroLerp(v1, v2, t)).Normalize();
	}


	// Special Operator Overloads
	float Vector3::operator[](const int index) const
	{
		assert((index >= 0) && (index < 3));
		return (&x)[index];
	}
	float& Vector3::operator[](const int index)
	{
		assert((index >= 0) && (index < 3));
		return (&x)[index];
	}
	Vector3 Vector3::operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	// Basic Operator Overloading
	Vector3 Vector3::operator+ (const Vector2& v) const
	{
		return Vector3(x + v.x, y + v.y, z);
	}
	Vector3 Vector3::operator- (const Vector2& v) const
	{
		return Vector3(x - v.x, y - v.y, z);
	}
	Vector3 Vector3::operator* (const Vector2& v) const
	{
		return Vector3(x * v.x, y * v.y, z);
	}
	Vector3 Vector3::operator/ (const Vector2& v) const
	{
		return Vector3(x / v.x, y / v.y, z);
	}
	Vector3& Vector3::operator+= (const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector3& Vector3::operator-= (const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector3& Vector3::operator*= (const Vector2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector3& Vector3::operator/= (const Vector2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vector3 Vector3::operator+ (const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 Vector3::operator- (const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 Vector3::operator* (const Vector3& v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}
	Vector3 Vector3::operator/ (const Vector3& v) const
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}
	Vector3& Vector3::operator+= (const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3& Vector3::operator-= (const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3& Vector3::operator*= (const Vector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector3& Vector3::operator/= (const Vector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vector3 Vector3::operator+ (const Vector4& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 Vector3::operator- (const Vector4& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 Vector3::operator* (const Vector4& v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}
	Vector3 Vector3::operator/ (const Vector4& v) const
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}
	Vector3& Vector3::operator+= (const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3& Vector3::operator-= (const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3& Vector3::operator*= (const Vector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector3& Vector3::operator/= (const Vector4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	// Operator Overloading with Float
	Vector3 operator+ (const Vector3& v, const float f)
	{
		return Vector3(v.x + f, v.y + f, v.z + f);
	}
	Vector3 operator- (const Vector3& v, const float f)
	{
		return Vector3(v.x - f, v.y - f, v.z - f);
	}
	Vector3 operator* (const Vector3& v, const float f)
	{
		return Vector3(v.x * f, v.y * f, v.z * f);
	}
	Vector3 operator/ (const Vector3& v, const float f)
	{
		float inva = 1.0f / f;
		return Vector3(v.x * inva, v.y * inva, v.z * inva);
	}

	Vector3 operator+ (const float f, const Vector3& v)
	{
		return v + f;
	}
	Vector3 operator- (const float f, const Vector3& v)
	{
		return v - f;
	}
	Vector3 operator* (const float f, const Vector3& v)
	{
		return v * f;
	}
	Vector3 operator/ (const float f, const Vector3& v)
	{
		return v / f;
	}


	Vector3& Vector3::operator+= (float f)
	{
		x += f;
		y += f;
		z += f;
		return *this;
	}
	Vector3& Vector3::operator-= (float f)
	{
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}
	Vector3& Vector3::operator*= (float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}
	Vector3& Vector3::operator/= (float f)
	{
		float inva = 1.0f / f;
		x *= inva;
		y *= inva;
		z *= inva;
		return *this;
	}

	// Comparison Operator Overloading
	Vector3& Vector3::operator= (const Vector2& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
	Vector3& Vector3::operator= (const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	Vector3& Vector3::operator= (const Vector4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	bool Vector3::operator== (const Vector3& v) const
	{
		return Compare(v);
	}
	bool Vector3::operator!= (const Vector3& v) const
	{
		return !Compare(v);
	}

	std::ostream &operator<<(std::ostream &stream, const Vector3& ob)
	{
		return (stream << '[' << ob.x << ' ' << ob.y << ' ' << ob.z << ']');
	}

	// Return Vec3
	Vector3 Vector3::GetVector3(int index1, int index2, int index3) const
	{
		return Vector3(*(&x + index1), *(&x + index2), *(&x + index3));
	}
	// Return Vec2
	Vector2 Vector3::GetVector2(int index1, int index2) const
	{
		return Vector2(*(&x + index1), *(&x + index2));
	}
}