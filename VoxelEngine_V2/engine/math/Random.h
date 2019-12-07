// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <random>

#pragma push_macro("MACRONAME")
#undef max
#undef min

namespace Vxl
{
	class Random
	{
	private:
		static std::random_device m_randomDevice;
		static std::mt19937 m_randomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
	public:
		static void Init()
		{
			m_randomEngine.seed(m_randomDevice);
		}
		static float Float()
		{
			return (float)m_distribution(m_randomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}
	
	};
	std::random_device Random::m_randomDevice;
	std::mt19937 Random::m_randomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> Random::m_distribution;
}

//	namespace Random
//	{
//		// Set Seed as Time
//		inline void Seed()
//		{
//			srand(u_int(time(NULL)));
//		}
//		// Set Seed by value
//		template<typename T>
//		inline void Seed(T index)
//		{
//			srand(index);
//		}
//	
//		// Get Random value from min to max
//		template<typename T>
//		inline T Range(T min, T max)
//		{
//			return min + static_cast <T> (rand()) / (static_cast <T> (RAND_MAX / (max - min)));
//		}
//		// Get Random value from 0 to max
//		template<typename T>
//		inline T Range(T max)
//		{
//			return static_cast <T> (rand()) / (static_cast <T> (RAND_MAX / max));
//		}
//		
//		// Get Random value from min to max (INT SPECIFIC)
//		template<>
//		inline int Range(int min, int max)
//		{
//			return min + (rand() % static_cast<int>(max - min + 1));
//		}
//		// Get Random value from 0 to max (INT SPECIFIC)
//		template<>
//		inline int Range(int max)
//		{
//			return(rand() % static_cast<int>(max + 1));
//		}
//	
//		// Get Random value from 0 to 1
//		template <typename T>
//		inline T OneToZero()
//		{
//			return static_cast <T> (rand()) / static_cast <T> (RAND_MAX);
//		}
//	}

#pragma pop_macro("MACRONAME")