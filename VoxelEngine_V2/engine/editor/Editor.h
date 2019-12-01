// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../utilities/Types.h"

#include "../rendering/Gizmo.h"

namespace Vxl
{
	static class Editor : public Singleton<class Editor>
	{
	public:
		std::vector<SceneNodeIndex>	m_selectedNodes;

		// [true] if at least one object is selected
		bool HasSelection(void) const;
		void RemoveSelection(SceneNodeIndex _node);
		void AddSelection(SceneNodeIndex _node);
		void ClearSelection();
		void DeleteSelection();

		void Update();

	} SingletonInstance(Editor);
}