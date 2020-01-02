// Copyright(c) 2020 Emmanuel Lajeunesse
#pragma once

#include "MathCore.h"

// Macro Functions
#define MacroLerp(a,b,t) (a * (1.0f - t) + b * t)
#define MacroBiLerp(a,b,c,t) (MacroLerp((MacroLerp(a, b, t)), (MacroLerp(b, c, t)), t))
#define MacroTriLerp(a,b,c,d,t) (MacroLerp((MacroLerp((MacroLerp(a, b, t)), (MacroLerp(b, c, t)), t)), (MacroLerp((MacroLerp(b, c, t)), (MacroLerp(c, d, t)), t))))
#define MacroHermite(a,b,c,d,t) (-a * 0.5f + (3.0f*b) * 0.5f - (3.0f*c) * 0.5f + d * 0.5f)*t*t*t + (a - (5.0f*b) * 0.5f + 2.0f*c - d * 0.5f)*t*t + (-a * 0.5f + c * 0.5f)*t + b
#define MacroInverseLerp(a,b,c) (c - min(a, b)) / (max(a, b) - min(a, b))
#define MacroBounce(x) (abs(sin(6.28f * (x + 1.f) * (x + 1.f)) * (1.f - x)))
#define MacroSmoothstep(a,b,t) MacroLerp(a, b, t * t * (3 - 2 * t))

namespace Vxl
{
	// Linear interpolation
	template<typename type>
	type lerp(type start, type end, float t)
	{
		return MacroLerp(start, end, t);
	}

	// Inverse Lerp
	template<typename type>
	float inverseLerp(type start, type end, type value)
	{
		return MacroInverseLerp(start, end, value);
	}

	// Ease in out [Sigmoid-like interpolation]
	template<typename type>
	type smoothstep(type start, type end, float t)
	{
		return MacroLerp(start, end, t * t * (3 - 2 * t));
	}

	// Sigmoid-like interpolation, get t value
	template<typename type>
	static type inverseSmoothstep(type value, type start, type end)
	{
		type t = MacroClamp((value - start) / (end - start), 0, 1);
		return t * t * (3 - 2 * t);
	}

	// Smooth step (lerp with ease in and ease out)
	template<typename type>
	type smoothstepPrecise(type start, type end, float t)
	{
		return MacroLerp(coserp(start, end, t), sinerp(start, end, t), t);
	}

	// Ease out
	template<typename type>
	type sinerp(type start, type end, float t)
	{
		return MacroLerp(start, end, sinf(t * HALF_PI));
	}

	// Ease in
	template<typename type>
	type coserp(type start, type end, float t)
	{
		return MacroLerp(start, end, (1 - cosf(t * HALF_PI)));
	}

	// Spring (overshoots a bit)
	template<typename type>
	type berp(type start, type end, float t)
	{
		t = MacroClamp(t, 0.f, 1.f);
		t = (sinf(t * PI * (0.2f + 2.5f * t * t * t)) * pow(1.0f - t, 2.2f) + t) * (1.0f + (1.2f * (1.0f - t)));
		return start + (end - start) * t;
	}

	// Bouncing up and down
	template<typename type>
	float bounce(type x)
	{
		return MacroBounce(x);
	}

	// Circular Lerp - Wraps around 0 to 360 for interpolating euler angles
	template<typename type>
	type clerp(type start, type end, float value)
	{
		float min = 0;
		float max = 360;
		float half = abs((max - min) / 2); // half the distance between min and max

		if ((end - start) < -half)
		{
			return start + ((max - start) + end) * value;
		}
		else if ((end - start) > half)
		{
			return start - ((max - end) + start) * value;
		}
		else
			return start + (end - start) * value;
	}

	template<typename type>
	void LERP_CONSOLE_TEST()
	{
		std::cout << "Sinerp [-100, 100]" << std::endl;
		for (int i = 0; i < 11; i++)
			std::cout << "t = " << (float)i / 10.0f << " [" << sinerp<type>(-100, +100, (float)i / 10.0f) << "]" << std::endl;

		std::cout << "~~~ \n";

		std::cout << "Coserp [-100, 100]" << std::endl;
		for (int i = 0; i < 11; i++)
			std::cout << "t = " << (float)i / 10.0f << " [" << coserp<type>(-100, +100, (float)i / 10.0f) << "]" << std::endl;

		std::cout << "~~~ \n";

		std::cout << "Smoothstep [-100, 100]" << std::endl;
		for (int i = 0; i < 11; i++)
			std::cout << "t = " << (float)i / 10.0f << " [" << smoothstep<type>(-100, +100, (float)i / 10.0f) << "]" << std::endl;

		std::cout << "~~~ \n";

		std::cout << "SmoothstepPrecise [-100, 100]" << std::endl;
		for (int i = 0; i < 11; i++)
			std::cout << "t = " << (float)i / 10.0f << " [" << smoothstepPrecise<type>(-100, +100, (float)i / 10.0f) << "]" << std::endl;

		std::cout << "~~~ \n";

		std::cout << "clerp [0, 720]" << std::endl;
		for (int i = 0; i < 21; i++)
			std::cout << "t = " << (float)i / 20.0f << " [" << clerp<type>(0, 720, (float)i / 20.0f) << "]" << std::endl;

		std::cout << "~~~ \n";

		std::cout << "berp [-100, 100]" << std::endl;
		for (int i = 0; i < 11; i++)
			std::cout << "t = " << (float)i / 10.0f << " [" << berp<type>(-100, +100, (float)i / 10.0f) << "]" << std::endl;

		std::cout << "~~~ \n";

		std::cout << "bounce" << std::endl;
		for (int i = 0; i < 11; i++)
			std::cout << "t = " << (float)i / 10.0f << " [" << bounce<type>((float)i / 10.0f) << "]" << std::endl;
	}
}