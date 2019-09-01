// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

namespace Vxl
{
	class Color3F
	{
	public:
		// Common Color Types
		static const Color3F WHITE;
		static const Color3F BLACK;
		static const Color3F RED;
		static const Color3F GREEN;
		static const Color3F BLUE;
		static const Color3F YELLOW;
		static const Color3F ORANGE;
		static const Color3F PURPLE;
		static const Color3F CYAN;
		static const Color3F GREY;

		Color3F(float r, float g, float b)
			: r(r), g(g), b(b)
		{}
		Color3F()
			: r(0), g(0), b(0)
		{}
		Color3F(float a)
			: r(a), g(a), b(a)
		{}

		// Data
		float r, g, b;

		static Color3F lerp(const Color3F& start, const Color3F& end, float t)
		{
			return (start * (1.0f - t) + end * t);
		}

		std::string ToString(void) const
		{
			return std::to_string(r) + ' ' + std::to_string(g) + ' ' + std::to_string(b);
		}

		// Overloads
		Color3F operator+(const Color3F& c) const;
		Color3F operator-(const Color3F& c) const;
		Color3F operator*(const Color3F& c) const;
		Color3F operator/(const Color3F& c) const;
		Color3F& operator+=(const Color3F& c);
		Color3F& operator-=(const Color3F& c);
		Color3F& operator*=(const Color3F& c);
		Color3F& operator/=(const Color3F& c);

		Color3F operator+(float t) const;
		Color3F operator-(float t) const;
		Color3F operator*(float t) const;
		Color3F operator/(float t) const;
		Color3F& operator+=(float t);
		Color3F& operator-=(float t);
		Color3F& operator*=(float t);
		Color3F& operator/=(float t);

		Color3F& operator=(const Color3F& other);

		float operator[](const int) const;
		float& operator[](const int);
	};

	class Color4F
	{
	public:
		// Common Color Types
		static const Color4F ZEROES;
		static const Color4F WHITE;
		static const Color4F BLACK;
		static const Color4F RED;
		static const Color4F GREEN;
		static const Color4F BLUE;
		static const Color4F YELLOW;
		static const Color4F ORANGE;
		static const Color4F PURPLE;
		static const Color4F CYAN;
		static const Color4F GREY;

		Color4F(float r, float g, float b, float a)
			: r(r), g(g), b(b), a(a)
		{}
		Color4F(float r, float g, float b)
			: r(r), g(g), b(b), a(1)
		{}

		Color4F()
			: r(0), g(0), b(0), a(0)
		{}
		Color4F(float _a)
			: r(_a), g(_a), b(_a), a(_a)
		{}

		Color4F(const Color3F& c)
			: r(c.r), g(c.g), b(c.b), a(1)
		{}
		Color4F(const Color3F& c, float a)
			: r(c.r), g(c.g), b(c.b), a(a)
		{}

		// Data
		float r, g, b, a;

		// Lerp
		static Color4F lerp(const Color4F& start, const Color4F& end, float t)
		{
			return (start * (1.0f - t) + end * t);
		}

		// Utility
		Color3F getRGB(void) const
		{
			return Color3F(r, g, b);
		}
		std::string ToString(void) const
		{
			return std::to_string(r) + ' ' + std::to_string(g) + ' ' + std::to_string(b) + ' ' + std::to_string(a);
		}

		// Overloads
		Color4F operator+(const Color4F& c) const;
		Color4F operator-(const Color4F& c) const;
		Color4F operator*(const Color4F& c) const;
		Color4F operator/(const Color4F& c) const;
		Color4F& operator+=(const Color4F& c);
		Color4F& operator-=(const Color4F& c);
		Color4F& operator*=(const Color4F& c);
		Color4F& operator/=(const Color4F& c);

		Color4F operator+(float t) const;
		Color4F operator-(float t) const;
		Color4F operator*(float t) const;
		Color4F operator/(float t) const;
		Color4F& operator+=(float t);
		Color4F& operator-=(float t);
		Color4F& operator*=(float t);
		Color4F& operator/=(float t);

		Color4F& operator=(const Color3F& other);
		Color4F& operator=(const Color4F& other);

		float operator[](const int) const;
		float& operator[](const int);
	};

	// Shorthands
	typedef Color3F color3f;
	typedef Color4F color4f;
}