// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ShaderErrors.h"

#include "../imgui/imgui.h"
#include "../engine/opengl/Shader.h"

namespace Vxl
{
	void ShaderErrors::Draw()
	{
		if (Shader::ShaderErrorLogSize > 0)
		{
			static bool open = true;
			if (ImGui::Begin("ShaderErrors", &open, ImVec2(700, 400), 0.9f))
			{
				for (auto Log : Shader::ShaderErrorLog)
				{
					if (ImGui::TreeNode(Log.first.c_str()))
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.5f, 0, 1));
						if (ImGui::TreeNode("Error"))
						{
							ImGui::PopStyleColor();

							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.f, 1.f));

							ImGui::Text(Log.second->GetErrorMessage().c_str());
							ImGui::TreePop();

							ImGui::PopStyleColor();
						}
						else
							ImGui::PopStyleColor();

						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.5f, 0, 1));
						if (ImGui::TreeNode("Source"))
						{
							ImGui::PopStyleColor();

							ImGui::Text(Log.second->GetCompiledCode().c_str());
							ImGui::TreePop();
						}
						else
							ImGui::PopStyleColor();

						ImGui::TreePop();
					}


					ImGui::Separator();
				}
			}
			ImGui::End();
		}
	}
}