// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Hierarchy.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../modules/RenderManager.h"
#include "../objects/GameObject.h"
#include "../modules/Material.h"

#include "../opengl/Geometry.h"

#include "../input/Input.h"

#include <iostream>

namespace Vxl
{
	void Hierarchy::DisplayEntity(Entity* _entity, int _depth)
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
				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Grey);
			else
			{
				Color3F c = _entity->GetLabelColor();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c.r, c.g, c.b,1));
			}

			// Node + Name
			std::string Name = _entity->GetName();
			if (Name.empty())
				Name = "[Unnamed]";

			ImGui::TreeNodeEx(Name.c_str(), flags);

			// selection case
			if (ImGui::IsItemClicked())
				_selectedEntity = _entity;

			// color end
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
				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Grey);
			else
			{
				Color3F c = _entity->GetLabelColor();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c.r, c.g, c.b, 1));
			}

			// Node + Name
			std::string Name = _entity->GetName();
			if (Name.empty())
				Name = "[Unnamed]";

			bool node_open = ImGui::TreeNodeEx(Name.c_str(), flags);

			// selection
			if (ImGui::IsItemClicked())
				_selectedEntity = _entity;

			// color end
			ImGui::PopStyleColor();

			// recursion node
			if (node_open)
			{
				ImGui::Indent();
				auto children = _entity->m_transform.getChildren();
				for (auto it = children.begin(); it != children.end(); it++)
				{
					DisplayEntity((*it)->GetOwner(), _depth + 1);
				}
				ImGui::Unindent();
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
			ImGui::Text("Scene Graph:\t");
			ImGui::SameLine();
			if (ImGui::Button("Delete") || Input.getKeyDown(KeyCode::DELETEKEY))
			{
				// Don't delete camera
				if (_selectedEntity && _selectedEntity->GetType() != EntityType::CAMERA)
				{
					Entity::Delete(_selectedEntity);
					_selectedEntity = nullptr;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Add Sphere"))
			{
				auto object = GameObject::Create("Generic Object");
				object->SetMesh(Geometry.GetSphereUV_Good());
				object->SetMaterial(Material::Get("gbuffer"));
				object->SetColor(Color3F(1, 0, 0));
			}

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