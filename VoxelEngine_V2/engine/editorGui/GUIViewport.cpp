// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUIViewport.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../textures/Texture2D.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/Shader.h"
#include "../rendering/RenderManager.h"
#include "../textures/RenderTexture.h"
#include "../window/window.h"

#include "../game/Scene_Game.h"

namespace Vxl
{
	void GUIViewport::InitGLResources()
	{
		m_fbo = FramebufferObject::Create("GUIViewport");
		m_fbo->SetSizeToWindowSize();
		m_fbo->Bind();
		m_renderTexture = m_fbo->CreateRenderTexture("");
		m_fbo->SetAttachment(0, m_renderTexture);
		m_fbo->checkFBOStatus();
		m_fbo->Unbind();

		

		//m_renderTexture = RenderTexture::Create("GUIViewport");
	}
	void GUIViewport::DestroyGLResources()
	{
		RenderTexture::DeleteUnnamedAsset(m_renderTexture);
		m_renderTexture = nullptr;

		FramebufferObject::DeleteNamedAsset(m_fbo);
		m_fbo = nullptr;

		//RenderTexture::DeleteNamedAsset(m_renderTexture);
		//m_renderTexture = nullptr;
	}

	void GUIViewport::DrawRenderTarget()
	{
		// Begin
		//Graphics::SetBlendState(false);

		m_shader_showRenderTarget = ShaderProgram::GetAsset("showRenderTarget");
		VXL_ASSERT(m_shader_showRenderTarget, "Missing showRenderTarget shader for GUIViewport");

		m_fbo->Bind();
		m_fbo->ClearBuffers();

		m_shader_showRenderTarget->Bind();
		//

		switch (m_renderMode)
		{
			// Composite
		case RenderMode::COMPOSITE:
			{
				m_shader_showRenderTarget->SetUniform("outputMode", 3);

				auto gbuffer	= FramebufferObject::GetAsset("gbuffer");
				auto editor		= FramebufferObject::GetAsset("EditorPost");
				gbuffer->bindTexture(0, TextureLevel::LEVEL0);
				editor->bindTexture(0, TextureLevel::LEVEL1);
				
				break;
			}
			// Albedo
		case RenderMode::ALBEDO:
			{
				m_shader_showRenderTarget->SetUniform("outputMode", 0);

				auto gbuffer	= FramebufferObject::GetAsset("gbuffer");
				gbuffer->bindTexture(0, TextureLevel::LEVEL0);

				break;
			}
			// Normal
		case RenderMode::NORMAL:
			{
				m_shader_showRenderTarget->SetUniform("outputMode", 1);

				auto gbuffer	= FramebufferObject::GetAsset("gbuffer");
				gbuffer->bindTexture(1, TextureLevel::LEVEL0);

				break;
			}
			// Normal
		case RenderMode::DEPTH:
			{
				m_shader_showRenderTarget->SetUniform("outputMode", 2);

				auto gbuffer	= FramebufferObject::GetAsset("gbuffer");
				gbuffer->bindDepth(TextureLevel::LEVEL0);

				break;
			}
			// EDITOR
		case RenderMode::EDITOR:
			{
				m_shader_showRenderTarget->SetUniform("outputMode", 0);

				auto editor		= FramebufferObject::GetAsset("EditorPost");
				editor->bindTexture(0, TextureLevel::LEVEL0);

				break;
			}
			// COLOR PICKER
		case RenderMode::COLOR_PICKER:
			{
				m_shader_showRenderTarget->SetUniform("outputMode", 4);

				auto gbuffer	= FramebufferObject::GetAsset("gbuffer");
				gbuffer->bindTexture(2, TextureLevel::LEVEL0);

				break;
			}
		}
		if(m_renderMode != RenderMode::NONE)
			RenderManager.RenderFullScreen();

		// End [prevent imgui from rendering to this fbo]
		m_fbo->Unbind();
	}

	void GUIViewport::Draw()
	{
		// TEST
		//	{
		//	
		//		Graphics::SetBlendState(false);
		//		ShaderProgram* _shader_showRenderTarget = ShaderProgram::GetAsset("showRenderTarget");
		//	
		//		m_fbo->Bind();
		//		m_fbo->ClearBuffers();
		//	
		//		_shader_showRenderTarget->Bind();
		//		_shader_showRenderTarget->SetUniform("outputMode", 3);
		//	
		//		auto gbuffer = FramebufferObject::GetAsset("gbuffer");
		//		auto editor = FramebufferObject::GetAsset("EditorPost");
		//		gbuffer->bindTexture(0, TextureLevel::LEVEL0);
		//		editor->bindTexture(0, TextureLevel::LEVEL1);
		//	
		//		RenderManager.RenderFullScreen();
		//	
		//		m_fbo->Unbind();
		//	}

		// Menu
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem(m_xrayMode ? "X-ray[x]" : "X-ray[ ]"))
			{
				m_xrayMode = !m_xrayMode;
			}

			if (ImGui::MenuItem("NONE"))
			{
				m_renderMode = RenderMode::NONE;
			}

			// show render targets here
			if (ImGui::BeginMenu("gbuffer"))
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
			ImGui::EndMenuBar();

			// Composite
		}

		// Render Correct RenderTexture information
		DrawRenderTarget();

		// Get Correct Size for texture
		ImVec2 size = ImGui::GetWindowSize();
		size.y -= (35.0f + 5.0f); // Padding from "name bar" and "menu bar"

		if (m_xrayMode)
		{
			ImVec2 guiSize = ImGui::GetWindowSize();
			guiSize.y -= 40.0f; // Fix padding
			ImVec2 guiPos = ImGui::GetWindowPos();
			guiPos.y += 40.0f; // Offset due to padding
			ImVec2 windowSize = ImVec2((float)Window.GetWindowWidth(), (float)Window.GetWindowHeight());

			ImVec2 uv_x = ImVec2(guiPos.x / windowSize.x, (guiPos.x + guiSize.x) / windowSize.x);
			ImVec2 uv_y = ImVec2(guiPos.y / windowSize.y, (guiPos.y + guiSize.y) / windowSize.y);
			uv_y.x = 1.0f - uv_y.x;
			uv_y.y = 1.0f - uv_y.y;

			if(m_renderTexture)
				ImGui::Image((void*)(intptr_t)m_renderTexture->GetID(), size, ImVec2(uv_x.x, uv_y.x), ImVec2(uv_x.y, uv_y.y));
		}
		else
		{
			// Display 
			if (m_renderTexture)
				ImGui::Image((void*)(intptr_t)m_renderTexture->GetID(), size, ImVec2(0, 1), ImVec2(1, 0));
			else
				ImGui::Image((void*)(intptr_t)0, size);
		}
	}
}

#endif