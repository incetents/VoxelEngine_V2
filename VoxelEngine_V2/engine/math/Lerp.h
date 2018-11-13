// Copyright(c) 2018 Emmanuel Lajeunesse
#pragma once

#include "MathCore.h"

// Macro Functions
#define MacroLerp(a,b,t) (a * (1.0f - t) + b * t)
#define MacroBiLerp(a,b,c,t) (MacroLerp((MacroLerp(a, b, t)), (MacroLerp(b, c, t)), t))
#define MacroTriLerp(a,b,c,d,t) (MacroLerp((MacroLerp((MacroLerp(a, b, t)), (MacroLerp(b, c, t)), t)), (MacroLerp((MacroLerp(b, c, t)), (MacroLerp(c, d, t)), t))))
#define MacroHermite(a,b,c,d,t) (-a * 0.5f + (3.0f*b) * 0.5f - (3.0f*c) * 0.5f + d * 0.5f)*t*t*t + (a - (5.0f*b) * 0.5f + 2.0f*c - d * 0.5f)*t*t + (-a * 0.5f + c * 0.5f)*t + b
#define MacroInverseLerp(a,b,c) (c - min(a, b)) / (max(a, b) - min(a, b))
#define MacroBounce(x) (abs(sin(6.28f * (x + 1.f) * (x + 1.f)) * (1.f - x)))

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
	float inverselerp(type start, type end, type value)
	{
		return MacroInverseLerp(start, end, value);
	}

	// Ease in out
	template<typename type>
	type hermite(type start, type end, float t)
	{
		return lerp<type>(start, end, t * t * (3.0f - 2.0f * t));
	}

	// Ease out
	template<typename type>
	type sinerp(type start, type end, float t)
	{
		return lerp<type>(start, end, sin(t * PI * 0.5f));
	}

	// Ease in
	template<typename type>
	type coserp(type start, type end, float t)
	{
		return lerp<type>(start, end, 1.0f - cos(t * PI * 0.5f));
	}

	// Spring (overshoots a bit)
	template<typename type>
	type berp(type start, type end, float t)
	{
		t = MacroClamp(t, 0.f, 1.f);
		t = (sin(value * PI * (0.2f + 2.5f * t * t * t)) * pow(1f - t, 2.2f) + t) * (1f + (1.2f * (1f - t)));
		return start + (end - start) * t;
	}

	// Smooth step (lerp with ease in and ease out)
	template<typename type>
	type smoothstep(type start, type end, float t)
	{
		t = MacroClamp(t, start, max);
		float v1 = (t - start) / (end - start);
		float v2 = (t - start) / (end - start);
		return (-2 * v1 * v1 * v1 + 3 * v2 * v2);
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
		float min = 0.0f;
		float max = 360.0f;
		float half = abs((max - min) * 0.5f);//half the distance between min and max
		float retval = 0.0f;
		float diff = 0.0f;

		if ((end - start) < -half)
		{
			diff = ((max - start) + end) * value;
			retval = start + diff;
		}
		else if ((end - start) > half)
		{
			diff = -((max - end) + start) * value;
			retval = start + diff;
		}
		else
			retval = start + (end - start) * value;

		return retval;
	}
}