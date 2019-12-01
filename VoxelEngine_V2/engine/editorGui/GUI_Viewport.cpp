// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_Viewport.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../rendering/FramebufferObject.h"
#include "../rendering/Shader.h"
#include "../rendering/RenderManager.h"
#include "../rendering/RenderBuffer.h"

#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"

#include "../utilities/Types.h"
#include "../utilities/Asset.h"

#include "../window/window.h"

#include "../game/Scene_Game.h"

namespace Vxl
{
	const char* GUI_Viewport::ChannelOutputNames[] =
	{
		"Channel:[RGBA]",
		"Channel:[RED]",
		"Channel:[GREEN]",
		"Channel:[BLUE]",
		"Channel:[ALPHA]"
	};

	void GUI_Viewport::InitGLResources()
	{
		m_fbo = GlobalAssets.createFramebuffer("GUI_Viewport");
		FramebufferObject* fbo = Assets::getFramebufferObject(m_fbo);
		fbo->setSizeToViewportSize();
		fbo->bind();

		m_renderTexture = GlobalAssets.createRenderTexture(
			"albedo",
			fbo->getWidth(), fbo->getHeight(),
			TextureFormat::RGBA8, TexturePixelType::UNSIGNED_BYTE, false
		);

		fbo->setRenderTexture(0, m_renderTexture);
		fbo->checkFBOStatus();
		fbo->unbind();

	}
	void GUI_Viewport::DestroyGLResources()
	{

	}

	void GUI_Viewport::DrawRenderTarget()
	{
		FramebufferObject*	fbo = Assets::getFramebufferObject(m_fbo);
		RenderTexture*		fbo_rt = Assets::getRenderTexture(m_renderTexture);
		ShaderProgram*		program_showRenderTarget = GlobalAssets.getShader_ShowRenderTarget();
		

		if (!fbo || !fbo_rt || !program_showRenderTarget)
			return;

		fbo->bind();
		fbo->clearBuffers();

		bool outputFound = false;
		if (m_outputRTIsDepth)
		{
			RenderTextureDepth* outputRT = Assets::getRenderTextureDepth(m_outputRT);
			if (outputRT)
			{
				outputRT->bind(TextureLevel::LEVEL0);
				outputFound = true;
			}
		}
		else
		{
			RenderTexture* outputRT = Assets::getRenderTexture(m_outputRT);
			if (outputRT)
			{
				outputRT->bind(TextureLevel::LEVEL0);
				outputFound = true;
			}
		}
		
		if (outputFound)
		{
			program_showRenderTarget->bind();
			// Uniforms
			program_showRenderTarget->sendUniform("channelOutput", (int)m_channelOut);
			program_showRenderTarget->sendUniform("outputMode", m_outputRTIsDepth ? 2 : 0);
			// Draw on screen
			RenderManager.RenderFullScreen();
		}

		fbo->unbind();
	}

	void GUI_Viewport::Draw()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem(m_xrayMode ? "X-ray:[x]" : "X-ray:[ ]"))
			{
				m_xrayMode = !m_xrayMode;
			}

			// FBO
			if (ImGui::BeginMenu(m_outputFBOName.c_str()))
			{
				// None option
				if (ImGui::MenuItem("None"))
				{
					m_outputFBO = -1;
					m_outputRT = -1;
					m_outputFBOName = "FBO[None]";
					m_outputRTName = "RenderTarget[None]";
				}
				// FBO Choices
				auto fbos = Assets::getAllFramebufferObject();
				for (const auto& fbo : fbos)
				{
					if (ImGui::MenuItem(fbo.second->getName().c_str()))
					{
						m_outputFBO = fbo.first;
						m_outputRT = -1;
						m_outputRTName = "RenderTarget[None]";

						FramebufferObject* fbo = Assets::getFramebufferObject(m_outputFBO);
						if (fbo)
							m_outputFBOName = "FBO:[" + fbo->getName() + ']';
						else
							m_outputFBOName = "FBO[None]";
					}
				}
				ImGui::EndMenu();
			}

			// Render Target
			FramebufferObject* selected_fbo = Assets::getFramebufferObject(m_outputFBO);
			if (selected_fbo)
			{
				if (ImGui::BeginMenu(m_outputRTName.c_str()))
				{
					// None option
					if (ImGui::MenuItem("None"))
					{
						m_outputRTName = -1;
					}
					// Attachments
					const auto& attachments = selected_fbo->getAttachments();
					for (const auto& attachment : attachments)
					{
						if (attachment.second.isRenderTexture())
						{
							if (ImGui::MenuItem(attachment.second.getRenderTexture()->getName().c_str()))
							{
								m_outputRT = attachment.second.getAssetIndex();
								m_outputRTIsDepth = false;

								RenderTexture* rt = Assets::getRenderTexture(m_outputRT);
								if (rt)
									m_outputRTName = "RenderTarget[" + rt->getName() + "]";
								else
									m_outputRTName = "RenderTarget[None]";
							}
						}
						else
						{
							ImGui::MenuItem(attachment.second.getRenderBuffer()->getName().c_str());
						}
					}
					// Depth
					if (selected_fbo->hasDepth())
					{
						const RenderTarget& RenderDepth = selected_fbo->getDepth();
						if (RenderDepth.isRenderTexture())
						{
							if (ImGui::MenuItem(RenderDepth.getRenderTextureDepth()->getName().c_str()))
							{
								m_outputRT = RenderDepth.getAssetIndex();
								m_outputRTIsDepth = true;

								RenderTextureDepth* rt = Assets::getRenderTextureDepth(m_outputRT);
								if (rt)
									m_outputRTName = "RenderTarget[" + rt->getName() + "]";
								else
									m_outputRTName = "RenderTarget[None]";
							}
						}
					}
					ImGui::EndMenu();
				}
			}
			else
			{
				ImGui::MenuItem("RenderTarget[NONE]");
			}

			if (ImGui::BeginMenu(ChannelOutputNames[(int)m_channelOut]))
			{
				if (ImGui::MenuItem("RGBA"))
				{
					m_channelOut = ChannelOutput::RGBA;
				}
				if (ImGui::MenuItem("RED"))
				{
					m_channelOut = ChannelOutput::RED;
				}
				if (ImGui::MenuItem("GREEN"))
				{
					m_channelOut = ChannelOutput::GREEN;
				}
				if (ImGui::MenuItem("BLUE"))
				{
					m_channelOut = ChannelOutput::BLUE;
				}
				if (ImGui::MenuItem("ALPHA"))
				{
					m_channelOut = ChannelOutput::ALPHA;
				}
				ImGui::EndMenu();
			}
		
			// END
			ImGui::EndMenuBar();
		}

		// Render Correct RenderTexture information
		DrawRenderTarget();

		// Vertical Window Padding
		static float WindowVerticalPadding = 38.0f;

		// Get Correct Size for texture
		ImVec2 size = ImGui::GetWindowSize();
		size.y -= (WindowVerticalPadding); // Padding from "name bar" and "menu bar"

		//	if (m_xrayMode)
		//	{
		//		//	ImVec2 guiSize = ImGui::GetWindowSize();
		//		//	guiSize.y -= WindowVerticalPadding; // Fix padding
		//		//	ImVec2 guiPos = ImGui::GetWindowPos();
		//		//	guiPos.y += WindowVerticalPadding; // Offset due to padding
		//		//	ImVec2 windowSize = ImVec2((float)Window.GetWindowWidth(), (float)Window.GetWindowHeight());
		//		//	
		//		//	ImVec2 uv_x = ImVec2(guiPos.x / windowSize.x, (guiPos.x + guiSize.x) / windowSize.x);
		//		//	ImVec2 uv_y = ImVec2(guiPos.y / windowSize.y, (guiPos.y + guiSize.y) / windowSize.y);
		//		//	uv_y.x = 1.0f - uv_y.x;
		//		//	uv_y.y = 1.0f - uv_y.y;
		//		//	
		//		//	//if(m_renderTexture)
		//		//	//	ImGui::Image((void*)(intptr_t)m_renderTexture->getID(), size, ImVec2(uv_x.x, uv_y.x), ImVec2(uv_x.y, uv_y.y));
		//	}
		//	else
		{
			// Display 
			RenderTexture* renderTexture = Assets::getRenderTexture(m_renderTexture);
			if (renderTexture)
				ImGui::Image((void*)(intptr_t)renderTexture->getID(), size, ImVec2(0, 1), ImVec2(1, 0));
			else
				ImGui::Image((void*)(intptr_t)0, size);
		}
	}
}

#endif