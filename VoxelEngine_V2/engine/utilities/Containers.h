// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <assert.h>

namespace Vxl
{
	template<typename Type>
	class RawArray
	{
	public:
		Type* start;
		unsigned int length;

		void Allocate(int size)
		{
			start = new Type[size];
			length = size;
		}
		void Deallocate()
		{
			delete[] start;
		}
		inline Type operator[](unsigned int index) const
		{
			assert(index < length);
			return start[index];
		}
	};
}