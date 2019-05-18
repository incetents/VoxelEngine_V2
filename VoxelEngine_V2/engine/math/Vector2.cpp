// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once
#include "Precompiled.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix2x2.h"
#include "MathCore.h"
#include "Lerp.h"

namespace Vxl
{
	const Vector2i Vector2i::ZERO = Vector2i(0, 0);
	const Vector2i Vector2i::ONE = Vector2i(1, 1);
	const Vector2i Vector2i::UP = Vector2i(0, +1);
	const Vector2i Vector2i::DOWN = Vector2i(0, -1);
	const Vector2i Vector2i::LEFT = Vector2i(-1, 0);
	const Vector2i Vector2i::RIGHT = Vector2i(+1, 0);

	// Empty Constructor
	Vector2i::Vector2i(void) : x(0), y(0) {}
	Vector2i::Vector2i(int all) : x(all), y(all) {}
	Vector2i::Vector2i(Vector3i v) : x(v.x), y(v.y) {}
	Vector2i::Vector2i(Vector4i v) : x(v.x), y(v.y) {}

	Vector2i::Vector2i(Radians all)
	{
		x = (int)all.Get();
		y = (int)all.Get();
	}
	Vector2i::Vector2i(Degrees all)
	{
		x = (int)all.Get();
		y = (int)all.Get();
	}
	Vector2i::Vector2i(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
	Vector2i::Vector2i(Radians _x, Radians _y)
	{
		x = (int)_x.Get();
		y = (int)_y.Get();
	}
	Vector2i::Vector2i(Degrees _x, Degrees _y)
	{
		x = (int)_x.Get();
		y = (int)_y.Get();
	}

	const Vector2 Vector2::ZERO = Vector2(0, 0);
	const Vector2 Vector2::ONE = Vector2(1, 1);
	const Vector2 Vector2::UP = Vector2(0, +1);
	const Vector2 Vector2::DOWN = Vector2(0, -1);
	const Vector2 Vector2::LEFT = Vector2(-1, 0);
	const Vector2 Vector2::RIGHT = Vector2(+1, 0);

	// Empty Constructor
	Vector2::Vector2(void) : x(0), y(0) {}
	Vector2::Vector2(float all) : x(all), y(all) {}
	Vector2::Vector2(Vector3 v) : x(v.x), y(v.y) {}
	Vector2::Vector2(Vector4 v) : x(v.x), y(v.y) {}

	Vector2::Vector2(Radians all)
	{
		x = all.Get();
		y = all.Get();
	}
	Vector2::Vector2(Degrees all)
	{
		x = all.Get();
		y = all.Get();
	}
	Vector2::Vector2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}
	Vector2::Vector2(Radians _x, Radians _y)
	{
		x = _x.Get();
		y = _y.Get();
	}
	Vector2::Vector2(Degrees _x, Degrees _y)
	{
		x = _x.Get();
		y = _y.Get();
	}

	// Cast
	Vector2::operator Vector3()
	{
		return Vector3(x, y, 0);
	}
	Vector2::operator Vector4()
	{
		return Vector4(x, y, 0, 0);
	}

	// Switch X and Y values
	Vector2& Vector2::Flip()
	{
		std::swap(x, y);
		return *this;
	}
	Vector2 Vector2::GetFlip()
	{
		return Vector2(y, x);
	}
	Vector2 Vector2::GetFlip(const Vector2& v)
	{
		return Vector2(v.y, v.x);
	}

	// Inverse
	Vector2 Vector2::Inverse() const
	{
		return Vector2(
			1.0f / x,
			1.0f / y
		);
	}
	Vector2 Vector2::Inverse(const Vector2& v)
	{
		return v.Inverse();
	}

	// Compare
	bool Vector2::Compare(const Vector2& v) const
	{
		if (abs(x - v.x) > FLOAT_EPSILON)
			return false;

		if (abs(y - v.y) > FLOAT_EPSILON)
			return false;

		return true;
	}
	bool Vector2::Compare(const Vector2& v1, const Vector2& v2)
	{
		return v1.Compare(v2);
	}

	// Compare Fuzzy
	bool Vector2::CompareFuzzy(const Vector2& v) const
	{
		if (fabs(x - v.x) > 0.01f)
			return false;

		else if (fabs(y - v.y) > 0.01f)
			return false;

		return true;
	}
	bool Vector2::CompareFuzzy(const Vector2& v1, const Vector2& v2)
	{
		return v1.CompareFuzzy(v2);
	}

	// Normalize Self (Returns Length)
	Vector2& Vector2::NormalizeSelf()
	{
		return (*this *= Vxl::InverseSqrt(x * x + y * y));
	}
	// Normalize
	Vector2 Vector2::Normalize() const
	{
		return Vector2(x, y) * Vxl::InverseSqrt(x * x + y * y);
	}
	Vector2 Vector2::Normalize(const Vector2& v)
	{
		return v.Normalize();
	}
	// Magnitude
	float Vector2::Magnitude() const
	{
		return sqrtf(x * x + y * y);
	}
	float Vector2::Magnitude(const Vector2& v)
	{
		return v.Magnitude();
	}
	// Length
	float Vector2::Length() const
	{
		return Magnitude();
	}
	float Vector2::Length(const Vector2& v)
	{
		return Magnitude(v);
	}
	// Length Squared
	float Vector2::LengthSqr() const
	{
		return Vector2::Dot();
	}
	float Vector2::LengthSqr(const Vector2& v)
	{
		return v.LengthSqr();
	}

	// Distance
	float Vector2::Distance(const Vector2& v)
	{
		return Length(*this - v);
	}
	float Vector2::Distance(const Vector2& v1, const Vector2& v2)
	{
		return Length(v1 - v2);
	}

	// Dot Product
	float Vector2::Dot() const
	{
		return (x * x + y * y);
	}
	float Vector2::Dot(const Vector2& v) const
	{
		return (x * v.x + y * v.y);
	}
	float Vector2::Dot(const Vector2& v1, const Vector2& v2)
	{
		return v1.Dot(v2);
	}

	// Reflect Vector
	Vector2 Vector2::Reflect(const Vector2& Normal)
	{
		return Reflect(*this, Normal);
	}
	Vector2 Vector2::Reflect(const Vector2& Incident, const Vector2& Normal)
	{
		return Incident - 2 * (Incident.Dot(Normal)) * Normal;
	}
	// Refract Vector
	Vector2 Vector2::Refract(const Vector2& Normal, float index)
	{
		return Refract(*this, Normal, index);
	}
	Vector2 Vector2::Refract(const Vector2& Incident, const Vector2& Normal, float index)
	{
		float dot = Dot(Incident, Normal);
		float k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
			return (index * Incident - (index * dot + sqrt(k)) * Normal);

		return Vector2();
	}

	// Perpendicular
	Vector2 Vector2::Perpendicular() const
	{
		return Vector2(y, -x);
	}

	// Project Vector
	Vector2 Vector2::Project(const Vector2& v) const
	{
		// Formula:	(Dot / (b length * b length)) * b
		float D = Vector2::Dot(*this, v);
		float S = (v.x * v.x + v.y * v.y);

		return v * (D / S);
	}
	Vector2 Vector2::Project(const Vector2& v1, const Vector2& v2)
	{
		Vector2 Result = v1.Project(v2);
		return Result;
	}
	float Vector2::ProjectLength(const Vector2& v) const
	{
		float D = Vector2::Dot(*this, v);
		float L = v.Length();
		return D / L;
	}
	float Vector2::ProjectLength(const Vector2& v1, const Vector2& v2)
	{
		return v1.ProjectLength(v2);
	}

	// Rotate Degrees
	Vector2 Vector2::Rotate(const Degrees& D, RotationDirection Rot)
	{
		// Alt Conversion
		return Rotate(Radians(D.GetRadians()), Rot);

	}
	// Rotate Radians
	Vector2 Vector2::Rotate(const Radians& R, RotationDirection Rot)
	{
		float Rad = R.Get();
		// Counter Clockwise Rotation
		if (Rot == RotationDirection::COUNTER_CLOCKWISE)
		{
			return Vector2(
				+x * cosf(Rad) - y * sinf(Rad),
				+x * sinf(Rad) + y * cosf(Rad));
		}
		// Clockwise Rotation
		return Vector2(
			+x * cosf(Rad) + y * sinf(Rad),
			-x * sinf(Rad) + y * cosf(Rad));
	}

	// Get Angle Degrees
	Degrees Vector2::GetAngleDegrees(Vector2& v)
	{
		return Degrees(GetAngleRadians(v));
	}
	Degrees Vector2::GetAngleDegrees(Vector2& v1, Vector2& v2)
	{
		return v1.GetAngleDegrees(v2);
	}

	// Get Angle Radians
	Radians Vector2::GetAngleRadians(Vector2& v)
	{
		Radians Rad(
			// Faster Method
			atan2f(v.x, v.y) - atan2f(x, y)
			// Original Method

			//acosf(
			//	Dot(v) / (Magnitude() * v.Magnitude())
			//)

		);
		return Rad;
	}
	Radians Vector2::GetAngleRadians(Vector2& v1, Vector2& v2)
	{
		return v1.GetAngleRadians(v2);
	}

	// Random Vector2 Unit Vector
	Vector2 Vector2::GetRandomUnitVector()
	{
		float _x = Random::Range(-1.0f, 1.0f);
		float _y = Random::Range(-1.0f, 1.0f);

		return Vector2(_x, _y).Normalize();
	}

	// Fix Denormals (Fix Numbers close to zero)
	bool Vector2::FixDenormals()
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
		return denormal;
	}
	bool Vector2::FixDenormals(Vector2& v)
	{
		return v.FixDenormals();
	}

	// Resize
	Vector2 Vector2::Resize(float length)
	{
		// If length is zero, return empty vector
		if (length == 0.0f)
			return Vector2();

		// Normalize then multiply length
		return (Normalize() * length);

	}

	// Abs
	Vector2& Vector2::Abs()
	{
		x = abs(x);
		y = abs(y);
		return *this;
	}
	Vector2 Vector2::Abs(const Vector2& v)
	{
		return Vector2(
			abs(v.x),
			abs(v.y)
		);
	}
	// Min
	Vector2& Vector2::Min(const Vector2& v)
	{
		x = min(x, v.x);
		y = min(y, v.y);
		return *this;
	}
	Vector2 Vector2::Min(const Vector2& v1, const Vector2& v2)
	{
		return Vector2(
			min(v1.x, v2.x),
			min(v1.y, v2.y)
		);
	}
	// Max
	Vector2& Vector2::Max(const Vector2& v)
	{
		x = max(x, v.x);
		y = max(y, v.y);
		return *this;
	}
	Vector2 Vector2::Max(const Vector2& v1, const Vector2& v2)
	{
		return Vector2(
			max(v1.x, v2.x),
			max(v1.y, v2.y)
		);
	}
	// Clamp
	void Vector2::Clamp(const Vector2& lower, const Vector2& upper)
	{
		// Fix X Values
		x = MacroClamp(x, lower.x, upper.x);
		// Fix Y Values
		y = MacroClamp(y, lower.y, upper.y);
	}
	Vector2 Vector2::Clamp(Vector2 v, const Vector2& min, const Vector2& max)
	{
		v.Clamp(min, max);
		return v;
	}

	// Snap to nearest integer value
	Vector2 Vector2::Snap()
	{
		return Vector2(floor(x + 0.5f), floor(y + 0.5f));
	}

	// Get Float pointer of values
	float* Vector2::GetStartPointer(void)
	{
		return &x;
	}
	float* Vector2::GetStartPointer(Vector2& v)
	{
		return &v.x;
	}

	// Vector to string
	std::string Vector2::ToString()
	{
		return std::to_string(x) + ' ' + std::to_string(y);
	}

	// Lerp
	Vector2 Vector2::Lerp(const Vector2& v, const float t)
	{
		return MacroLerp(*this, v, t);
	}
	Vector2 Vector2::Lerp(const Vector2& v1, const Vector2& v2, float t)
	{
		return MacroLerp(v1, v2, t);
	}
	// Slerp
	Vector2 Vector2::Slerp(const Vector2& v, const float t)
	{
		return Slerp(*this, v, t);
	}
	Vector2 Vector2::Slerp(const Vector2& v1, const Vector2& v2, float t)
	{
		float dot = Vector2::Dot(v1, v2);
		MacroClamp(dot, -1.0f, 1.0f);
		float theta = acosf(dot)*t;
		Vector2 RelativeVec = v2 - v1 * dot;
		RelativeVec.Normalize();

		return ((v1*cosf(theta)) + (RelativeVec*sinf(theta)));
	}
	// Nlerp
	Vector2 Vector2::Nlerp(const Vector2& v, const float t)
	{
		return Nlerp(*this, v, t);
	}
	Vector2 Vector2::Nlerp(const Vector2& v1, const Vector2& v2, float t)
	{
		return (MacroLerp(v1, v2, t)).Normalize();
	}


	// Special Operator Overloading
	float Vector2::operator[](const int index) const
	{
		assert((index >= 0) && (index < 2));
		return (&x)[index];
	}
	float& Vector2::operator[](const int index)
	{
		assert((index >= 0) && (index < 2));
		return (&x)[index];
	}
	Vector2 Vector2::operator-() const
	{
		return Vector2(-x, -y);
	}


	// Basic Operator Overloading
	Vector2 Vector2::operator+ (const Vector2& v) const
	{
		return Vector2(x + v.x, y + v.y);
	}
	Vector2 Vector2::operator- (const Vector2& v) const
	{
		return Vector2(x - v.x, y - v.y);
	}
	Vector2 Vector2::operator* (const Vector2& v) const
	{
		return Vector2(x * v.x, y * v.y);
	}
	Vector2 Vector2::operator/ (const Vector2& v) const
	{
		return Vector2(x / v.x, y / v.y);
	}
	Vector2& Vector2::operator+= (const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2& Vector2::operator-= (const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector2& Vector2::operator*= (const Vector2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector2& Vector2::operator/= (const Vector2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vector2 Vector2::operator+ (const Vector3& v) const
	{
		return Vector2(x + v.x, y + v.y);
	}
	Vector2 Vector2::operator- (const Vector3& v) const
	{
		return Vector2(x - v.x, y - v.y);
	}
	Vector2 Vector2::operator* (const Vector3& v) const
	{
		return Vector2(x * v.x, y * v.y);
	}
	Vector2 Vector2::operator/ (const Vector3& v) const
	{
		return Vector2(x / v.x, y / v.y);
	}
	Vector2& Vector2::operator+= (const Vector3& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2& Vector2::operator-= (const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector2& Vector2::operator*= (const Vector3& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector2& Vector2::operator/= (const Vector3& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vector2 Vector2::operator+ (const Vector4& v) const
	{
		return Vector2(x + v.x, y + v.y);
	}
	Vector2 Vector2::operator- (const Vector4& v) const
	{
		return Vector2(x - v.x, y - v.y);
	}
	Vector2 Vector2::operator* (const Vector4& v) const
	{
		return Vector2(x * v.x, y * v.y);
	}
	Vector2 Vector2::operator/ (const Vector4& v) const
	{
		return Vector2(x / v.x, y / v.y);
	}
	Vector2& Vector2::operator+= (const Vector4& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2& Vector2::operator-= (const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector2& Vector2::operator*= (const Vector4& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector2& Vector2::operator/= (const Vector4& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	// Operator Overloading with Float
	Vector2 operator+ (const Vector2& v, const float f)
	{
		return Vector2(v.x + f, v.y + f);
	}
	Vector2 operator- (const Vector2& v, const float f)
	{
		return Vector2(v.x - f, v.y - f);
	}
	Vector2 operator* (const Vector2& v, const float f)
	{
		return Vector2(v.x * f, v.y * f);
	}
	Vector2 operator/ (const Vector2& v, const float f)
	{
		float inva = 1.0f / f;
		return Vector2(v.x * inva, v.y * inva);
	}


	Vector2 operator+ (const float f, const Vector2& v)
	{
		return v + f;
	}
	Vector2 operator- (const float f, const Vector2& v)
	{
		return v - f;
	}
	Vector2 operator* (const float f, const Vector2& v)
	{
		return v * f;
	}
	Vector2 operator/ (const float f, const Vector2& v)
	{
		return v / f;
	}


	Vector2& Vector2::operator+= (const float f)
	{
		x += f;
		y += f;
		return *this;
	}
	Vector2& Vector2::operator-= (const float f)
	{
		x -= f;
		y -= f;
		return *this;
	}
	Vector2& Vector2::operator*= (const float f)
	{
		x *= f;
		y *= f;
		return *this;
	}
	Vector2& Vector2::operator/= (const float f)
	{
		float inva = 1.0f / f;
		x *= inva;
		y *= inva;
		return *this;
	}


	// Operator Comparison Overloading
	Vector2& Vector2::operator= (const Vector2& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
	Vector2& Vector2::operator= (const Vector3& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
	Vector2& Vector2::operator= (const Vector4& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
	bool Vector2::operator== (const Vector2& v) const
	{
		return Compare(v);
	}
	bool Vector2::operator!= (const Vector2& v) const
	{
		return !Compare(v);
	}

	std::ostream &operator<<(std::ostream &stream, const Vector2& ob)
	{
		return (stream << '[' << ob.x << ' ' << ob.y << ']');
	}
}