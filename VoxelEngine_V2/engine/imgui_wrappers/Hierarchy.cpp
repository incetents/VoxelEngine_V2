// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Hierarchy.h"

#include "../imgui/imgui.h"

#include "../modules/RenderManager.h"
#include "../modules/Entity.h"

#include "../input/Input.h"

#include <iostream>

namespace Vxl
{
	void Hierarchy::DisplayEntity(Entity* _entity, int _depth)
	{
		static ImVec4 Color_Normal = ImVec4(1, 1, 1, 1);
		static ImVec4 Color_Grey = ImVec4(0.3f, 0.3f, 0.3f, 1);

		// Only show entities with names
		if (!_entity->m_name.empty())
		{
			// Display entity
			//	-> Collapsing if it has children
			//	-> Text if it has no children
			auto ChildCount = _entity->m_transform.getChildCount();
			if (ChildCount == 0)
			{
				// flags
				int flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog;
				if (_entity == _selectedEntity)
					flags |= ImGuiTreeNodeFlags_Selected;

				// color start
				if (!_entity->IsFamilyActive())
					ImGui::PushStyleColor(ImGuiCol_Text, Color_Grey);

				// node
				ImGui::TreeNodeEx(_entity->m_name.c_str(), flags);
				// selection case
				if (ImGui::IsItemClicked())
					_selectedEntity = _entity;

				// color end
				if (!_entity->IsFamilyActive())
					ImGui::PopStyleColor();
			}
			else
			{
				// flags
				int flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
				if (_entity == _selectedEntity)
					flags |= ImGuiTreeNodeFlags_Selected;

				// color start
				if (!_entity->IsFamilyActive())
					ImGui::PushStyleColor(ImGuiCol_Text, Color_Grey);

				// node
				bool node_open = ImGui::TreeNodeEx(_entity->m_name.c_str(), flags);

				// selection
				if (ImGui::IsItemClicked())
					_selectedEntity = _entity;

				// color end
				if (!_entity->IsFamilyActive())
					ImGui::PopStyleColor();

				// recursion node
				if (node_open)
				{
					ImGui::Indent();
					auto children = _entity->m_transform.getChildren();
					for (auto it = children.begin(); it != children.end(); it++)
					{
						DisplayEntity((*it)->GetOwner(), _depth+1);
					}
					ImGui::Unindent();
				}
				
			}
		}
	}

	void Hierarchy::Draw()
	{
		static bool open = true;

		if (Input.getKeyDown(KeyCode::F3))
			open = !open;

		if (!open)
			return;

		if (ImGui::Begin("[F3] Hierarchy", &open, ImVec2(280, 380), 0.9f))
		{
			auto AllEntities = RenderManager.m_allEntities;
			for (auto it = AllEntities.begin(); it != AllEntities.end(); it++)
			{
				// Only display entities without parents
				if((*it)->m_transform.getParent() == nullptr)
					DisplayEntity(*it, 0);
			}
		}
		ImGui::End();
	}
}