// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Color.h"

#include <assert.h>

namespace Vxl
{
	const Color3F Color3F::WHITE	= Color3F(1, 1, 1);
	const Color3F Color3F::BLACK	= Color3F(0, 0, 0);
	const Color3F Color3F::RED		= Color3F(230.f / 255.f, 25.f / 255.f, 75.f / 255.f);
	const Color3F Color3F::GREEN	= Color3F(60.f / 255.f, 180.f / 255.f, 75.f / 255.f);
	const Color3F Color3F::BLUE		= Color3F(0, 130.f / 255.f, 200.f / 255.f);
	const Color3F Color3F::YELLOW	= Color3F(1, 1, 25.f / 255.f);
	const Color3F Color3F::ORANGE	= Color3F(245.f / 255.f, 130.f / 255.f, 48.f / 255.f);
	const Color3F Color3F::PURPLE	= Color3F(145.f / 255.f, 30.f / 255.f, 180.f / 255.f);
	const Color3F Color3F::CYAN		= Color3F(70.f / 255.f, 240.f / 255.f, 240.f / 255.f);

	// Color3f Overloads
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

	const Color4F Color4F::ZEROES	= Color4F(0, 0, 0, 0);
	const Color4F Color4F::WHITE	= Color4F(1, 1, 1, 1);
	const Color4F Color4F::BLACK	= Color4F(0, 0, 0, 1);
	const Color4F Color4F::RED		= Color4F(230.f / 255.f, 25.f / 255.f, 75.f / 255.f, 1);
	const Color4F Color4F::GREEN	= Color4F(60.f / 255.f, 180.f / 255.f, 75.f / 255.f, 1);
	const Color4F Color4F::BLUE		= Color4F(0, 130.f / 255.f, 200.f / 255.f, 1);
	const Color4F Color4F::YELLOW	= Color4F(1, 1, 25.f / 255.f, 1);
	const Color4F Color4F::ORANGE	= Color4F(245.f / 255.f, 130.f / 255.f, 48.f / 255.f, 1);
	const Color4F Color4F::PURPLE	= Color4F(145.f / 255.f, 30.f / 255.f, 180.f / 255.f, 1);
	const Color4F Color4F::CYAN		= Color4F(70.f / 255.f, 240.f / 255.f, 240.f / 255.f, 1);

	// Color4f Overloads
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