// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "DevConsole.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../opengl/glUtil.h"

#include "../utilities/Time.h"
#include "../utilities/Util.h"

#include "../window/window.h"

#include "../modules/RenderManager.h"
#include "../modules/Entity.h"

#include "../input/Input.h"
#include "../input/XGamePad.h"

#include "../math/Camera.h"
#include "../math/Transform.h"

#include "../game/Scene_Game.h"

#include "windows.h"

namespace Vxl
{

	void DevConsole::Draw_Master(Scene* scene)
	{
		Scene_Game* Game = dynamic_cast<Scene_Game*>(scene);

		ImGui::Text("FPS: %f", Time.GetFPS());
		ImGui::Text("Time: %f", Time.GetTime());

		float* _fpsGraph = Time.GetFPSHistogram();
		UINT _fpsGraphSize = Time.GetFPSHistogramSize();
		ImGui::PlotHistogram("FPS Histogram", _fpsGraph, _fpsGraphSize, 0, NULL, 0.0f, 100.0f, ImVec2(0, 70));

		ImGui::Separator();

		std::string m1 = "GPU TOTAL: " + std::to_string(glUtil::GetGPUMem_TotalKB() / 1000) + " [MB]";
		ImGui::Text(m1.c_str());
		
		std::string m2 = "GPU Current: " + std::to_string(glUtil::GetGPUMem_CurrentKB() / 1000) + " [MB]";
		ImGui::Text(m2.c_str());

		ImGui::Separator();


		if (ImGui::Button("Reload Window"))
		{
			RenderManager.ReloadWindow();
		}
		if (ImGui::Button("Reload Shaders"))
		{
			RenderManager.ReloadShaders();
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Window"))
		{
			ImGui::Text("Window Size: %d %d", Window.GetWindowWidth(), Window.GetWindowHeight());
			ImGui::Text("Window Viewport: %d %d", Window.GetScreenWidth(), Window.GetScreenHeight());
			ImGui::Text("Window Resolution: %d %d", Window.GetResolutionWidth(), Window.GetResolutionHeight());
			ImGui::Text("Window Aspect: %f", Window.GetAspectRatio());
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Camera"))
		{
			if (Game->_camera)
			{
				ImGui::Text("CamPos: %f %f %f", Game->_camera->getPosition().x, Game->_camera->getPosition().y, Game->_camera->getPosition().z);
				ImGui::Text("CamForward: %f %f %f", Game->_camera->getForward().x, Game->_camera->getForward().y, Game->_camera->getForward().z);

				float cameraFOV = Game->_camera->getFOV();
				float cameraZNear = Game->_camera->getZnear();
				float cameraZFar = Game->_camera->getZfar();

				if (ImGui::SliderFloat("FOV", &cameraFOV, 60.0f, 180.0f))
					Game->_camera->updatePerspective(cameraFOV, Window.GetAspectRatio());

				if (ImGui::SliderFloat("ZNEAR", &cameraZNear, 0.01f, 5.0f))
					Game->_camera->setZnear(cameraZNear);

				if (ImGui::SliderFloat("ZFAR", &cameraZFar, 1.0f, 50.0f))
					Game->_camera->setZfar(cameraZFar);

				ImGui::Separator();
			}
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Resolutions"))
		{
			ImGui::BeginChild("Resolutions", ImVec2(0, 150), true);
			{
				for (unsigned int i = 0; i < Util::ResolutionInfo::VariationsCount; i++)
				{
					std::string Name = "Set Size: " + Util::ResolutionInfo::Variations[i].StrFormat;
					if (ImGui::Button(Name.c_str()))
					{
						Window.SetSize(
							Util::ResolutionInfo::Variations[i].Width,
							Util::ResolutionInfo::Variations[i].Height
						);
					}
				}
			}
			ImGui::EndChild();
		}

		if (ImGui::CollapsingHeader("Aspect Ratios"))
		{
			ImGui::BeginChild("Aspect Ratios", ImVec2(0, 150), true);
			{
				if (ImGui::Button("Free Aspect Ratio"))
				{
					Window.SetCustomAspectRatio(false);
				}

				for (unsigned int i = 0; i < Util::AspectRatioInfo::VariationsCount; i++)
				{
					if (ImGui::Button(Util::AspectRatioInfo::Variations[i].StrFormat.c_str()))
					{
						Window.SetCustomAspectRatio(true, Util::AspectRatioInfo::Variations[i].AspectRatio);
					}
				}
			}
			ImGui::EndChild();
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("FBO Override"))
		{
			const char* fbo_override_choices[] = {
				"[Default]",
				"Albedo",
				"Normal",
				"Depth"
			};
			static int fbo_override_current = 0;
			ImGui::ListBox("Select Output", &fbo_override_current, fbo_override_choices, IM_ARRAYSIZE(fbo_override_choices), 6);

			Game->FBO_OVERRIDE = fbo_override_current;
		}

		ImGui::Separator();

		//ImGui::Text("Dear ImGui, %s", ImGui::GetVersion());

		if (GamePad1.IsConnected())
			ImGui::Text("CONNECTED");
		else
			ImGui::Text("NOT connected");

		//ImGui::Separator();
		if (GamePad1.GetButton(XGamePad::Buttons::A))
			ImGui::Text("[A] YES");
		else
			ImGui::Text("[A] NO");

		ImGui::Text("Left Analog Mag: %f", GamePad1.GetLeftAnalogMagnitude());
		ImGui::Text("Left Analog X: %f", GamePad1.GetLeftAnalogNormalized().x);
		ImGui::Text("Left Analog Y: %f", GamePad1.GetLeftAnalogNormalized().y);
	}
	void DevConsole::Draw_CustomValues()
	{
		ImGui::PushItemWidth(-1);

		// Booleans
		ImGui::Text("Booleans:");
		if (m_bools.size() > 0)
		{
			for (auto it = m_bools.begin(); it != m_bools.end(); it++)
			{
				ImGui::Checkbox(it->first.c_str(), &it->second);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Integers
		ImGui::Text("Integers:");
		if (m_integers.size() > 0)
		{
			for (auto it = m_integers.begin(); it != m_integers.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				ImGui::InputInt(it->first.c_str(), &it->second);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Floats
		ImGui::Text("Floats:");
		if (m_floats.size() > 0)
		{
			for (auto it = m_floats.begin(); it != m_floats.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				ImGui::InputFloat(it->first.c_str(), &it->second);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Double
		ImGui::Text("Doubles:");
		if (m_doubles.size() > 0)
		{
			for (auto it = m_doubles.begin(); it != m_doubles.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				ImGui::InputDouble(it->first.c_str(), &it->second);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Vector2
		ImGui::Text("Vector2s:");
		if (m_vec2.size() > 0)
		{
			for (auto it = m_vec2.begin(); it != m_vec2.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				float val[2] = { it->second.x, it->second.y };
				ImGui::InputFloat2(it->first.c_str(), val);
				it->second[0] = val[0];
				it->second[1] = val[1];
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Vector3
		ImGui::Text("Vector3s:");
		if (m_vec3.size() > 0)
		{
			for (auto it = m_vec3.begin(); it != m_vec3.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				float val[3] = { it->second.x, it->second.y, it->second.z };
				ImGui::InputFloat3(it->first.c_str(), val);
				it->second[0] = val[0];
				it->second[1] = val[1];
				it->second[2] = val[2];
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Vector4
		ImGui::Text("Vector4s:");
		if (m_vec4.size() > 0)
		{
			for (auto it = m_vec4.begin(); it != m_vec4.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				float val[4] = { it->second.x, it->second.y, it->second.z, it->second.w };
				ImGui::InputFloat4(it->first.c_str(), val);
				it->second[0] = val[0];
				it->second[1] = val[1];
				it->second[2] = val[2];
				it->second[3] = val[4];
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Color3F
		ImGui::Text("Color3F:");
		if (m_color3.size() > 0)
		{
			for (auto it = m_color3.begin(); it != m_color3.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				float val[3] = { it->second.r, it->second.g, it->second.b, };
				ImGui::ColorEdit3(it->first.c_str(), val);
				it->second[0] = val[0];
				it->second[1] = val[1];
				it->second[2] = val[2];
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Color4F
		ImGui::Text("Color3F:");
		if (m_color4.size() > 0)
		{
			for (auto it = m_color4.begin(); it != m_color4.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				float val[4] = { it->second.r, it->second.g, it->second.b, it->second.a };
				ImGui::ColorEdit3(it->first.c_str(), val);
				it->second[0] = val[0];
				it->second[1] = val[1];
				it->second[2] = val[2];
				it->second[3] = val[3];
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		ImGui::PopItemWidth();
	}

	void DevConsole::Draw(Scene* scene)
	{
		static bool open = true;

		if (Input.getKeyDown(KeyCode::F1))
			open = !open;

		if (!open)
			return;

		if (ImGui::Begin("[F1] DevConsole", &open, ImVec2(280, 380), 0.9f, ImGuiWindowFlags_MenuBar))
		{
			// Menu
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Mode"))
				{
					if (ImGui::MenuItem("Master", NULL, m_State == MenuState::MASTER, m_State != MenuState::MASTER))
						m_State = MenuState::MASTER;
					if (ImGui::MenuItem("Custom Values", NULL, m_State == MenuState::CUSTOM_VALUES, m_State != MenuState::CUSTOM_VALUES))
						m_State = MenuState::CUSTOM_VALUES;

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			switch (m_State)
			{
			case MenuState::MASTER:
				Draw_Master(scene);
				break;
			case MenuState::CUSTOM_VALUES:
				Draw_CustomValues();
				break;
			}

		}
		ImGui::End();

	}
}