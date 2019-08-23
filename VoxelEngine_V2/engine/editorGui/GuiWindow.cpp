// Copyright (c) 2019 Emmanuel Lajeunesse
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

		bool begin = ImGui::Begin(m_name.c_str(), &m_open, ImVec2(m_size.x, m_size.y), m_alpha, m_flags);

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