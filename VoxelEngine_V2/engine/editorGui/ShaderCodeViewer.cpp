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
		auto _ShaderPrograms = ShaderProgram::GetAllNamedAssets();
		for (const auto& _Program : _ShaderPrograms)
		{
			if (ImGui::CollapsingHeader(_Program.first.c_str()))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));

				auto _Shaders = _Program.second->GetShaders();
				for (const auto& _Shader : _Shaders)
				{
					if (ImGui::TreeNode(_Shader->GetName().c_str()))
					{
						if (_Shader->GetCompiledCode().empty())
							ImGui::TextColored(ImVec4(1, 0.4f, 0.3f, 1), "Shader Code Not Stored");
						else
							ImGui::TextColored(ImVec4(1,1,1,1), _Shader->GetCompiledCode().c_str());
						ImGui::TreePop();
					}
				}

				ImGui::PopStyleColor();
			}
		}
	}
}

#endif