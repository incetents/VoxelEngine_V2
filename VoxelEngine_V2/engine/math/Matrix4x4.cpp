// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "MathCore.h"
#include "Lerp.h"

namespace Vxl
{
	// Identity Matrix
	const Matrix4x4 Matrix4x4::Identity = Matrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	//Constructor
	Matrix4x4::Matrix4x4(void)
	{
		BeIdentity();
	}
	Matrix4x4::Matrix4x4(const Matrix2x2& m)
	{
		BeIdentity();

		_Val[0x0] = m[0];
		_Val[0x1] = m[1];
		_Val[0x4] = m[2];
		_Val[0x5] = m[3];
	}
	Matrix4x4::Matrix4x4(const Matrix2x2& m, const Vector3& center)
	{
		BeIdentity();

		_Val[0x0] = m[0];
		_Val[0x1] = m[1];
		_Val[0x4] = m[2];
		_Val[0x5] = m[3];

		OverrideCenter(center);
	}
	Matrix4x4::Matrix4x4(const Matrix3x3& m)
	{
		BeIdentity();

		_Val[0x0] = m[0];
		_Val[0x1] = m[1];
		_Val[0x2] = m[2];

		_Val[0x4] = m[3];
		_Val[0x5] = m[4];
		_Val[0x6] = m[5];

		_Val[0x8] = m[6];
		_Val[0x9] = m[7];
		_Val[0xA] = m[8];
	}
	Matrix4x4::Matrix4x4(const Matrix3x3& m, const Vector3& center)
	{
		BeIdentity();

		_Val[0x0] = m[0];
		_Val[0x1] = m[1];
		_Val[0x2] = m[2];

		_Val[0x4] = m[3];
		_Val[0x5] = m[4];
		_Val[0x6] = m[5];

		_Val[0x8] = m[6];
		_Val[0x9] = m[7];
		_Val[0xA] = m[8];

		OverrideCenter(center);
	}
	// Create mat4 from vec3 rows
	Matrix4x4::Matrix4x4(const Vector3& Row1, const Vector3& Row2, const Vector3& Row3)
	{
		*this = Matrix3x3(
			Row1,
			Row2,
			Row3
		);
	}
	// Create mat4 from vector3
	Matrix4x4::Matrix4x4(Vector3* Rows)
	{
		*this = Matrix3x3(
			*(Rows + 0),
			*(Rows + 1),
			*(Rows + 2)
		);
	}
	Matrix4x4::Matrix4x4(Vector4* Rows)
	{
		*this = Matrix4x4(
			*(Rows + 0),
			*(Rows + 1),
			*(Rows + 2),
			*(Rows + 3)
		);
	}
	Matrix4x4::Matrix4x4(const Vector4& Row1, const Vector4& Row2, const Vector4& Row3, const Vector4& Row4)
	{
		_Val[0x0] = Row1.x;
		_Val[0x1] = Row1.y;
		_Val[0x2] = Row1.z;
		_Val[0x3] = Row1.w;
		_Val[0x4] = Row2.x;
		_Val[0x5] = Row2.y;
		_Val[0x6] = Row2.z;
		_Val[0x7] = Row2.w;
		_Val[0x8] = Row3.x;
		_Val[0x9] = Row3.y;
		_Val[0xA] = Row3.z;
		_Val[0xB] = Row3.w;
		_Val[0xC] = Row4.x;
		_Val[0xD] = Row4.y;
		_Val[0xE] = Row4.z;
		_Val[0xF] = Row4.w;
	}
	Matrix4x4::Matrix4x4(
		const float xx, const float xy, const float xz, const float xw,
		const float yx, const float yy, const float yz, const float yw,
		const float zx, const float zy, const float zz, const float zw,
		const float wx, const float wy, const float wz, const float ww)
	{
		_Val[0x0] = xx;
		_Val[0x1] = xy;
		_Val[0x2] = xz;
		_Val[0x3] = xw;
		_Val[0x4] = yx;
		_Val[0x5] = yy;
		_Val[0x6] = yz;
		_Val[0x7] = yw;
		_Val[0x8] = zx;
		_Val[0x9] = zy;
		_Val[0xA] = zz;
		_Val[0xB] = zw;
		_Val[0xC] = wx;
		_Val[0xD] = wy;
		_Val[0xE] = wz;
		_Val[0xF] = ww;
	}
	Matrix4x4::Matrix4x4(const float Values[4][4])
	{
		_Val[0x0] = Values[0][0];
		_Val[0x1] = Values[0][1];
		_Val[0x2] = Values[0][2];
		_Val[0x3] = Values[0][3];
		_Val[0x4] = Values[1][0];
		_Val[0x5] = Values[1][1];
		_Val[0x6] = Values[1][2];
		_Val[0x7] = Values[1][3];
		_Val[0x8] = Values[2][0];
		_Val[0x9] = Values[2][1];
		_Val[0xA] = Values[2][2];
		_Val[0xB] = Values[2][3];
		_Val[0xC] = Values[3][0];
		_Val[0xD] = Values[3][1];
		_Val[0xE] = Values[3][2];
		_Val[0xF] = Values[3][3];
	}

	// Identity
	void Matrix4x4::BeIdentity()
	{
		*this = Matrix4x4::Identity;
	}

	// View Matrix
	Matrix4x4 Matrix4x4::LookAt(Vector3 Eye, Vector3 Target, Vector3 Up = Vector3(0, 1, 0))
	{
		Vector3 f = Vector3::Normalize(Target - Eye);
		Vector3 s = Vector3::Normalize(Vector3::Cross(Up, f));
		Vector3 u = Vector3::Cross(f, s);

		return Matrix4x4
		(
			s.x, u.x, f.x, 0,
			s.y, u.y, f.y, 0,
			s.z, u.z, f.z, 0,
			-Vector3::Dot(s, Eye), -Vector3::Dot(u, Eye), -Vector3::Dot(f, Eye), 1.f
		);
	}

	// Perspective Matrix
	Matrix4x4 Matrix4x4::Perspective(float fovy, float aspect, float znear, float zfar)
	{
		Matrix4x4 M;

		float temp = 1.0f / tanf(Deg_To_Rad(fovy) * 0.5f);
		
		M[0x0] = temp / aspect;
		M[0x5] = temp;
		M[0xA] = (zfar + znear) / (znear - zfar);
		M[0xB] = -1.0f;
		M[0xE] = (2.0f * zfar * znear) / (znear - zfar);
		M[0xF] = 0.0f;

		return M;
	}
	Matrix4x4 Matrix4x4::PerspectiveInverse(float fovy, float aspect, float znear, float zfar)
	{
		Matrix4x4 M;

		float temp = 1.0f / tanf(Deg_To_Rad(fovy) * 0.5f);

		M[0x0] = aspect / temp;
		M[0x5] = 1.0f / temp;
		M[0xA] = 1.0f;
		M[0xB] = (znear - zfar) / (2.0f * zfar * znear);
		M[0xE] = -1.0f;
		M[0xF] = +0.5f;

		return M;
	}
	void Matrix4x4::Perspective_UpdateFov(Matrix4x4& M, float fovy, float aspect)
	{
		float temp = 1.0f / tanf(Deg_To_Rad(fovy) * 0.5f);

		M[0x0] = temp / aspect;
		M[0x5] = temp;
	}
	void Matrix4x4::Perspective_UpdateZ(Matrix4x4& M, float znear, float zfar)
	{
		M[0xA] = (zfar + znear) / (znear - zfar);
		M[0xE] = (2.0f * zfar * znear) / (znear - zfar);
	}
	void Matrix4x4::PerspectiveInverse_UpdateFov(Matrix4x4& M, float fovy, float aspect)
	{
		float temp = 1.0f / tanf(Deg_To_Rad(fovy) * 0.5f);

		M[0x0] = aspect / temp;
		M[0x5] = 1.0f / temp;
	}
	void Matrix4x4::PerspectiveInverse_UpdateZ(Matrix4x4& M, float znear, float zfar)
	{
		M[0xB] = (znear - zfar) / (2.0f * zfar * znear);
	}

	// Orthographic Matrix
	Matrix4x4 Matrix4x4::Orthographic(float xmin, float xmax, float ymin, float ymax, float znear, float zfar)
	{
		Matrix4x4 M;

		M[0x0] = +2.0f / (xmax - xmin);
		M[0x5] = +2.0f / (ymax - ymin);
		M[0xA] = -2.0f / (zfar - znear);
		M[0xC] = -(xmax + xmin) / (xmax - xmin);
		M[0xD] = -(ymax + ymin) / (ymax - ymin);
		M[0xE] = -(zfar + znear) / (zfar - znear);
		M[0xF] = 1.0f;

		return M;
	}
	Matrix4x4 Matrix4x4::OrthographicInverse(float xmin, float xmax, float ymin, float ymax, float znear, float zfar)
	{
		Matrix4x4 M;

		M[0x0] = (xmax - xmin)  / +2.0f;
		M[0x5] = (ymax - ymin)  / +2.0f;
		M[0xA] = (zfar - znear) / -2.0f;
		M[0xC] = (xmax + xmin)  / +2.0f;
		M[0xD] = (ymax + ymin)  / +2.0f;
		M[0xE] = (zfar + znear) / -2.0f;
		M[0xF] = 1.0f;

		return M;
	}
	void Matrix4x4::Orthographic_UpdateX(Matrix4x4& M, float xmin, float xmax)
	{
		M[0x0] = +2.0f / (xmax - xmin);
		M[0xC] = -(xmax + xmin) / (xmax - xmin);
	}
	void Matrix4x4::Orthographic_UpdateY(Matrix4x4& M, float ymin, float ymax)
	{
		M[0x5] = +2.0f / (ymax - ymin);
		M[0xD] = -(ymax + ymin) / (ymax - ymin);
	}
	void Matrix4x4::Orthographic_UpdateZ(Matrix4x4& M, float znear, float zfar)
	{
		M[0xA] = -2.0f / (zfar - znear);
		M[0xE] = -(zfar + znear) / (zfar - znear);
	}
	void Matrix4x4::OrthographicInverse_UpdateX(Matrix4x4& M, float xmin, float xmax)
	{
		M[0x0] = (xmax - xmin) / +2.0f;
		M[0xC] = (xmax + xmin) / +2.0f;
	}
	void Matrix4x4::OrthographicInverse_UpdateY(Matrix4x4& M, float ymin, float ymax)
	{
		M[0x5] = (ymax - ymin) / +2.0f;
		M[0xD] = (ymax + ymin) / +2.0f;
	}
	void Matrix4x4::OrthographicInverse_UpdateZ(Matrix4x4& M, float znear, float zfar)
	{
		M[0xA] = (zfar - znear) / -2.0f;
		M[0xE] = (zfar + znear) / -2.0f;
	}

	
	// Become Rotation Matrix (Degrees)
	Matrix4x4 Matrix4x4::RotationX(const Degrees& deg, RotationDirection rot)
	{
		return RotationX(Radians(deg), rot);
	}
	Matrix4x4 Matrix4x4::RotationY(const Degrees& deg, RotationDirection rot)
	{
		return RotationY(Radians(deg), rot);
	}
	Matrix4x4 Matrix4x4::RotationZ(const Degrees& deg, RotationDirection rot)
	{
		return RotationZ(Radians(deg), rot);
	}
	Matrix4x4 Matrix4x4::Rotation(const Degrees& deg, const Vector3& direction, RotationDirection rot)
	{
		return Rotation(Radians(deg), direction, rot);
	}
	// Become Rotation Matrix (Radians)
	Matrix4x4 Matrix4x4::RotationX(const Radians& rad, RotationDirection rot)
	{
		Matrix3x3 m = Matrix3x3::RotationX(rad, rot);
		return Matrix4x4(m);
	}
	Matrix4x4 Matrix4x4::RotationY(const Radians& rad, RotationDirection rot)
	{
		Matrix3x3 m = Matrix3x3::RotationY(rad, rot);
		return Matrix4x4(m);
	}
	Matrix4x4 Matrix4x4::RotationZ(const Radians& rad, RotationDirection rot)
	{
		Matrix3x3 m = Matrix3x3::RotationZ(rad, rot);
		return Matrix4x4(m);
	}
	Matrix4x4 Matrix4x4::Rotation(const Radians& rad, const Vector3& direction, RotationDirection rot)
	{
		Matrix3x3 m = Matrix3x3::Rotation(rad, direction, rot);
		return Matrix4x4(m);
	}

	// Set Horizontal Line
	Matrix4x4& Matrix4x4::SetRow(int index, const Vector4& r)
	{
		assert((index >= 0) && (index < Matrix4x4_Length));
		_Val[index * Matrix4x4_Length + 0] = r.x;
		_Val[index * Matrix4x4_Length + 1] = r.y;
		_Val[index * Matrix4x4_Length + 2] = r.z;
		_Val[index * Matrix4x4_Length + 3] = r.w;
		return *this;
	}
	Matrix4x4& Matrix4x4::SetRow(Matrix4x4& m, int index, const Vector4& r)
	{
		assert((index >= 0) && (index < Matrix4x4_Length));
		return m.SetRow(index, r);
	}
	// Set Vertical Line
	Matrix4x4& Matrix4x4::SetColumn(int index, const Vector4& c)
	{
		assert((index >= 0) && (index < Matrix4x4_Length));
		_Val[(index + 0x0)] = c.x;
		_Val[(index + 0x4)] = c.y;
		_Val[(index + 0x8)] = c.z;
		_Val[(index + 0xC)] = c.w;
		return *this;
	}
	Matrix4x4& Matrix4x4::SetColumn(Matrix4x4& m, int index, const Vector4& c)
	{
		assert((index >= 0) && (index < Matrix4x4_Length));
		return m.SetColumn(index, c);
	}

	// Get Horizontal Line
	Vector4 Matrix4x4::GetRow(int index) const
	{
		assert((index >= 0) && (index < Matrix4x4_Length));
		return Vector4(
			_Val[0 + (index * Matrix4x4_Length)],
			_Val[1 + (index * Matrix4x4_Length)],
			_Val[2 + (index * Matrix4x4_Length)],
			_Val[3 + (index * Matrix4x4_Length)]
		);
	}
	Vector4 Matrix4x4::GetRow(const Matrix4x4& m, int index)
	{
		assert((index >= 0) && (index < Matrix4x4_Length));
		return m.GetRow(index);
	}
	// Get Vertical Line
	Vector4 Matrix4x4::GetColumn(int index) const
	{
		assert((index >= 0) && (index < Matrix4x4_Length));
		return Vector4(
			_Val[(index + 0x0)],
			_Val[(index + 0x4)],
			_Val[(index + 0x8)],
			_Val[(index + 0xC)]
		);
	}
	Vector4 Matrix4x4::GetColumn(const Matrix4x4& m, int index)
	{
		assert((index >= 0) && (index < Matrix4x4_Length));
		return m.GetColumn(index);
	}

	// Special Sets
	Matrix4x4& Matrix4x4::OverrideCenter(const Vector3& center)
	{
		OverrideCenter(center.x, center.y, center.z);
		return *this;
	}
	Matrix4x4& Matrix4x4::OverrideCenter(float x, float y, float z)
	{
		_Val[0xC] = x;
		_Val[0xD] = y;
		_Val[0xE] = z;
		return *this;
	}
	Matrix4x4& Matrix4x4::OverrideScale(const Vector3& scale)
	{
		OverrideScale(scale.x, scale.y, scale.y);
		return *this;
	}
	Matrix4x4& Matrix4x4::OverrideScale(float x, float y, float z)
	{
		_Val[0x0] = x;
		_Val[0x5] = y;
		_Val[0xA] = z;
		return *this;
	}

	// Become Translation Matrix
	Matrix4x4 Matrix4x4::Translate(const Vector3& center)
	{
		return Translate(center.x, center.y, center.z);
	}
	Matrix4x4 Matrix4x4::Translate(float x, float y, float z)
	{
		Matrix4x4 M;
		M.OverrideCenter(x, y, z);
		return M;
	}
	// Become Scale Matrix
	Matrix4x4 Matrix4x4::Scale(const Vector3& center)
	{
		return Scale(center.x, center.y, center.z);
	}
	Matrix4x4 Matrix4x4::Scale(float x, float y, float z)
	{
		Matrix4x4 M;
		M.OverrideScale(x, y, z);
		return M;
	}

	// Determinant
	float Matrix4x4::Determinant() const
	{
		return
			(_Val[0x0] * _Val[0x5] * _Val[0xA] * _Val[0xF]) + (_Val[0x0] * _Val[0x6] * _Val[0xB] * _Val[0xD]) + (_Val[0x0] * _Val[0x7] * _Val[0x9] * _Val[0xE]) +
			(_Val[0x1] * _Val[0x4] * _Val[0xB] * _Val[0xE]) + (_Val[0x1] * _Val[0x6] * _Val[0x8] * _Val[0xF]) + (_Val[0x1] * _Val[0x7] * _Val[0xA] * _Val[0xC]) +
			(_Val[0x2] * _Val[0x4] * _Val[0x9] * _Val[0xF]) + (_Val[0x2] * _Val[0x5] * _Val[0xB] * _Val[0xC]) + (_Val[0x2] * _Val[0x7] * _Val[0x8] * _Val[0xD]) +
			(_Val[0x3] * _Val[0x4] * _Val[0xA] * _Val[0xD]) + (_Val[0x3] * _Val[0x5] * _Val[0x8] * _Val[0xE]) + (_Val[0x3] * _Val[0x6] * _Val[0x9] * _Val[0xC]) -
			(_Val[0x0] * _Val[0x5] * _Val[0xB] * _Val[0xE]) - (_Val[0x0] * _Val[0x6] * _Val[0x9] * _Val[0xF]) - (_Val[0x0] * _Val[0x7] * _Val[0xA] * _Val[0xD]) -
			(_Val[0x1] * _Val[0x4] * _Val[0xA] * _Val[0xF]) - (_Val[0x1] * _Val[0x6] * _Val[0xB] * _Val[0xC]) - (_Val[0x1] * _Val[0x7] * _Val[0x8] * _Val[0xE]) -
			(_Val[0x2] * _Val[0x4] * _Val[0xB] * _Val[0xD]) - (_Val[0x2] * _Val[0x5] * _Val[0x8] * _Val[0xF]) - (_Val[0x2] * _Val[0x7] * _Val[0x9] * _Val[0xC]) -
			(_Val[0x3] * _Val[0x4] * _Val[0x9] * _Val[0xE]) - (_Val[0x3] * _Val[0x5] * _Val[0xA] * _Val[0xC]) - (_Val[0x3] * _Val[0x6] * _Val[0x8] * _Val[0xD]);
	}
	float Matrix4x4::Determinant(const Matrix4x4& m)
	{
		return m.Determinant();
	}

	// Transpose
	Matrix4x4 Matrix4x4::Transpose() const
	{
		return Matrix4x4(
			_Val[0x0], _Val[0x4], _Val[0x8], _Val[0xC],
			_Val[0x1], _Val[0x5], _Val[0x9], _Val[0xD],
			_Val[0x2], _Val[0x6], _Val[0xA], _Val[0xE],
			_Val[0x3], _Val[0x7], _Val[0xB], _Val[0xF]);
	}
	// Transpose Self
	Matrix4x4& Matrix4x4::TransposeSelf()
	{
		*this = Transpose();

		return *this;
	}

	// Inverse
	Matrix4x4 Matrix4x4::Inverse() const
	{
		float det = Determinant();
		if (abs(det) < MATRIX_INVERSE_EPSILON)
			return *this;

		float inva = 1.0f / det;
		return Matrix4x4(
			_Val[0x6] * _Val[0xB] * _Val[0xD] - _Val[0x7] * _Val[0xA] * _Val[0xD] + _Val[0x7] * _Val[0x9] * _Val[0xE] - _Val[0x5] * _Val[0xB] * _Val[0xE] - _Val[0x6] * _Val[0x9] * _Val[0xF] + _Val[0x5] * _Val[0xA] * _Val[0xF],
			_Val[0x3] * _Val[0xA] * _Val[0xD] - _Val[0x2] * _Val[0xB] * _Val[0xD] - _Val[0x3] * _Val[0x9] * _Val[0xE] + _Val[0x1] * _Val[0xB] * _Val[0xE] + _Val[0x2] * _Val[0x9] * _Val[0xF] - _Val[0x1] * _Val[0xA] * _Val[0xF],
			_Val[0x2] * _Val[0x7] * _Val[0xD] - _Val[0x3] * _Val[0x6] * _Val[0xD] + _Val[0x3] * _Val[0x5] * _Val[0xE] - _Val[0x1] * _Val[0x7] * _Val[0xE] - _Val[0x2] * _Val[0x5] * _Val[0xF] + _Val[0x1] * _Val[0x6] * _Val[0xF],
			_Val[0x3] * _Val[0x6] * _Val[0x9] - _Val[0x2] * _Val[0x7] * _Val[0x9] - _Val[0x3] * _Val[0x5] * _Val[0xA] + _Val[0x1] * _Val[0x7] * _Val[0xA] + _Val[0x2] * _Val[0x5] * _Val[0xB] - _Val[0x1] * _Val[0x6] * _Val[0xB],
			_Val[0x7] * _Val[0xA] * _Val[0xC] - _Val[0x6] * _Val[0xB] * _Val[0xC] - _Val[0x7] * _Val[0x8] * _Val[0xE] + _Val[0x4] * _Val[0xB] * _Val[0xE] + _Val[0x6] * _Val[0x8] * _Val[0xF] - _Val[0x4] * _Val[0xA] * _Val[0xF],
			_Val[0x2] * _Val[0xB] * _Val[0xC] - _Val[0x3] * _Val[0xA] * _Val[0xC] + _Val[0x3] * _Val[0x8] * _Val[0xE] - _Val[0x0] * _Val[0xB] * _Val[0xE] - _Val[0x2] * _Val[0x8] * _Val[0xF] + _Val[0x0] * _Val[0xA] * _Val[0xF],
			_Val[0x3] * _Val[0x6] * _Val[0xC] - _Val[0x2] * _Val[0x7] * _Val[0xC] - _Val[0x3] * _Val[0x4] * _Val[0xE] + _Val[0x0] * _Val[0x7] * _Val[0xE] + _Val[0x2] * _Val[0x4] * _Val[0xF] - _Val[0x0] * _Val[0x6] * _Val[0xF],
			_Val[0x2] * _Val[0x7] * _Val[0x8] - _Val[0x3] * _Val[0x6] * _Val[0x8] + _Val[0x3] * _Val[0x4] * _Val[0xA] - _Val[0x0] * _Val[0x7] * _Val[0xA] - _Val[0x2] * _Val[0x4] * _Val[0xB] + _Val[0x0] * _Val[0x6] * _Val[0xB],
			_Val[0x5] * _Val[0xB] * _Val[0xC] - _Val[0x7] * _Val[0x9] * _Val[0xC] + _Val[0x7] * _Val[0x8] * _Val[0xD] - _Val[0x4] * _Val[0xB] * _Val[0xD] - _Val[0x5] * _Val[0x8] * _Val[0xF] + _Val[0x4] * _Val[0x9] * _Val[0xF],
			_Val[0x3] * _Val[0x9] * _Val[0xC] - _Val[0x1] * _Val[0xB] * _Val[0xC] - _Val[0x3] * _Val[0x8] * _Val[0xD] + _Val[0x0] * _Val[0xB] * _Val[0xD] + _Val[0x1] * _Val[0x8] * _Val[0xF] - _Val[0x0] * _Val[0x9] * _Val[0xF],
			_Val[0x1] * _Val[0x7] * _Val[0xC] - _Val[0x3] * _Val[0x5] * _Val[0xC] + _Val[0x3] * _Val[0x4] * _Val[0xD] - _Val[0x0] * _Val[0x7] * _Val[0xD] - _Val[0x1] * _Val[0x4] * _Val[0xF] + _Val[0x0] * _Val[0x5] * _Val[0xF],
			_Val[0x3] * _Val[0x5] * _Val[0x8] - _Val[0x1] * _Val[0x7] * _Val[0x8] - _Val[0x3] * _Val[0x4] * _Val[0x9] + _Val[0x0] * _Val[0x7] * _Val[0x9] + _Val[0x1] * _Val[0x4] * _Val[0xB] - _Val[0x0] * _Val[0x5] * _Val[0xB],
			_Val[0x6] * _Val[0x9] * _Val[0xC] - _Val[0x5] * _Val[0xA] * _Val[0xC] - _Val[0x6] * _Val[0x8] * _Val[0xD] + _Val[0x4] * _Val[0xA] * _Val[0xD] + _Val[0x5] * _Val[0x8] * _Val[0xE] - _Val[0x4] * _Val[0x9] * _Val[0xE],
			_Val[0x1] * _Val[0xA] * _Val[0xC] - _Val[0x2] * _Val[0x9] * _Val[0xC] + _Val[0x2] * _Val[0x8] * _Val[0xD] - _Val[0x0] * _Val[0xA] * _Val[0xD] - _Val[0x1] * _Val[0x8] * _Val[0xE] + _Val[0x0] * _Val[0x9] * _Val[0xE],
			_Val[0x2] * _Val[0x5] * _Val[0xC] - _Val[0x1] * _Val[0x6] * _Val[0xC] - _Val[0x2] * _Val[0x4] * _Val[0xD] + _Val[0x0] * _Val[0x6] * _Val[0xD] + _Val[0x1] * _Val[0x4] * _Val[0xE] - _Val[0x0] * _Val[0x5] * _Val[0xE],
			_Val[0x1] * _Val[0x6] * _Val[0x8] - _Val[0x2] * _Val[0x5] * _Val[0x8] + _Val[0x2] * _Val[0x4] * _Val[0x9] - _Val[0x0] * _Val[0x6] * _Val[0x9] - _Val[0x1] * _Val[0x4] * _Val[0xA] + _Val[0x0] * _Val[0x5] * _Val[0xA]
		) * inva;
	}
	// Inverse Self
	Matrix4x4& Matrix4x4::InverseSelf()
	{
		return (*this = this->Inverse());
	}

	// Multiply
	Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& m) const
	{
		Matrix4x4 Result;

		for (int y = 0; y < Matrix4x4_Length; y++)
		{
			for (int x = 0; x < Matrix4x4_Length; x++)
			{
				Result[x + y * Matrix4x4_Length] = Vector4::Dot(GetRow(y), m.GetColumn(x));
			}
		}

		return Result;
	}
	Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return m1.Multiply(m2);
	}

	// Compare
	bool Matrix4x4::Compare(const Matrix4x4& m) const
	{
		for (int i = 0; i < Matrix4x4_Size; i++)
		{
			if (_Val[i] != m._Val[i])
				return false;
		}

		return true;
	}
	bool Matrix4x4::Compare(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return m1.Compare(m2);
	}

	// To String
	std::string Matrix4x4::ToString()
	{
		return std::string(
			std::to_string(_Val[0x0]) + ' ' +
			std::to_string(_Val[0x1]) + ' ' +
			std::to_string(_Val[0x2]) + ' ' +
			std::to_string(_Val[0x3]) + ' ' +
			std::to_string(_Val[0x4]) + ' ' +
			std::to_string(_Val[0x5]) + ' ' +
			std::to_string(_Val[0x6]) + ' ' +
			std::to_string(_Val[0x7]) + ' ' +
			std::to_string(_Val[0x8]) + ' ' +
			std::to_string(_Val[0x9]) + ' ' +
			std::to_string(_Val[0xA]) + ' ' +
			std::to_string(_Val[0xB]) + ' ' +
			std::to_string(_Val[0xC]) + ' ' +
			std::to_string(_Val[0xD]) + ' ' +
			std::to_string(_Val[0xE]) + ' ' +
			std::to_string(_Val[0xF]) + ' '
		);
	}

	// Lerp
	Matrix4x4 Matrix4x4::Lerp(const Matrix4x4& m, float t) const
	{
		Matrix4x4 Result;

		for (int i = 0; i < Matrix4x4_Length; i++)
		{
			Result[i] = MacroLerp(_Val[i], m._Val[i], t);
		}

		return Result;
	}
	Matrix4x4 Matrix4x4::Lerp(const Matrix4x4& m1, const Matrix4x4& m2, float t)
	{
		return m1.Lerp(m2, t);
	}

	// Operator Overloads
	float& Matrix4x4::operator[](int index)
	{
		assert((index >= 0) && (index < Matrix4x4_Size));
		return _Val[index];
	}
	float Matrix4x4::operator[](int index) const
	{
		assert((index >= 0) && (index < Matrix4x4_Size));
		return _Val[index];
	}
	Matrix4x4 Matrix4x4::operator-() const
	{
		return Matrix4x4(
			-_Val[0x0],
			-_Val[0x1],
			-_Val[0x2],
			-_Val[0x3],
			-_Val[0x4],
			-_Val[0x5],
			-_Val[0x6],
			-_Val[0x7],
			-_Val[0x8],
			-_Val[0x9],
			-_Val[0xA],
			-_Val[0xB],
			-_Val[0xC],
			-_Val[0xD],
			-_Val[0xE],
			-_Val[0xF]
		);
	}

	// Matrix -- Vector Multiplication
	Vector4 operator*(const Matrix4x4& m, const Vector4& v)
	{
		return Vector4(
			m._Val[0x0] * v.x + m._Val[0x1] * v.y + m._Val[0x2] * v.z + m._Val[0x3] * v.w,
			m._Val[0x4] * v.x + m._Val[0x5] * v.y + m._Val[0x6] * v.z + m._Val[0x7] * v.w,
			m._Val[0x8] * v.x + m._Val[0x9] * v.y + m._Val[0xA] * v.z + m._Val[0xB] * v.w,
			m._Val[0xC] * v.x + m._Val[0xD] * v.y + m._Val[0xE] * v.z + m._Val[0xF] * v.w
		);
	}
	// Treats vector lilke Vec2(x,y,z,1)
	Vector3 operator*(const Matrix4x4& m, const Vector3& v)
	{
		return Vector3(
			m._Val[0x0] * v.x + m._Val[0x1] * v.y + m._Val[0x2] * v.z + m._Val[0x3],
			m._Val[0x4] * v.x + m._Val[0x5] * v.y + m._Val[0x6] * v.z + m._Val[0x7],
			m._Val[0x8] * v.x + m._Val[0x9] * v.y + m._Val[0xA] * v.z + m._Val[0xB]
		);
	}
	// Treats vector lilke Vec2(x,y,0,1)
	Vector2 operator*(const Matrix4x4& m, const Vector2& v)
	{
		return Vector2(
			m._Val[0x0] * v.x + m._Val[0x1] * v.y + m._Val[0x3],
			m._Val[0x4] * v.x + m._Val[0x5] * v.y + m._Val[0x7]
		);
	}

	Vector4 operator*(const Vector4& v, const Matrix4x4& m)
	{
		return Vector4(
			m._Val[0x0] * v.x + m._Val[0x1] * v.x + m._Val[0x2] * v.x + m._Val[0x3] * v.x,
			m._Val[0x4] * v.y + m._Val[0x5] * v.y + m._Val[0x6] * v.y + m._Val[0x7] * v.y,
			m._Val[0x8] * v.z + m._Val[0x9] * v.z + m._Val[0xA] * v.z + m._Val[0xB] * v.z,
			m._Val[0xC] * v.w + m._Val[0xD] * v.w + m._Val[0xE] * v.w + m._Val[0xF] * v.w
		);
	}
	Vector3 operator*(const Vector3& v, const Matrix4x4& m)
	{
		return Vector3(
			m._Val[0x0] * v.x + m._Val[0x1] * v.x + m._Val[0x2] * v.x + m._Val[0x3],
			m._Val[0x4] * v.y + m._Val[0x5] * v.y + m._Val[0x6] * v.y + m._Val[0x7],
			m._Val[0x8] * v.z + m._Val[0x9] * v.z + m._Val[0xA] * v.z + m._Val[0xB]
		);
	}
	Vector2 operator*(const Vector2& v, const Matrix4x4& m)
	{
		return Vector2(
			m._Val[0x0] * v.x + m._Val[0x1] * v.x + m._Val[0x2] * v.x + m._Val[0x3],
			m._Val[0x4] * v.y + m._Val[0x5] * v.y + m._Val[0x6] * v.y + m._Val[0x7]
		);
	}
	// Matrix -- Matrix Multiplication
	Matrix4x4  Matrix4x4::operator*(const Matrix4x4& m) const
	{
		return Multiply(m);
	}

	Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& m)
	{
		*this = Multiply(m);

		return *this;
	}


	// Operator Overloads
	Matrix4x4  Matrix4x4::operator+(const Matrix4x4& m) const
	{
		return Matrix4x4(
			_Val[0x0] + m._Val[0x0],
			_Val[0x1] + m._Val[0x1],
			_Val[0x2] + m._Val[0x2],
			_Val[0x3] + m._Val[0x3],
			_Val[0x4] + m._Val[0x4],
			_Val[0x5] + m._Val[0x5],
			_Val[0x6] + m._Val[0x6],
			_Val[0x7] + m._Val[0x7],
			_Val[0x8] + m._Val[0x8],
			_Val[0x9] + m._Val[0x9],
			_Val[0xA] + m._Val[0xA],
			_Val[0xB] + m._Val[0xB],
			_Val[0xC] + m._Val[0xC],
			_Val[0xD] + m._Val[0xD],
			_Val[0xE] + m._Val[0xE],
			_Val[0xF] + m._Val[0xF]
		);
	}

	Matrix4x4  Matrix4x4::operator-(const Matrix4x4& m) const
	{
		return Matrix4x4(
			_Val[0x0] - m._Val[0x0],
			_Val[0x1] - m._Val[0x1],
			_Val[0x2] - m._Val[0x2],
			_Val[0x3] - m._Val[0x3],
			_Val[0x4] - m._Val[0x4],
			_Val[0x5] - m._Val[0x5],
			_Val[0x6] - m._Val[0x6],
			_Val[0x7] - m._Val[0x7],
			_Val[0x8] - m._Val[0x8],
			_Val[0x9] - m._Val[0x9],
			_Val[0xA] - m._Val[0xA],
			_Val[0xB] - m._Val[0xB],
			_Val[0xC] - m._Val[0xC],
			_Val[0xD] - m._Val[0xD],
			_Val[0xE] - m._Val[0xE],
			_Val[0xF] - m._Val[0xF]
		);
	}

	Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& m)
	{
		for (int i = 0; i < Matrix4x4_Size; i++)
		{
			_Val[i] += m._Val[i];
		}

		return *this;
	}

	Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& m)
	{
		for (int i = 0; i < Matrix4x4_Size; i++)
		{
			_Val[i] -= m._Val[i];
		}

		return *this;
	}

	Matrix4x4  Matrix4x4::operator+(const float f) const
	{
		return Matrix4x4(
			_Val[0x0] + f,
			_Val[0x1] + f,
			_Val[0x2] + f,
			_Val[0x3] + f,
			_Val[0x4] + f,
			_Val[0x5] + f,
			_Val[0x6] + f,
			_Val[0x7] + f,
			_Val[0x8] + f,
			_Val[0x9] + f,
			_Val[0xA] + f,
			_Val[0xB] + f,
			_Val[0xC] + f,
			_Val[0xD] + f,
			_Val[0xE] + f,
			_Val[0xF] + f
		);
	}

	Matrix4x4  Matrix4x4::operator-(const float f) const
	{
		return Matrix4x4(
			_Val[0x0] - f,
			_Val[0x1] - f,
			_Val[0x2] - f,
			_Val[0x3] - f,
			_Val[0x4] - f,
			_Val[0x5] - f,
			_Val[0x6] - f,
			_Val[0x7] - f,
			_Val[0x8] - f,
			_Val[0x9] - f,
			_Val[0xA] - f,
			_Val[0xB] - f,
			_Val[0xC] - f,
			_Val[0xD] - f,
			_Val[0xE] - f,
			_Val[0xF] - f
		);
	}

	Matrix4x4  Matrix4x4::operator*(const float f) const
	{
		return Matrix4x4(
			_Val[0x0] * f,
			_Val[0x1] * f,
			_Val[0x2] * f,
			_Val[0x3] * f,
			_Val[0x4] * f,
			_Val[0x5] * f,
			_Val[0x6] * f,
			_Val[0x7] * f,
			_Val[0x8] * f,
			_Val[0x9] * f,
			_Val[0xA] * f,
			_Val[0xB] * f,
			_Val[0xC] * f,
			_Val[0xD] * f,
			_Val[0xE] * f,
			_Val[0xF] * f
		);
	}

	Matrix4x4  Matrix4x4::operator/(const float f) const
	{
		float inva = 1.0f / f;

		return Matrix4x4(
			_Val[0x0] * inva,
			_Val[0x1] * inva,
			_Val[0x2] * inva,
			_Val[0x3] * inva,
			_Val[0x4] * inva,
			_Val[0x5] * inva,
			_Val[0x6] * inva,
			_Val[0x7] * inva,
			_Val[0x8] * inva,
			_Val[0x9] * inva,
			_Val[0xA] * inva,
			_Val[0xB] * inva,
			_Val[0xC] * inva,
			_Val[0xD] * inva,
			_Val[0xE] * inva,
			_Val[0xF] * inva
		);
	}

	Matrix4x4& Matrix4x4::operator+=(const float f)
	{
		for (int i = 0; i < Matrix4x4_Size; i++)
		{
			_Val[i] += f;
		}

		return *this;
	}

	Matrix4x4& Matrix4x4::operator-=(const float f)
	{
		for (int i = 0; i < Matrix4x4_Size; i++)
		{
			_Val[i] -= f;
		}

		return *this;
	}

	Matrix4x4& Matrix4x4::operator*=(const float f)
	{
		for (int i = 0; i < Matrix4x4_Size; i++)
		{
			_Val[i] *= f;
		}

		return *this;
	}

	Matrix4x4& Matrix4x4::operator/=(const float f)
	{
		float inva = 1.0f / f;

		for (int i = 0; i < Matrix4x4_Size; i++)
		{
			_Val[i] *= inva;
		}

		return *this;
	}

	// Operator Comparison Overloading
	Matrix4x4& Matrix4x4::operator= (const Matrix4x4& m)
	{
		for (int i = 0; i < Matrix4x4_Size; i++)
		{
			_Val[i] = m[i];
		}

		return *this;
	}
	Matrix4x4& Matrix4x4::operator= (const Matrix3x3& m)
	{
		for (int i = 0; i < Matrix3x3_Size; i++)
		{
			_Val[i] = m[i];
		}

		return *this;
	}
	Matrix4x4& Matrix4x4::operator= (const Matrix2x2& m)
	{
		for (int i = 0; i < Matrix2x2_Size; i++)
		{
			_Val[i] = m[i];
		}

		return *this;
	}
	bool Matrix4x4::operator== (const Matrix4x4& m) const
	{
		return Compare(m);
	}
	bool Matrix4x4::operator!= (const Matrix4x4& m) const
	{
		return !Compare(m);
	}

	std::ostream &operator<<(std::ostream &stream, const Matrix4x4& m)
	{
		Vector4 a = m.GetRow(0);
		Vector4 b = m.GetRow(1);
		Vector4 c = m.GetRow(2);
		Vector4 d = m.GetRow(3);
		return (stream << a << '\n' << b << '\n' << c << '\n' << d);
	}
}