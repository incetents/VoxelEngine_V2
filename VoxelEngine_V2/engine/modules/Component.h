// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <typeinfo>
#include <unordered_map>

namespace Vxl
{
	class Entity;

	class Component
	{
		friend class ComponentHandler;
	protected:
		Entity* m_owner;
	};

	class ComponentHandler
	{
	protected:
		std::unordered_map<const std::type_info*, Component*> m_components;
	public:

		// Check for component
		template <typename T>
		bool CheckComponentExists()
		{
			return m_components.count(&typeid(T)) != 0;
		}

		// Add Component
		template <typename T = Component *>
		T* AddComponent(T* a_component, Entity* owner)
		{
			if (a_component != nullptr && m_components.count(&typeid(*a_component)) == 0)
			{
				m_components[&typeid(*a_component)] = (a_component);
				m_components[&typeid(*a_component)]->m_owner = owner;

				return a_component;
			}
			return nullptr;
		}

		// Add Component (new)
		template <typename T = Component * >
		T* AddComponent(Entity* owner)
		{
			T* _data = new T();
			return AddComponent(_data, owner);
		}

		// Get Component
		template <typename T>
		T* GetComponent()
		{
			if (m_components.count(&typeid(T)) != 0)
			{
				return static_cast<T*>(m_components[&typeid(T)]);
			}
			return nullptr;
		}
	};

	
}

