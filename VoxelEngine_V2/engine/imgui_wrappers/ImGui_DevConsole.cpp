// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ImGui_DevConsole.h"

#include "../imgui/imgui.h"

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


namespace Vxl
{
	std::unordered_map<std::string, int> Imgui_DevConsole::m_integers;
	std::unordered_map<std::string, float> Imgui_DevConsole::m_floats;
	std::unordered_map<std::string, double> Imgui_DevConsole::m_doubles;
	std::unordered_map<std::string, Vector2> Imgui_DevConsole::m_vec2;
	std::unordered_map<std::string, Vector3> Imgui_DevConsole::m_vec3;
	std::unordered_map<std::string, Vector4> Imgui_DevConsole::m_vec4;

	void Imgui_DevConsole::Draw(Scene* scene)
	{
		Scene_Game* Game = dynamic_cast<Scene_Game*>(scene);

		static bool open = true;
		if (ImGui::Begin("DevConsole", &open, ImVec2(280, 380), 0.9f))
		{
			ImGui::Text("FPS: %f", Time.GetFPS());
			ImGui::Text("Time: %f", Time.GetTime());

			float* _fpsGraph = Time.GetFPSHistogram();
			UINT _fpsGraphSize = Time.GetFPSHistogramSize();
			ImGui::PlotHistogram("FPS Histogram", _fpsGraph, _fpsGraphSize, 0, NULL, 0.0f, 100.0f, ImVec2(0, 70));

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

			if (ImGui::CollapsingHeader("Debug Variables"))
			{
				// Integers
				if (m_integers.size() > 0)
				{
					ImGui::Text("Integers:");
					for (auto it = m_integers.begin(); it != m_integers.end(); it++)
					{
						ImGui::InputInt(it->first.c_str(), &it->second);
					}
				}
				// Floats
				if (m_floats.size() > 0)
				{
					ImGui::Text("Floats:");
					for (auto it = m_floats.begin(); it != m_floats.end(); it++)
					{
						ImGui::InputFloat(it->first.c_str(), &it->second);
					}
				}
				// Double
				if (m_doubles.size() > 0)
				{
					ImGui::Text("Doubles:");
					for (auto it = m_doubles.begin(); it != m_doubles.end(); it++)
					{
						ImGui::InputDouble(it->first.c_str(), &it->second);
					}
				}
				// Vector2
				if (m_vec2.size() > 0)
				{
					ImGui::Text("Vector2s:");
					for (auto it = m_vec2.begin(); it != m_vec2.end(); it++)
					{
						float val[2] = { it->second.x, it->second.y };
						ImGui::InputFloat2(it->first.c_str(), val);
						it->second[0] = val[0];
						it->second[1] = val[1];
					}
				}
				// Vector3
				if (m_vec3.size() > 0)
				{
					ImGui::Text("Vector3s:");
					for (auto it = m_vec3.begin(); it != m_vec3.end(); it++)
					{
						float val[3] = { it->second.x, it->second.y, it->second.z };
						ImGui::InputFloat3(it->first.c_str(), val);
						it->second[0] = val[0];
						it->second[1] = val[1];
						it->second[2] = val[2];
					}
				}
				// Vector4
				if (m_vec4.size() > 0)
				{
					ImGui::Text("Vector4s:");
					for (auto it = m_vec4.begin(); it != m_vec4.end(); it++)
					{
						float val[4] = { it->second.x, it->second.y, it->second.z, it->second.w };
						ImGui::InputFloat4(it->first.c_str(), val);
						it->second[0] = val[0];
						it->second[1] = val[1];
						it->second[2] = val[2];
						it->second[3] = val[4];
					}
				}
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
				if (MAIN_CAMERA)
				{
					ImGui::Text("CamPos: %f %f %f", MAIN_CAMERA->getPosition().x, MAIN_CAMERA->getPosition().y, MAIN_CAMERA->getPosition().z);
					ImGui::Text("CamForward: %f %f %f", MAIN_CAMERA->getForward().x, MAIN_CAMERA->getForward().y, MAIN_CAMERA->getForward().z);
				}
			}

			ImGui::Separator();

			ImGui::Text("Crate1 Transform");
			ImGui::Text("Local: %f %f %f", Game->_crate1->m_transform.getPosition().x, Game->_crate1->m_transform.getPosition().y, Game->_crate1->m_transform.getPosition().z);
			ImGui::Text("World: %f %f %f", Game->_crate1->m_transform.getWorldPosition().x, Game->_crate1->m_transform.getWorldPosition().y, Game->_crate1->m_transform.getWorldPosition().z);

			ImGui::Separator();

			ImGui::Text("Crate2 Transform");
			ImGui::Text("Local: %f %f %f", Game->_crate2->m_transform.getPosition().x, Game->_crate2->m_transform.getPosition().y, Game->_crate2->m_transform.getPosition().z);
			ImGui::Text("World: %f %f %f", Game->_crate2->m_transform.getWorldPosition().x, Game->_crate2->m_transform.getWorldPosition().y, Game->_crate2->m_transform.getWorldPosition().z);

			ImGui::Separator();

			ImGui::Checkbox("Wireframe GBUFFER", &GBUFFER_WIREFRAME);

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

			ImGui::InputFloat("FOV", &CAMFOV, 5.0f, 5.0f, 1);
			ImGui::SliderFloat("ZNEAR", &CAM_ZNEAR, 0.01f, 5.0f);
			ImGui::SliderFloat("ZFAR", &CAM_ZFAR, 1.0, 50.0f);

			ImGui::Separator();

			ImGui::Checkbox("SHOW NORMALS", &SHOW_NORMALS);
			ImGui::Checkbox("SHOW DEPTH", &SHOW_DEPTH);

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
		ImGui::End();

	}
}