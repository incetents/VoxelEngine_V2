// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Hierarchy.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../rendering/RenderManager.h"
#include "../modules/Material.h"

#include "../objects/GameObject.h"
#include "../objects/Camera.h"
#include "../objects/LightObject.h"

#include "../rendering/Geometry.h"

#include "../input/Input.h"

#include "../utilities/Util.h"
#include "../utilities/Asset.h"

#include "../editor/Editor.h"

#include <iostream>

namespace Vxl
{
	void Hierarchy::DisplayEntity(EntityIndex _entity, int _depth)
	{
		Entity* entity = Assets::getEntity(_entity);
		VXL_ASSERT(entity, "Missing Entity");

		// Display entity
		//	-> Collapsing if it has children
		//	-> Text if it has no children
		auto ChildCount = entity->m_transform.getChildCount();
		if (ChildCount == 0)
		{
			// flags
			int flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog;
			if (entity->IsSelected())
				flags |= ImGuiTreeNodeFlags_Selected;

			// color start
			if (!entity->IsFamilyActive())
				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Grey);
			else
			{
				Color3F c = entity->m_labelColor;
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c.r, c.g, c.b, 1));
			}

			// Node + Name
			std::string Name = entity->m_name;
			if (Name.empty())
				Name = "[Unnamed]";

			ImGui::TreeNodeEx(Name.c_str(), flags);

			// selection case
			if (ImGui::IsItemClicked())
			{
				if (Input.getKey(KeyCode::LEFT_CONTROL))
				{
					if (!entity->IsSelected())
						Editor.AddSelection(_entity);
					else
						Editor.RemoveSelection(_entity);
				}
				else
				{
					Editor.ClearSelection();
					Editor.AddSelection(_entity);
				}
			}

			// color end
			ImGui::PopStyleColor();
		}
		else
		{
			// flags
			int flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (entity->IsSelected())
				flags |= ImGuiTreeNodeFlags_Selected;

			// color start
			if (!entity->IsFamilyActive())
				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Grey);
			else
			{
				Color3F c = entity->m_labelColor;
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c.r, c.g, c.b, 1));
			}

			// Node + Name
			std::string Name = entity->m_name;
			if (Name.empty())
				Name = "[Unnamed]";

			bool node_open = ImGui::TreeNodeEx(Name.c_str(), flags);

			// selection
			if (ImGui::IsItemClicked())
			{
				if (Input.getKey(KeyCode::LEFT_CONTROL))
				{
					if (!entity->IsSelected())
						Editor.AddSelection(_entity);
					else
						Editor.RemoveSelection(_entity);
				}
				else
				{
					Editor.ClearSelection();
					Editor.AddSelection(_entity);
				}
			}

			// color end
			ImGui::PopStyleColor();

			// recursion node
			if (node_open)
			{
				ImGui::Indent();
				auto children = entity->m_transform.getChildren();
				for (auto it = children.begin(); it != children.end(); it++)
				{
					DisplayEntity((*it)->m_owner, _depth + 1);
				}
				ImGui::Unindent();
			}

		}
	}

	void Hierarchy::Draw()
	{
		auto Entities = Editor.m_selectedEntities;

		ImGui::Text("Scene Graph:\t");

		if (ImGui::Button("Delete") || Input.getKeyDown(KeyCode::DELETEKEY))
		{
			for (auto Entity : Entities)
			{
				Assets::deleteEntity(Entity);
			}
			Editor.ClearSelection();
		}
		//	ImGui::SameLine();
		//	if (ImGui::Button("Add Sphere"))
		//	{
		//		auto object = GameObject::Create("Generic Object");
		//		object->SetMesh(Geometry.GetSphereUV_Good());
		//		object->SetMaterial(Material::GetAsset("gbuffer"));
		//		object->SetColor(Color3F(1, 0, 0));
		//	}
		//	ImGui::SameLine();
		//	if (ImGui::Button("Duplicate GameObject(s)"))
		//	{
		//		for (auto Entity : Entities)
		//		{
		//			if (Entity->GetType() == EntityType::GAMEOBJECT)
		//			{
		//				auto GEntity = dynamic_cast<GameObject*>(Entity);
		//	
		//				auto object = GameObject::Create(GEntity->GetName());
		//				object->SetMesh(GEntity->GetMesh());
		//				object->SetMaterial(GEntity->GetMaterial());
		//				object->SetColor(GEntity->GetColor());
		//				object->m_useTransform = GEntity->m_useTransform;
		//				object->m_isActive = GEntity->m_isActive;
		//				object->m_isColoredObject = GEntity->m_isColoredObject;
		//				object->SetSelectable(GEntity->IsSelectable());
		//				object->m_transform.setWorldPosition(GEntity->m_transform.getWorldPosition());
		//				object->m_transform.setRotation(GEntity->m_transform.getRotationEuler());
		//				object->m_transform.setScale(GEntity->m_transform.getScale());
		//				object->m_transform.setParent(GEntity->m_transform.getParent());
		//			}
		//		}
		//	}

		// Display Hierarchy with selected entities
		auto& entities = SceneAssets.getAllEntity();
		for (auto& entity : entities)
		{
			if (entity.second->m_transform.getParent() == nullptr)
				DisplayEntity(entity.first, 0);
		}
		//

		//	// Move Selection
		//	if (ImGui::IsWindowFocused())
		//	{
		//		if (Input.getKeyDown(KeyCode::DOWN))
		//		{
		//			if (Entities.size() == 1)
		//			{
		//				for (int i = 0; i < EntityCount; i++)
		//				{
		//					if (AllEntities[i] == Entities[0])
		//					{
		//						Editor.ClearSelection();
		//						auto Entity = AllEntities[(i + 1) % EntityCount];
		//	
		//						// Check for children
		//						if (Entity->m_transform.getChildCount() > 0)
		//						{
		//	
		//						}
		//						else
		//						{
		//	
		//						}
		//	
		//	
		//						// Get Next Entity
		//	
		//						Editor.AddSelection(Entity);
		//						break;
		//					}
		//				}
		//			}
		//		}
		//		else if (Input.getKeyDown(KeyCode::UP))
		//		{
		//			if (Entities.size() == 1)
		//			{
		//				for (int i = 0; i < EntityCount; i++)
		//				{
		//					if (AllEntities[i] == Entities[0])
		//					{
		//						Editor.ClearSelection();
		//						Editor.AddSelection(AllEntities[(i - 1) < 0 ? EntityCount - 1 : i - 1]);
		//						break;
		//					}
		//				}
		//			}
		//		}
		//	}
	}
}
#endif