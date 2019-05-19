// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../math/Vector3.h"
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
		Vector3 m_averageSelectionLocation;
		void UpdateAverageSelectionLocation()
		{
			m_averageSelectionLocation = Vector3::ZERO;
			float m_selectedEntitiesCount = (float)m_selectedEntities.size();

			for (auto Entity : m_selectedEntities)
				m_averageSelectionLocation += Entity->m_transform.getWorldPosition() / m_selectedEntitiesCount;
		}

		// Utility
		void DisplayEntity(Entity* _ent, int _depth);
	public:

		// Utility
		const std::vector<Entity*>& GetSelectedEntities(void) const
		{
			return m_selectedEntities;
		}
		// [true] if at least one object is selected
		bool CheckSelection() const
		{
			return m_selectedEntities.size() > 0;
		}

		void RemoveSelection(Entity* _entity);
		void AddSelection(Entity* _entity);
		void ClearSelection();

		const Vector3& GetAverageSelectionLocation() const
		{
			return m_averageSelectionLocation;
		}

		// Behaviour
		void Draw();

	} SingletonInstance(Hierarchy);
}