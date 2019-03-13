// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Inspector.h"

#include "../imgui/imgui.h"

#include "Hierarchy.h"
#include "../modules/Entity.h"

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

#include "../input/Input.h"

namespace Vxl
{
	void Inspector::Draw()
	{
		static bool open = true;

		if (Input.getKeyDown(KeyCode::F2))
			open = !open;

		if (!open)
			return;

		if (ImGui::Begin("[F2] Inspector", &open, ImVec2(380, 280), 0.9f))
		{
			auto Entity = Hierarchy._selectedEntity;
			if (Entity != nullptr)
			{
				static float DragSpeed = 0.1f;

				ImGui::Text(("Name: " + Entity->m_name).c_str());

				ImGui::PushItemWidth(-1);
				ImGui::Text("DragSpeed: "); ImGui::SameLine(); ImGui::DragFloat("DragSpeed", &DragSpeed, 0.05f, 0.1f, 10.0f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox("Active", &Entity->m_isActive);

				// Transform
				if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
				{
					float p[3] = { Entity->m_transform.getPosition().x, Entity->m_transform.getPosition().y, Entity->m_transform.getPosition().z };
					float r[3] = { Entity->m_transform.getRotationEuler().x, Entity->m_transform.getRotationEuler().y, Entity->m_transform.getRotationEuler().z };
					float s[3] = { Entity->m_transform.getScale().x, Entity->m_transform.getScale().y, Entity->m_transform.getScale().z };

					ImGui::PushItemWidth(-1);
					ImGui::Text("Position:"); ImGui::SameLine(); ImGui::DragFloat3("Position", p, DragSpeed);
					ImGui::Text("Rotation:"); ImGui::SameLine(); ImGui::DragFloat3("Rotation", r, DragSpeed);
					ImGui::Text("Scale:   "); ImGui::SameLine(); ImGui::DragFloat3("Scale", s, DragSpeed);
					ImGui::PopItemWidth();

					Entity->m_transform.setPosition(p[0], p[1], p[2]);
					Entity->m_transform.setRotation(r[0], r[1], r[2]);
					Entity->m_transform.setScale(s[0], s[1], s[2]);
				}
			}
		}
		ImGui::End();

		//ImGui::IsWindowFocused();
	}
}