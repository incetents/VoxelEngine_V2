// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include <vector>

namespace Vxl
{
	class Entity;

	static class Hierarchy : public Singleton<class Hierarchy>
	{
		friend class RenderManager;
		friend class Inspector;
	private:
		// Data
		std::vector<Entity*> m_selectedEntities;

		// Utility
		void DisplayEntity(Entity* _ent, int _depth);
	public:

		// Utility
		const std::vector<Entity*>& GetSelectedEntities(void) const
		{
			return m_selectedEntities;
		}
		void RemoveSelection(Entity* _entity);
		void AddSelection(Entity* _entity);
		void ClearSelection();

		// Behaviour
		void Draw();

	} SingletonInstance(Hierarchy);
}