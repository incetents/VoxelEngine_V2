// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector.h"
#include "MathCore.h"
#include "Lerp.h"

#include "../utilities/Macros.h"

#ifdef _DEBUG
#define OUT_OF_RANGE_INDEX_CHECK_LENGTH(index) VXL_ASSERT(index < Matrix4x4_Length, "Matrix4x4 index out of range")
#define OUT_OF_RANGE_INDEX_CHECK_SIZE(index) VXL_ASSERT(index < Matrix4x4_Size, "Matrix4x4 index out of range")
#else
#define OUT_OF_RANGE_INDEX_CHECK_LENGTH(index) __noop;
#define OUT_OF_RANGE_INDEX_CHECK_SIZE(index) __noop;
#endif

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

	// View Matrix [ROW MAJOR]
	Matrix4x4 Matrix4x4::LookAt(const Vector3& Eye, const Vector3& Forward, const Vector3& Right, const Vector3& Up)
	{
		return Matrix4x4
		(
			Right.x,	Right.y,	Right.z,	-Vector3::Dot(Right, Eye),
			Up.x,		Up.y,		Up.z,		-Vector3::Dot(Up, Eye),
			Forward.x,	Forward.y,	Forward.z,	-Vector3::Dot(Forward, Eye),
			0,			0,			0,			1
		);
	}

	// Perspective Matrix [ROW MAJOR]
	Matrix4x4 Matrix4x4::Perspective(float fovy, float aspect, float znear, float zfar)
	{
		float temp = 1.0f / tanf(Deg_To_Rad(fovy) * 0.5f);
		float zdiff = znear - zfar;

		return Matrix4x4
		(
			temp / aspect,	0,		0,							0,
			0,				temp,	0,							0,
			0,				0,		(zfar + znear) / (zdiff),	(2.0f * zfar * znear) / (zdiff),
			0,				0,		-1.0f,						0.0f
		);
	}
	// [ROW MAJOR]
	Matrix4x4 Matrix4x4::PerspectiveInverse(float fovy, float aspect, float znear, float zfar)
	{
		float inva_temp = tanf(Deg_To_Rad(fovy) * 0.5f);

		return Matrix4x4
		(
			aspect * inva_temp, 0,			0,										0,
			0,					inva_temp,	0,										0,
			0,					0,			1.0f,									-1.0f,
			0,					0,			(znear - zfar) / (2.0f * zfar * znear), +0.5f
		);
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
		M[0xB] = (2.0f * zfar * znear) / (znear - zfar);
	}
	void Matrix4x4::PerspectiveInverse_UpdateFov(Matrix4x4& M, float fovy, float aspect)
	{
		float inva_temp = tanf(Deg_To_Rad(fovy) * 0.5f);

		M[0x0] = aspect * inva_temp;
		M[0x5] = inva_temp;
	}
	void Matrix4x4::PerspectiveInverse_UpdateZ(Matrix4x4& M, float znear, float zfar)
	{
		M[0xE] = (znear - zfar) / (2.0f * zfar * znear);
	}

	// Orthographic Matrix [Row Major]
	Matrix4x4 Matrix4x4::Orthographic(float xmin, float xmax, float ymin, float ymax, float znear, float zfar)
	{
		float xdiff = xmax - xmin;
		float ydiff = ymax - ymin;
		float zdiff = zfar - znear;

		return Matrix4x4(
			2.0f / xdiff, 0, 0, -(xmax + xmin) / xdiff,
			0, 2.0f / ydiff, 0, -(ymax + ymin) / ydiff,
			0, 0, -2.0f / zdiff, -(zfar + znear) / zdiff,
			0, 0, 0, 1.0f
		);
	}
	// [ROW MAJOR]
	Matrix4x4 Matrix4x4::OrthographicInverse(float xmin, float xmax, float ymin, float ymax, float znear, float zfar)
	{
		return Matrix4x4(
			(xmax - xmin) * 0.5f, 0, 0, (xmax + xmin) * 0.5f,
			0, (ymax - ymin) * 0.5f, 0, (ymax + ymin) * 0.5f,
			0, 0, (zfar - znear) * -0.5f, (zfar + znear) * -0.5f,
			0, 0, 0, 1.0f
		);
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


	// Become Rotation Matrix (Radians)
	Matrix4x4 Matrix4x4::GetRotationX(float _radians, bool _CCW)
	{
		Matrix3x3 m = Matrix3x3::GetRotationX(_radians, _CCW);
		return Matrix4x4(m);
	}
	Matrix4x4 Matrix4x4::GetRotationY(float _radians, bool _CCW)
	{
		Matrix3x3 m = Matrix3x3::GetRotationY(_radians, _CCW);
		return Matrix4x4(m);
	}
	Matrix4x4 Matrix4x4::GetRotationZ(float _radians, bool _CCW)
	{
		Matrix3x3 m = Matrix3x3::GetRotationZ(_radians, _CCW);
		return Matrix4x4(m);
	}
	Matrix4x4 Matrix4x4::GetRotation(float _radians, const Vector3& _direction, bool _CCW)
	{
		Matrix3x3 m = Matrix3x3::GetRotation(_radians, _direction, _CCW);
		return Matrix4x4(m);
	}

	// Set Horizontal Line
	Matrix4x4& Matrix4x4::SetRow(unsigned int index, const Vector4& r)
	{
		OUT_OF_RANGE_INDEX_CHECK_LENGTH(index);
		_Val[index * Matrix4x4_Length + 0] = r.x;
		_Val[index * Matrix4x4_Length + 1] = r.y;
		_Val[index * Matrix4x4_Length + 2] = r.z;
		_Val[index * Matrix4x4_Length + 3] = r.w;
		return *this;
	}
	Matrix4x4& Matrix4x4::SetRow(Matrix4x4& m, unsigned int index, const Vector4& r)
	{
		return m.SetRow(index, r);
	}
	// Set Vertical Line
	Matrix4x4& Matrix4x4::SetColumn(unsigned int index, const Vector4& c)
	{
		OUT_OF_RANGE_INDEX_CHECK_LENGTH(index);
		_Val[(index + 0x0)] = c.x;
		_Val[(index + 0x4)] = c.y;
		_Val[(index + 0x8)] = c.z;
		_Val[(index + 0xC)] = c.w;
		return *this;
	}
	Matrix4x4& Matrix4x4::SetColumn(Matrix4x4& m, unsigned int index, const Vector4& c)
	{
		return m.SetColumn(index, c);
	}

	// Get Horizontal Line
	Vector4 Matrix4x4::GetRow(unsigned int index) const
	{
		OUT_OF_RANGE_INDEX_CHECK_LENGTH(index);
		return Vector4(
			_Val[0 + (index * Matrix4x4_Length)],
			_Val[1 + (index * Matrix4x4_Length)],
			_Val[2 + (index * Matrix4x4_Length)],
			_Val[3 + (index * Matrix4x4_Length)]
		);
	}
	Vector4 Matrix4x4::GetRow(const Matrix4x4& m, unsigned int index)
	{
		return m.GetRow(index);
	}
	// Get Vertical Line
	Vector4 Matrix4x4::GetColumn(unsigned int index) const
	{
		OUT_OF_RANGE_INDEX_CHECK_LENGTH(index);
		return Vector4(
			_Val[(index + 0x0)],
			_Val[(index + 0x4)],
			_Val[(index + 0x8)],
			_Val[(index + 0xC)]
		);
	}
	Vector4 Matrix4x4::GetColumn(const Matrix4x4& m, unsigned int index)
	{
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
		// ROW MAJOR
		_Val[0x3] = x;
		_Val[0x7] = y;
		_Val[0xB] = z;

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
	Matrix4x4 Matrix4x4::GetTranslate(float x, float y, float z)
	{
		Matrix4x4 M;
		M.OverrideCenter(x, y, z);
		return M;
	}
	Matrix4x4 Matrix4x4::GetTranslate(const Vector3& center)
	{
		return GetTranslate(center.x, center.y, center.z);
	}
	// Become Scale Matrix
	Matrix4x4 Matrix4x4::GetScale(float all)
	{
		Matrix4x4 M;
		M.OverrideScale(all, all, all);
		return M;
	}
	Matrix4x4 Matrix4x4::GetScale(float x, float y, float z)
	{
		Matrix4x4 M;
		M.OverrideScale(x, y, z);
		return M;
	}
	Matrix4x4 Matrix4x4::GetScale(const Vector3& scale)
	{
		return GetScale(scale.x, scale.y, scale.z);
	}
	// Become Scale and Translation Matrix
	Matrix4x4 Matrix4x4::GetTranslateScale(const Vector3& center, const Vector3& scale)
	{
		Matrix4x4 M;
		M.OverrideCenter(center);
		M.OverrideScale(scale);
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
	float& Matrix4x4::operator[](unsigned int index)
	{
		OUT_OF_RANGE_INDEX_CHECK_SIZE(index);
		return _Val[index];
	}
	float Matrix4x4::operator[](unsigned int index) const
	{
		OUT_OF_RANGE_INDEX_CHECK_SIZE(index);
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
		VXL_ASSERT(f != 0.f, "Matrix4x4 division by 0")
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
		_Val[0] = m[0];
		_Val[1] = m[1];
		_Val[2] = m[2];
		_Val[4] = m[3];
		_Val[5] = m[4];
		_Val[6] = m[5];
		_Val[8] = m[6];
		_Val[9] = m[7];
		_Val[10] = m[8];
		return *this;
	}
	Matrix4x4& Matrix4x4::operator= (const Matrix2x2& m)
	{
		_Val[0] = m._Val[0];
		_Val[1] = m._Val[1];
		_Val[3] = m._Val[2];
		_Val[4] = m._Val[3];
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