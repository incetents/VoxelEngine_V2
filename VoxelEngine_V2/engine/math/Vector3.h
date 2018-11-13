// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <string>

namespace Vxl
{
	class Vector2;
	class Vector4;
	class Matrix3x3;
	class Radians;
	class Degrees;
	enum class RotationDirection;
	enum class Axis;

	class Vector3
	{
	public:
		// Common Vector Types
		static const Vector3 ZERO;
		static const Vector3 ONE;
		static const Vector3 N_ONE;
		static const Vector3 HALF;
		static const Vector3 UP;
		static const Vector3 DOWN;
		static const Vector3 FORWARD;
		static const Vector3 BACK;
		static const Vector3 LEFT;
		static const Vector3 RIGHT;

		// Data
		float x, y, z;

		// Is Empty
		inline bool Is_Empty()
		{
			if (x && y && z) return true;
			return false;
		}

		// Constructors
		Vector3(void);
		explicit Vector3(float all);
		explicit Vector3(Vector2 all);
		explicit Vector3(Vector4 all);
		explicit Vector3(Radians all);
		explicit Vector3(Degrees all);
		explicit Vector3(float _x, float _y, float _z);
		explicit Vector3(Radians _x, Radians _y, Radians _z);
		explicit Vector3(Degrees _x, Degrees _y, Degrees _z);
		explicit Vector3(Vector2 v, float z);
		explicit Vector3(float x, Vector2 v);

		// Inverse
		Vector3 Inverse() const;
		static Vector3 Inverse(const Vector3&);

		// Compare
		inline bool Compare(const Vector3&) const;
		static bool Compare(const Vector3&, const Vector3&);

		// Normalize Self
		Vector3& NormalizeSelf();
		// Normalize
		Vector3 Normalize() const;
		static Vector3 Normalize(const Vector3&);
		// Magnitude
		float Magnitude() const;
		static float Magnitude(const Vector3&);
		// Length
		float Length() const;
		static float Length(const Vector3&);
		// Length Squared
		float LengthSqr() const;
		static float LengthSqr(const Vector3&);

		// Dot Product
		float Dot() const;
		float Dot(const Vector3&) const;
		static float Dot(const Vector3&, const Vector3&);

		// Reflect Vector
		Vector3 Reflect(const Vector3& Normal);
		static Vector3 Reflect(const Vector3& Incident, const Vector3& Normal);

		// Project Vector
		Vector3 Project(const Vector3&) const;
		static Vector3 Project(const Vector3&, const Vector3&);
		// Projection Length
		float ProjectLength(const Vector3&) const;
		static float ProjectLength(const Vector3&, const Vector3&);

		// Cross Product
		Vector3 Cross(const Vector3&) const;
		static Vector3 Cross(const Vector3&, const Vector3&);

		// Rotate Degrees
		Vector3 RotateX(const Degrees&, RotationDirection = RotationDirection(1)) const;
		Vector3 RotateY(const Degrees&, RotationDirection = RotationDirection(1)) const;
		Vector3 RotateZ(const Degrees&, RotationDirection = RotationDirection(1)) const;
		Vector3 Rotate(const Degrees&, const Vector3&, RotationDirection = RotationDirection(1)) const;
		// Rotate Radians
		Vector3 RotateX(const Radians&, RotationDirection = RotationDirection(1)) const;
		Vector3 RotateY(const Radians&, RotationDirection = RotationDirection(1)) const;
		Vector3 RotateZ(const Radians&, RotationDirection = RotationDirection(1)) const;
		Vector3 Rotate(const Radians&, const Vector3&, RotationDirection = RotationDirection(1)) const;

		// Get Angle Degrees
		Degrees GetAngleDegrees(Vector3&);
		static Degrees GetAngleDegrees(Vector3&, Vector3&);
		// Get Angle Radians
		Radians GetAngleRadians(Vector3&);
		static Radians GetAngleRadians(Vector3&, Vector3&);

		// Random Vector3 Unit Vector
		static Vector3 GetRandomUnitVector();

		// Fix Denormals (Fix Numbers close to zero)
		bool FixDenormals();
		static bool FixDenormals(Vector3&);

		// Resize Vector
		Vector3 Resize(float length);

		// Clamp
		void Clamp(const Vector3& min, const Vector3& max);
		static Vector3 Clamp(Vector3, const Vector3& min, const Vector3& max);

		// Snap to nearest integer value
		Vector3 Snap(void);

		// Get Float pointer of values
		inline float* GetStartPointer(void);
		inline static float* GetStartPointer(Vector3&);

		// Vector to string
		std::string ToString();

		// Lerp
		Vector3 Lerp(const Vector3&, const float t);
		static Vector3 Lerp(const Vector3&, const Vector3&, float t);
		// Slerp
		Vector3 Slerp(const Vector3&, const float t);
		static Vector3 Slerp(const Vector3&, const Vector3&, float t);
		// Nlerp
		Vector3 Nlerp(const Vector3&, const float t);
		static Vector3 Nlerp(const Vector3&, const Vector3&, float t);

		// Special Operator Overloads
		float operator[](const int) const;
		float& operator[](const int);
		Vector3 operator-() const;

		// Basic Operator Overloading
		Vector3 operator+ (const Vector2& v) const;
		Vector3 operator- (const Vector2& v) const;
		Vector3 operator* (const Vector2& v) const;
		Vector3 operator/ (const Vector2& v) const;
		Vector3& operator+= (const Vector2& v);
		Vector3& operator-= (const Vector2& v);
		Vector3& operator*= (const Vector2& v);
		Vector3& operator/= (const Vector2& v);

		Vector3 operator+ (const Vector3& v) const;
		Vector3 operator- (const Vector3& v) const;
		Vector3 operator* (const Vector3& v) const;
		Vector3 operator/ (const Vector3& v) const;
		Vector3& operator+= (const Vector3& v);
		Vector3& operator-= (const Vector3& v);
		Vector3& operator*= (const Vector3& v);
		Vector3& operator/= (const Vector3& v);

		Vector3 operator+ (const Vector4&) const;
		Vector3 operator- (const Vector4&) const;
		Vector3 operator* (const Vector4&) const;
		Vector3 operator/ (const Vector4&) const;
		Vector3& operator+= (const Vector4&);
		Vector3& operator-= (const Vector4&);
		Vector3& operator*= (const Vector4&);
		Vector3& operator/= (const Vector4&);

		// Operator Overloading with Float
		friend Vector3 operator+ (const Vector3&, const float);
		friend Vector3 operator- (const Vector3&, const float);
		friend Vector3 operator* (const Vector3&, const float);
		friend Vector3 operator/ (const Vector3&, const float);

		friend Vector3 operator+ (const float, const Vector3&);
		friend Vector3 operator- (const float, const Vector3&);
		friend Vector3 operator* (const float, const Vector3&);
		friend Vector3 operator/ (const float, const Vector3&);

		Vector3& operator+= (const float);
		Vector3& operator-= (const float);
		Vector3& operator*= (const float);
		Vector3& operator/= (const float);

		// Operator Comparison Overloading
		Vector3& operator= (const Vector2&);
		Vector3& operator= (const Vector3&);
		Vector3& operator= (const Vector4&);
		bool operator== (const Vector3&) const;
		bool operator!= (const Vector3&) const;

		// Cout Overload
		friend std::ostream &operator<<(std::ostream &stream, const Vector3& ob);

		// Return Vec3
		Vector3 GetVector3(int index1, int index2, int index3) const;
		// Return Vec2
		Vector2 GetVector2(int index1, int index2) const;
	};

	// Shorthand
	typedef Vector3 Vec3;
	typedef Vector3 vec3;
}