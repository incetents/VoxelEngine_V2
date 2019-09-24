// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

#include "../rendering/Gizmo.h"

namespace Vxl
{
	class Entity;

	static class Editor : public Singleton<class Editor>
	{
	public:
		std::vector<Entity*>	m_selectedEntities;
		
		// [true] if at least one object is selected
		inline bool HasSelection() const
		{
			return m_selectedEntities.size() > 0;
		}
		void RemoveSelection(Entity* _entity);
		void AddSelection(Entity* _entity);
		void ClearSelection();


	} SingletonInstance(Editor);
}