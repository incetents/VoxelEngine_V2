// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Performance.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../input/Input.h"

#include "../utilities/Time.h"

namespace Vxl
{
	void Performance::Draw()
	{
		static bool open = true;

		if (Input.getKeyDown(KeyCode::F4))
			open = !open;

		if (!open)
			return;

		if (ImGui::Begin("[F4] Performance", &open, ImVec2(280, 680), 0.9f, 0))
		{
			ImGui::Columns(2, "GPUTimers"); // 4-ways, with border
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Miliseconds"); ImGui::NextColumn();
			ImGui::Separator();

			double MS_TOTAL = 0.0f;

			for (auto _gputimer : GPUTimer::m_timers)
			{
				MS_TOTAL += _gputimer.second->m_elapsedTime_MS_average;

				if (ImGui::Selectable(_gputimer.first.c_str(), _selected == _gputimer.second, ImGuiSelectableFlags_SpanAllColumns))
					_selected = _gputimer.second;
				//bool hovered = ImGui::IsItemHovered();

				ImGui::NextColumn();
				ImGui::Text("%.4f", _gputimer.second->m_elapsedTime_MS_average);
				ImGui::NextColumn();
			}

			ImGui::Separator();

			ImGui::TextColored(ImGuiColor::Yellow, "TOTAL"); ImGui::NextColumn();
			ImGui::TextColored(ImGuiColor::Yellow, "%.4f", MS_TOTAL); ImGui::NextColumn();

			ImGui::Columns(1);
			ImGui::Separator();
		}
		ImGui::End();
	}
}