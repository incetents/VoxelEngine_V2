// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"
#include "../utilities/Types.h"

namespace Vxl
{
	class SceneNode;

	static class Hierarchy : public Singleton<class Hierarchy>, public GuiWindow
	{
#ifdef GLOBAL_IMGUI
		friend class Inspector;
	private:
		// Utility
		void DisplayNode(SceneNode* _node, int _depth);
	public:

		// Behaviour
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(Hierarchy);
}