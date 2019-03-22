// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Performance.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../input/Input.h"

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
			ImGui::End();
		}
	}
}