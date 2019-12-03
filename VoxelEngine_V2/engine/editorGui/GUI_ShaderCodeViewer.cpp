// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_ShaderCodeViewer.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"
#include "../engine/rendering/Shader.h"
#include "../utilities/Asset.h"

namespace Vxl
{
	void ShaderCodeViewer::Draw()
	{
		auto _ShaderPrograms = Assets.getAllShaderProgram();
		for (const auto& _Program : _ShaderPrograms)
		{
			if (ImGui::CollapsingHeader(_Program.second->m_name.c_str()))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Orange);

				auto& _Shaders = _Program.second->m_shaders;
				for (const auto& _Shader : _Shaders)
				{
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

							size_t size = _Shader->m_sourceLines.size();
							ImGui::TextColored(ImGuiColor::Orange, _Shader->m_sourceLines.c_str(), _Shader->m_sourceLines.c_str() + size);

							ImGui::NextColumn();

							size = _Shader->m_source.size();
							ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::White);
							ImGui::TextUnformatted(_Shader->m_source.c_str(), _Shader->m_source.c_str() + size);
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