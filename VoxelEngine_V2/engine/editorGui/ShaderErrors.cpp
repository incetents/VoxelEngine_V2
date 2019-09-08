// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ShaderErrors.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../engine/rendering/Shader.h"

namespace Vxl
{
	bool ShaderErrors::HasErrors()
	{
		return (Shader::ShaderErrorLogSize > 0 || ShaderProgram::ProgramsFailedSize > 0);
	}

	void ShaderErrors::Draw()
	{
		if (Shader::ShaderErrorLogSize == 0 && ShaderProgram::ProgramsFailedSize == 0)
		{
			ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.f, 1.f), "No Shader Errors");
		}
		else
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

						if (Log.second->GetCompiledCode().empty())
							ImGui::Text("Shader Code Storage Disabled from Macro -> GLOBAL_SHADER_CODE_BACKUP");
						else
							ImGui::Text(Log.second->GetCompiledCode().c_str());
						ImGui::TreePop();
					}
					else
						ImGui::PopStyleColor();

					ImGui::TreePop();
				}


				ImGui::Separator();
			}

			if (ShaderProgram::ProgramsFailedSize > 0)
				ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.f, 1.f), "Program Link Failures:");

			// Program Linking Errors
			for (auto Program : ShaderProgram::ProgramsFailed)
			{
				if (ImGui::TreeNode(Program->GetName().c_str()))
				{
					auto errorMessage = Program->GetErrorMessage();

					if (errorMessage.empty())
						ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), "Could not retrieve program compilation log");
					else
						ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), errorMessage.c_str());

					ImGui::TreePop();
				}
				ImGui::Separator();
			}
		}
	}
}
#endif