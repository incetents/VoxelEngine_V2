// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Editor.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"
#include "../input/Input.h"
#include "../modules/Entity.h"
#include "../rendering/RenderManager.h"
#include "../utilities/Asset.h"

#include "../editorGui/GUI_DevConsole.h"
#include "../editorGui/GUI_Viewport.h"
#include "../editorGui/GUI_Hierarchy.h"
#include "../editorGui/GUI_Inspector.h"
#include "../editorGui/GUI_Performance.h"
#include "../editorGui/GUI_ShaderErrors.h"
#include "../editorGui/GUI_ShaderCodeViewer.h"

namespace Vxl
{
	bool Editor::hasSelection(void) const
	{
		return m_selectedNodes.size() > 0;
	}
	void Editor::removeSelection(SceneNodeIndex _node)
	{
		Util::RemoveFromVector(m_selectedNodes, _node);

		// Internally remembers
		SceneNode* node = Assets.getSceneNode(_node);
		if(node)
			node->m_isSelected = false;
	}
	void Editor::addSelection(SceneNodeIndex _node)
	{
		for (const auto& node : m_selectedNodes)
			if (node == _node)
				return;

		m_selectedNodes.push_back(_node);

		// Internally remembers
		SceneNode* node = Assets.getSceneNode(_node);
		if(node)
			node->m_isSelected = true;
	}
	void Editor::clearSelection()
	{
		for (const auto& _node : m_selectedNodes)
		{
			SceneNode* node = Assets.getSceneNode(_node);
			if(node)
				node->m_isSelected = false;
		}
		m_selectedNodes.clear();
	}
	void Editor::deleteSelection()
	{
		for (SceneNodeIndex _node : m_selectedNodes)
		{
			Assets.deleteSceneNode(_node);
		}
		m_selectedNodes.clear();
	}

	void Editor::init()
	{
		// GL Resources

#ifdef GLOBAL_IMGUI
		// Imgui Initial Values
		DevConsole.Init("DevConsole", Vector2(280.f, 380.f), 0.9f, ImGuiWindowFlags_MenuBar);
		ShaderErrors.Init("ShaderErrors", Vector2(700.f, 400.f), 0.9f);
		ShaderErrors.SetOpen(false);
		Inspector.Init("Inspector", Vector2(380, 280), 0.9f);
		Hierarchy.Init("Hierarchy", Vector2(280, 380), 0.9f);
		Performance.Init("Performance", Vector2(280, 680), 0.9f);
		GUI_Viewport.Init("Viewport", Vector2(500, 500), 0.9f, ImGuiWindowFlags_MenuBar);
		GUI_Viewport.SetPadding(false);
		GUI_Viewport.SetOpen(false);
		ShaderCodeViewer.Init("ShaderCodeViewer", Vector2(700, 400), 0.9f);
		ShaderCodeViewer.SetOpen(false);

		m_guiWindows.push_back(&DevConsole.instanceRef);
		m_guiWindows.push_back(&ShaderErrors.instanceRef);
		m_guiWindows.push_back(&Inspector.instanceRef);
		m_guiWindows.push_back(&Hierarchy.instanceRef);
		m_guiWindows.push_back(&Performance.instanceRef);
		m_guiWindows.push_back(&GUI_Viewport.instanceRef);
		m_guiWindows.push_back(&ShaderCodeViewer.instanceRef);
#endif
	}

	void Editor::update()
	{
		// Imgui
#ifdef GLOBAL_IMGUI
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			// VIEW //
			if (ImGui::BeginMenu("View"))
			{
				for (GuiWindow* window : m_guiWindows)
				{
					if (ImGui::MenuItem(window->GetName().c_str(), "", window->IsOpen()))
					{
						window->ToggleOpen();
					}
				}
				//	if (ImGui::MenuItem(DevConsole.GetName().c_str(), "", DevConsole.IsOpen()))
				//	{ 
				//		DevConsole.ToggleOpen();
				//	}

				ImGui::EndMenu();
			}

			// SHADER ERRORS //
			{
				bool hasErrors = ShaderErrors.HasErrors();

				if (hasErrors)
					ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::RedLight);
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Grey);

				if (ImGui::MenuItem(hasErrors ? "Shader Errors !" : "Shader Errors"))
				{
					ShaderErrors.ToggleOpen();
				}

				ImGui::PopStyleColor();
			}
		}
		ImGui::EndMainMenuBar();

		// Drawing All Imgui Windows here
		for (auto& GUI : m_guiWindows)
		{
			if (!GUI->IsOpen())
				continue;

			GUI->SetScene(RenderManager.m_currentScene);
			if (GUI->Begin())
			{
				GUI->Draw();
			}
			GUI->End();
		}

		//m_currentScene->DrawImGui();

		ImGui::Render();

		// Selection
		if (ImGui::GetIO().KeysDown[(int)KeyCode::DELETEKEY] || ImGui::GetIO().KeysDown[(int)KeyCode::BACKSPACE])
		{
			deleteSelection();
		}
#endif
	}
}