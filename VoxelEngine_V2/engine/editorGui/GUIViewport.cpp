// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUIViewport.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../textures/Texture2D.h"
#include "../rendering/FramebufferObject.h"

#include "../game/Scene_Game.h"

namespace Vxl
{
	void GUIViewport::Draw()
	{
		//ImGui

		// Menu
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("NONE"))
			{
				m_renderTexture = 0;
			}

			// show render targets here
			if (ImGui::BeginMenu("gbuffer"))
			{
				if (ImGui::MenuItem("composite"))
				{
					FramebufferObject* fbo = FramebufferObject::GetAsset("showRenderTarget");
					m_renderTexture = fbo->GetAttachmentTextureID(0);

					auto game = dynamic_cast<Scene_Game*>(m_scene);
					game->renderTargetID = 0;
				}
				if (ImGui::MenuItem("albedo"))
				{
					FramebufferObject* fbo = FramebufferObject::GetAsset("showRenderTarget");
					m_renderTexture = fbo->GetAttachmentTextureID(0);

					auto game = dynamic_cast<Scene_Game*>(m_scene);
					game->renderTargetID = 1;
				}
				if (ImGui::MenuItem("normal"))
				{
					FramebufferObject* fbo = FramebufferObject::GetAsset("showRenderTarget");
					m_renderTexture = fbo->GetAttachmentTextureID(0);

					auto game = dynamic_cast<Scene_Game*>(m_scene);
					game->renderTargetID = 2;
				}
				if (ImGui::MenuItem("depth"))
				{
					FramebufferObject* fbo = FramebufferObject::GetAsset("showRenderTarget");
					m_renderTexture = fbo->GetAttachmentTextureID(0);

					auto game = dynamic_cast<Scene_Game*>(m_scene);
					game->renderTargetID = 3;
				}
				if (ImGui::MenuItem("editor"))
				{
					FramebufferObject* fbo = FramebufferObject::GetAsset("showRenderTarget");
					m_renderTexture = fbo->GetAttachmentTextureID(0);

					auto game = dynamic_cast<Scene_Game*>(m_scene);
					game->renderTargetID = 4;
				}
				if (ImGui::MenuItem("color Picker"))
				{
					FramebufferObject* fbo = FramebufferObject::GetAsset("showRenderTarget");
					m_renderTexture = fbo->GetAttachmentTextureID(0);

					auto game = dynamic_cast<Scene_Game*>(m_scene);
					game->renderTargetID = 5;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();

			// Composite
		}

		// Get Correct Size for texture
		ImVec2 size = ImGui::GetWindowSize();
		size.y -= (35.0f + 5.0f); // Padding from "name bar" and "menu bar"

		// Display
		ImGui::Image((void*)(intptr_t)m_renderTexture, size, ImVec2(0, 1), ImVec2(1, 0));
	}
}

#endif