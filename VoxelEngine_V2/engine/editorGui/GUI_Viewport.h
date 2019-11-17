// Copyright(c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"
#include "../utilities/Types.h"

namespace Vxl
{
	class FramebufferObject;
	class RenderTexture;

	static class GUI_Viewport : public Singleton<class GUI_Viewport>, public GuiWindow
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
		enum ChannelOutput
		{
			RGBA = 0,
			RED = 1,
			GREEN = 2,
			BLUE = 3,
			ALPHA = 4
		};

#ifdef GLOBAL_IMGUI
		friend class RenderManager;
	private:
		bool		m_xrayMode = false;
		RenderMode	m_renderMode = RenderMode::NONE;
		ChannelOutput m_channelOut = ChannelOutput::RGBA;

		// Gl Resources
		//FramebufferObjectIndex	m_fbo;
		//RenderTextureIndex		m_renderTexture;
		//ShaderProgram*		m_shader_showRenderTarget = nullptr;

		void InitGLResources();
		void DestroyGLResources();
		// Render showrendertarget
		void DrawRenderTarget();
	public:

		// Scene Data
		FramebufferObjectIndex fboIndex_gbuffer;
		FramebufferObjectIndex fboIndex_editor;

		// Behaviour
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(GUI_Viewport);
}