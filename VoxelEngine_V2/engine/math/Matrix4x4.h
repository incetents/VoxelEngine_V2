// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <string>

namespace Vxl
{
	class Matrix2x2;
	class Matrix3x3;
	class Vector2;
	class Vector3;
	class Vector4;
	class Degrees;
	class Radians;
	enum class Axis;
	enum class RotationDirection;

	class Matrix4x4
	{
	public:

		// Data
		float _Val[16];
		// 0 1 2 3
		// 4 5 6 7
		// 8 9 A B
		// C D E F

	public:
		// Identity Matrix
		static const Matrix4x4 Identity;

		// Constructors
		Matrix4x4(void);
		explicit Matrix4x4(const Matrix2x2& m);
		explicit Matrix4x4(const Matrix2x2& m, const Vector3& center);
		explicit Matrix4x4(const Matrix3x3& m);
		explicit Matrix4x4(const Matrix3x3& m, const Vector3& center);
		explicit Matrix4x4(Vector3* Rows);
		explicit Matrix4x4(Vector4* Rows);
		explicit Matrix4x4(const Vector3& Row1, const Vector3& Row2, const Vector3& Row3);
		explicit Matrix4x4(const Vector4& Row1, const Vector4& Row2, const Vector4& Row3, const Vector4& Row4);
		explicit Matrix4x4(const float Values[4][4]);
		explicit Matrix4x4(
			const float xx, const float xy, const float xz, const float xw,
			const float yx, const float yy, const float yz, const float yw,
			const float zx, const float zy, const float zz, const float zw,
			const float wx, const float wy, const float wz, const float ww);

		// Become default empty matrix
		void BeIdentity();

		// Become View Matrix
				Matrix4x4&    LookAt(Vector3 Eye, Vector3 Target, Vector3 Up);
		static	Matrix4x4  GetLookAt(Vector3 Eye, Vector3 Target, Vector3 Up);
		// Become Perspective Matrix
				Matrix4x4&    Perspective(float fovy, float aspect, float near, float far);
		static	Matrix4x4  GetPerspective(float fovy, float aspect, float near, float far);
		// Become Orthographic Matrix
				Matrix4x4&    Orthographic(float xmin, float xmax, float ymin, float ymax, float znear, float zfar);
		static	Matrix4x4  GetOrthographic(float xmin, float xmax, float ymin, float ymax, float znear, float zfar);

		// Special Sets
		Matrix4x4& OverrideCenter(const Vector3& center);
		Matrix4x4& OverrideCenter(float x, float y, float z);
		Matrix4x4&  OverrideScale(const Vector3& scale);
		Matrix4x4&  OverrideScale(float x, float y, float z);

		// Become Rotation Matrix (Degrees)
		static	Matrix4x4 RotationX(const Degrees&, RotationDirection = RotationDirection(1));
		static	Matrix4x4 RotationY(const Degrees&, RotationDirection = RotationDirection(1));
		static	Matrix4x4 RotationZ(const Degrees&, RotationDirection = RotationDirection(1));
		static	Matrix4x4 Rotation(const Degrees&, const Vector3&, RotationDirection = RotationDirection(1));
		// Become Rotation Matrix (Radians)
		static	Matrix4x4 RotationX(const Radians&, RotationDirection = RotationDirection(1));
		static	Matrix4x4 RotationY(const Radians&, RotationDirection = RotationDirection(1));
		static	Matrix4x4 RotationZ(const Radians&, RotationDirection = RotationDirection(1));
		static	Matrix4x4 Rotation(const Radians&, const Vector3&, RotationDirection = RotationDirection(1));

		// Set Horizontal Line
				Matrix4x4& SetRow(int index, const Vector4&);
		static	Matrix4x4& SetRow(Matrix4x4&, int index, const Vector4&);
		// Set Vertical Line
				Matrix4x4& SetColumn(int index, const Vector4&);
		static	Matrix4x4& SetColumn(Matrix4x4&, int index, const Vector4&);

		// Get Horizontal Line
				Vector4 GetRow(int index) const;
		static	Vector4 GetRow(const Matrix4x4&, int index);
		// Get Vertical Line
				Vector4 GetColumn(int index) const;
		static	Vector4 GetColumn(const Matrix4x4&, int index);

		// Determinant
				float Determinant() const;
		static	float Determinant(const Matrix4x4&);

		// Transpose
		Matrix4x4 Transpose() const;
		// Transpose Self
		Matrix4x4& TransposeSelf();

		// Inverse
		Matrix4x4 Inverse() const;
		// Inverse Self
		Matrix4x4& InverseSelf();

		// Multiply
				Matrix4x4 Multiply(const Matrix4x4&) const;
		static	Matrix4x4 Multiply(const Matrix4x4&, const Matrix4x4&);

		// Compare
				bool Compare(const Matrix4x4&) const;
		static	bool Compare(const Matrix4x4&, const Matrix4x4&);

		// Get Start Pointer
		inline const float* GetStartPointer()
		{
			return &_Val[0];
		}
		// Get Start Reference
		inline const float& GetStartReference() const
		{
			return _Val[0];
		}

		// Vector to string
		std::string ToString();

		// Lerp
				Matrix4x4 Lerp(const Matrix4x4&, float t) const;
		static	Matrix4x4 Lerp(const Matrix4x4&, const Matrix4x4&, float t);

		// Special Operator Overloads
		float& operator[](int);
		float operator[](int) const;
		Matrix4x4 operator-(void) const;

		// Matrix -- Vector Multiplication
		friend Vector4 operator*(const Matrix4x4& m, const Vector4& v);
		friend Vector3 operator*(const Matrix4x4& m, const Vector3& v);
		friend Vector2 operator*(const Matrix4x4& m, const Vector2& v);
		friend Vector4 operator*(const Vector4& v, const Matrix4x4& m);
		friend Vector3 operator*(const Vector3& v, const Matrix4x4& m);
		friend Vector2 operator*(const Vector2& v, const Matrix4x4& m);
		// Matrix -- Matrix Multiplication
		Matrix4x4 operator*(const Matrix4x4&) const;
		Matrix4x4& operator*=(const Matrix4x4&);

		// Operator Overloads
		Matrix4x4  operator+(const Matrix4x4&) const;
		Matrix4x4  operator-(const Matrix4x4&) const;
		Matrix4x4& operator+=(const Matrix4x4&);
		Matrix4x4& operator-=(const Matrix4x4&);

		Matrix4x4  operator+(const float) const;
		Matrix4x4  operator-(const float) const;
		Matrix4x4  operator*(const float) const;
		Matrix4x4  operator/(const float) const;

		Matrix4x4& operator+=(const float);
		Matrix4x4& operator-=(const float);
		Matrix4x4& operator*=(const float);
		Matrix4x4& operator/=(const float);

		// Operator Comparison Overloading
		Matrix4x4& operator= (const Matrix4x4&);
		Matrix4x4& operator= (const Matrix3x3&);
		Matrix4x4& operator= (const Matrix2x2&);
		bool operator== (const Matrix4x4&) const;
		bool operator!= (const Matrix4x4&) const;

		// Cout Overload
		friend std::ostream &operator<<(std::ostream &stream, const Matrix4x4&);
	};

	// Shorthand
	typedef Matrix4x4 Mat4;
}