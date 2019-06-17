// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once
#include "Precompiled.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "MathCore.h"
#include "Lerp.h"

#include "../utilities/Macros.h"

#ifdef _DEBUG
#define OUT_OF_RANGE_INDEX_CHECK(index) VXL_ASSERT(index < 4, "Vector4 index out of range")
#else
#define OUT_OF_RANGE_INDEX_CHECK(index) __noop
#endif

namespace Vxl
{
	// Constructors
	Vector4i::Vector4i(void) : x(0), y(0), z(0), w(0) {}
	Vector4i::Vector4i(int all) : x(all), y(all), z(all), w(all) {}
	Vector4i::Vector4i(Vector2i v) : x(v.x), y(v.y), z(0), w(0) {}
	Vector4i::Vector4i(Vector3i v) : x(v.x), y(v.y), z(v.z), w(0) {}
	Vector4i::Vector4i(Radians all)
	{
		x = (int)all.Get();
		y = (int)all.Get();
		z = (int)all.Get();
		w = (int)all.Get();
	}
	Vector4i::Vector4i(Degrees all)
	{
		x = (int)all.Get();
		y = (int)all.Get();
		z = (int)all.Get();
		w = (int)all.Get();
	}
	Vector4i::Vector4i(int _x, int _y, int _z, int _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	Vector4i::Vector4i(Radians _x, Radians _y, Radians _z, Radians _w)
	{
		x = (int)_x.Get();
		y = (int)_y.Get();
		z = (int)_z.Get();
		w = (int)_w.Get();
	}
	Vector4i::Vector4i(Degrees _x, Degrees _y, Degrees _z, Degrees _w)
	{
		x = (int)_x.Get();
		y = (int)_y.Get();
		z = (int)_z.Get();
		w = (int)_w.Get();
	}
	Vector4i::Vector4i(Vector3i v, int _w)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = _w;
	}


	// Constructors
	Vector4::Vector4(void) : x(0), y(0), z(0), w(0) {}
	Vector4::Vector4(float all) : x(all), y(all), z(all), w(all) {}
	Vector4::Vector4(Vector2 v) : x(v.x), y(v.y), z(0), w(0) {}
	Vector4::Vector4(Vector3 v) : x(v.x), y(v.y), z(v.z), w(0) {}
	Vector4::Vector4(Radians all)
	{
		x = all.Get();
		y = all.Get();
		z = all.Get();
		w = all.Get();
	}
	Vector4::Vector4(Degrees all)
	{
		x = all.Get();
		y = all.Get();
		z = all.Get();
		w = all.Get();
	}
	Vector4::Vector4(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	Vector4::Vector4(Radians _x, Radians _y, Radians _z, Radians _w)
	{
		x = _x.Get();
		y = _y.Get();
		z = _z.Get();
		w = _w.Get();
	}
	Vector4::Vector4(Degrees _x, Degrees _y, Degrees _z, Degrees _w)
	{
		x = _x.Get();
		y = _y.Get();
		z = _z.Get();
		w = _w.Get();
	}
	Vector4::Vector4(Vector3 v, float _w)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = _w;
	}

	// Cast
	Vector4::operator Vector2()
	{
		return Vector2(x, y);
	}
	Vector4::operator Vector3()
	{
		return Vector3(x, y, z);
	}

	// Swap Values
	Vector4& Vector4::SwapXY()
	{
		std::swap(x, y);
		return *this;
	}
	Vector4& Vector4::SwapXZ()
	{
		std::swap(x, z);
		return *this;
	}
	Vector4& Vector4::SwapXW()
	{
		std::swap(x, w);
		return *this;
	}
	Vector4& Vector4::SwapYZ()
	{
		std::swap(y, z);
		return *this;
	}
	Vector4& Vector4::SwapYW()
	{
		std::swap(y, w);
		return *this;
	}
	Vector4& Vector4::SwapZW()
	{
		std::swap(z, w);
		return *this;
	}

	// Inverse
	Vector4 Vector4::Inverse() const
	{
		return Vector4(
			1.0f / x,
			1.0f / y,
			1.0f / z,
			1.0f / w
		);
	}
	Vector4 Vector4::Inverse(const Vector4& v)
	{
		return v.Inverse();
	}

	// Compare
	bool Vector4::Compare(const Vector4& v) const
	{
		if (abs(x - v.x) > FLOAT_EPSILON)
			return false;

		if (abs(y - v.y) > FLOAT_EPSILON)
			return false;

		if (abs(z - v.z) > FLOAT_EPSILON)
			return false;

		if (abs(w - v.w) > FLOAT_EPSILON)
			return false;

		return true;
	}
	bool Vector4::Compare(const Vector4& v1, const Vector4& v2)
	{
		return v1.Compare(v2);
	}

	// Compare Fuzzy
	bool Vector4::CompareFuzzy(const Vector4& v) const
	{
		if (fabs(x - v.x) > 0.01f)
			return false;

		else if (fabs(y - v.y) > 0.01f)
			return false;

		else if (fabs(z - v.z) > 0.01f)
			return false;

		else if (fabs(w - v.w) > 0.01f)
			return false;

		return true;
	}
	bool Vector4::CompareFuzzy(const Vector4& v1, const Vector4& v2)
	{
		return v1.CompareFuzzy(v2);
	}

	// Normalize Self (Returns Length)
	Vector4& Vector4::NormalizeSelf()
	{
		return (*this *= Vxl::InverseSqrt(x * x + y * y + z * z + w * w));
	}
	// Normalize
	Vector4 Vector4::Normalize() const
	{
		return Vector4(x, y, z, w) * Vxl::InverseSqrt(x * x + y * y + z * z + w * w);
	}
	Vector4 Vector4::Normalize(const Vector4& v)
	{
		return v.Normalize();
	}
	// Magnitude
	float Vector4::Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}
	float Vector4::Magnitude(const Vector4& v)
	{
		return v.Magnitude();
	}
	// Length
	float Vector4::Length() const
	{
		return Magnitude();
	}
	float Vector4::Length(const Vector4& v)
	{
		return Magnitude(v);
	}
	// Length Squared
	float Vector4::LengthSqr() const
	{
		return Vector4::Dot();
	}
	float Vector4::LengthSqr(const Vector4& v)
	{
		return v.LengthSqr();
	}

	// Distance
	float Vector4::Distance(const Vector4& v)
	{
		return Length(*this - v);
	}
	float Vector4::Distance(const Vector4& v1, const Vector4& v2)
	{
		return Length(v1 - v2);
	}

	// Dot Product
	float Vector4::Dot() const
	{
		return (x * x + y * y + z * z + w * w);
	}
	float Vector4::Dot(const Vector4& v) const
	{
		return (x * v.x + y * v.y + z * v.z + w * v.w);
	}
	float Vector4::Dot(const Vector4& v1, const Vector4& v2)
	{
		return v1.Dot(v2);
	}

	// Reflect Vector
	Vector4 Vector4::Reflect(const Vector4& Normal)
	{
		return Reflect(*this, Normal);
	}
	Vector4 Vector4::Reflect(const Vector4& Incident, const Vector4& Normal)
	{
		return Incident - 2 * (Incident.Dot(Normal)) * Normal;
	}
	// Refract Vector
	Vector4 Vector4::Refract(const Vector4& Normal, float index)
	{
		return Refract(*this, Normal, index);
	}
	Vector4 Vector4::Refract(const Vector4& Incident, const Vector4& Normal, float index)
	{
		float dot = Dot(Incident, Normal);
		float k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
			return (index * Incident - (index * dot + sqrt(k)) * Normal);

		return Vector4();
	}

	// Project Vector
	Vector4 Vector4::Project(const Vector4& v) const
	{
		// Formula:	(Dot / (b length * b length)) * b
		float D = Vector4::Dot(*this, v);
		float S = (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);

		return v * (D / S);
	}
	Vector4 Vector4::Project(const Vector4& v1, const Vector4& v2)
	{
		Vector4 Result = v1.Project(v2);
		return Result;
	}
	float Vector4::ProjectLength(const Vector4& v) const
	{
		float D = Vector4::Dot(*this, v);
		float L = v.Length();
		return D / L;
	}
	float Vector4::ProjectLength(const Vector4& v1, const Vector4& v2)
	{
		return v1.ProjectLength(v2);
	}

	// Cross Product
	Vector4 Vector4::Cross(const Vector4& v) const
	{
		return Vector4(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x,
			w
		);
	}
	Vector4 Vector4::Cross(const Vector4& v1, const Vector4& v2)
	{
		return v1.Cross(v2);
	}



	// Get Angle Degrees
	Degrees Vector4::GetAngleDegrees(Vector4& v)
	{
		return Degrees(GetAngleRadians(v));
	}
	Degrees Vector4::GetAngleDegrees(Vector4& v1, Vector4& v2)
	{
		return v1.GetAngleDegrees(v2);
	}

	// Get Angle Radians
	Radians Vector4::GetAngleRadians(Vector4& v)
	{
		return Radians(
			acosf(
				Dot(v) / (Magnitude() * v.Magnitude())
			)
		);
	}
	Radians Vector4::GetAngleRadians(Vector4& v1, Vector4& v2)
	{
		return v1.GetAngleRadians(v2);
	}

	// Random Vector4 Unit Vector
	Vector4 Vector4::GetRandomUnitVector()
	{
		float _x = Random::Range(-1.0f, 1.0f);
		float _y = Random::Range(-1.0f, 1.0f);
		float _z = Random::Range(-1.0f, 1.0f);
		float _w = Random::Range(-1.0f, 1.0f);

		return Vector4(_x, _y, _z, _w).Normalize();
	}

	// Fix Denormals (Fix Numbers close to zero)
	bool Vector4::FixDenormals()
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
		if (fabs(w) < 1e-30f) {
			w = 0.0f;
			denormal = true;
		}
		return denormal;
	}
	bool Vector4::FixDenormals(Vector4& v)
	{
		return v.FixDenormals();
	}

	// Resize
	Vector4 Vector4::Resize(float length)
	{
		// If length is zero, return empty vector
		if (!length)
			return Vector4();

		// Normalize then multiply length
		return Normalize() * length;

	}

	// Abs
	Vector4& Vector4::Abs()
	{
		x = abs(x);
		y = abs(y);
		z = abs(z);
		w = abs(w);
		return *this;
	}
	Vector4 Vector4::Abs(const Vector4& v)
	{
		return Vector4(
			abs(v.x),
			abs(v.y),
			abs(v.z),
			abs(v.w)
		);
	}
	// Min
	Vector4& Vector4::Min(const Vector4& v)
	{
		x = min(x, v.x);
		y = min(y, v.y);
		z = min(z, v.z);
		w = min(w, v.w);
		return *this;
	}
	Vector4 Vector4::Min(const Vector4& v1, const Vector4& v2)
	{
		return Vector4(
			min(v1.x, v2.x),
			min(v1.y, v2.y),
			min(v1.z, v2.z),
			min(v1.w, v2.w)
		);
	}
	// Max
	Vector4& Vector4::Max(const Vector4& v)
	{
		x = max(x, v.x);
		y = max(y, v.y);
		z = max(z, v.z);
		w = max(w, v.w);
		return *this;
	}
	Vector4 Vector4::Max(const Vector4& v1, const Vector4& v2)
	{
		return Vector4(
			max(v1.x, v2.x),
			max(v1.y, v2.y),
			max(v1.z, v2.z),
			max(v1.w, v2.w)
		);
	}
	// Clamp
	void Vector4::Clamp(const Vector4& lower, const Vector4& upper)
	{
		// Fix X Values
		x = MacroClamp(x, lower.x, upper.x);
		// Fix Y Values
		y = MacroClamp(y, lower.y, upper.y);
		// Fix Y Values
		z = MacroClamp(z, lower.z, upper.z);
		// Fix W Values
		w = MacroClamp(w, lower.w, upper.w);
	}
	Vector4 Vector4::Clamp(Vector4 v, const Vector4& min, const Vector4& max)
	{
		v.Clamp(min, max);
		return v;
	}

	// Snap to nearest integer value
	Vector4 Vector4::Snap()
	{
		return Vector4(
			floor(x + 0.5f),
			floor(y + 0.5f),
			floor(z + 0.5f),
			floor(w + 0.5f)
		);
	}

	// Get Float pointer of values
	float* Vector4::GetStartPointer(void)
	{
		return &x;
	}
	float* Vector4::GetStartPointer(Vector4& v)
	{
		return &v.x;
	}

	// Vector to string
	std::string Vector4::ToString()
	{
		return std::to_string(x) + ' ' + std::to_string(y) + ' ' + std::to_string(z) + ' ' + std::to_string(w);
	}

	// Lerp
	Vector4 Vector4::Lerp(const Vector4& v, const float t)
	{
		return MacroLerp(*this, v, t);
	}
	Vector4 Vector4::Lerp(const Vector4& v1, const Vector4& v2, float t)
	{
		return MacroLerp(v1, v2, t);
	}
	// Slerp
	Vector4 Vector4::Slerp(const Vector4& v, const float t)
	{
		return Slerp(*this, v, t);
	}
	Vector4 Vector4::Slerp(const Vector4& v1, const Vector4& v2, float t)
	{
		float dot = Vector4::Dot(v1, v2);
		MacroClamp(dot, -1.0f, 1.0f);
		float theta = acosf(dot)*t;
		Vector4 RelativeVec = v2 - v1 * dot;
		RelativeVec.Normalize();

		return ((v1*cosf(theta)) + (RelativeVec*sinf(theta)));
	}
	// Nlerp
	Vector4 Vector4::Nlerp(const Vector4& v, const float t)
	{
		return Nlerp(*this, v, t);
	}
	Vector4 Vector4::Nlerp(const Vector4& v1, const Vector4& v2, float t)
	{
		return (MacroLerp(v1, v2, t)).Normalize();
	}


	// Special Operator Overloads
	float Vector4::operator[](unsigned int index) const
	{
		OUT_OF_RANGE_INDEX_CHECK(index);
		return (&x)[index];
	}
	float& Vector4::operator[](unsigned int index)
	{
		OUT_OF_RANGE_INDEX_CHECK(index);
		return (&x)[index];
	}
	Vector4 Vector4::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	// Basic Operator Overloading
	Vector4 Vector4::operator+ (const Vector2& v) const
	{
		return Vector4(x + v.x, y + v.y, z, w);
	}
	Vector4 Vector4::operator- (const Vector2& v) const
	{
		return Vector4(x - v.x, y - v.y, z, w);
	}
	Vector4 Vector4::operator* (const Vector2& v) const
	{
		return Vector4(x * v.x, y * v.y, z, w);
	}
	Vector4 Vector4::operator/ (const Vector2& v) const
	{
		return Vector4(x / v.x, y / v.y, z, w);
	}
	Vector4& Vector4::operator+= (const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector4& Vector4::operator-= (const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector4& Vector4::operator*= (const Vector2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector4& Vector4::operator/= (const Vector2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vector4 Vector4::operator+ (const Vector3& v) const
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w);
	}
	Vector4 Vector4::operator- (const Vector3& v) const
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w);
	}
	Vector4 Vector4::operator* (const Vector3& v) const
	{
		return Vector4(x * v.x, y * v.y, z * v.z, w);
	}
	Vector4 Vector4::operator/ (const Vector3& v) const
	{
		return Vector4(x / v.x, y / v.y, z / v.z, w);
	}
	Vector4& Vector4::operator+= (const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector4& Vector4::operator-= (const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector4& Vector4::operator*= (const Vector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector4& Vector4::operator/= (const Vector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vector4 Vector4::operator+ (const Vector4& v) const
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	Vector4 Vector4::operator- (const Vector4& v) const
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	Vector4 Vector4::operator* (const Vector4& v) const
	{
		return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}
	Vector4 Vector4::operator/ (const Vector4& v) const
	{
		return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}
	Vector4& Vector4::operator+= (const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	Vector4& Vector4::operator-= (const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	Vector4& Vector4::operator*= (const Vector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}
	Vector4& Vector4::operator/= (const Vector4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	// Operator Overloading with Float
	Vector4 operator+ (const Vector4& v, const float f)
	{
		return Vector4(v.x + f, v.y + f, v.z + f, v.w + f);
	}
	Vector4 operator- (const Vector4& v, const float f)
	{
		return Vector4(v.x - f, v.y - f, v.z - f, v.w - f);
	}
	Vector4 operator* (const Vector4& v, const float f)
	{
		return Vector4(v.x * f, v.y * f, v.z * f, v.w * f);
	}
	Vector4 operator/ (const Vector4& v, const float f)
	{
		float inva = 1.0f / f;
		return Vector4(v.x * inva, v.y * inva, v.z * inva, v.w * inva);
	}

	Vector4 operator+ (const float f, const Vector4& v)
	{
		return v + f;
	}
	Vector4 operator- (const float f, const Vector4& v)
	{
		return v - f;
	}
	Vector4 operator* (const float f, const Vector4& v)
	{
		return v * f;
	}
	Vector4 operator/ (const float f, const Vector4& v)
	{
		return v / f;
	}


	Vector4& Vector4::operator+= (float f)
	{
		x += f;
		y += f;
		z += f;
		w += f;
		return *this;
	}
	Vector4& Vector4::operator-= (float f)
	{
		x -= f;
		y -= f;
		z -= f;
		w -= f;
		return *this;
	}
	Vector4& Vector4::operator*= (float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}
	Vector4& Vector4::operator/= (float f)
	{
		VXL_ASSERT(f, "Vector3i dividing by 0");
		float inva = 1.0f / f;
		x *= inva;
		y *= inva;
		z *= inva;
		w *= inva;
		return *this;
	}

	Vector4& Vector4::operator= (const Vector2& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
	Vector4& Vector4::operator= (const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	Vector4& Vector4::operator= (const Vector4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}
	bool Vector4::operator== (const Vector4& v) const
	{
		return Compare(v);
	}
	bool Vector4::operator!= (const Vector4& v) const
	{
		return !Compare(v);
	}

	std::ostream &operator<<(std::ostream &stream, const Vector4& ob)
	{
		return (stream << '[' << ob.x << ' ' << ob.y << ' ' << ob.z << ' ' << ob.w << ']');
	}

	// Return Vec4
	Vector4 Vector4::GetVector4(int index1, int index2, int index3, int index4) const
	{
		return Vector4(*(&x + index1), *(&x + index2), *(&x + index3), *(&x + index4));
	}
	// Return Vec3
	Vector3 Vector4::GetVector3(int index1, int index2, int index3) const
	{
		return Vector3(*(&x + index1), *(&x + index2), *(&x + index3));
	}
	// Return Vec2
	Vector2 Vector4::GetVector2(int index1, int index2) const
	{
		return Vector2(*(&x + index1), *(&x + index2));
	}
}