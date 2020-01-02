// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GuiWindow.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

namespace Vxl
{
	bool GuiWindow::Begin()
	{
		if(!m_padding)
			 ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::SetNextWindowSize(ImVec2(m_size.x, m_size.y), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(m_alpha);
		bool begin = ImGui::Begin(m_name.c_str(), &m_open, m_flags);

		ImGui::PopStyleVar(2);

		if(!m_padding)
			ImGui::PopStyleVar();

		return begin;
	}
	void GuiWindow::End()
	{
		ImGui::End();
	}
}

#endif