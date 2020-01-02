// Copyright (c) 2020 Emmanuel Lajeunesse
#pragma once

#include "Vector.h"

namespace Vxl
{
	class Matrix3x3;
	class Matrix4x4;
	class RotationValue;

	class Quaternion
	{
	private:
		// Identity Quaternion
		static const Quaternion _Identity;

	public:

		// Values
		float x, y, z, w;

		// Create Angle Axis Quaternion
		explicit Quaternion();
		explicit Quaternion(const float _x, const float _y, const float _z, const float _w);
		explicit Quaternion(float _radians, const Vector3& _axis, bool _counterClockWise = true);

		// Become default empty quaternion
		void Identity();

		// Create Quaternion from euler angles
		static Quaternion ToQuaternion_ZYX(float _yaw, float _pitch, float _roll);
		static Quaternion ToQuaternion_YXZ(float _yaw, float _pitch, float _roll);
		// Convert Quaternion to euler angles
		static void ToEuler_ZYX(const Quaternion& q, float& _roll, float& _pitch, float& _yaw);

		// Get Matrix version of Quaternion
		Matrix3x3 GetMatrix3x3() const;
		Matrix4x4 GetMatrix4x4() const;
		Matrix4x4 GetMatrix4x4(const Vector3& center) const;

		// Create Angle Axis
		static Quaternion AngleAxis(float _radians, const Vector3& axis, bool _counterClockWise = true);

		// Create Angle Axis from Matrix
		static Quaternion AngleAxis(const Matrix3x3& matrix);
		static Quaternion AngleAxis(const Matrix4x4& matrix);

		// Inverse
		Quaternion& InverseSelf();
		Quaternion  Inverse() const;
		static Quaternion  Inverse(const Quaternion&);

		// Normalize
		Quaternion& NormalizeSelf();
		Quaternion  Normalize();
		static Quaternion  Normalize(const Quaternion&);

		// Dot Product
		inline float Dot();
		inline float Dot(const Quaternion&);
		static float Dot(const Quaternion&, const Quaternion&);

		// Special Operator Overloads
		float				operator[](const unsigned int) const;
		float&				operator[](const unsigned int);
		Quaternion			operator-();
		// Operator Overload (Math)
		Quaternion			operator+(const Quaternion&) const;
		Quaternion			operator-(const Quaternion&) const;
		Quaternion			operator*(const Quaternion&) const;
		Quaternion			operator*(const float) const;
		// Rotate Vector
		friend Vector3		operator*(const Quaternion&, const Vector3&);
		// Operator Overload (Math Change)
		Quaternion&			operator*=(const Quaternion& other);
		Quaternion&			operator*=(float s);
		// Operator Comparison Overloading
		inline Quaternion&	operator= (const Quaternion&);
		//inline Quaternion& operator=(const Matrix4x4&);
		bool				operator== (const Quaternion&) const;
		bool				operator!= (const Quaternion&) const;
	};
}