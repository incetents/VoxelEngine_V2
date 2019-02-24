// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <winsock.h>
#include <cmath>
#include <iostream>
#include <ostream>

namespace Vxl
{
	class Degrees;
	class Radians;

	enum class RotationDirection
	{
		COUNTER_CLOCKWISE = 1,
		CLOCKWISE = -1
	};

	class RotationValue
	{
	public:
		float value;

		// Constructor
		explicit RotationValue(float _value)
		{
			value = _value;
		}
		// Getter
		inline const float& Get() const
		{
			return value;
		}

		// Round Values
		inline float Round(u_int Decimals);
		inline float& RoundSelf(u_int Decimals);

		// Output
		friend std::ostream& operator<<(std::ostream& os, const RotationValue& r);
	};


	class Radians : public RotationValue
	{
	public:
		// Constructor
		explicit Radians(float r) : RotationValue(r) {}
		explicit Radians(const Degrees& deg);

		// Get As Degrees
		inline float GetDegrees() const;

		// Special Operator Overloads
		Radians& operator-()
		{
			value = -value;
			return *this;
		}

		// Operator overloads with Degrees
		Radians operator+(Degrees& v);
		Radians& operator+=(Degrees& v);

		Radians operator-(Degrees& v);
		Radians& operator-=(Degrees& v);

		Radians operator*(Degrees& v);
		Radians& operator*=(Degrees& v);

		Radians operator/(Degrees& v);
		Radians& operator/=(Degrees& v);

		Radians& operator=(Degrees& v);
		Radians& operator=(const float v);

		bool operator==(Degrees& v);
		bool operator!=(Degrees& v);
		bool operator==(const Radians& v);
		bool operator!=(const Radians& v);

		//	friend ostream& operator<<(ostream& os, const Radians& r)
		//	{
		//		os << r.value;
		//		return os;
		//	}
	};


	class Degrees : public RotationValue
	{
	public:
		// Constructor
		explicit Degrees(float d) : RotationValue(d) {}
		explicit Degrees(const Radians& deg);

		// Get As Radians
		inline float GetRadians() const;

		// Special Operator Overloads
		Degrees& operator-()
		{
			value = -value;
			return *this;
		}

		// Operator overloads with Radians
		Degrees operator+(Radians& v);
		Degrees& operator+=(Radians& v);

		Degrees operator-(Radians& v);
		Degrees& operator-=(Radians& v);

		Degrees operator*(Radians& v);
		Degrees& operator*=(Radians& v);

		Degrees operator/(Radians& v);
		Degrees& operator/=(Radians& v);

		Degrees& operator=(Radians& v);
		Degrees& operator=(const float v);

		bool operator==(Radians& v);
		bool operator!=(Radians& v);
		bool operator==(const Degrees& v);
		bool operator!=(const Degrees& v);

		//	friend ostream& operator<<(ostream& os, const Degrees& r)
		//	{
		//		os << r.value;
		//		return os;
		//	}
	};
}