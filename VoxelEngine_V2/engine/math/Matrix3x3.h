// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Vector.h"

#include <string>

namespace Vxl
{
	class Matrix2x2;
	class Matrix4x4;
	enum class Axis;

	class Matrix3x3
	{
		friend class Matrix2x2;
		friend class Matrix4x4;
	private:
		// Data
		float _Val[9];
		// 0 1 2
		// 3 4 5
		// 6 7 8

	public:
		// Identity Matrix
		static const Matrix3x3 Identity;

		// Constructors
		Matrix3x3(void);
		explicit Matrix3x3(const Matrix2x2& m);
		explicit Matrix3x3(const Matrix4x4& m);
		explicit Matrix3x3(const Vector3* Rows);
		explicit Matrix3x3(const Vector3& Row1, const Vector3& Row2, const Vector3& Row3);
		explicit Matrix3x3(const float Values[3][3]);
		explicit Matrix3x3(
			const float xx, const float xy, const float xz,
			const float yx, const float yy, const float yz,
			const float zx, const float zy, const float zz);

		// Become default empty matrix
		void BeIdentity();

		// Creation from Vectors
		static Matrix3x3 createFromRows(const Vector3& Row1, const Vector3& Row2, const Vector3& Row3);
		static Matrix3x3 createFromColumns(const Vector3& Column1, const Vector3& Column2, const Vector3& Column3);

		// Become Scale Matrix
		Matrix3x3& Scale(float size);
		Matrix3x3& Scale(float x, float y, float z);
		Matrix3x3& Scale(const Vector3& v);
		// Get Scale Matrix
		static Matrix3x3 GetScale(float size);
		static Matrix3x3 GetScale(float x, float y, float z);
		static Matrix3x3 GetScale(const Vector3& v);

		// Get Rotation Matrix (Radians)
		static Matrix3x3 GetRotationX(float _radians, bool _CCW = true);
		static Matrix3x3 GetRotationY(float _radians, bool _CCW = true);
		static Matrix3x3 GetRotationZ(float _radians, bool _CCW = true);
		static Matrix3x3 GetRotation(float _radians, const Vector3& _axis, bool _CCW = true);

		// Set Horizontal Line
				Matrix3x3& SetRow(unsigned int index, const Vector3&);
		static  Matrix3x3& SetRow(Matrix3x3&, unsigned int index, const Vector3&);
		// Set Vertical Line
				Matrix3x3& SetColumn(unsigned int index, const Vector3&);
		static  Matrix3x3& SetColumn(Matrix3x3&, unsigned int index, const Vector3&);

		// Get Horizontal Line
				Vector3 GetRow(unsigned int index) const;
		static  Vector3 GetRow(const Matrix3x3&, unsigned int index);
				Vector2 GetRowVec2(unsigned int index) const;
		static  Vector2 GetRowVec2(const Matrix3x3&, unsigned int index);
		// Get Vertical Line
				Vector3 GetColumn(unsigned int index) const;
		static  Vector3 GetColumn(const Matrix3x3&, unsigned int index);
				Vector2 GetColumnVec2(unsigned int index) const;
		static  Vector2 GetColumnVec2(const Matrix3x3&, unsigned int index);

		// Determinant
				float Determinant() const;
		static  float Determinant(const Matrix3x3&);

		// Transpose
				Matrix3x3 Transpose() const;
		static  Matrix3x3 Transpose(const Matrix3x3&);
		// Transpose Self
				Matrix3x3& TransposeSelf();
		static  Matrix3x3& TransposeSelf(Matrix3x3&);

		// Inverse
				Matrix3x3 Inverse() const;
		static  Matrix3x3 Inverse(const Matrix3x3&);
		// Inverse Self
				Matrix3x3& InverseSelf();
		static	Matrix3x3& InverseSelf(Matrix3x3&);

		// Multiply
				Matrix3x3 Multiply(const Matrix3x3&) const;
		static	Matrix3x3 Multiply(const Matrix3x3&, const Matrix3x3&);

		// Compare
				bool Compare(const Matrix3x3&) const;
		static	bool Compare(const Matrix3x3&, const Matrix3x3&);

		inline const float* GetStartPointer() const
		{
			return &_Val[0];
		}
		inline const float& GetStartReference() const
		{
			return _Val[0];
		}

		// Vector to string
		std::string ToString();

		// Lerp
		Matrix3x3 Lerp(const Matrix3x3&, float t) const;
		static	Matrix3x3 Lerp(const Matrix3x3&, const Matrix3x3&, float t);

		// Special Operator Overloads
		float& operator[](unsigned int);
		float operator[](unsigned int) const;
		Matrix3x3 operator-() const;

		// Matrix -- Vector Multiplication
		friend Vector3 operator*(const Matrix3x3& m, const Vector3& v);
		friend Vector3 operator*(const Vector3& v, const Matrix3x3& m);
		// Matrix -- Matrix Multiplication
		Matrix3x3  operator*(const Matrix3x3&) const;
		Matrix3x3& operator*=(const Matrix3x3&);

		// Operator Overloads
		Matrix3x3  operator+(const Matrix3x3&) const;
		Matrix3x3  operator-(const Matrix3x3&) const;
		Matrix3x3& operator+=(const Matrix3x3&);
		Matrix3x3& operator-=(const Matrix3x3&);
		Matrix3x3  operator+(const float) const;
		Matrix3x3  operator-(const float) const;
		Matrix3x3  operator*(const float) const;
		Matrix3x3  operator/(const float) const;
		Matrix3x3& operator+=(const float);
		Matrix3x3& operator-=(const float);
		Matrix3x3& operator*=(const float);
		Matrix3x3& operator/=(const float);

		// Operator Comparison Overloading
		Matrix3x3& operator= (const Matrix4x4&);
		Matrix3x3& operator= (const Matrix3x3&);
		Matrix3x3& operator= (const Matrix2x2&);
		bool operator== (const Matrix3x3&) const;
		bool operator!= (const Matrix3x3&) const;

		// Cout Overload
		friend std::ostream &operator<<(std::ostream &stream, const Matrix3x3&);
	};

	// Shorthand
	typedef Matrix3x3 Mat3;
}