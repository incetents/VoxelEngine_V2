// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Color.h"

#include <assert.h>

namespace Vxl
{
	// C3 Overloads
	Color3F Color3F::operator+(const Color3F& c) const
	{
		Color3F m;
		m.r = r + c.r;
		m.g = g + c.g;
		m.b = b + c.b;
		return m;
	}
	Color3F Color3F::operator-(const Color3F& c) const
	{
		Color3F m;
		m.r = r - c.r;
		m.g = g - c.g;
		m.b = b - c.b;
		return m;
	}
	Color3F Color3F::operator*(const Color3F& c) const
	{
		Color3F m;
		m.r = r * c.r;
		m.g = g * c.g;
		m.b = b * c.b;
		return m;
	}
	Color3F Color3F::operator/(const Color3F& c) const
	{
		Color3F m;
		m.r = r / c.r;
		m.g = g / c.g;
		m.b = b / c.b;
		return m;
	}
	Color3F& Color3F::operator+=(const Color3F& c)
	{
		r += c.r;
		g += c.g;
		b += c.b;
		return *this;
	}
	Color3F& Color3F::operator-=(const Color3F& c)
	{
		r -= c.r;
		g -= c.g;
		b -= c.b;
		return *this;
	}
	Color3F& Color3F::operator*=(const Color3F& c)
	{
		r *= c.r;
		g *= c.g;
		b *= c.b;
		return *this;
	}
	Color3F& Color3F::operator/=(const Color3F& c)
	{
		r /= c.r;
		g /= c.g;
		b /= c.b;
		return *this;
	}

	Color3F Color3F::operator+(float t) const
	{
		Color3F m;
		m.r = r + t;
		m.g = g + t;
		m.b = b + t;
		return m;
	}
	Color3F Color3F::operator-(float t) const
	{
		Color3F m;
		m.r = r - t;
		m.g = g - t;
		m.b = b - t;
		return m;
	}
	Color3F Color3F::operator*(float t) const
	{
		Color3F m;
		m.r = r * t;
		m.g = g * t;
		m.b = b * t;
		return m;
	}
	Color3F Color3F::operator/(float t) const
	{
		Color3F m;
		m.r = r / t;
		m.g = g / t;
		m.b = b / t;
		return m;
	}
	Color3F& Color3F::operator+=(float t)
	{
		r += t;
		g += t;
		b += t;
		return *this;
	}
	Color3F& Color3F::operator-=(float t)
	{
		r -= t;
		g -= t;
		b -= t;
		return *this;
	}
	Color3F& Color3F::operator*=(float t)
	{
		r *= t;
		g *= t;
		b *= t;
		return *this;
	}
	Color3F& Color3F::operator/=(float t)
	{
		r /= t;
		g /= t;
		b /= t;
		return *this;
	}

	float Color3F::operator[](const int index) const
	{
		assert((index >= 0) && (index < 3));
		return (&r)[index];
	}
	float& Color3F::operator[](const int index)
	{
		assert((index >= 0) && (index < 3));
		return (&r)[index];
	}

	// C4 Overloads
	Color4F Color4F::operator+(const Color4F& c) const
	{
		Color4F m;
		m.r = r + c.r;
		m.g = g + c.g;
		m.b = b + c.b;
		m.a = a + c.a;
		return m;
	}
	Color4F Color4F::operator-(const Color4F& c) const
	{
		Color4F m;
		m.r = r - c.r;
		m.g = g - c.g;
		m.b = b - c.b;
		m.a = a - c.a;
		return m;
	}
	Color4F Color4F::operator*(const Color4F& c) const
	{
		Color4F m;
		m.r = r * c.r;
		m.g = g * c.g;
		m.b = b * c.b;
		m.a = a * c.a;
		return m;
	}
	Color4F Color4F::operator/(const Color4F& c) const
	{
		Color4F m;
		m.r = r / c.r;
		m.g = g / c.g;
		m.b = b / c.b;
		m.a = a / c.a;
		return m;
	}
	Color4F& Color4F::operator+=(const Color4F& c)
	{
		r += c.r;
		g += c.g;
		b += c.b;
		a += c.a;
		return *this;
	}
	Color4F& Color4F::operator-=(const Color4F& c)
	{
		r -= c.r;
		g -= c.g;
		b -= c.b;
		a -= c.a;
		return *this;
	}
	Color4F& Color4F::operator*=(const Color4F& c)
	{
		r *= c.r;
		g *= c.g;
		b *= c.b;
		a *= c.a;
		return *this;
	}
	Color4F& Color4F::operator/=(const Color4F& c)
	{
		r /= c.r;
		g /= c.g;
		b /= c.b;
		a /= c.a;
		return *this;
	}

	Color4F Color4F::operator+(float t) const
	{
		Color4F m;
		m.r = r + t;
		m.g = g + t;
		m.b = b + t;
		m.a = a + t;
		return m;
	}
	Color4F Color4F::operator-(float t) const
	{
		Color4F m;
		m.r = r - t;
		m.g = g - t;
		m.b = b - t;
		m.a = a - t;
		return m;
	}
	Color4F Color4F::operator*(float t) const
	{
		Color4F m;
		m.r = r * t;
		m.g = g * t;
		m.b = b * t;
		m.a = a * t;
		return m;
	}
	Color4F Color4F::operator/(float t) const
	{
		Color4F m;
		m.r = r / t;
		m.g = g / t;
		m.b = b / t;
		m.a = a / t;
		return m;
	}
	Color4F& Color4F::operator+=(float t)
	{
		r += t;
		g += t;
		b += t;
		a += t;
		return *this;
	}
	Color4F& Color4F::operator-=(float t)
	{
		r -= t;
		g -= t;
		b -= t;
		a -= t;
		return *this;
	}
	Color4F& Color4F::operator*=(float t)
	{
		r *= t;
		g *= t;
		b *= t;
		a *= t;
		return *this;
	}
	Color4F& Color4F::operator/=(float t)
	{
		r /= t;
		g /= t;
		b /= t;
		a /= t;
		return *this;
	}

	float Color4F::operator[](const int index) const
	{
		assert((index >= 0) && (index < 3));
		return (&r)[index];
	}
	float& Color4F::operator[](const int index)
	{
		assert((index >= 0) && (index < 3));
		return (&r)[index];
	}
}