// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ShaderErrors.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"
#include "../engine/rendering/Shader.h"

namespace Vxl
{
	auto& shaders = Assets::getAllShader();

	bool ShaderErrors::HasErrors()
	{
		return _Shader::m_brokenShaders.size() > 0 || _ShaderProgram::m_brokenShaderPrograms.size() > 0;
	}

	void ShaderErrors::Draw()
	{
		if (_Shader::m_brokenShaders.size() == 0 && _ShaderProgram::m_brokenShaderPrograms.size() == 0)
		{
			ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.f, 1.f), "No Shader Errors");
		}
		else
		{
			if (_Shader::m_brokenShaders.size() > 0)
			{
				ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.f, 1.f), "Shader Compilation Failures:");
				ImGui::Separator();
			}


			// Shader Compilation Errors
			for (auto brokenShader : _Shader::m_brokenShaders)
			{
				if (ImGui::TreeNode(brokenShader.second->m_name.c_str()))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.f, 1.f));
					if (ImGui::TreeNode("Error"))
					{
						ImGui::PopStyleColor();

						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.f, 1.f));

						ImGui::Separator();
						ImGui::Text(brokenShader.second->m_errorMessage.c_str());
						ImGui::Separator();

						ImGui::TreePop();

						ImGui::PopStyleColor();
					}
					else
						ImGui::PopStyleColor();

					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.f, 1.f));
					if (ImGui::TreeNode("Source"))
					{
						ImGui::PopStyleColor();

						if (brokenShader.second->m_source.empty())
							ImGui::Text("Shader Code Storage Disabled from Macro -> GLOBAL_SHADER_CODE_BACKUP");
						else
						{
							//
							ImGui::Columns(2, "ShaderCode", false);
							ImGui::SetColumnWidth(0, 40);
							
							ImGui::Separator();

							size_t size = brokenShader.second->m_sourceLines.size();
							ImGui::TextColored(ImGuiColor::Orange, brokenShader.second->m_sourceLines.c_str(), brokenShader.second->m_sourceLines.c_str() + size);

							ImGui::NextColumn();

							size = brokenShader.second->m_source.size();
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,1,1));
							ImGui::TextUnformatted(brokenShader.second->m_source.c_str(), brokenShader.second->m_source.c_str() + size);
							ImGui::PopStyleColor();
							ImGui::NextColumn();

							ImGui::Columns(1);
							ImGui::Separator();
							//
						}
						ImGui::TreePop();
					}
					else
						ImGui::PopStyleColor();

					ImGui::TreePop();
				}


				ImGui::Separator();
			}

			if (_ShaderProgram::m_brokenShaderPrograms.size() > 0)
			{
				ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.f, 1.f), "Program Link Failures:");
				ImGui::Separator();
			}

			// Program Linking Errors
			for (auto brokenProgram : _ShaderProgram::m_brokenShaderPrograms)
			{
				if (ImGui::TreeNode(brokenProgram.second->m_name.c_str()))
				{
					if (brokenProgram.second->m_errorMessage.empty())
						ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), "Could not retrieve program compilation log");
					else
						ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), brokenProgram.second->m_errorMessage.c_str());

					ImGui::TreePop();
				}
				ImGui::Separator();
			}
		}
	}
}
#endif