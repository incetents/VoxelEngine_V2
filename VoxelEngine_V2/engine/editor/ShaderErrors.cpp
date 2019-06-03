// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ShaderErrors.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../engine/opengl/Shader.h"

namespace Vxl
{
	void ShaderErrors::Draw()
	{
		if (Shader::ShaderErrorLogSize > 0 || ShaderProgram::ProgramsFailedSize > 0)
		{
			static bool open = true;
			if (ImGui::Begin("ShaderErrors", &open, ImVec2(700.f, 400.f), 0.9f))
			{
				if (Shader::ShaderErrorLogSize > 0)
					ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.f, 1.f), "Shader Compilation Failures:");

				// Shader Compilation Errors
				for (auto Log : Shader::ShaderErrorLog)
				{
					if (ImGui::TreeNode(Log.first.c_str()))
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.f, 1.f));
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

						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.f, 1.f));
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

				if(ShaderProgram::ProgramsFailedSize > 0)
					ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.f, 1.f), "Program Link Failures:");

				// Program Linking Errors
				for (auto Program : ShaderProgram::ProgramsFailed)
				{
					if (ImGui::TreeNode(Program->GetName().c_str()))
					{
						if (Program->GetErrorMessage().empty())
							ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), "Could not retrieve program compilation log");
						else
							ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), Program->GetErrorMessage().c_str());
						
						ImGui::TreePop();
					}
					ImGui::Separator();
				}
			}
			ImGui::End();
		}
	}
}
#endif