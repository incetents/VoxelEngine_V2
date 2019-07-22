// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Macros.h"
#include <assert.h>

namespace Vxl
{
	template<typename Type>
	class RawArray
	{
	public:
		Type* start = nullptr;
		unsigned int length = 0;

		void Allocate(int size)
		{
			if (start != nullptr)
				delete[] start;

			start = new Type[size];
			length = size;
		}
		void Deallocate()
		{
			if (start != nullptr)
			{
				delete[] start;
				start = nullptr;
				length = 0;
			}
		}
		inline Type operator[](unsigned int index) const
		{
			VXL_ASSERT(start != nullptr, "RawArray: no data allocated to this RawArray");
			VXL_ASSERT(index < length, "RawArray: index out of bounds");
			return start[index];
		}
	};
}