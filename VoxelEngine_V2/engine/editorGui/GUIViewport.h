// Copyright(c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

namespace Vxl
{

	static class GUIViewport : public Singleton<class GUIViewport>, public GuiWindow
	{
#ifdef GLOBAL_IMGUI
		friend class RenderManager;
	private:
		uint32_t m_renderTexture = 0;
	public:
		// Behaviour
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(GUIViewport);
}