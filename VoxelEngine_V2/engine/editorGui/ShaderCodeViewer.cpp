// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ShaderCodeViewer.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../engine/rendering/Shader.h"

namespace Vxl
{
	void ShaderCodeViewer::Draw()
	{
		auto _ShaderPrograms = Assets::getAllShaderProgram();
		for (const auto& _Program : _ShaderPrograms)
		{
			if (ImGui::CollapsingHeader(_Program.second->m_name.c_str()))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));

				auto& _Shaders = _Program.second->m_shaders;
				for (const auto& _Shader : _Shaders)
				{
					if (ImGui::TreeNode(_Shader->m_name.c_str()))
					{
						if (_Shader->m_source.empty())
							ImGui::TextColored(ImVec4(1, 0.4f, 0.3f, 1), "Shader Code Not Stored");
						else
							ImGui::TextColored(ImVec4(1,1,1,1), _Shader->m_source.c_str());
						ImGui::TreePop();
					}
				}

				ImGui::PopStyleColor();
			}
		}
	}
}

#endif