// Copyright (c) 2020 Emmanuel Lajeunesse
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
		bool hasSelection(void) const;
		void removeSelection(SceneNodeIndex _node);
		void addSelection(SceneNodeIndex _node);
		void clearSelection();
		void deleteSelection();

		// Imgui render window
		std::vector<GuiWindow*> m_guiWindows;

		// Behaviour
		void init();
		void update();

	} SingletonInstance(Editor);
}