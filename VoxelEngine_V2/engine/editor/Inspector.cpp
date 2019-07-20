// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Inspector.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_colors.h"

#include "Editor.h"
#include "../modules/Entity.h"

#include "../math/Vector.h"

#include "../input/Input.h"

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
		static bool open = true;

		if (Input.getKeyDown(KeyCode::F2))
			open = !open;

		if (!open)
			return;

		if (ImGui::Begin("[F2] Inspector", &open, ImVec2(380, 280), 0.9f))
		{
			if (Editor.GetSelectedEntities().size() == 1)
			{
				auto Entity = Editor.GetSelectedEntities()[0];

				// Name
				static char Name[MAX_ENTITY_NAME_LENGTH];
				strcpy_s(Name, Entity->GetName().c_str());

				ImGui::Text("Name: "); ImGui::SameLine();

				ImGui::PushItemWidth(-1);
				if (ImGui::InputText("input text", Name, IM_ARRAYSIZE(Name)))
					Entity->SetName(std::string(Name));

				// ID
				ImGui::TextColored(ImGuiColor::Orange, "ID: %d", Entity->GetUniqueID());

				// Color
				float EntityColor[3] = { Entity->GetLabelColor().r, Entity->GetLabelColor().g, Entity->GetLabelColor().b };

				if (ImGui::ColorEdit3("Label Color: ", EntityColor, ImGuiColorEditFlags_NoInputs))
				{
					Entity->SetLabelColor(Color3F(EntityColor[0], EntityColor[1], EntityColor[2]));
				}
				ImGui::SameLine();
				if (ImGui::Button("Copy Color"))
				{
					LabelColorClipboard.r = EntityColor[0];
					LabelColorClipboard.g = EntityColor[1];
					LabelColorClipboard.b = EntityColor[2];
				}
				ImGui::SameLine();
				if (ImGui::Button("Paste Color"))
				{
					Entity->SetLabelColor(LabelColorClipboard);
				}

				// Drag Speed
				static float DragSpeed = 0.1f;
				ImGui::Text("DragSpeed: "); ImGui::SameLine(); ImGui::DragFloat("DragSpeed", &DragSpeed, 0.05f, 0.1f, 10.0f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				// Active
				ImGui::Checkbox("Active", &Entity->m_isActive);

				// Transform
				if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
				{
					float p[3] = { Entity->m_transform.getPosition().x, Entity->m_transform.getPosition().y, Entity->m_transform.getPosition().z };
					float r[3] = { Entity->m_transform.getRotationEuler().x, Entity->m_transform.getRotationEuler().y, Entity->m_transform.getRotationEuler().z };
					float s[3] = { Entity->m_transform.getScale().x, Entity->m_transform.getScale().y, Entity->m_transform.getScale().z };

					float pw[3] = { Entity->m_transform.getWorldPosition().x, Entity->m_transform.getWorldPosition().y, Entity->m_transform.getWorldPosition().z };
					float rw[3] = { Entity->m_transform.getForward().x, Entity->m_transform.getForward().y, Entity->m_transform.getForward().z };
					float sw[3] = { Entity->m_transform.getWorldScale().x, Entity->m_transform.getWorldScale().y, Entity->m_transform.getWorldScale().z };

					// Camera has reversed forward
					if (Entity->GetType() == EntityType::CAMERA)
					{
						rw[0] = -rw[0];
						rw[1] = -rw[1];
						rw[2] = -rw[2];
					}
					
					ImGui::PushItemWidth(-1);

					ImGui::TextColored(ImGuiColor::Orange, Entity->m_useTransform ? "Local:" : "Local: [LOCKED]");

					// Lock if transform is not used
					if (!Entity->m_useTransform)
						DisableStart();

					ImGui::Text("Position:"); ImGui::SameLine();
					if (ImGui::DragFloat3("Position", p, DragSpeed))
						Entity->m_transform.setPosition(p[0], p[1], p[2]);

					// NO LOCAL EFFECT
					if (Entity->GetType() == EntityType::LIGHT)
						ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Orange);

					ImGui::Text("Rotation:"); ImGui::SameLine();
					if (ImGui::DragFloat3("Rotation", r, DragSpeed))
						Entity->m_transform.setRotation(r[0], r[1], r[2]);

					// ~~~
					if (Entity->GetType() == EntityType::LIGHT)
						ImGui::PopStyleColor();


					// NO LOCAL EFFECT
					if (Entity->GetType() == EntityType::LIGHT || Entity->GetType() == EntityType::CAMERA)
						ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Orange);

					ImGui::Text("Scale:   "); ImGui::SameLine();
					if (ImGui::DragFloat3("Scale", s, DragSpeed))
						Entity->m_transform.setScale(s[0], s[1], s[2]);

					// ~~~
					if (Entity->GetType() == EntityType::LIGHT || Entity->GetType() == EntityType::CAMERA)
						ImGui::PopStyleColor();

					// Lock if transform is not used
					if (!Entity->m_useTransform)
						DisableEnd();

					ImGui::Separator();

					// World Read Only //
					ImGui::TextColored(ImGuiColor::Orange, "World: [READ ONLY]");

					DisableStart();
					ImGui::Text("Position:"); ImGui::SameLine(); ImGui::DragFloat3("PositionW", pw, DragSpeed);
					ImGui::Text("Forward: "); ImGui::SameLine(); ImGui::DragFloat3("RotationW", rw, DragSpeed);
					ImGui::Text("Scale:   "); ImGui::SameLine(); ImGui::DragFloat3("ScaleW", sw, DragSpeed);
					DisableEnd();

					ImGui::PopItemWidth();
				}
			}
		}
		ImGui::End();

		//ImGui::IsWindowFocused();
	}
}
#endif