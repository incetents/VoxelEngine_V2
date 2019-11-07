// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../utilities/Asset.h"

#include "../rendering/Gizmo.h"

namespace Vxl
{
	static class Editor : public Singleton<class Editor>
	{
	public:
		std::vector<EntityIndex>	m_selectedEntities;

		// [true] if at least one object is selected
		bool HasSelection(void) const;
		void RemoveSelection(EntityIndex _entity);
		void AddSelection(EntityIndex _entity);
		void ClearSelection();


	} SingletonInstance(Editor);
}