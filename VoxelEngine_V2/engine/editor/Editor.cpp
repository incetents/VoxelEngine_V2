// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "Editor.h"
#include "../modules/Entity.h"
#include "../utilities/Asset.h"
#include "../rendering/RenderManager.h"
#include "../input/Input.h"
#include "../imgui/imgui.h"

namespace Vxl
{
	bool Editor::HasSelection(void) const
	{
		return m_selectedNodes.size() > 0;
	}
	void Editor::RemoveSelection(SceneNodeIndex _node)
	{
		Util::RemoveFromVector(m_selectedNodes, _node);

		// Internally remembers
		SceneNode* node = Assets::getSceneNode(_node);
		if(node)
			node->m_isSelected = false;
	}
	void Editor::AddSelection(SceneNodeIndex _node)
	{
		for (const auto& node : m_selectedNodes)
			if (node == _node)
				return;

		m_selectedNodes.push_back(_node);

		// Internally remembers
		SceneNode* node = Assets::getSceneNode(_node);
		if(node)
			node->m_isSelected = true;
	}
	void Editor::ClearSelection()
	{
		for (const auto& _node : m_selectedNodes)
		{
			SceneNode* node = Assets::getSceneNode(_node);
			if(node)
				node->m_isSelected = false;
		}
		m_selectedNodes.clear();
	}
	void Editor::DeleteSelection()
	{
		for (SceneNodeIndex _node : m_selectedNodes)
		{
			Assets::deleteSceneNode(_node);
		}
		m_selectedNodes.clear();
	}


	void Editor::Update()
	{
		if (ImGui::GetIO().KeysDown[(int)KeyCode::DELETEKEY] || ImGui::GetIO().KeysDown[(int)KeyCode::BACKSPACE])
		{
			DeleteSelection();
		}
	}
}