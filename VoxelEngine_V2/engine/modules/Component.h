// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Types.h"

namespace Vxl
{
	class Component
	{
	public:
		EntityIndex m_owner = -1;
	};

	template<typename Type = Component*>
	class ComponentMaster
	{
	public:
		static std::vector<Component*> allComponents;
		static std::vector<Type*> getAll()
		{
			std::vector<Type*> result;

			std::transform(
				allComponents.begin(), allComponents.end(),
				back_inserter(result),
				[](Component* c) { return static_cast<Type*>(c); }
			);

			return result;
		}
	};

	template<typename Type>
	std::vector<Component*> ComponentMaster<Type>::allComponents;
}

