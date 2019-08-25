// Copyright(c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	class FramebufferObject;
	class RenderTexture;

	static class GUIViewport : public Singleton<class GUIViewport>, public GuiWindow
	{
#ifdef GLOBAL_IMGUI
		friend class RenderManager;
	private:
		bool m_xrayMode = false;
		//uint32_t m_renderTexture = 0;
		
		// Gl Resources
		FramebufferObject* m_fbo = nullptr;
		RenderTexture* m_renderTexture = nullptr;
		void InitGLResources();
		void DestroyGLResources();
	public:
		// Behaviour
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(GUIViewport);
}