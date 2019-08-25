// Copyright(c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	class FramebufferObject;
	class RenderTexture;
	class ShaderProgram;

	static class GUIViewport : public Singleton<class GUIViewport>, public GuiWindow
	{
		enum RenderMode
		{
			NONE,
			COMPOSITE,
			ALBEDO,
			NORMAL,
			DEPTH,
			EDITOR,
			COLOR_PICKER
		};

#ifdef GLOBAL_IMGUI
		friend class RenderManager;
	private:
		bool		m_xrayMode = false;
		RenderMode	m_renderMode = RenderMode::NONE;
		
		// Gl Resources
		FramebufferObject*	m_fbo = nullptr;
		RenderTexture*		m_renderTexture = nullptr;
		ShaderProgram*		m_shader_showRenderTarget = nullptr;

		void InitGLResources();
		void DestroyGLResources();
		// Render showrendertarget
		void DrawRenderTarget();
	public:
		// Behaviour
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(GUIViewport);
}