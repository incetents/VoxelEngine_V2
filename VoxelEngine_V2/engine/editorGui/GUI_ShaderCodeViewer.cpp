// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_ShaderCodeViewer.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"
#include "../engine/rendering/Shader.h"
#include "../utilities/Asset.h"

namespace Vxl
{
	ShaderCodeViewer::ShaderCodeViewer()
	{
		for (uint32_t i = 1; i < SHADER_LINE_MAX; i++)
		{
			m_linesCounted += std::to_string(i) + '\n';
		}
	}

	void ShaderCodeViewer::Draw()
	{
		auto _ShaderPrograms = Assets.getAllShaderProgram();
		for (const auto& _Program : _ShaderPrograms)
		{
			if (ImGui::CollapsingHeader(_Program.second->m_name.c_str()))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Orange);

				auto& _Shaders = _Program.second->m_shaders;
				for (uint32_t _ShaderIndex : _Shaders)
				{
					Shader* _Shader = Assets.getShader(_ShaderIndex);
					if (!_Shader)
						continue;

					if (ImGui::TreeNode(_Shader->m_name.c_str()))
					{
						if (_Shader->m_source.empty())
							ImGui::TextColored(ImVec4(1, 0.4f, 0.3f, 1), "Shader Code Not Stored");
						else
						{
							//
							ImGui::Columns(2, "ShaderCode", false);
							ImGui::SetColumnWidth(0, 40);

							ImGui::Separator();

							// Lines
							ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::ImGuiColor::Orange);
							// find correct end line in a giant list of numbers (1\n2\n3\n4\n5\n...etc..)
							uint32_t adjustedLineCount = _Shader->m_sourceLineCount * 2;
							if (_Shader->m_sourceLineCount  > 9)
								adjustedLineCount += _Shader->m_sourceLineCount - 9;
							if (_Shader->m_sourceLineCount  > 99)
								adjustedLineCount += _Shader->m_sourceLineCount - 99;
							if (_Shader->m_sourceLineCount  > 999)
								adjustedLineCount += _Shader->m_sourceLineCount - 999;
							if (_Shader->m_sourceLineCount  > 9999)
								adjustedLineCount += _Shader->m_sourceLineCount - 9999;
							if (_Shader->m_sourceLineCount  > 99999)
								adjustedLineCount += _Shader->m_sourceLineCount - 99999;

							ImGui::TextUnformatted(m_linesCounted.c_str(), m_linesCounted.c_str() + adjustedLineCount);

							ImGui::PopStyleColor();

							ImGui::NextColumn();

							// Source Code
							ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::White);
							ImGui::TextUnformatted(_Shader->m_source.c_str(), _Shader->m_source.c_str() + _Shader->m_source.size());
							ImGui::PopStyleColor();
							
							ImGui::NextColumn();

							ImGui::Columns(1);
							ImGui::Separator();
							//
						}
						ImGui::TreePop();
					}
				}

				ImGui::PopStyleColor();
			}
		}
	}
}

#endif