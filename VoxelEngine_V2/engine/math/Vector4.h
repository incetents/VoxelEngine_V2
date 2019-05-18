// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <string>

namespace Vxl
{
	class Vector2;
	class Vector2i;
	class Vector3;
	class Vector3i;
	class Matrix4x4;
	class Radians;
	class Degrees;
	enum class RotationDirection;
	enum class Axis;

	class Vector4i
	{
	public:
		// Data
		int x, y, z, w;

		// Is Empty
		inline bool Is_Empty()
		{
			return !(x && y && z);
		}

		// Constructors
		Vector4i(void);
		explicit Vector4i(int all);
		explicit Vector4i(Vector2i v);
		explicit Vector4i(Vector3i v);
		explicit Vector4i(Radians all);
		explicit Vector4i(Degrees all);
		explicit Vector4i(int _x, int _y, int _z, int _w);
		explicit Vector4i(Radians _x, Radians _y, Radians _z, Radians _w);
		explicit Vector4i(Degrees _x, Degrees _y, Degrees _z, Degrees _w);
		explicit Vector4i(Vector3i v, int _w);
	};

	class Vector4
	{
	public:
		// Data
		float x, y, z, w;

		// Is Empty
		inline bool Is_Empty()
		{
			return !(x && y && z);
		}

		// Constructors
		Vector4(void);
		explicit Vector4(float all);
		explicit Vector4(Vector2 v);
		explicit Vector4(Vector3 v);
		explicit Vector4(Radians all);
		explicit Vector4(Degrees all);
		explicit Vector4(float _x, float _y, float _z, float _w);
		explicit Vector4(Radians _x, Radians _y, Radians _z, Radians _w);
		explicit Vector4(Degrees _x, Degrees _y, Degrees _z, Degrees _w);
		explicit Vector4(Vector3 v, float _w);

		// Cast
		explicit operator Vector2();
		explicit operator Vector3();

		// Swap Values
		Vector4& SwapXY();
		Vector4& SwapXZ();
		Vector4& SwapXW();
		Vector4& SwapYZ();
		Vector4& SwapYW();
		Vector4& SwapZW();

		// Inverse
		Vector4 Inverse() const;
		static Vector4 Inverse(const Vector4&);

		// Compare
		bool Compare(const Vector4&) const;
		static bool Compare(const Vector4&, const Vector4&);

		// Compare (0.01 precision)
		inline bool CompareFuzzy(const Vector4&) const;
		static bool CompareFuzzy(const Vector4&, const Vector4&);

		// Normalize Self
		Vector4& NormalizeSelf();
		// Normalize
		Vector4 Normalize() const;
		static Vector4 Normalize(const Vector4&);
		// Magnitude
		float Magnitude() const;
		static float Magnitude(const Vector4&);
		// Length
		float Length() const;
		static float Length(const Vector4&);
		// Length Squared
		float LengthSqr() const;
		static float LengthSqr(const Vector4&);

		// Distance
		float Distance(const Vector4&);
		static float Distance(const Vector4&, const Vector4&);

		// Dot Product
		float Dot() const;
		float Dot(const Vector4&) const;
		static float Dot(const Vector4&, const Vector4&);

		// Reflect Vector
		Vector4 Reflect(const Vector4& Normal);
		static Vector4 Reflect(const Vector4& Incident, const Vector4& Normal);
		// Refract Vector
		Vector4 Refract(const Vector4& Normal, float index);
		static Vector4 Refract(const Vector4& Incident, const Vector4& Normal, float index);

		// Project Vector
		Vector4 Project(const Vector4&) const;
		static Vector4 Project(const Vector4&, const Vector4&);
		// Projection Length
		float ProjectLength(const Vector4&) const;
		static float ProjectLength(const Vector4&, const Vector4&);

		// Cross Product
		Vector4 Cross(const Vector4&) const;
		static Vector4 Cross(const Vector4&, const Vector4&);

		// Get Angle Degrees
		Degrees GetAngleDegrees(Vector4&);
		static Degrees GetAngleDegrees(Vector4&, Vector4&);
		// Get Angle Radians
		Radians GetAngleRadians(Vector4&);
		static Radians GetAngleRadians(Vector4&, Vector4&);

		// Random Vector3 Unit Vector
		static Vector4 GetRandomUnitVector();

		// Fix Denormals (Fix Numbers close to zero)
		bool FixDenormals();
		static bool FixDenormals(Vector4&);

		// Resize Vector
		Vector4 Resize(float length);

		// Abs
		Vector4& Abs();
		static Vector4 Abs(const Vector4&);
		// Min
		Vector4& Min(const Vector4&);
		static Vector4 Min(const Vector4&, const Vector4&);
		// Max
		Vector4& Max(const Vector4&);
		static Vector4 Max(const Vector4&, const Vector4&);
		// Clamp
		void Clamp(const Vector4& min, const Vector4& max);
		static Vector4 Clamp(Vector4, const Vector4& min, const Vector4& max);

		// Snap to nearest integer value
		Vector4 Snap(void);

		// Get Float pointer of values
		inline float* GetStartPointer(void);
		inline static float* GetStartPointer(Vector4&);

		// Vector to string
		std::string ToString();

		// Lerp
		Vector4 Lerp(const Vector4&, const float t);
		static Vector4 Lerp(const Vector4&, const Vector4&, float t);
		// Slerp
		Vector4 Slerp(const Vector4&, const float t);
		static Vector4 Slerp(const Vector4&, const Vector4&, float t);
		// Nlerp
		Vector4 Nlerp(const Vector4&, const float t);
		static Vector4 Nlerp(const Vector4&, const Vector4&, float t);

		// Special Operator Overloads
		float operator[](const int) const;
		float& operator[](const int);
		Vector4 operator-() const;

		// Basic Operator Overloading
		Vector4 operator+ (const Vector2&) const;
		Vector4 operator- (const Vector2&) const;
		Vector4 operator* (const Vector2&) const;
		Vector4 operator/ (const Vector2&) const;
		Vector4& operator+= (const Vector2&);
		Vector4& operator-= (const Vector2&);
		Vector4& operator*= (const Vector2&);
		Vector4& operator/= (const Vector2&);

		Vector4 operator+ (const Vector3&) const;
		Vector4 operator- (const Vector3&) const;
		Vector4 operator* (const Vector3&) const;
		Vector4 operator/ (const Vector3&) const;
		Vector4& operator+= (const Vector3&);
		Vector4& operator-= (const Vector3&);
		Vector4& operator*= (const Vector3&);
		Vector4& operator/= (const Vector3&);

		Vector4 operator+ (const Vector4&) const;
		Vector4 operator- (const Vector4&) const;
		Vector4 operator* (const Vector4&) const;
		Vector4 operator/ (const Vector4&) const;
		Vector4& operator+= (const Vector4&);
		Vector4& operator-= (const Vector4&);
		Vector4& operator*= (const Vector4&);
		Vector4& operator/= (const Vector4&);

		// Operator Overloading with Float
		friend Vector4 operator+ (const Vector4&, const float);
		friend Vector4 operator- (const Vector4&, const float);
		friend Vector4 operator* (const Vector4&, const float);
		friend Vector4 operator/ (const Vector4&, const float);

		friend Vector4 operator+ (const float, const Vector4&);
		friend Vector4 operator- (const float, const Vector4&);
		friend Vector4 operator* (const float, const Vector4&);
		friend Vector4 operator/ (const float, const Vector4&);

		Vector4& operator+= (const float);
		Vector4& operator-= (const float);
		Vector4& operator*= (const float);
		Vector4& operator/= (const float);

		// Comparison Operator Overloading
		Vector4& operator= (const Vector2&);
		Vector4& operator= (const Vector3&);
		Vector4& operator= (const Vector4&);
		bool operator== (const Vector4&) const;
		bool operator!= (const Vector4&) const;

		// Cout Overload
		friend std::ostream &operator<<(std::ostream &stream, const Vector4& ob);

		// Return Vec4
		Vector4 GetVector4(int index1, int index2, int index3, int index4) const;
		// Return Vec3
		Vector3 GetVector3(int index1, int index2, int index3) const;
		// Return Vec2
		Vector2 GetVector2(int index1, int index2) const;

	};

	// Shorthand
	typedef Vector4 Vec4;
	typedef Vector4 vec4;
	typedef Vector4i Vec4i;
	typedef Vector4i vec4i;
}