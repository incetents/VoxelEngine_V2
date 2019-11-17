// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_Performance.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../input/Input.h"

#include "../utilities/Time.h"

namespace Vxl
{
	void Performance::Draw()
	{
		if (m_mode == Mode::GPU)
			ImGui::TextColored(ImGuiColor::Orange, "[GPU]");
		else if (m_mode == Mode::CPU)
			ImGui::TextColored(ImGuiColor::Orange, "[CPU]");

		ImGui::SameLine();

		if (ImGui::SmallButton("GPU"))
			m_mode = Mode::GPU;
		ImGui::SameLine();
		if (ImGui::SmallButton("CPU"))
			m_mode = Mode::CPU;

		if (m_mode == Mode::GPU)
		{
#ifdef GLOBAL_GPU_TIMERS
			ImGui::Columns(2, "GPUTimers"); // 4-ways, with border
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Miliseconds"); ImGui::NextColumn();
			ImGui::Separator();

			double MS_TOTAL = 0.0f;

			for (auto _gputimer : GPUTimer::m_timers)
			{
				MS_TOTAL += _gputimer.second->m_elapsedTime_MS_average;

				if (ImGui::Selectable(_gputimer.first.c_str(), m_GPUTimerselected == _gputimer.second, ImGuiSelectableFlags_SpanAllColumns))
					m_GPUTimerselected = _gputimer.second;
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
#else
			ImGui::TextColored(ImGuiColor::Orange, "[Disabled]");
#endif
		}
		else if (m_mode == Mode::CPU)
		{
			ImGui::Columns(2, "CPUTimers"); // 4-ways, with border
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Miliseconds"); ImGui::NextColumn();
			ImGui::Separator();

			double MS_TOTAL = 0.0f;

			for (auto _gputimer : CPUTimer::m_timers)
			{
				MS_TOTAL += _gputimer.second->m_elapsedTime_MS_average;

				if (ImGui::Selectable(_gputimer.first.c_str(), m_CPUTimerselected == _gputimer.second, ImGuiSelectableFlags_SpanAllColumns))
					m_CPUTimerselected = _gputimer.second;
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
	}
}
#endif