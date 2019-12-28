// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_Hierarchy.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../rendering/RenderManager.h"
#include "../modules/Material.h"

#include "../objects/GameObject.h"
#include "../objects/Camera.h"
#include "../objects/LightObject.h"

#include "../rendering/Primitives.h"

#include "../input/Input.h"

#include "../utilities/Util.h"
#include "../utilities/Asset.h"

#include "../editor/Editor.h"

#include <iostream>

namespace Vxl
{
	void Hierarchy::DisplayNode(SceneNode* node, int _depth)
	{
		//SceneNode* node = Assets.getSceneNode(_node);
		//VXL_ASSERT(node, "Missing SceneNode");

		// Display entity
		//	-> Collapsing if it has children
		//	-> Text if it has no children
		auto ChildCount = node->m_transform.getChildCount();
		if (ChildCount == 0)
		{
			// flags
			int flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog;
			if (node->IsSelected())
				flags |= ImGuiTreeNodeFlags_Selected;

			// color start
			if (!node->IsFamilyActive())
				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Grey);
			else
			{
				Color3F c = node->m_labelColor;
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c.r, c.g, c.b, 1));
			}

			// Node + Name
			std::string Name = node->m_name;
			if (Name.empty())
				Name = "[Unnamed]";

			ImGui::TreeNodeEx(Name.c_str(), flags);

			// selection case
			if (ImGui::IsItemClicked())
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					if (!node->IsSelected())
						Editor.addSelection(node->m_uniqueID);
					else
						Editor.removeSelection(node->m_uniqueID);
				}
				else
				{
					Editor.clearSelection();
					Editor.addSelection(node->m_uniqueID);
				}
			}

			// color end
			ImGui::PopStyleColor();
		}
		else
		{
			// flags
			int flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (node->IsSelected())
				flags |= ImGuiTreeNodeFlags_Selected;

			// color start
			if (!node->IsFamilyActive())
				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Grey);
			else
			{
				Color3F c = node->m_labelColor;
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c.r, c.g, c.b, 1));
			}

			// Node + Name
			std::string Name = node->m_name;
			if (Name.empty())
				Name = "[Unnamed]";

			bool node_open = ImGui::TreeNodeEx(Name.c_str(), flags);

			// selection
			if (ImGui::IsItemClicked())
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					if (!node->IsSelected())
						Editor.addSelection(node->m_uniqueID);
					else
						Editor.removeSelection(node->m_uniqueID);
				}
				else
				{
					Editor.clearSelection();
					Editor.addSelection(node->m_uniqueID);
				}
			}

			// color end
			ImGui::PopStyleColor();

			// recursion node
			if (node_open)
			{
				ImGui::Indent();
				auto children = node->m_transform.getChildren();
				for (auto it = children.begin(); it != children.end(); it++)
				{
					SceneNode* childNode = ((*it)->m_sceneNode);
					if(childNode)
						DisplayNode(childNode, _depth + 1);
				}
				ImGui::Unindent();
			}

		}
	}

	void Hierarchy::Draw()
	{
		ImGui::Text("Scene Graph:\t");

		// Right Click Menu for selection
		if (Editor.m_selectedNodes.size() > 0)
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::Selectable("Delete"))
				{
					Editor.deleteSelection();
				}
				ImGui::EndPopup();
			}
		}

		//	ImGui::SameLine();
		//	if (ImGui::Button("Add Sphere"))
		//	{
		//		auto object = GameObject::Create("Generic Object");
		//		object->SetMesh(Primitives.GetSphereUV_Good());
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
		auto& nodes = SceneAssets.getAllSceneNode();
		for (auto& node : nodes)
		{
			if (node.second->m_transform.getParent() == nullptr)
				DisplayNode(node.second, 0);
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