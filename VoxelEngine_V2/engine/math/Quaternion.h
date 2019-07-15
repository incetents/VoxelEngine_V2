// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

namespace Vxl
{
	class Matrix3x3;
	class Matrix4x4;
	class Degrees;
	class Radians;
	class RotationValue;
	enum class RotationDirection;

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
		explicit Quaternion(const Degrees& rotation, const Vector3& axis, RotationDirection = RotationDirection(0));
		explicit Quaternion(const Radians& rotation, const Vector3& axis, RotationDirection = RotationDirection(0));

		// Become default empty quaternion
		void Identity();

		// Create Quaternion from euler angles
		static Quaternion ToQuaternion_ZYX(const Degrees& yaw, const Degrees& pitch, const Degrees& roll);
		static Quaternion ToQuaternion_ZYX(const Radians& yaw, const Radians& pitch, const Radians& roll);

		static Quaternion ToQuaternion_YXZ(const Degrees& yaw, const Degrees& pitch, const Degrees& roll);
		static Quaternion ToQuaternion_YXZ(const Radians& yaw, const Radians& pitch, const Radians& roll);
		// Convert Quaternion to euler angles
		static void ToEuler(const Quaternion& q, Degrees& roll, Degrees& pitch, Degrees& yaw);
		static void ToEuler(const Quaternion& q, Radians& roll, Radians& pitch, Radians& yaw);

		// Get Matrix version of Quaternion
		Matrix3x3 GetMatrix3x3() const;
		Matrix4x4 GetMatrix4x4() const;
		Matrix4x4 GetMatrix4x4(const Vector3& center) const;

		// Create Angle Axis
		static Quaternion AngleAxis(const Degrees& rotation, const Vector3& axis, RotationDirection = RotationDirection(0));
		static Quaternion AngleAxis(const Radians& rotation, const Vector3& axis, RotationDirection = RotationDirection(0));

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