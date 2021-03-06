// Copyright(c) 2020 Emmanuel Lajeunesse
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
		friend class RenderManager;
	public:
		enum ChannelOutput
		{
			RGBA = 0,
			RED = 1,
			GREEN = 2,
			BLUE = 3,
			ALPHA = 4
		};
		static const char* ChannelOutputNames[];

	private:
#ifdef GLOBAL_IMGUI
		bool		  m_xrayMode = false;
		ChannelOutput m_channelOut = ChannelOutput::RGBA;
		RenderTexture::ViewMode	m_viewMode = RenderTexture::ViewMode::BLIT;

		// Gl Resources
		FramebufferObjectIndex	m_fbo;
		RenderTextureIndex		m_renderTexture;

		// Output
		FramebufferObjectIndex m_outputFBO = -1;
		std::string m_outputFBOName = "FBO[None]";
		RenderTextureIndex m_outputRT = -1;
		std::string m_outputRTName = "RenderTarget[None]";
		bool m_outputRTIsDepth = false;

		void InitGLResources();
		void DestroyGLResources();
		//
		void DrawRenderTarget();
	public:

		// Behaviour
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(GUI_Viewport);
}