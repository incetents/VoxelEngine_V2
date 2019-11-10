// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "Editor.h"
#include "../modules/Entity.h"
#include "../utilities/Asset.h"
#include "../rendering/RenderManager.h"

namespace Vxl
{
	bool Editor::HasSelection(void) const
	{
		return m_selectedEntities.size() > 0;
	}
	void Editor::RemoveSelection(EntityIndex _entity)
	{
		Util::RemoveFromVector(m_selectedEntities, _entity);
		Entity* entity = Assets::getEntity(_entity);
		entity->m_isSelected = false;
	}
	void Editor::AddSelection(EntityIndex _entity)
	{
		for (const auto& _Entity : m_selectedEntities)
			if (_Entity == _entity)
				return;

		m_selectedEntities.push_back(_entity);
		Entity* entity = Assets::getEntity(_entity);
		entity->m_isSelected = true;
	}
	void Editor::ClearSelection()
	{
		for (auto _entity : m_selectedEntities)
		{
			Entity* entity = Assets::getEntity(_entity);
			if(entity)
				entity->m_isSelected = false;
		}
		m_selectedEntities.clear();
	}

}