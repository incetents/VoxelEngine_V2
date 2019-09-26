// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "Editor.h"
#include "../modules/Entity.h"
#include "../rendering/RenderManager.h"

namespace Vxl
{
	bool Editor::HasSelection(void) const
	{
		return m_selectedEntities.size() > 0;
	}
	void Editor::RemoveSelection(Entity* _entity)
	{
		Util::RemoveFromVector(m_selectedEntities, _entity);
		_entity->m_isSelected = false;
	}
	void Editor::AddSelection(Entity* _entity)
	{
		VXL_ASSERT(_entity, "Adding nullptr in Editor::AddSelection");

		for (const auto& _Entity : m_selectedEntities)
			if (_Entity == _entity)
				return;

		m_selectedEntities.push_back(_entity);
		_entity->m_isSelected = true;
	}
	void Editor::ClearSelection()
	{
		for (auto Entity : m_selectedEntities)
			Entity->m_isSelected = false;
		m_selectedEntities.clear();
	}

}