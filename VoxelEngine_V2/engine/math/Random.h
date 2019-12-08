// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <random>

#include "../utilities/Singleton.h"

namespace Vxl
{
	static class Random : public Singleton<class Random>
	{
	private:
		std::mt19937 m_generator;
		std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
	public:
		void init()
		{
			m_generator.seed(std::random_device()());
		}
		void setSeed(uint32_t seedID)
		{
			m_generator.seed(seedID);
		}

		// Random number between [0, 1] inclusif
		float getFloat01();
		// Random number between [0, max] inclusif
		float getFloat(float max);
		// Random number between [min, max] inclusif
		float getFloat(float min, float max);

		// Random number between [0, 1] inclusif
		double getDouble01();
		// Random number between [0, max] inclusif
		double getDouble(double max);
		// Random number between [min, max] inclusif
		double getDouble(double min, double max);

		// Random number between [min, max] inclusif
		int getInt(int min, int max);
	
	} SingletonInstance(Random);
}
