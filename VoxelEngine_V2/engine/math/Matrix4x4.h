// Copyright (c) 2020 Emmanuel Lajeunesse
#pragma once

#include "Vector.h"

#include <string>

namespace Vxl
{
	class Matrix2x2;
	class Matrix3x3;
	enum class Axis;

	class Matrix4x4
	{
		friend class Matrix2x2;
		friend class Matrix3x3;
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

		// Creation from Vectors
		static Matrix4x4 createFromRows(const Vector4& Row1, const Vector4& Row2, const Vector4& Row3, const Vector4& Row4);
		static Matrix4x4 createFromColumns(const Vector4& Column1, const Vector4& Column2, const Vector4& Column3, const Vector4& Column4);

		// View Matrix
		static Matrix4x4  LookAt(const Vector3& Eye, const Vector3& Forward, const Vector3& Right, const Vector3& Up);
		// Perspective Matrix
		static Matrix4x4  Perspective(float fovy, float aspect, float near, float far);
		static Matrix4x4  PerspectiveInverse(float fovy, float aspect, float near, float far);
		static void Perspective_UpdateFov(Matrix4x4&, float fovy, float aspect);
		static void Perspective_UpdateZ(Matrix4x4&, float znear, float zfar);
		static void PerspectiveInverse_UpdateFov(Matrix4x4&, float fovy, float aspect);
		static void PerspectiveInverse_UpdateZ(Matrix4x4&, float znear, float zfar);
		// Orthographic Matrix
		static Matrix4x4  Orthographic(float xmin, float xmax, float ymin, float ymax, float znear, float zfar);
		static Matrix4x4  OrthographicInverse(float xmin, float xmax, float ymin, float ymax, float znear, float zfar);
		static void Orthographic_UpdateX(Matrix4x4&, float xmin, float xmax);
		static void Orthographic_UpdateY(Matrix4x4&, float ymin, float ymax);
		static void Orthographic_UpdateZ(Matrix4x4&, float zmin, float zmax);
		static void OrthographicInverse_UpdateX(Matrix4x4&, float xmin, float xmax);
		static void OrthographicInverse_UpdateY(Matrix4x4&, float ymin, float ymax);
		static void OrthographicInverse_UpdateZ(Matrix4x4&, float zmin, float zmax);

		// Special Sets
		Matrix4x4& OverrideCenter(const Vector3& center);
		Matrix4x4& OverrideCenter(float x, float y, float z);
		Matrix4x4& OverrideScale(const Vector3& scale);
		Matrix4x4& OverrideScale(float x, float y, float z);

		// Become Translation Matrix
		static Matrix4x4 GetTranslate(float x, float y, float z);
		static Matrix4x4 GetTranslate(const Vector3& center);
		// Become Scale Matrix
		static Matrix4x4 GetScale(float all);
		static Matrix4x4 GetScale(float x, float y, float z);
		static Matrix4x4 GetScale(const Vector3& scale);
		// Become Scale and Translation Matrix
		static Matrix4x4 GetTranslateScale(const Vector3& center, const Vector3& scale);

		// Become Rotation Matrix (Radians)
		static	Matrix4x4 GetRotationX(float _radians, bool _CCW = true);
		static	Matrix4x4 GetRotationY(float _radians, bool _CCW = true);
		static	Matrix4x4 GetRotationZ(float _radians, bool _CCW = true);
		static	Matrix4x4 GetRotation(float _radians, const Vector3& axis, bool _CCW = true);

		// Set Horizontal Line
				Matrix4x4& SetRow(unsigned int index, const Vector4&);
		static	Matrix4x4& SetRow(Matrix4x4&, unsigned int index, const Vector4&);
		// Set Vertical Line
				Matrix4x4& SetColumn(unsigned int index, const Vector4&);
		static	Matrix4x4& SetColumn(Matrix4x4&, unsigned int index, const Vector4&);

		// Get Horizontal Line
				Vector4 GetRow(unsigned int index) const;
		static	Vector4 GetRow(const Matrix4x4&, unsigned int index);
				Vector2 GetRowVec2(unsigned int index) const;
		static	Vector2 GetRowVec2(const Matrix4x4&, unsigned int index);
				Vector3 GetRowVec3(unsigned int index) const;
		static	Vector3 GetRowVec3(const Matrix4x4&, unsigned int index);
		// Get Vertical Line
				Vector4 GetColumn(unsigned int index) const;
		static	Vector4 GetColumn(const Matrix4x4&, unsigned int index);
				Vector2 GetColumnVec2(unsigned int index) const;
		static	Vector2 GetColumnVec2(const Matrix4x4&, unsigned int index);
				Vector3 GetColumnVec3(unsigned int index) const;
		static	Vector3 GetColumnVec3(const Matrix4x4&, unsigned int index);

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
		inline const float* GetStartPointer() const
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
		float& operator[](unsigned int);
		float operator[](unsigned int) const;
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