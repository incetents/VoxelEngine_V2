// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Random.h"

#undef max
#undef min

namespace Vxl
{
	float Random::getFloat01()
	{
		return (float)m_distribution(m_generator) / (float)std::numeric_limits<uint32_t>::max();
	}
	float Random::getFloat(float max)
	{
		return max * (float)m_distribution(m_generator) / (float)std::numeric_limits<uint32_t>::max();
	}
	float Random::getFloat(float min, float max)
	{
		return (max - min) * (float)m_distribution(m_generator) / (float)std::numeric_limits<uint32_t>::max() + min;
	}

	double Random::getDouble01()
	{
		return (double)m_distribution(m_generator) / (double)std::numeric_limits<uint32_t>::max();
	}
	double Random::getDouble(double max)
	{
		return max * (double)m_distribution(m_generator) / (double)std::numeric_limits<uint32_t>::max();
	}
	double Random::getDouble(double min, double max)
	{
		return (max - min) * (double)m_distribution(m_generator) / (double)std::numeric_limits<uint32_t>::max() + min;
	}

	int Random::getInt(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(m_generator);
	}
}