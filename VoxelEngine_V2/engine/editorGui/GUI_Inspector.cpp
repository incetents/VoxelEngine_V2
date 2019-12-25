// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_Inspector.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_colors.h"

#include "../editor/Editor.h"

#include "../input/Input.h"

#include "../math/Vector.h"

#include "../modules/Entity.h"

#include "../utilities/Asset.h"

namespace Vxl
{
	// Imgui Locking Shortcut
	bool Locked = false;
	void DisableStart()
	{
		if (Locked == true)
			return;

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

		Locked = true;
	}
	void DisableEnd()
	{
		if (Locked == false)
			return;

		ImGui::PopItemFlag();
		ImGui::PopStyleVar();

		Locked = false;
	}
	//

	void Inspector::Draw()
	{
		if (Editor.hasSelection())
		{
			SceneNodeIndex index = Editor.m_selectedNodes[0];
			SceneNode* node = Assets.getSceneNode(index);

			// Name
			static char Name[MAX_ENTITY_NAME_LENGTH];
			strcpy_s(Name, node->m_name.c_str());

			ImGui::Text("Name: "); ImGui::SameLine();

			// ~ Naming Scheme
			ImGui::PushItemWidth(-1);

			// New Name
			if (ImGui::InputText("input text", Name, IM_ARRAYSIZE(Name), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				node->m_name = std::string(Name);
			}

			// ID
			ImGui::TextColored(ImGuiColor::Orange, "ID: %d", node->m_uniqueID);

			// Color
			ImGui::TextColored(ImGuiColor::Orange, "Label Color:");
			ImGui::SameLine();

			float EntityColor[3] = { node->m_labelColor.r, node->m_labelColor.g, node->m_labelColor.b };

			if (ImGui::ColorEdit3("Label Color", EntityColor, ImGuiColorEditFlags_NoInputs))
			{
				node->m_labelColor = Color3F(EntityColor[0], EntityColor[1], EntityColor[2]);
			}

			// ~ Naming Scheme
			ImGui::PopItemWidth();

			if (ImGui::Button("Copy Color"))
			{
				LabelColorClipboard.r = EntityColor[0];
				LabelColorClipboard.g = EntityColor[1];
				LabelColorClipboard.b = EntityColor[2];
			}
			ImGui::SameLine();
			if (ImGui::Button("Paste Color"))
			{
				node->m_labelColor = LabelColorClipboard;
			}
			ImGui::PushItemWidth(-1);

			// Drag Speed
			static float DragSpeed = 0.1f;
			ImGui::Text("DragSpeed: "); ImGui::SameLine(); ImGui::DragFloat("DragSpeed", &DragSpeed, 0.05f, 0.1f, 10.0f);
			ImGui::PopItemWidth();

			ImGui::Separator();

			// Active
			ImGui::Checkbox("Active", &node->m_isActive);

			// Transform
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
			{
				float p[3] = { node->m_transform.getPosition().x, node->m_transform.getPosition().y, node->m_transform.getPosition().z };
				float r[3] = { node->m_transform.getRotationEuler().x, node->m_transform.getRotationEuler().y, node->m_transform.getRotationEuler().z };
				float s[3] = { node->m_transform.getScale().x, node->m_transform.getScale().y, node->m_transform.getScale().z };

				float pw[3] = { node->m_transform.getWorldPosition().x, node->m_transform.getWorldPosition().y, node->m_transform.getWorldPosition().z };
				float rw[3] = { node->m_transform.getForward().x, node->m_transform.getForward().y, node->m_transform.getForward().z };
				float sw[3] = { node->m_transform.getWorldScale().x, node->m_transform.getWorldScale().y, node->m_transform.getWorldScale().z };

				//	// Camera has reversed forward
				//	if (entity->GetType() == EntityType::CAMERA)
				//	{
				//		rw[0] = -rw[0];
				//		rw[1] = -rw[1];
				//		rw[2] = -rw[2];
				//	}

				ImGui::PushItemWidth(-1);

				ImGui::TextColored(ImGuiColor::Orange, node->m_useTransform ? "Local:" : "Local: [LOCKED]");

				// Lock if transform is not used
				if (!node->m_useTransform)
					DisableStart();

				ImGui::Text("Position:"); ImGui::SameLine();
				if (ImGui::DragFloat3("Position", p, DragSpeed))
					node->m_transform.setPosition(p[0], p[1], p[2]);

				// NO LOCAL EFFECT
				//if (entity->GetType() == EntityType::LIGHT)
				//	ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Orange);

				ImGui::Text("Rotation:"); ImGui::SameLine();
				if (ImGui::DragFloat3("Rotation", r, DragSpeed))
					node->m_transform.setRotation(r[0], r[1], r[2]);

				// ~~~
				//if (entity->GetType() == EntityType::LIGHT)
				//	ImGui::PopStyleColor();


				// NO LOCAL EFFECT
				//if (entity->GetType() == EntityType::LIGHT || Entity->GetType() == EntityType::CAMERA)
				//	ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Orange);

				ImGui::Text("Scale:   "); ImGui::SameLine();
				if (ImGui::DragFloat3("Scale", s, DragSpeed))
					node->m_transform.setScale(s[0], s[1], s[2]);

				// ~~~
				//if (entity->GetType() == EntityType::LIGHT || Entity->GetType() == EntityType::CAMERA)
				//	ImGui::PopStyleColor();

				// Lock if transform is not used
				if (!node->m_useTransform)
					DisableEnd();

				ImGui::Separator();

				// World Position //
				ImGui::TextColored(ImGuiColor::Orange, "World:");

				ImGui::Text("Position:");
				ImGui::SameLine();
				if (ImGui::DragFloat3("PositionW", pw, DragSpeed))
					node->m_transform.setWorldPosition(Vector3(pw[0], pw[1], pw[2]));
				
				// Forward / Scale [READ ONLY]
				DisableStart();
				ImGui::Text("Forward: "); ImGui::SameLine(); ImGui::DragFloat3("RotationW", rw, DragSpeed);
				ImGui::Text("Scale:   "); ImGui::SameLine(); ImGui::DragFloat3("ScaleW", sw, DragSpeed);
				DisableEnd();

				ImGui::PopItemWidth();
			}
		}
	}
}
#endif