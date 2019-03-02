// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Imgui_ShaderErrors.h"

#include "../imgui/imgui.h"
#include "../engine/opengl/Shader.h"

namespace Vxl
{
	void Imgui_ShaderErrors::Draw()
	{
		if (Shader::ShaderErrorLogSize > 0)
		{
			static bool open = true;
			if (ImGui::Begin("ShaderErrors", &open, ImVec2(700, 400), 0.9f))
			{
				for (auto Log : Shader::ShaderErrorLog)
				{
					if (ImGui::CollapsingHeader(Log.first.c_str()))
						ImGui::Text(Log.second.c_str());

					ImGui::Separator();
				}
			}
			ImGui::End();
		}
	}
}