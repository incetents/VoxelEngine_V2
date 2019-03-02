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

			if (ImGui::Button("Reload Shaders"))
			{
				RenderManager.ReloadShaders();
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