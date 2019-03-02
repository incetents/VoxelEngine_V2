// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Rotation.h"
#include "MathCore.h"

namespace Vxl
{
	// Rotation Value Functions
	float RotationValue::Round(u_int decimals)
	{
		return Vxl::Round<float>(value, decimals);
	}
	float& RotationValue::RoundSelf(u_int decimals)
	{
		return (value = Vxl::Round<float>(value, decimals));
	}

	std::ostream& operator<<(std::ostream& os, const RotationValue& r)
	{
		os << r.value;
		return os;
	}


	// Getters
	float Radians::GetDegrees() const
	{
		return Rad_To_Deg(value);
	}
	float Degrees::GetRadians() const
	{
		return Deg_To_Rad(value);
	}

	// Constructors
	Radians::Radians(const Degrees& deg) : RotationValue(Deg_To_Rad(deg.value)) {}
	Degrees::Degrees(const Radians& rad) : RotationValue(Rad_To_Deg(rad.value)) {}

	// Operator overloads with Degrees
	Radians Radians::operator+(Degrees& v)
	{
		return Radians(value + v.GetRadians());
	}
	Radians& Radians::operator+=(Degrees& v)
	{
		value += v.GetRadians();
		return *this;
	}

	Radians Radians::operator-(Degrees& v)
	{
		return Radians(value - v.GetRadians());
	}
	Radians& Radians::operator-=(Degrees& v)
	{
		value -= v.GetRadians();
		return *this;
	}

	Radians Radians::operator*(Degrees& v)
	{
		return Radians(value * v.GetRadians());
	}
	Radians& Radians::operator*=(Degrees& v)
	{
		value *= v.GetRadians();
		return *this;
	}

	Radians Radians::operator/(Degrees& v)
	{
		return Radians(value / v.GetRadians());
	}
	Radians& Radians::operator/=(Degrees& v)
	{
		value /= v.GetRadians();
		return *this;
	}

	Radians& Radians::operator=(Degrees& v)
	{
		value = v.GetRadians();
		return *this;
	}
	Radians& Radians::operator=(const float v)
	{
		value = v;
		return *this;
	}

	bool Radians::operator==(Degrees& v)
	{
		return (abs(value - v.GetRadians()) < FLOAT_EPSILON);
	}
	bool Radians::operator!=(Degrees& v)
	{
		return (abs(value - v.GetRadians()) > FLOAT_EPSILON);
	}
	bool Radians::operator==(const Radians& v)
	{
		return (abs(value - v.value) < FLOAT_EPSILON);
	}
	bool Radians::operator!=(const Radians& v)
	{
		return (abs(value - v.value) > FLOAT_EPSILON);
	}

	// Operator overloads with Radians
	Degrees Degrees::operator+(Radians& v)
	{
		return Degrees(value + v.GetDegrees());
	}
	Degrees& Degrees::operator+=(Radians& v)
	{
		value += v.GetDegrees();
		return *this;
	}

	Degrees Degrees::operator-(Radians& v)
	{
		return Degrees(value - v.GetDegrees());
	}
	Degrees& Degrees::operator-=(Radians& v)
	{
		value -= v.GetDegrees();
		return *this;
	}

	Degrees Degrees::operator*(Radians& v)
	{
		return Degrees(value * v.GetDegrees());
	}
	Degrees& Degrees::operator*=(Radians& v)
	{
		value *= v.GetDegrees();
		return *this;
	}

	Degrees Degrees::operator/(Radians& v)
	{
		return Degrees(value / v.GetDegrees());
	}
	Degrees& Degrees::operator/=(Radians& v)
	{
		value /= v.GetDegrees();
		return *this;
	}

	Degrees& Degrees::operator=(Radians& v)
	{
		value = v.GetDegrees();
		return *this;
	}
	Degrees& Degrees::operator=(const float v)
	{
		value = v;
		return *this;
	}

	bool Degrees::operator==(Radians& v)
	{
		return (abs(value - v.GetDegrees()) < FLOAT_EPSILON);
	}
	bool Degrees::operator!=(Radians& v)
	{
		return (abs(value - v.GetDegrees()) > FLOAT_EPSILON);
	}
	bool Degrees::operator==(const Degrees& v)
	{
		return (abs(value - v.value) < FLOAT_EPSILON);
	}
	bool Degrees::operator!=(const Degrees& v)
	{
		return (abs(value - v.value) > FLOAT_EPSILON);
	}
}