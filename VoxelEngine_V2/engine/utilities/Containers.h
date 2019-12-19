// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Macros.h"

#include "../rendering/Graphics.h"

#include <assert.h>

namespace Vxl
{
	struct RawData
	{
		std::shared_ptr<void> data;
		UniformType type;

		template<typename Type>
		RawData(UniformType _type, Type _data)
			: type(_type)
		{
			data = std::make_shared<Type>(_data);
		}
		RawData() {}

		template<typename Type>
		void setData(Type newData)
		{
			*static_cast<Type*>(data.get()) = newData;
		}

		template<typename Type>
		Type* getData() const
		{
			return static_cast<Type*>(data.get());
		}
	};

	// RAW ARRAY CONTAINER //
	template<typename Type>
	class RawArray
	{
	public:
		Type* start = nullptr;
		unsigned int length = 0;

		bool IsEmpty(void) const
		{
			return start == nullptr;
		}
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