// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUIViewport.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../rendering/FramebufferObject.h"
#include "../rendering/Shader.h"
#include "../rendering/RenderManager.h"
#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"
#include "../utilities/Types.h"
#include "../window/window.h"

#include "../game/Scene_Game.h"

namespace Vxl
{
	void GUIViewport::InitGLResources()
	{
		//	m_fbo = GlobalAssets.createFramebuffer();
		//	FramebufferObject* fbo = GlobalAssets.getFramebufferObject(m_fbo);
		//	fbo->setSizeToViewportSize();
		//	fbo->bind();
		//	
		//	m_renderTexture = GlobalAssets.createRenderTexture(
		//		fbo->getWidth(), fbo->getHeight(),
		//		TextureFormat::RGBA8, TexturePixelType::UNSIGNED_BYTE, false
		//	);
		//	fbo->setAttachment(0, GlobalAssets.getRenderTexture(m_renderTexture));
		//	
		//	fbo->checkFBOStatus();
		//	fbo->unbind();
		//	
		//	// Store reference of FBO renderTexture
		//	m_renderTexture = GlobalAssets.getRenderTexture(m_renderTexture);

	}
	void GUIViewport::DestroyGLResources()
	{
		//	//RenderTexture::DeleteUnnamedAsset(m_renderTexture);
		//	m_renderTexture = nullptr;
		//	
		//	//FramebufferObject::DeleteNamedAsset(m_fbo);
		//	m_fbo = nullptr;
		//	
		//	//RenderTexture::DeleteNamedAsset(m_renderTexture);
		//	m_renderTexture = nullptr;
	}

	void GUIViewport::DrawRenderTarget()
	{
		//	// Begin
		//	FramebufferObject* fbo_gbuffer = SceneAssets.getFramebufferObject(fboIndex_gbuffer);
		//	FramebufferObject* fbo_editor = SceneAssets.getFramebufferObject(fboIndex_editor);
		//	
		//	VXL_ASSERT(fbo_gbuffer, "Missing FBO Gbuffer");
		//	VXL_ASSERT(fbo_editor, "Missing FBO Editor");
		//	
		//	//Graphics::SetBlendState(false);
		//	
		//	m_shader_showRenderTarget = ShaderProgram::GetAsset("showRenderTarget");
		//	VXL_ASSERT(m_shader_showRenderTarget, "Missing showRenderTarget shader for GUIViewport");
		//	
		//	m_fbo->Bind();
		//	m_fbo->ClearBuffers();
		//	
		//	m_shader_showRenderTarget->Bind();
		//	m_shader_showRenderTarget->SetUniform("channelOutput", (int)m_channelOut);
		//	//
		//	
		//	switch (m_renderMode)
		//	{
		//		// Composite
		//	case RenderMode::COMPOSITE:
		//		{
		//			m_shader_showRenderTarget->SetUniform("outputMode", 3);
		//	
		//			fbo_gbuffer->bindTexture(0, TextureLevel::LEVEL0);
		//			fbo_editor->bindTexture(0, TextureLevel::LEVEL1);
		//			
		//			break;
		//		}
		//		// Albedo
		//	case RenderMode::ALBEDO:
		//		{
		//			m_shader_showRenderTarget->SetUniform("outputMode", 0);
		//	
		//			fbo_gbuffer->bindTexture(0, TextureLevel::LEVEL0);
		//	
		//			break;
		//		}
		//		// Normal
		//	case RenderMode::NORMAL:
		//		{
		//			m_shader_showRenderTarget->SetUniform("outputMode", 1);
		//	
		//			fbo_gbuffer->bindTexture(1, TextureLevel::LEVEL0);
		//	
		//			break;
		//		}
		//		// Normal
		//	case RenderMode::DEPTH:
		//		{
		//			m_shader_showRenderTarget->SetUniform("outputMode", 2);
		//	
		//			fbo_gbuffer->bindDepth(TextureLevel::LEVEL0);
		//	
		//			break;
		//		}
		//		// EDITOR
		//	case RenderMode::EDITOR:
		//		{
		//			m_shader_showRenderTarget->SetUniform("outputMode", 0);
		//	
		//			fbo_editor->bindTexture(0, TextureLevel::LEVEL0);
		//	
		//			break;
		//		}
		//		// COLOR PICKER
		//	case RenderMode::COLOR_PICKER:
		//		{
		//			m_shader_showRenderTarget->SetUniform("outputMode", 4);
		//	
		//			fbo_gbuffer->bindTexture(3, TextureLevel::LEVEL0);
		//	
		//			break;
		//		}
		//	}
		//	if(m_renderMode != RenderMode::NONE)
		//		RenderManager.RenderFullScreen();
		//	
		//	// End [prevent imgui from rendering to this fbo]
		//	m_fbo->Unbind();
	}

	void GUIViewport::Draw()
	{
		// Menu
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem(m_xrayMode ? "X-ray[x]" : "X-ray[ ]"))
			{
				m_xrayMode = !m_xrayMode;
			}

			// Select Channel to output
			std::string menuName_Channels;
			switch (m_channelOut)
			{
			case ChannelOutput::RGBA:
				menuName_Channels = "Channel [RGBA]";
				break;
			case ChannelOutput::RED:
				menuName_Channels = "Channel [R]";
				break;
			case ChannelOutput::GREEN:
				menuName_Channels = "Channel [G]";
				break;
			case ChannelOutput::BLUE:
				menuName_Channels = "Channel [B]";
				break;
			case ChannelOutput::ALPHA:
				menuName_Channels = "Channel [A]";
				break;
			}

			if (ImGui::BeginMenu(menuName_Channels.c_str()))
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

			if (ImGui::MenuItem("{None}"))
			{
				m_renderMode = RenderMode::NONE;
			}

			// show render targets here
			if (ImGui::BeginMenu("{Gbuffer}"))
			{
				if (ImGui::MenuItem("composite"))
				{
					m_renderMode = RenderMode::COMPOSITE;
				}
				if (ImGui::MenuItem("albedo"))
				{
					m_renderMode = RenderMode::ALBEDO;
				}
				if (ImGui::MenuItem("normal"))
				{
					m_renderMode = RenderMode::NORMAL;
				}
				if (ImGui::MenuItem("depth"))
				{
					m_renderMode = RenderMode::DEPTH;
				}
				if (ImGui::MenuItem("editor"))
				{
					m_renderMode = RenderMode::EDITOR;
				}
				if (ImGui::MenuItem("color Picker"))
				{
					m_renderMode = RenderMode::COLOR_PICKER;
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

		if (m_xrayMode)
		{
			ImVec2 guiSize = ImGui::GetWindowSize();
			guiSize.y -= WindowVerticalPadding; // Fix padding
			ImVec2 guiPos = ImGui::GetWindowPos();
			guiPos.y += WindowVerticalPadding; // Offset due to padding
			ImVec2 windowSize = ImVec2((float)Window.GetWindowWidth(), (float)Window.GetWindowHeight());

			ImVec2 uv_x = ImVec2(guiPos.x / windowSize.x, (guiPos.x + guiSize.x) / windowSize.x);
			ImVec2 uv_y = ImVec2(guiPos.y / windowSize.y, (guiPos.y + guiSize.y) / windowSize.y);
			uv_y.x = 1.0f - uv_y.x;
			uv_y.y = 1.0f - uv_y.y;

			//if(m_renderTexture)
			//	ImGui::Image((void*)(intptr_t)m_renderTexture->GetID(), size, ImVec2(uv_x.x, uv_y.x), ImVec2(uv_x.y, uv_y.y));
		}
		else
		{
			//// Display 
			//if (m_renderTexture)
			//	ImGui::Image((void*)(intptr_t)m_renderTexture->GetID(), size, ImVec2(0, 1), ImVec2(1, 0));
			//else
			//	ImGui::Image((void*)(intptr_t)0, size);
		}
	}
}

#endif