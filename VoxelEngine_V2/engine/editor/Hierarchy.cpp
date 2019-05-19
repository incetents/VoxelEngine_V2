// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Hierarchy.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../modules/RenderManager.h"
#include "../modules/Material.h"

#include "../objects/GameObject.h"

#include "../opengl/Geometry.h"

#include "../input/Input.h"

#include "../utilities/Util.h"

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
			if (_entity->m_isSelected)
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

			ImGui::TreeNodeEx(Name.c_str(), flags);

			// selection case
			if (ImGui::IsItemClicked())
			{
				if (Input.getKey(KeyCode::LEFT_CONTROL))
				{
					if (!_entity->m_isSelected)
						AddSelection(_entity);
					else
						RemoveSelection(_entity);
				}
				else
				{
					ClearSelection();
					AddSelection(_entity);
				}
			}

			// color end
			ImGui::PopStyleColor();
		}
		else
		{
			// flags
			int flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (_entity->m_isSelected)
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
			{
				if (Input.getKey(KeyCode::LEFT_CONTROL))
				{
					if (!_entity->m_isSelected)
						AddSelection(_entity);
					else
						RemoveSelection(_entity);
				}
				else
				{
					ClearSelection();
					AddSelection(_entity);
				}
			}

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

			if (ImGui::Button("Delete") || Input.getKeyDown(KeyCode::DELETEKEY))
			{
				// Don't delete camera
				for (auto Entity : m_selectedEntities)
				{
					if (Entity->GetType() != EntityType::CAMERA)
					{
						Entity::Delete(Entity);
					}
				}
				ClearSelection();
			}
			ImGui::SameLine();
			if (ImGui::Button("Add Sphere"))
			{
				auto object = GameObject::Create("Generic Object");
				object->SetMesh(Geometry.GetSphereUV_Good());
				object->SetMaterial(Material::Get("gbuffer"));
				object->SetColor(Color3F(1, 0, 0));
			}
			ImGui::SameLine();
			if (ImGui::Button("Duplicate Selected GameObject(s)"))
			{
				for (auto Entity : m_selectedEntities)
				{
					if (Entity->GetType() == EntityType::GAMEOBJECT)
					{
						auto GEntity = dynamic_cast<GameObject*>(Entity);

						auto object = GameObject::Create(GEntity->GetName());
						object->SetMesh(GEntity->GetMesh());
						object->SetMaterial(GEntity->GetMaterial());
						object->SetColor(GEntity->GetColor());
						object->m_useTransform = GEntity->m_useTransform;
						object->m_isActive = GEntity->m_isActive;
						object->m_isColoredObject = GEntity->m_isColoredObject;
						object->SetSelectable(GEntity->IsSelectable());
						object->m_transform.setWorldPosition(GEntity->m_transform.getWorldPosition());
						object->m_transform.setRotation(GEntity->m_transform.getRotationEuler());
						object->m_transform.setScale(GEntity->m_transform.getScale());
						object->m_transform.setParent(GEntity->m_transform.getParent());
					}
				}
			}

			// Display Hierarchy with selected entities
			auto AllEntities = RenderManager.m_allEntities;
			int EntityCount = (int)AllEntities.size();
			for (int i = 0; i < EntityCount; i++)
			{
				auto Entity = AllEntities[i];

				if (Entity->m_transform.getParent() == nullptr)
					DisplayEntity(Entity, 0);
			}
			//

			// Move Selection
			if (ImGui::IsWindowFocused())
			{
				if (Input.getKeyDown(KeyCode::DOWN))
				{
					if (m_selectedEntities.size() == 1)
					{
						for (int i = 0; i < EntityCount; i++)
						{
							if (AllEntities[i] == m_selectedEntities[0])
							{
								ClearSelection();
								auto Entity = AllEntities[(i + 1) % EntityCount];

								// Check for children
								if (Entity->m_transform.getChildCount() > 0)
								{

								}
								else
								{

								}


								// Get Next Entity

								AddSelection(Entity);
								break;
							}
						}
					}
				}
				else if (Input.getKeyDown(KeyCode::UP))
				{
					if (m_selectedEntities.size() == 1)
					{
						for (int i = 0; i < EntityCount; i++)
						{
							if (AllEntities[i] == m_selectedEntities[0])
							{
								ClearSelection();
								AddSelection(AllEntities[(i - 1) < 0 ? EntityCount - 1 : i - 1]);
								break;
							}
						}
					}
				}
			}
		}
		ImGui::End();
	}

	void Hierarchy::RemoveSelection(Entity* _entity)
	{
		Util::RemoveFromVector(m_selectedEntities, _entity);
		_entity->m_isSelected = false;

		UpdateAverageSelectionLocation();
	}
	void Hierarchy::AddSelection(Entity* _entity)
	{
		for (auto Entity : m_selectedEntities)
			if (Entity == _entity)
				return;

		m_selectedEntities.push_back(_entity);
		_entity->m_isSelected = true;

		UpdateAverageSelectionLocation();
	}
	void Hierarchy::ClearSelection()
	{
		for (auto Entity : m_selectedEntities)
			Entity->m_isSelected = false;
		m_selectedEntities.clear();

		m_averageSelectionLocation = Vector3::ZERO;
	}
}

