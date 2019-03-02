// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <random>
#include <time.h>

namespace Random
{
	// Set Seed as Time
	inline void Seed()
	{
		srand(u_int(time(NULL)));
	}
	// Set Seed by value
	template<typename T>
	inline void Seed(T index)
	{
		srand(index);
	}

	// Get Random value from min to max
	template<typename T>
	inline T Range(T min, T max)
	{
		return min + static_cast <T> (rand()) / (static_cast <T> (RAND_MAX / (max - min)));
	}
	// Get Random value from 0 to max
	template<typename T>
	inline T Range(T max)
	{
		return static_cast <T> (rand()) / (static_cast <T> (RAND_MAX / max));
	}
	
	// Get Random value from min to max (INT SPECIFIC)
	template<>
	inline int Range(int min, int max)
	{
		return min + (rand() % static_cast<int>(max - min + 1));
	}
	// Get Random value from 0 to max (INT SPECIFIC)
	template<>
	inline int Range(int max)
	{
		return(rand() % static_cast<int>(max + 1));
	}

	// Get Random value from 0 to 1
	template <typename T>
	inline T OneToZero()
	{
		return static_cast <T> (rand()) / static_cast <T> (RAND_MAX);
	}
}