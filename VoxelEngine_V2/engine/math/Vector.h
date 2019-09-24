// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/MathCore.h"
#include "../math/Lerp.h"
#include "../utilities/Macros.h"

#include <string>

#define RadToDeg(x) (x * (180.0f / PI))
#define DegToRad(x) (x * (PI / 180.0f))

#ifdef _DEBUG
#define OUT_OF_RANGE_INDEX_CHECK_2(index) VXL_ASSERT(index < 2, "Vector3 index out of range")
#define OUT_OF_RANGE_INDEX_CHECK_3(index) VXL_ASSERT(index < 3, "Vector3 index out of range")
#define OUT_OF_RANGE_INDEX_CHECK_4(index) VXL_ASSERT(index < 2, "Vector3 index out of range")
#else
#define OUT_OF_RANGE_INDEX_CHECK_2(index) __noop
#define OUT_OF_RANGE_INDEX_CHECK_3(index) __noop
#define OUT_OF_RANGE_INDEX_CHECK_4(index) __noop
#endif

#define MacroDot2(x, y) x * x + y * y
#define MacroDot3(x, y, z) x * x + y * y + z * z
#define MacroDot4(x, y, z, w) x * x + y * y + z * z + w * w

namespace Vxl
{
	enum class RotationDirection;
	enum class Axis;

	template<typename Type>
	class _Vector2
	{
	public:
		// ~ Data ~ //
		Type x, y;

		// Is Empty
		inline bool IsEmpty()
		{
			return !(x || y);
		}
		inline bool IsNotEmpty()
		{
			return (x || y);
		}

		// Common Vector Types
		static const _Vector2<Type> ZERO;
		static const _Vector2<Type> ONE;
		static const _Vector2<Type> UP;
		static const _Vector2<Type> DOWN;
		static const _Vector2<Type> LEFT;
		static const _Vector2<Type> RIGHT;
		static const _Vector2<Type> HALF;

		// ~ Copy Constructors ~ //
		_Vector2<Type>& operator=(const _Vector2<Type>& v) { x = v.x; y = v.y; return *this; }

		// ~ Constructors ~ //
		explicit _Vector2(void) { x = 0; y = 0; }
		explicit _Vector2(Type _all) { x = _all; y = _all; }
		explicit _Vector2(Type _x, Type _y) { x = _x; y = _y; }
		//explicit _Vector2(const _Vector2& v) { x = v.x; y = v.y; }

		// ~ Swap Values ~ //
		_Vector2<Type>& Swap(void) { Type tmp = x; x = y; y = tmp; return *this; }

		// ~ Inverse ~ //
		_Vector2<Type> Inverse(void) const
		{
			return _Vector2<Type>((Type)1 / x, (Type)1 / y);
		}
		static _Vector2<Type> Inverse(const _Vector2<Type>& v)
		{
			return _Vector2<Type>((Type)1 / v.x, (Type)1 / v.y);
		}

		// ~ Compare ~ //
		inline bool Compare(const _Vector2<Type>& v) const;
		inline bool CompareFuzzy(const _Vector2<Type>& v) const;

		// ~ Normalizing ~ //
		_Vector2<Type>& NormalizeSelf()
		{
			return (*this *= Vxl::InverseSqrt<Type>(x * x + y * y));
		}
		_Vector2<Type> Normalize() const
		{
			return (*this * Vxl::InverseSqrt<Type>(x * x + y * y));
		}
		_Vector2<Type> NormalizeAccurate() const
		{
			float _invaLength = 1.0f / Magnitudef();
			return *this * _invaLength;
		}

		// ~ Length ~ //
				Type InverseMagnitude(void) const { return Vxl::InverseSqrt<Type>(MacroDot2(x,y)); }
		static	Type InverseMagnitude(const _Vector2<Type>& v) { return Vxl::InverseSqrt<Type>(MacroDot2(v.x,v.y)); }
		static	Type InverseMagnitude(Type x, Type y) { return Vxl::InverseSqrt<Type>(MacroDot2(x,y)); }
				float Magnitudef(void) const { return sqrtf(MacroDot2(x,y)); }
		static	float Magnitudef(const _Vector2<Type>& v) { return sqrtf(MacroDot2(v.x,v.y)); }
		static	float Magnitudef(Type x, Type y) { return sqrtf(MacroDot2(x,y)); }
				double Magnituded(void) const { return sqrt(MacroDot2(x,y)); }
		static	double Magnituded(const _Vector2<Type>& v) { return sqrt(MacroDot2(v.x,v.y)); }
		static	double Magnituded(Type x, Type y) { return sqrt(MacroDot2(x,y)); }
				Type Length(void) const { return sqrtf(MacroDot2(x,y)); }
		static	Type Length(const _Vector2<Type>& v) { return sqrtf(MacroDot2(v.x,v.y)); }
		static	Type Length(Type x, Type y) { return sqrtf(MacroDot2(x,y)); }
				Type LengthSqr(void) const { return (MacroDot2(x,y)); }
		static	Type LengthSqr(const _Vector2<Type>& v) { return (MacroDot2(v.x,v.y)); }
		static	Type LengthSqr(Type x, Type y) { return (MacroDot2(x,y)); }

		// ~ Distance ~ //
				Type Distance(const _Vector2<Type>& v) { return Length(*this - v); }
		static	Type Distance(const _Vector2<Type>& v1, const _Vector2<Type>& v2) { return Length(v1 - v2); }

		// ~ Dot Product ~ //
				Type Dot() const { return LengthSqr(); }
				Type Dot(const _Vector2<Type>& v) const { return (x * v.x + y * v.y); }
		static	Type Dot(const _Vector2<Type>& v1, const _Vector2<Type>& v2) { return (v1.x * v2.x + v1.y * v2.y); }

		// ~ Reflection ~ //
		_Vector2<Type> Reflect(const _Vector2<Type>& Normal)
		{
			return Reflect(*this, Normal);
		}
		static _Vector2<Type> Reflect(const _Vector2<Type>& Incident, const _Vector2<Type>& Normal)
		{
			return Incident - 2 * (Incident.Dot(Normal)) * Normal;
		}
		// ~ Refraction ~ //
		_Vector2<Type> Refract(const _Vector2<Type>& Normal, float index)
		{
			return Refract(*this, Normal, index);
		}
		static _Vector2<Type> Refract(const _Vector2<Type>& Incident, const _Vector2<Type>& Normal, float index)
		{
			float dot = Dot(Incident, Normal);
			float k = 1.0f - index * index * (1.0f - dot * dot);

			if (k >= 0.0f)
				return (index * Incident - (index * dot + sqrt(k)) * Normal);

			return _Vector2<Type>();
		}

		// ~ Projection ~ //
		_Vector2<Type> Project(const _Vector2<Type>& v) const
		{
			return Project(*this, v);
		}
		static _Vector2<Type> Project(const _Vector2<Type>& v1, const _Vector2<Type>& v2)
		{
			// Formula:	(Dot / (b length * b length)) * b
			float D = Vector3::Dot(v1, v2);
			float S = v2.LengthSqr();

			return v2 * (D / S);
		}
		// ~ Projection Length ~ //
		Type ProjectLength(const _Vector2<Type>& v) const
		{
			return ProjectLength(*this, v);
		}
		static Type ProjectLength(const _Vector2<Type>& v1, const _Vector2<Type>& v2)
		{
			float D = Dot(v1, v2);
			float L = v2.Length();
			return D / L;
		}

		// ~ Cross Product ~ //
		// NONE //

		// ~ Rotation ~ //
		_Vector2<Type> Rotate(float _radians, bool CCW = true)
		{
			// Counter Clockwise Rotation
			if (CCW)
			{
				return _Vector2<Type>(
					+x * cosf(_radians) - y * sinf(_radians),
					+x * sinf(_radians) + y * cosf(_radians)
					);
			}
			// Clockwise Rotation
			return _Vector2<Type>(
				+x * cosf(_radians) + y * sinf(_radians),
				-x * sinf(_radians) + y * cosf(_radians)
				);
		}
		static _Vector2<Type> GetRotated(float _radians, bool CCW = true)
		{
			// Counter Clockwise Rotation
			if (CCW)
			{
				return _Vector2<Type>(
					sinf(_radians),
					cosf(_radians)
					);
			}
			// Clockwise Rotation
			return _Vector2<Type>(
				sinf(_radians),
				cosf(_radians)
				);
		}

		// ~ Angles ~ //
		float GetAngleDegrees(const _Vector2<Type>& v) const
		{
			return RadToDeg(GetAngleRadians(*this, v));
		}
		static float GetAngleDegrees(const _Vector2<Type>& v1, const _Vector2<Type>& v2)
		{
			return RadToDeg(GetAngleRadians(v1, v2));
		}
		float GetAngleRadians(const _Vector2<Type>& v) const
		{
			return GetAngleRadians(*this, v);
		}
		static float GetAngleRadians(const _Vector2<Type>& v1, const _Vector2<Type>& v2)
		{
			return acosf(
				Dot(v1, v2) / (v1.Magnitudef() * v2.Magnitudef())
			);
		}

		// ~ Random ~ //
		static _Vector2<Type> GetRandomUnitVector()
		{
			float val = Random::Range(-1.0f, 1.0f);
			return _Vector2<Type>(sinf(val), cosf(val));
		}

		// ~ Denormals ~ // (Fix Numbers close to zero)
		bool FixDenormals(void)
		{
			return FixDenormals(*this);
		}
		static bool FixDenormals(_Vector2<Type>& v)
		{
			bool denormal = false;
			if (fabs(v.x) < 1e-30f) {
				v.x = 0.0f;
				denormal = true;
			}
			if (fabs(v.y) < 1e-30f) {
				v.y = 0.0f;
				denormal = true;
			}
			return denormal;
		}

		// Resize Vector
		_Vector2<Type> Resize(Type length)
		{
			// No length = nothing
			if (!length)
				return _Vector2<Type>();

			// Normalize then mutiply
			return Normalize() * length;
		}


		// Abs
		_Vector2<Type>& Abs()
		{
			return Abs(*this);
		}
		static _Vector2<Type> Abs(const _Vector2<Type>& v)
		{
			return _Vector2<Type>(
				fabs(v.x),
				fabs(v.y)
				);
		}
		// Min
		_Vector2<Type>& Min(Type f)
		{
			return (*this = Min(*this, f));
		}
		_Vector2<Type>& Min(const _Vector2<Type>& v)
		{
			return (*this = Min(*this, v));
		}
		static _Vector2<Type> Min(const _Vector2<Type>& v1, float f)
		{
			return _Vector2<Type>(
				min(v1.x, f),
				min(v1.y, f)
				);
		}
		static _Vector2<Type> Min(const _Vector2<Type>& v1, const _Vector2<Type>& v2)
		{
			return _Vector2<Type>(
				min(v1.x, v2.x),
				min(v1.y, v2.y)
				);
		}
		// Max
		_Vector2<Type>& Max(Type f)
		{
			return (*this = Max(*this, f));
		}
		_Vector2<Type>& Max(const _Vector2<Type>& v)
		{
			return (*this = Max(*this, v));
		}
		static _Vector2<Type> Max(const _Vector2<Type>& v1, float f)
		{
			return _Vector2<Type>(
				max(v1.x, f),
				max(v1.y, f)
				);
		}
		static _Vector2<Type> Max(const _Vector2<Type>& v1, const _Vector2<Type>& v2)
		{
			return _Vector2<Type>(
				max(v1.x, v2.x),
				max(v1.y, v2.y)
				);
		}
		// Round
		_Vector2<Type> Round()
		{
			return _Vector2<Type>(
				round(x),
				round(y)
				);
		}
		// Clamp
		_Vector2<Type> Clamp(float _min, float _max)
		{
			return Clamp(*this, _min, _max);
		}
		_Vector2<Type> Clamp(const _Vector2<Type>& _min, const _Vector2<Type>& _max)
		{
			return Clamp(*this, _min, _max);
		}
		static _Vector2<Type> Clamp(const _Vector2<Type>& v, float _min, float _max)
		{
			return _Vector2<Type>(
				MacroClamp(v.x, _min, _max),
				MacroClamp(v.y, _min, _max)
				);
		}
		static _Vector2<Type> Clamp(const _Vector2<Type>& v, const _Vector2<Type>& _min, const _Vector2<Type>& _max)
		{
			return _Vector2<Type>(
				MacroClamp(v.x, _min.x, _max.x),
				MacroClamp(v.y, _min.y, _max.y)
				);
		}

		// Snap to nearest integer value
		_Vector2<Type> Snap(void)
		{
			return _Vector2<Type>(
				floor(x + 0.5f),
				floor(y + 0.5f)
				);
		}

		// Get Float pointer of values
		inline Type* GetStartPointer(void)
		{
			return &x;
		}
		inline static Type* GetStartPointer(_Vector2<Type>& v)
		{
			return &v.x;
		}

		// ~ TOSTRING ~ //
		std::string ToString()
		{
			return std::to_string(x) + ' ' + std::to_string(y);
		}

		// ~ Lerp ~ //
		_Vector2<Type> Lerp(const _Vector2<Type>& v, const float t)
		{
			return Lerp(*this, v, t);
		}
		static _Vector2<Type> Lerp(const _Vector2<Type>& v1, const _Vector2<Type>& v2, float t)
		{
			return MacroLerp(v1, v2, t);
		}

		// ~ Slerp ~ //
		_Vector2<Type> Slerp(const _Vector2<Type>& v, const float t)
		{
			return Slerp(*this, v, t);
		}
		static _Vector2<Type> Slerp(const _Vector2<Type>& v1, const _Vector2<Type>& v2, float t)
		{
			Type dot = Dot(v1, v2);
			MacroClamp(dot, -1.0f, 1.0f);
			Type theta = acosf(dot)*t;
			_Vector2<Type> RelativeVec = v2 - v1 * dot;
			RelativeVec.Normalize();

			return ((v1*cosf(theta)) + (RelativeVec*sinf(theta)));
		}
		// ~ Nlerp ~ //
		_Vector2<Type> Nlerp(const _Vector2<Type>& v, const float t)
		{
			return Nlerp(*this, v, t);
		}
		static _Vector2<Type> Nlerp(const _Vector2<Type>& v1, const _Vector2<Type>& v2, float t)
		{
			return (MacroLerp(v1, v2, t)).Normalize();
		}

		// ~ Operator Overloads ~ //
		float operator[](unsigned int index) const
		{
			OUT_OF_RANGE_INDEX_CHECK_2(index);
			return (&x)[index];
		}
		float& operator[](unsigned int index)
		{
			OUT_OF_RANGE_INDEX_CHECK_2(index);
			return (&x)[index];
		}

		// Negative Operator Overloading
		_Vector2<Type> operator-(void) const{ return _Vector2<Type>(-x,-y); }

		// Basic Operator Overloading
		_Vector2<Type> operator+ (const _Vector2<Type>& v) const { return _Vector2<Type>(x + v.x, y + v.y); }
		_Vector2<Type> operator- (const _Vector2<Type>& v) const { return _Vector2<Type>(x - v.x, y - v.y); }
		_Vector2<Type> operator* (const _Vector2<Type>& v) const { return _Vector2<Type>(x * v.x, y * v.y); }
		_Vector2<Type> operator/ (const _Vector2<Type>& v) const { return _Vector2<Type>(x / v.x, y / v.y); }
		_Vector2<Type>& operator+= (const _Vector2<Type>& v) { x += v.x; y += v.y; return *this; }
		_Vector2<Type>& operator-= (const _Vector2<Type>& v) { x -= v.x; y -= v.y; return *this; }
		_Vector2<Type>& operator*= (const _Vector2<Type>& v) { x *= v.x; y *= v.y; return *this; }
		_Vector2<Type>& operator/= (const _Vector2<Type>& v) { x /= v.x; y /= v.y; return *this; }

		// Operator Overloading with Float
		friend _Vector2<Type> operator+ (const _Vector2<Type>& v, Type f) { return _Vector2<Type>(v.x + f, v.y + f); }
		friend _Vector2<Type> operator- (const _Vector2<Type>& v, Type f) { return _Vector2<Type>(v.x - f, v.y - f); }
		friend _Vector2<Type> operator* (const _Vector2<Type>& v, Type f) { return _Vector2<Type>(v.x * f, v.y * f); }
		friend _Vector2<Type> operator/ (const _Vector2<Type>& v, Type f) { return _Vector2<Type>(v.x / f, v.y / f); }

		friend _Vector2<Type> operator+ (Type f, const _Vector2<Type>& v) { return _Vector2<Type>(v.x + f, v.y + f); }
		friend _Vector2<Type> operator- (Type f, const _Vector2<Type>& v) { return _Vector2<Type>(v.x - f, v.y - f); }
		friend _Vector2<Type> operator* (Type f, const _Vector2<Type>& v) { return _Vector2<Type>(v.x * f, v.y * f); }
		friend _Vector2<Type> operator/ (Type f, const _Vector2<Type>& v) { return _Vector2<Type>(v.x / f, v.y / f); }

		_Vector2<Type>& operator+= (Type f) { x += f; y += f; return *this; }
		_Vector2<Type>& operator-= (Type f) { x -= f; y -= f; return *this; }
		_Vector2<Type>& operator*= (Type f) { x *= f; y *= f; return *this; }
		_Vector2<Type>& operator/= (Type f)
		{
			VXL_ASSERT(f != 0, "DIVISION BY ZERO");
			Type inva = (Type)1 / f;
			x *= inva; y *= inva; return *this;
		}

		// Operator Comparison Overloading
		bool operator== (const _Vector2<Type>& v) const { return Compare(v);}
		bool operator!= (const _Vector2<Type>& v) const { return !Compare(v);}

		// Cout Overload
		friend std::ostream &operator<<(std::ostream &stream, const _Vector2<Type>& ob)
		{
			return (stream << '[' << ob.x << ", " << ob.y << ']');
		}

		// ~ Return ~ //
		_Vector2<Type> GetVector2(uint32_t index1, uint32_t index2) const
		{
			return _Vector2<Type>(*(&x + index1), *(&x + index2));
		}
	};

	template<typename Type>
	class _Vector3
	{
	public:
		// ~ Data ~ //
		Type x, y, z;

		// Is Empty
		inline bool IsEmpty()
		{
			return !(x || y || z);
		}
		inline bool IsNotEmpty()
		{
			return (x || y || z);
		}

		// Common Vector Types
		static const _Vector3<Type> ZERO;
		static const _Vector3<Type> ONE;
		static const _Vector3<Type> N_ONE;
		static const _Vector3<Type> HALF;
		static const _Vector3<Type> MINUS_HALF;
		static const _Vector3<Type> QUARTER;
		static const _Vector3<Type> MINUS_QUARTER;
		static const _Vector3<Type> UP;
		static const _Vector3<Type> DOWN;
		static const _Vector3<Type> FORWARD;
		static const _Vector3<Type> BACK;
		static const _Vector3<Type> LEFT;
		static const _Vector3<Type> RIGHT;

		// ~ Copy Constructors ~ //
		_Vector3<Type>& operator=(const _Vector2<Type>& v) { x = v.x; y = v.y; return *this; }
		_Vector3<Type>& operator=(const _Vector3<Type>& v) { x = v.x; y = v.y; z = v.z; return *this; }

		// ~ Constructors ~ //
		explicit _Vector3(void) { x = 0; y = 0; z = 0; }
		explicit _Vector3(Type _all) { x = _all; y = _all; z = _all; }
		explicit _Vector3(Type _x, Type _y, Type _z) { x = _x; y = _y; z = _z; }
		explicit _Vector3(const _Vector2<Type>& v) { x = v.x; y = v.y; z = 0; }
		//explicit _Vector3(const _Vector3<Type>& v) { x = v.x; y = v.y; z = v.z; }

		// ~ Cast ~ //
		explicit operator _Vector2<Type>(void) const { return _Vector2<Type>(x, y); }

		// ~ Swap Values ~ //
		_Vector3<Type>& SwapXY(void) { Type tmp = x; x = y; y = tmp; return *this; }
		_Vector3<Type>& SwapXZ(void) { Type tmp = x; x = z; z = tmp; return *this; }
		_Vector3<Type>& SwapYZ(void) { Type tmp = y; y = z; z = tmp; return *this; }

		// ~ Inverse ~ //
		_Vector3<Type> Inverse(void) const
		{
			return _Vector3<Type>((Type)1 / x, (Type)1 / y, (Type)1 / z);
		}
		static _Vector3<Type> Inverse(const _Vector3<Type>& v)
		{
			return _Vector3<Type>((Type)1 / v.x, (Type)1 / v.y, (Type)1 / v.z);
		}

		// ~ Compare ~ //
		inline bool Compare(const _Vector3<Type>& v) const;
		inline bool CompareFuzzy(const _Vector3<Type>& v) const;

		// ~ Normalizing ~ //
		_Vector3<Type>& NormalizeSelf()
		{
			return (*this *= Vxl::InverseSqrt<Type>(x * x + y * y + z * z));
		}
		_Vector3<Type> Normalize() const
		{
			return (*this * Vxl::InverseSqrt<Type>(x * x + y * y + z * z));
		}
		_Vector3<Type> NormalizeXY() const
		{
			return (_Vector3<Type>(x, y, 0) * Vxl::InverseSqrt<Type>(x * x + y * y));
		}
		_Vector3<Type> NormalizeYZ() const
		{
			return (_Vector3<Type>(0, y, z) * Vxl::InverseSqrt<Type>(y * y + z * z));
		}
		_Vector3<Type> NormalizeXZ() const
		{
			return (_Vector3<Type>(x, 0, z) * Vxl::InverseSqrt<Type>(x * x + z * z));
		}
		_Vector3<Type> NormalizeAccurate() const
		{
			float _invaLength = 1.0f / Magnitudef();
			return *this * _invaLength;
		}

		// ~ Length ~ //
				Type InverseMagnitude(void) const { return Vxl::InverseSqrt<Type>(MacroDot3(x,y,z)); }
		static	Type InverseMagnitude(const _Vector3<Type>& v) { return Vxl::InverseSqrt<Type>(MacroDot3(v.x,v.y,v.z)); }
		static	Type InverseMagnitude(Type x, Type y, Type z) { return Vxl::InverseSqrt<Type>(MacroDot3(x,y,z)); }
				float Magnitudef(void) const { return sqrtf(MacroDot3(x,y,z)); }
		static	float Magnitudef(const _Vector3<Type>& v) { return sqrtf(MacroDot3(v.x,v.y,v.z)); }
		static	float Magnitudef(Type x, Type y, Type z) { return sqrtf(MacroDot3(x,y,z)); }
				double Magnituded(void) const { return sqrt(MacroDot3(x,y,z)); }
		static	double Magnituded(const _Vector3<Type>& v) { return sqrt(MacroDot3(v.x,v.y,v.z)); }
		static	double Magnituded(Type x, Type y, Type z) { return sqrt(MacroDot3(x,y,z)); }
				Type Length(void) const { return sqrtf(MacroDot3(x,y,z)); }
		static	Type Length(const _Vector3<Type>& v) { return sqrtf(MacroDot3(v.x,v.y,v.z)); }
		static	Type Length(Type x, Type y, Type z) { return sqrtf(MacroDot3(x,y,z)); }
				Type LengthSqr(void) const { return (MacroDot3(x,y,z)); }
		static	Type LengthSqr(const _Vector3<Type>& v) { return (MacroDot3(v.x,v.y,v.z)); }
		static	Type LengthSqr(Type x, Type y, Type z) { return (MacroDot3(x,y,z)); }

		// ~ Distance ~ //
				Type Distance(const _Vector3<Type>& v) { return Length(*this - v); }
		static	Type Distance(const _Vector3<Type>& v1, const _Vector3<Type>& v2) { return Length(v1 - v2); }

		// ~ Dot Product ~ //
				Type Dot() const { return LengthSqr(); }
				Type Dot(const _Vector3<Type>& v) const { return (x * v.x + y * v.y + z * v.z); }
		static	Type Dot(const _Vector3<Type>& v1, const _Vector3<Type>& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }

		// ~ Reflection ~ //
		_Vector3<Type> Reflect(const _Vector3<Type>& Normal)
		{
			return Reflect(*this, Normal);
		}
		static _Vector3<Type> Reflect(const _Vector3<Type>& Incident, const _Vector3<Type>& Normal)
		{
			return Incident - 2 * (Incident.Dot(Normal)) * Normal;
		}
		// ~ Refraction ~ //
		_Vector3<Type> Refract(const _Vector3<Type>& Normal, float index)
		{
			return Refract(*this, Normal, index);
		}
		static _Vector3<Type> Refract(const _Vector3<Type>& Incident, const _Vector3<Type>& Normal, float index)
		{
			float dot = Dot(Incident, Normal);
			float k = 1.0f - index * index * (1.0f - dot * dot);

			if (k >= 0.0f)
				return (index * Incident - (index * dot + sqrt(k)) * Normal);

			return _Vector3<Type>();
		}

		// ~ Projection ~ //
		_Vector3<Type> Project(const _Vector3<Type>& v) const
		{
			return Project(*this, v);
		}
		static _Vector3<Type> Project(const _Vector3<Type>& v1, const _Vector3<Type>& v2)
		{
			// Formula:	(Dot / (b length * b length)) * b
			float D = Vector3::Dot(v1, v2);
			float S = v2.LengthSqr();

			return v2 * (D / S);
		}
		// ~ Projection Length ~ //
		Type ProjectLength(const _Vector3<Type>& v) const
		{
			return ProjectLength(*this, v);
		}
		static Type ProjectLength(const _Vector3<Type>& v1, const _Vector3<Type>& v2)
		{
			float D = Dot(v1, v2);
			float L = v2.Length();
			return D / L;
		}

		// ~ Cross Product ~ //
		_Vector3<Type> Cross(const _Vector3<Type>& v) const
		{
			return Cross(*this, v);
		}
		static _Vector3<Type> Cross(const _Vector3<Type>& v1, const _Vector3<Type>& v2)
		{
			return _Vector3<Type>(
				v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x
			);
		}

		// ~ Rotation ~ //
		_Vector3<Type> RotateX(float _radians, bool CCW = true)
		{
			float value = CCW ? _radians : -_radians;

			return _Vector3<Type>(
				+x,
				+y * cosf(value) - z * sinf(value),
				+z * sinf(value) + z * cosf(value)
			);
		}
		_Vector3<Type> RotateY(float _radians, bool CCW = true)
		{
			float value = CCW ? _radians : -_radians;

			return _Vector3<Type>(
				+x * cosf(value) + z * sinf(value),
				+y,
				-x * sinf(value) + z * cosf(value)
			);
		}
		_Vector3<Type> RotateZ(float _radians, bool CCW = true)
		{
			float value = CCW ? _radians : -_radians;

			return _Vector3<Type>(
				+x * cosf(value) - y * sinf(value),
				+x * sinf(value) + y * cosf(value),
				+z
			);
		}
		_Vector3<Type> Rotate(const _Vector3<Type>& axis, float _radians, bool CCW = true)
		{
			_Vector3<Type> a = axis.Normalize();
			float c = cosf(_radians);
			float s = sinf(_radians);
			float t = 1 - c;

			return _Vector3<Type>(
				x * (t*a.x*a.x+c) + y * (t*a.x*a.y - a.z*s) + z * (t*a.x*a.z + a.y*s),
				x * (t*a.x*a.y + a.z*s) + y * (t*a.y*a.y + c) + z * (t*a.y*a.z - a.x*s),
				x * (t*a.x*a.z - a.y*s) + y * (t*a.y*a.z + a.x*s) + z * (t*a.z*a.z + c)
			);
		}

		// ~ Angles ~ //
		float GetAngleDegrees(const _Vector3<Type>& v) const
		{
			return RadToDeg(GetAngleRadians(*this, v));
		}
		static float GetAngleDegrees(const _Vector3<Type>& v1, const _Vector3<Type>& v2)
		{
			return RadToDeg(GetAngleRadians(v1, v2));
		}
		float GetAngleRadians(const _Vector3<Type>& v) const
		{
			return GetAngleRadians(*this, v);
		}
		static float GetAngleRadians(const _Vector3<Type>& v1, const _Vector3<Type>& v2)
		{
			return acosf(
				Dot(v1, v2) / (v1.Magnitudef() * v2.Magnitudef())
			);
		}

		// ~ Random ~ //
		static _Vector3<Type> GetRandomUnitVector()
		{
			float x = Random::Range(-1.0f, 1.0f);
			float y = Random::Range(-1.0f, 1.0f);
			float z = Random::Range(-1.0f, 1.0f);
			_Vector3<Type> v = _Vector3<Type>(x, y, z);
			v.FixDenormals();
			if (v.LengthSqr() == 0.0f)
				return _Vector3<Type>(0, 1, 0);
			else
				return v.Normalize();
		}

		// ~ Denormals ~ // (Fix Numbers close to zero)
		bool FixDenormals(void)
		{
			return FixDenormals(*this);
		}
		static bool FixDenormals(_Vector3<Type>& v)
		{
			bool denormal = false;
			if (fabs(v.x) < 1e-30f) {
				v.x = 0.0f;
				denormal = true;
			}
			if (fabs(v.y) < 1e-30f) {
				v.y = 0.0f;
				denormal = true;
			}
			if (fabs(v.z) < 1e-30f) {
				v.z = 0.0f;
				denormal = true;
			}
			return denormal;
		}

		// Resize Vector
		_Vector3<Type> Resize(Type length)
		{
			// No length = nothing
			if (!length)
				return _Vector3<Type>();

			// Normalize then mutiply
			return Normalize() * length;
		}


		// Abs
		_Vector3<Type>& Abs()
		{
			return Abs(*this);
		}
		static _Vector3<Type> Abs(const _Vector3<Type>& v)
		{
			return _Vector3<Type>(
				fabs(v.x),
				fabs(v.y),
				fabs(v.z)
				);
		}
		// Min
		_Vector3<Type>& Min(Type f)
		{
			return Min(*this, f);
		}
		_Vector3<Type>& Min(const _Vector3<Type>& v)
		{
			return Min(*this, v);
		}
		static _Vector3<Type> Min(const _Vector3<Type>& v1, float f)
		{
			return _Vector3<Type>(
				min(v1.x, f),
				min(v1.y, f),
				min(v1.z, f)
				);
		}
		static _Vector3<Type> Min(const _Vector3<Type>& v1, const _Vector3<Type>& v2)
		{
			return _Vector3<Type>(
				min(v1.x, v2.x),
				min(v1.y, v2.y),
				min(v1.z, v2.z)
				);
		}
		// Max
		_Vector3<Type>& Max(Type f)
		{
			return Max(*this, f);
		}
		_Vector3<Type>& Max(const _Vector3<Type>& v)
		{
			return Max(*this, v);
		}
		static _Vector3<Type> Max(const _Vector3<Type>& v1, float f)
		{
			return _Vector3<Type>(
				max(v1.x, f),
				max(v1.y, f),
				max(v1.z, f)
				);
		}
		static _Vector3<Type> Max(const _Vector3<Type>& v1, const _Vector3<Type>& v2)
		{
			return _Vector3<Type>(
				max(v1.x, v2.x),
				max(v1.y, v2.y),
				max(v1.z, v2.z)
				);
		}
		// Round
		_Vector3<Type> Round()
		{
			return _Vector3<Type>(
				round(x),
				round(y),
				round(z)
				);
		}
		// Clamp
		_Vector3<Type> Clamp(float _min, float _max)
		{
			return Clamp(*this, _min, _max);
		}
		_Vector3<Type> Clamp(const _Vector3<Type>& _min, const _Vector3<Type>& _max)
		{
			return Clamp(*this, _min, _max);
		}
		static _Vector3<Type> Clamp(const _Vector3<Type>& v, float _min, float _max)
		{
			return _Vector3<Type>(
				MacroClamp(v.x, _min, _max),
				MacroClamp(v.y, _min, _max),
				MacroClamp(v.z, _min, _max)
				);
		}
		static _Vector3<Type> Clamp(const _Vector3<Type>& v, const _Vector3<Type>& _min, const _Vector3<Type>& _max)
		{
			return _Vector3<Type>(
				MacroClamp(v.x, _min.x, _max.x),
				MacroClamp(v.y, _min.y, _max.y),
				MacroClamp(v.z, _min.z, _max.z)
				);
		}

		// Snap to nearest integer value
		_Vector3<Type> Snap(void)
		{
			return _Vector3<Type>(
				floor(x + 0.5f),
				floor(y + 0.5f),
				floor(z + 0.5f)
				);
		}

		// Get Float pointer of values
		inline Type* GetStartPointer(void)
		{
			return &x;
		}
		inline static Type* GetStartPointer(_Vector3<Type>& v)
		{
			return &v.x;
		}

		// ~ TOSTRING ~ //
		std::string ToString()
		{
			return std::to_string(x) + ' ' + std::to_string(y) + ' ' + std::to_string(z);
		}

		// ~ Lerp ~ //
		_Vector3<Type> Lerp(const _Vector3<Type>& v, const float t)
		{
			return Lerp(*this, v, t);
		}
		static _Vector3<Type> Lerp(const _Vector3<Type>& v1, const _Vector3<Type>& v2, float t)
		{
			return MacroLerp(v1, v2, t);
		}

		// ~ Slerp ~ //
		_Vector3<Type> Slerp(const _Vector3<Type>& v, const float t)
		{
			return Slerp(*this, v, t);
		}
		static _Vector3<Type> Slerp(const _Vector3<Type>& v1, const _Vector3<Type>& v2, float t)
		{
			Type dot = Dot(v1, v2);
			MacroClamp(dot, -1.0f, 1.0f);
			Type theta = acosf(dot)*t;
			_Vector3<Type> RelativeVec = v2 - v1 * dot;
			RelativeVec.Normalize();

			return ((v1*cosf(theta)) + (RelativeVec*sinf(theta)));
		}
		// ~ Nlerp ~ //
		_Vector3<Type> Nlerp(const _Vector3<Type>& v, const float t)
		{
			return Nlerp(*this, v, t);
		}
		static _Vector3<Type> Nlerp(const _Vector3<Type>& v1, const _Vector3<Type>& v2, float t)
		{
			return (MacroLerp(v1, v2, t)).Normalize();
		}

		// ~ Operator Overloads ~ //
		float operator[](unsigned int index) const
		{
			OUT_OF_RANGE_INDEX_CHECK_3(index);
			return (&x)[index];
		}
		float& operator[](unsigned int index)
		{
			OUT_OF_RANGE_INDEX_CHECK_3(index);
			return (&x)[index];
		}

		// Negative Operator Overloading
		_Vector3<Type> operator-(void) const{ return _Vector3<Type>(-x,-y,-z); }

		// Basic Operator Overloading
		_Vector3<Type> operator+ (const _Vector3<Type>& v) const { return _Vector3<Type>(x + v.x, y + v.y, z + v.z); }
		_Vector3<Type> operator- (const _Vector3<Type>& v) const { return _Vector3<Type>(x - v.x, y - v.y, z - v.z); }
		_Vector3<Type> operator* (const _Vector3<Type>& v) const { return _Vector3<Type>(x * v.x, y * v.y, z * v.z); }
		_Vector3<Type> operator/ (const _Vector3<Type>& v) const { return _Vector3<Type>(x / v.x, y / v.y, z / v.z); }
		_Vector3<Type>& operator+= (const _Vector3<Type>& v) { x += v.x; y += v.y; z += v.z; return *this; }
		_Vector3<Type>& operator-= (const _Vector3<Type>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		_Vector3<Type>& operator*= (const _Vector3<Type>& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		_Vector3<Type>& operator/= (const _Vector3<Type>& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

		// Operator Overloading with Float
		friend _Vector3<Type> operator+ (const _Vector3<Type>& v, Type f) { return _Vector3<Type>(v.x + f, v.y + f, v.z + f); }
		friend _Vector3<Type> operator- (const _Vector3<Type>& v, Type f) { return _Vector3<Type>(v.x - f, v.y - f, v.z - f); }
		friend _Vector3<Type> operator* (const _Vector3<Type>& v, Type f) { return _Vector3<Type>(v.x * f, v.y * f, v.z * f); }
		friend _Vector3<Type> operator/ (const _Vector3<Type>& v, Type f)
		{
			Type inva = (Type)1 / f;
			return _Vector3<Type>(v.x * inva, v.y * inva, v.z * inva);
		}

		friend _Vector3<Type> operator+ (Type f, const _Vector3<Type>& v) { return _Vector3<Type>(v.x + f, v.y + f, v.z + f); }
		friend _Vector3<Type> operator- (Type f, const _Vector3<Type>& v) { return _Vector3<Type>(v.x - f, v.y - f, v.z - f); }
		friend _Vector3<Type> operator* (Type f, const _Vector3<Type>& v) { return _Vector3<Type>(v.x * f, v.y * f, v.z * f); }
		friend _Vector3<Type> operator/ (Type f, const _Vector3<Type>& v)
			{
			Type inva = (Type)1 / f;
			return _Vector3<Type>(v.x * inva, v.y * inva, v.z * inva);
		}

		_Vector3<Type>& operator+= (Type f) { x += f; y += f; z += f; return *this; }
		_Vector3<Type>& operator-= (Type f) { x -= f; y -= f; z -= f; return *this; }
		_Vector3<Type>& operator*= (Type f) { x *= f; y *= f; z *= f; return *this; }
		_Vector3<Type>& operator/= (Type f)
		{
			VXL_ASSERT(f != 0, "DIVISION BY ZERO");
			Type inva = (Type)1 / f;
			x *= inva; y *= inva; z *= inva; return *this;
		}

		// Operator Comparison Overloading
		bool operator== (const _Vector3<Type>& v) const { return Compare(v);}
		bool operator!= (const _Vector3<Type>& v) const { return !Compare(v);}

		// Cout Overload
		friend std::ostream &operator<<(std::ostream &stream, const _Vector3<Type>& ob)
		{
			return (stream << '[' << ob.x << ", " << ob.y << ", " << ob.z << ']');
		}

		// ~ Return ~ //
		_Vector2<Type> GetVector2() const
		{
			return _Vector2<Type>(x, y);
		}
		_Vector2<Type> GetVector2(uint32_t index1, uint32_t index2) const
		{
			return _Vector2<Type>(*(&x + index1), *(&x + index2));
		}
		_Vector3<Type> GetVector3(uint32_t index1, uint32_t index2, uint32_t index3) const
		{
			return _Vector3<Type>(*(&x + index1), *(&x + index2), *(&x + index3));
		}
	};

	template<typename Type>
	class _Vector4
	{
	public:
		// ~ Data ~ //
		Type x, y, z, w;

		// Is Empty
		inline bool IsEmpty()
		{
			return !(x || y || z || w);
		}
		inline bool IsNotEmpty()
		{
			return (x || y || z || w);
		}

		// ~ Copy Constructors ~ //
		_Vector4<Type>& operator=(const _Vector2<Type>& v) { x = v.x; y = v.y; return *this; }
		_Vector4<Type>& operator=(const _Vector3<Type>& v) { x = v.x; y = v.y; z = v.z; return *this; }
		_Vector4<Type>& operator=(const _Vector4<Type>& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }

		// ~ Constructors ~ //
		explicit _Vector4(void) { x = 0; y = 0; z = 0; w = 0; }
		explicit _Vector4(Type _all) { x = _all; y = _all; z = _all; w = _all; }
		explicit _Vector4(Type _x, Type _y, Type _z, Type _w) { x = _x; y = _y; z = _z; w = _w; }
		explicit _Vector4(const _Vector2<Type>& v) { x = v.x; y = v.y; z = 0; w = 0; }
		explicit _Vector4(const _Vector2<Type>& v, Type f1) { x = v.x; y = v.y; z = f1; w = 0; }
		explicit _Vector4(const _Vector2<Type>& v, Type f1, Type f2) { x = v.x; y = v.y; z = f1; w = f2; }
		explicit _Vector4(const _Vector3<Type>& v) { x = v.x; y = v.y; z = v.z; w = 0; }
		explicit _Vector4(const _Vector3<Type>& v, Type f) { x = v.x; y = v.y; z = v.z; w = f; }

		// ~ Cast ~ //
		explicit operator _Vector2<Type>(void) const { return _Vector2<Type>(x, y); }
		explicit operator _Vector3<Type>(void) const { return _Vector3<Type>(x, y, z); }

		// ~ Swap Values ~ //
		_Vector4<Type>& SwapXY(void) { Type tmp = x; x = y; y = tmp; return *this; }
		_Vector4<Type>& SwapXZ(void) { Type tmp = x; x = z; z = tmp; return *this; }
		_Vector4<Type>& SwapXW(void) { Type tmp = x; x = w; w = tmp; return *this; }
		_Vector4<Type>& SwapYZ(void) { Type tmp = y; y = z; z = tmp; return *this; }
		_Vector4<Type>& SwapYW(void) { Type tmp = y; y = w; w = tmp; return *this; }
		_Vector4<Type>& SwapZW(void) { Type tmp = z; z = w; w = tmp; return *this; }

		// ~ Inverse ~ //
		_Vector4<Type> Inverse(void) const
		{
			return _Vector4<Type>((Type)1 / x, (Type)1 / y, (Type)1 / z, (Type)1 / w);
		}
		static _Vector4<Type> Inverse(const _Vector4<Type>& v)
		{
			return _Vector4<Type>((Type)1 / v.x, (Type)1 / v.y, (Type)1 / v.z, (Type)1 / v.w);
		}

		// ~ Compare ~ //
		inline bool Compare(const _Vector4<Type>& v) const;
		inline bool CompareFuzzy(const _Vector4<Type>& v) const;

		// ~ Normalizing ~ //
		_Vector4<Type>& NormalizeSelf()
		{
			return (*this *= Vxl::InverseSqrt<Type>(x * x + y * y + z * z + w * w));
		}
		_Vector4<Type> Normalize() const
		{
			return (*this * Vxl::InverseSqrt<Type>(x * x + y * y + z * z + w * w));
		}
		_Vector4<Type> NormalizeAccurate() const
		{
			float _invaLength = 1.0f / Magnitudef();
			return *this * _invaLength;
		}

		// ~ Length ~ //
				Type InverseMagnitude(void) const { return Vxl::InverseSqrt<Type>(MacroDot4(x,y,z,w)); }
		static	Type InverseMagnitude(const _Vector4<Type>& v) { return Vxl::InverseSqrt<Type>(MacroDot4(v.x,v.y,v.z,v.w)); }
		static	Type InverseMagnitude(Type x, Type y, Type z, Type w) { return Vxl::InverseSqrt<Type>(MacroDot4(x,y,z,w)); }
				float Magnitudef(void) const { return sqrtf(MacroDot4(x,y,z,w)); }
		static	float Magnitudef(const _Vector4<Type>& v) { return sqrtf(MacroDot4(v.x,v.y,v.z,v.w)); }
		static	float Magnitudef(Type x, Type y, Type z, Type w) { return sqrtf(MacroDot4(x,y,z,w)); }
				double Magnituded(void) const { return sqrt(MacroDot4(x,y,z,w)); }
		static	double Magnituded(const _Vector4<Type>& v) { return sqrt(MacroDot4(v.x,v.y,v.z,v.w)); }
		static	double Magnituded(Type x, Type y, Type z, Type w) { return sqrt(MacroDot4(x,y,z,w)); }
				Type Length(void) const { return sqrtf(MacroDot4(x,y,z,w)); }
		static	Type Length(const _Vector4<Type>& v) { return sqrtf(MacroDot4(v.x,v.y,v.z,v.w)); }
		static	Type Length(Type x, Type y, Type z, Type w) { return sqrtf(MacroDot4(x,y,z,w)); }
				Type LengthSqr(void) const { return (MacroDot4(x,y,z,w)); }
		static	Type LengthSqr(const _Vector4<Type>& v) { return (MacroDot4(v.x,v.y,v.z,v.w)); }
		static	Type LengthSqr(Type x, Type y, Type z, Type w) { return (MacroDot4(x,y,z,w)); }

		// ~ Distance ~ //
				Type Distance(const _Vector4<Type>& v) { return Length(*this - v); }
		static	Type Distance(const _Vector4<Type>& v1, const _Vector4<Type>& v2) { return Length(v1 - v2); }

		// ~ Dot Product ~ //
				Type Dot() const { return LengthSqr(); }
				Type Dot(const _Vector4<Type>& v) const { return (x * v.x + y * v.y + z * v.z + w * v.w); }
		static	Type Dot(const _Vector4<Type>& v1, const _Vector4<Type>& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w); }

		// ~ Reflection ~ //
		_Vector4<Type> Reflect(const _Vector4<Type>& Normal)
		{
			return Reflect(*this, Normal);
		}
		static _Vector4<Type> Reflect(const _Vector4<Type>& Incident, const _Vector4<Type>& Normal)
		{
			return Incident - 2 * (Incident.Dot(Normal)) * Normal;
		}
		// ~ Refraction ~ //
		_Vector4<Type> Refract(const _Vector4<Type>& Normal, float index)
		{
			return Refract(*this, Normal, index);
		}
		static _Vector4<Type> Refract(const _Vector4<Type>& Incident, const _Vector4<Type>& Normal, float index)
		{
			float dot = Dot(Incident, Normal);
			float k = 1.0f - index * index * (1.0f - dot * dot);

			if (k >= 0.0f)
				return (index * Incident - (index * dot + sqrt(k)) * Normal);

			return _Vector4<Type>();
		}

		// ~ Projection ~ //
		_Vector4<Type> Project(const _Vector4<Type>& v) const
		{
			return Project(*this, v);
		}
		static _Vector4<Type> Project(const _Vector4<Type>& v1, const _Vector4<Type>& v2)
		{
			// Formula:	(Dot / (b length * b length)) * b
			float D = Vector4::Dot(v1, v2);
			float S = v2.LengthSqr();

			return v2 * (D / S);
		}
		// ~ Projection Length ~ //
		Type ProjectLength(const _Vector4<Type>& v) const
		{
			return ProjectLength(*this, v);
		}
		static Type ProjectLength(const _Vector4<Type>& v1, const _Vector4<Type>& v2)
		{
			float D = Dot(v1, v2);
			float L = v2.Length();
			return D / L;
		}

		// ~ Cross Product ~ //
		_Vector4<Type> Cross(const _Vector4<Type>& v) const
		{
			return Cross(*this, v);
		}
		static _Vector4<Type> Cross(const _Vector4<Type>& v1, const _Vector4<Type>& v2)
		{
			return _Vector4<Type>(
				v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x
			);
		}

		// ~ Rotation ~ //
		// None //

		// ~ Angles ~ //
		float GetAngleDegrees(const _Vector4<Type>& v) const
		{
			return RadToDeg(GetAngleRadians(*this, v));
		}
		static float GetAngleDegrees(const _Vector4<Type>& v1, const _Vector4<Type>& v2)
		{
			return RadToDeg(GetAngleRadians(v1, v2));
		}
		float GetAngleRadians(const _Vector4<Type>& v) const
		{
			return GetAngleRadians(*this, v);
		}
		static float GetAngleRadians(const _Vector4<Type>& v1, const _Vector4<Type>& v2)
		{
			return acosf(
				Dot(v1, v2) / (v1.Magnitudef() * v2.Magnitudef())
			);
		}

		// ~ Random ~ //
		static _Vector4<Type> GetRandomUnitVector()
		{
			float x = Random::Range(-1.0f, 1.0f);
			float y = Random::Range(-1.0f, 1.0f);
			float z = Random::Range(-1.0f, 1.0f);
			_Vector4<Type> v = _Vector4<Type>(x, y, z, 0);
			v.FixDenormals();
			if (v.LengthSqr() == 0.0f)
				return _Vector4<Type>(0, 1, 0, 0);
			else
				return v.Normalize();
		}

		// ~ Denormals ~ // (Fix Numbers close to zero)
		bool FixDenormals(void)
		{
			return FixDenormals(*this);
		}
		static bool FixDenormals(_Vector4<Type>& v)
		{
			bool denormal = false;
			if (fabs(v.x) < 1e-30f) {
				v.x = 0.0f;
				denormal = true;
			}
			if (fabs(v.y) < 1e-30f) {
				v.y = 0.0f;
				denormal = true;
			}
			if (fabs(v.z) < 1e-30f) {
				v.z = 0.0f;
				denormal = true;
			}
			if (fabs(v.w) < 1e-30f) {
				v.w = 0.0f;
				denormal = true;
			}
			return denormal;
		}

		// Resize Vector
		_Vector4<Type> Resize(Type length)
		{
			// No length = nothing
			if (!length)
				return _Vector4<Type>();

			// Normalize then mutiply
			return Normalize() * length;
		}


		// Abs
		_Vector4<Type>& Abs()
		{
			return Abs(*this);
		}
		static _Vector4<Type> Abs(const _Vector4<Type>& v)
		{
			return _Vector4<Type>(
				fabs(v.x),
				fabs(v.y),
				fabs(v.z),
				fabs(v.w)
				);
		}
		// Min
		_Vector4<Type>& Min(Type f)
		{
			return Min(*this, f);
		}
		_Vector4<Type>& Min(const _Vector4<Type>& v)
		{
			return Min(*this, v);
		}
		static _Vector4<Type> Min(const _Vector4<Type>& v1, float f)
		{
			return _Vector4<Type>(
				min(v1.x, f),
				min(v1.y, f),
				min(v1.w, f),
				min(v1.z, f)
				);
		}
		static _Vector4<Type> Min(const _Vector4<Type>& v1, const _Vector4<Type>& v2)
		{
			return _Vector4<Type>(
				min(v1.x, v2.x),
				min(v1.y, v2.y),
				min(v1.z, v2.z),
				min(v1.w, v2.w)
				);
		}
		// Max
		_Vector4<Type>& Max(Type f)
		{
			return Max(*this, f);
		}
		_Vector4<Type>& Max(const _Vector4<Type>& v)
		{
			return Max(*this, v);
		}
		static _Vector4<Type> Max(const _Vector4<Type>& v1, float f)
		{
			return _Vector4<Type>(
				max(v1.x, f),
				max(v1.y, f),
				max(v1.z, f)
				);
		}
		static _Vector4<Type> Max(const _Vector4<Type>& v1, const _Vector4<Type>& v2)
		{
			return _Vector4<Type>(
				max(v1.x, v2.x),
				max(v1.y, v2.y),
				max(v1.z, v2.z)
				);
		}
		// Round
		_Vector4<Type> Round()
		{
			return _Vector4<Type>(
				round(x),
				round(y),
				round(z),
				round(w)
				);
		}
		// Clamp
		_Vector4<Type> Clamp(float _min, float _max)
		{
			return Clamp(*this, _min, _max);
		}
		_Vector4<Type> Clamp(const _Vector4<Type>& _min, const _Vector4<Type>& _max)
		{
			return Clamp(*this, _min, _max);
		}
		static _Vector4<Type> Clamp(const _Vector4<Type>& v, float _min, float _max)
		{
			return _Vector4<Type>(
				MacroClamp(v.x, _min, _max),
				MacroClamp(v.y, _min, _max),
				MacroClamp(v.z, _min, _max)
				);
		}
		static _Vector4<Type> Clamp(const _Vector4<Type>& v, const _Vector4<Type>& _min, const _Vector4<Type>& _max)
		{
			return _Vector4<Type>(
				MacroClamp(v.x, _min.x, _max.x),
				MacroClamp(v.y, _min.y, _max.y),
				MacroClamp(v.z, _min.z, _max.z),
				MacroClamp(v.w, _min.w, _max.w)
				);
		}

		// Snap to nearest integer value
		_Vector4<Type> Snap(void)
		{
			return _Vector4<Type>(
				floor(x + 0.5f),
				floor(y + 0.5f),
				floor(z + 0.5f),
				floor(w + 0.5f)
				);
		}

		// Get Float pointer of values
		inline Type* GetStartPointer(void)
		{
			return &x;
		}
		inline static Type* GetStartPointer(_Vector4<Type>& v)
		{
			return &v.x;
		}

		// ~ TOSTRING ~ //
		std::string ToString()
		{
			return std::to_string(x) + ' ' + std::to_string(y) + ' ' + std::to_string(z) + ' ' + std::to_string(w);
		}

		// ~ Lerp ~ //
		_Vector4<Type> Lerp(const _Vector4<Type>& v, const float t)
		{
			return Lerp(*this, v, t);
		}
		static _Vector4<Type> Lerp(const _Vector4<Type>& v1, const _Vector4<Type>& v2, float t)
		{
			return MacroLerp(v1, v2, t);
		}

		// ~ Slerp ~ //
		_Vector4<Type> Slerp(const _Vector4<Type>& v, const float t)
		{
			return Slerp(*this, v, t);
		}
		static _Vector4<Type> Slerp(const _Vector4<Type>& v1, const _Vector4<Type>& v2, float t)
		{
			Type dot = Dot(v1, v2);
			MacroClamp(dot, -1.0f, 1.0f);
			Type theta = acosf(dot)*t;
			_Vector4<Type> RelativeVec = v2 - v1 * dot;
			RelativeVec.Normalize();

			return ((v1*cosf(theta)) + (RelativeVec*sinf(theta)));
		}
		// ~ Nlerp ~ //
		_Vector4<Type> Nlerp(const _Vector4<Type>& v, const float t)
		{
			return Nlerp(*this, v, t);
		}
		static _Vector4<Type> Nlerp(const _Vector4<Type>& v1, const _Vector4<Type>& v2, float t)
		{
			return (MacroLerp(v1, v2, t)).Normalize();
		}

		// ~ Operator Overloads ~ //
		float operator[](unsigned int index) const
		{
			OUT_OF_RANGE_INDEX_CHECK_4(index);
			return (&x)[index];
		}
		float& operator[](unsigned int index)
		{
			OUT_OF_RANGE_INDEX_CHECK_4(index);
			return (&x)[index];
		}

		// Negative Operator Overloading
		_Vector4<Type> operator-(void) const{ return _Vector4<Type>(-x,-y,-z,-w); }

		// Basic Operator Overloading
		_Vector4<Type> operator+ (const _Vector4<Type>& v) const { return _Vector4<Type>(x + v.x, y + v.y, z + v.z, w + v.w); }
		_Vector4<Type> operator- (const _Vector4<Type>& v) const { return _Vector4<Type>(x - v.x, y - v.y, z - v.z, w - v.w); }
		_Vector4<Type> operator* (const _Vector4<Type>& v) const { return _Vector4<Type>(x * v.x, y * v.y, z * v.z, w * v.w); }
		_Vector4<Type> operator/ (const _Vector4<Type>& v) const { return _Vector4<Type>(x / v.x, y / v.y, z / v.z, w / v.w); }
		_Vector4<Type>& operator+= (const _Vector4<Type>& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		_Vector4<Type>& operator-= (const _Vector4<Type>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		_Vector4<Type>& operator*= (const _Vector4<Type>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
		_Vector4<Type>& operator/= (const _Vector4<Type>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }

		// Operator Overloading with Float
		friend _Vector4<Type> operator+ (const _Vector4<Type>& v, Type f) { return _Vector4<Type>(v.x + f, v.y + f, v.z + f, v.w + f); }
		friend _Vector4<Type> operator- (const _Vector4<Type>& v, Type f) { return _Vector4<Type>(v.x - f, v.y - f, v.z - f, v.w - f); }
		friend _Vector4<Type> operator* (const _Vector4<Type>& v, Type f) { return _Vector4<Type>(v.x * f, v.y * f, v.z * f, v.w * f); }
		friend _Vector4<Type> operator/ (const _Vector4<Type>& v, Type f) { return _Vector4<Type>(v.x / f, v.y / f, v.z / f, v.w / f); }

		friend _Vector4<Type> operator+ (Type f, const _Vector4<Type>& v) { return _Vector4<Type>(v.x + f, v.y + f, v.z + f, v.w + f); }
		friend _Vector4<Type> operator- (Type f, const _Vector4<Type>& v) { return _Vector4<Type>(v.x - f, v.y - f, v.z - f, v.w - f); }
		friend _Vector4<Type> operator* (Type f, const _Vector4<Type>& v) { return _Vector4<Type>(v.x * f, v.y * f, v.z * f, v.w * f); }
		friend _Vector4<Type> operator/ (Type f, const _Vector4<Type>& v) { return _Vector4<Type>(v.x / f, v.y / f, v.z / f, v.w / f); }

		_Vector4<Type>& operator+= (Type f) { x += f; y += f; z += f; w += f; return *this; }
		_Vector4<Type>& operator-= (Type f) { x -= f; y -= f; z -= f; w -= f; return *this; }
		_Vector4<Type>& operator*= (Type f) { x *= f; y *= f; z *= f; w *= f; return *this; }
		_Vector4<Type>& operator/= (Type f)
		{
			VXL_ASSERT(f != 0, "DIVISION BY ZERO");
			Type inva = (Type)1 / f;
			x *= inva; y *= inva; z *= inva; w *= inva; return *this;
		}

		// Operator Comparison Overloading
		bool operator== (const _Vector4<Type>& v) const { return Compare(v);}
		bool operator!= (const _Vector4<Type>& v) const { return !Compare(v);}

		// Cout Overload
		friend std::ostream &operator<<(std::ostream &stream, const _Vector4<Type>& ob)
		{
			return (stream << '[' << ob.x << ", " << ob.y << ", " << ob.z << ", " << ob.w << ']');
		}

		// ~ Return ~ //
		_Vector2<Type> GetVector2() const
		{
			return _Vector2<Type>(x, y);
		}
		_Vector2<Type> GetVector2(uint32_t index1, uint32_t index2) const
		{
			return _Vector2<Type>(*(&x + index1), *(&x + index2));
		}
		_Vector3<Type> GetVector3() const
		{
			return _Vector3<Type>(x, y, z);
		}
		_Vector3<Type> GetVector3(uint32_t index1, uint32_t index2, uint32_t index3) const
		{
			return _Vector3<Type>(*(&x + index1), *(&x + index2), *(&x + index3));
		}
		_Vector4<Type> GetVector4(uint32_t index1, uint32_t index2, uint32_t index3, uint32_t index4) const
		{
			return _Vector4<Type>(*(&x + index1), *(&x + index2), *(&x + index3), *(&x + index4));
		}
	};

	// ~ VECTOR 2 Template Specialization ~ //
	template<>
	bool _Vector2<float>::Compare(const _Vector2<float>& v) const 
	{
		if (fabs(x - v.x) > FLOAT_EPSILON)
			return false;

		else if (fabs(y - v.y) > FLOAT_EPSILON)
			return false;

		return true;
	}
	template<>
	bool _Vector2<float>::CompareFuzzy(const _Vector2<float>& v) const
	{
		if (fabs(x - v.x) > 0.01f)
			return false;

		else if (fabs(y - v.y) > 0.01f)
			return false;

		return true;
	}
	template<>
	bool _Vector2<double>::Compare(const _Vector2<double>& v) const
	{
		if (fabs(x - v.x) > DOUBLE_EPSILON)
			return false;

		else if (fabs(y - v.y) > DOUBLE_EPSILON)
			return false;

		return true;
	}
	template<>
	bool _Vector2<double>::CompareFuzzy(const _Vector2<double>& v) const
	{
		if (fabs(x - v.x) > 0.01)
			return false;

		else if (fabs(y - v.y) > 0.01)
			return false;

		return true;
	}
	template<>
	bool _Vector2<int>::Compare(const _Vector2<int>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector2<int>::CompareFuzzy(const _Vector2<int>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector2<char>::Compare(const _Vector2<char>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector2<char>::CompareFuzzy(const _Vector2<char>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		return true;
	}
	// ~						 ~ //
	// ~ VECTOR 3 Template Specialization ~ //
	template<>
	bool _Vector3<float>::Compare(const _Vector3<float>& v) const
	{
		if (fabs(x - v.x) > FLOAT_EPSILON)
			return false;

		else if (fabs(y - v.y) > FLOAT_EPSILON)
			return false;

		else if (fabs(z - v.z) > FLOAT_EPSILON)
			return false;

		return true;
	}
	template<>
	bool _Vector3<float>::CompareFuzzy(const _Vector3<float>& v) const
	{
		if (fabs(x - v.x) > 0.01f)
			return false;

		else if (fabs(y - v.y) > 0.01f)
			return false;

		else if (fabs(z - v.z) > 0.01f)
			return false;

		return true;
	}
	template<>
	bool _Vector3<double>::Compare(const _Vector3<double>& v) const
	{
		if (fabs(x - v.x) > DOUBLE_EPSILON)
			return false;

		else if (fabs(y - v.y) > DOUBLE_EPSILON)
			return false;

		else if (fabs(z - v.z) > DOUBLE_EPSILON)
			return false;

		return true;
	}
	template<>
	bool _Vector3<double>::CompareFuzzy(const _Vector3<double>& v) const
	{
		if (fabs(x - v.x) > 0.01)
			return false;

		else if (fabs(y - v.y) > 0.01)
			return false;

		else if (fabs(z - v.z) > 0.01)
			return false;

		return true;
	}
	template<>
	bool _Vector3<int>::Compare(const _Vector3<int>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		else if (fabs(z - v.z) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector3<int>::CompareFuzzy(const _Vector3<int>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		else if (fabs(z - v.z) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector3<char>::Compare(const _Vector3<char>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		else if (fabs(z - v.z) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector3<char>::CompareFuzzy(const _Vector3<char>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		else if (fabs(z - v.z) > 0)
			return false;

		return true;
	}
	// ~						 ~ //
	// ~ VECTOR 4 Template Specialization ~ //
	template<>
	bool _Vector4<float>::Compare(const _Vector4<float>& v) const
	{
		if (fabs(x - v.x) > FLOAT_EPSILON)
			return false;

		else if (fabs(y - v.y) > FLOAT_EPSILON)
			return false;

		else if (fabs(z - v.z) > FLOAT_EPSILON)
			return false;

		else if (fabs(w - v.w) > FLOAT_EPSILON)
			return false;

		return true;
	}
	template<>
	bool _Vector4<float>::CompareFuzzy(const _Vector4<float>& v) const
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
	template<>
	bool _Vector4<double>::Compare(const _Vector4<double>& v) const
	{
		if (fabs(x - v.x) > DOUBLE_EPSILON)
			return false;

		else if (fabs(y - v.y) > DOUBLE_EPSILON)
			return false;

		else if (fabs(z - v.z) > DOUBLE_EPSILON)
			return false;

		else if (fabs(w - v.w) > DOUBLE_EPSILON)
			return false;

		return true;
	}
	template<>
	bool _Vector4<double>::CompareFuzzy(const _Vector4<double>& v) const
	{
		if (fabs(x - v.x) > 0.01)
			return false;

		else if (fabs(y - v.y) > 0.01)
			return false;

		else if (fabs(z - v.z) > 0.01)
			return false;

		else if (fabs(w - v.w) > 0.01)
			return false;

		return true;
	}
	template<>
	bool _Vector4<int>::Compare(const _Vector4<int>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		else if (fabs(z - v.z) > 0)
			return false;

		else if (fabs(w - v.w) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector4<int>::CompareFuzzy(const _Vector4<int>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		else if (fabs(z - v.z) > 0)
			return false;

		else if (fabs(w - v.w) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector4<char>::Compare(const _Vector4<char>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		else if (fabs(z - v.z) > 0)
			return false;

		else if (fabs(w - v.w) > 0)
			return false;

		return true;
	}
	template<>
	bool _Vector4<char>::CompareFuzzy(const _Vector4<char>& v) const
	{
		if (fabs(x - v.x) > 0)
			return false;

		else if (fabs(y - v.y) > 0)
			return false;

		else if (fabs(z - v.z) > 0)
			return false;

		else if (fabs(w - v.w) > 0)
			return false;

		return true;
	}
	// ~						 ~ //

	// Common Vector Types
	const _Vector2<float> _Vector2<float>::ZERO = _Vector2<float>(0,0);
	const _Vector2<float> _Vector2<float>::ONE = _Vector2<float>(1,1);
	const _Vector2<float> _Vector2<float>::UP = _Vector2<float>(0,1);
	const _Vector2<float> _Vector2<float>::DOWN = _Vector2<float>(0,-1);
	const _Vector2<float> _Vector2<float>::LEFT = _Vector2<float>(-1,0);
	const _Vector2<float> _Vector2<float>::RIGHT = _Vector2<float>(1,0);
	const _Vector2<float> _Vector2<float>::HALF = _Vector2<float>(0.5f,0.5f);

	const _Vector3<float> _Vector3<float>::ZERO = _Vector3<float>(0,0,0);
	const _Vector3<float> _Vector3<float>::ONE = _Vector3<float>(1,1,1);
	const _Vector3<float> _Vector3<float>::N_ONE = _Vector3<float>(-1,-1,-1);
	const _Vector3<float> _Vector3<float>::HALF = _Vector3<float>(0.5f,0.5f,0.5f);
	const _Vector3<float> _Vector3<float>::MINUS_HALF = _Vector3<float>(-0.5f,-0.5f,-0.5f);
	const _Vector3<float> _Vector3<float>::QUARTER = _Vector3<float>(0.25f,0.25f,0.25f);
	const _Vector3<float> _Vector3<float>::MINUS_QUARTER = _Vector3<float>(-0.25f,-0.25f,-0.25f);
	const _Vector3<float> _Vector3<float>::UP = _Vector3<float>(0,1,0);
	const _Vector3<float> _Vector3<float>::DOWN = _Vector3<float>(0,-1,0);
	const _Vector3<float> _Vector3<float>::FORWARD = _Vector3<float>(0,0,1);
	const _Vector3<float> _Vector3<float>::BACK = _Vector3<float>(0,0,-1);
	const _Vector3<float> _Vector3<float>::LEFT = _Vector3<float>(-1,0,0);
	const _Vector3<float> _Vector3<float>::RIGHT = _Vector3<float>(1,0,0);

	// Vector Creation
	typedef _Vector2<float> Vector2;
	typedef _Vector2<float> Vec2;
	typedef _Vector2<float> vec2;
	typedef _Vector2<double> Vector2d;
	typedef _Vector2<double> Vec2d;
	typedef _Vector2<double> vec2d;
	typedef _Vector2<int> Vector2i;
	typedef _Vector2<int> Vec2i;
	typedef _Vector2<int> vec2i;
	typedef _Vector2<unsigned int> Vector2ui;
	typedef _Vector2<unsigned int> Vec2ui;
	typedef _Vector2<unsigned int> vec2ui;
	typedef _Vector2<char> Vector2c;
	typedef _Vector2<char> Vec2c;
	typedef _Vector2<char> vec2c;

	typedef _Vector3<float> Vector3;
	typedef _Vector3<float> Vec3;
	typedef _Vector3<float> vec3;
	typedef _Vector3<double> Vector3d;
	typedef _Vector3<double> Vec3d;
	typedef _Vector3<double> vec3d;
	typedef _Vector3<int> Vector3i;
	typedef _Vector3<int> Vec3i;
	typedef _Vector3<int> vec3i;
	typedef _Vector3<unsigned int> Vector3ui;
	typedef _Vector3<unsigned int> Vec3ui;
	typedef _Vector3<unsigned int> vec3ui;
	typedef _Vector3<char> Vector3c;
	typedef _Vector3<char> Vec3c;
	typedef _Vector3<char> vec3c;

	typedef _Vector4<float> Vector4;
	typedef _Vector4<float> Vec4;
	typedef _Vector4<float> vec4;
	typedef _Vector4<double> Vector4d;
	typedef _Vector4<double> Vec4d;
	typedef _Vector4<double> vec4d;
	typedef _Vector4<int> Vector4i;
	typedef _Vector4<int> Vec4i;
	typedef _Vector4<int> vec4i;
	typedef _Vector4<unsigned int> Vector4ui;
	typedef _Vector4<unsigned int> Vec4ui;
	typedef _Vector4<unsigned int> vec4ui;
	typedef _Vector4<char> Vector4c;
	typedef _Vector4<char> Vec4c;
	typedef _Vector4<char> vec4c;
}