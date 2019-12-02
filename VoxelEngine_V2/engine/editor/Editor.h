// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../utilities/Types.h"

#include "../rendering/Gizmo.h"

namespace Vxl
{
	class GuiWindow;

	static class Editor : public Singleton<class Editor>
	{
	public:
		// Selection
		std::vector<SceneNodeIndex>	m_selectedNodes;
		bool HasSelection(void) const;
		void RemoveSelection(SceneNodeIndex _node);
		void AddSelection(SceneNodeIndex _node);
		void ClearSelection();
		void DeleteSelection();

		// Imgui render window
		std::vector<GuiWindow*> m_guiWindows;

		// Behaviour
		void Init();
		void Update();

	} SingletonInstance(Editor);
}