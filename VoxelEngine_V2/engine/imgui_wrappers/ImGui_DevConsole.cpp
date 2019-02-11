// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ImGui_DevConsole.h"

#include "../imgui/imgui.h"

#include "../utilities/Time.h"
#include "../window/window.h"
#include "../modules/RenderManager.h"
#include "../input/Input.h"
#include "../input/XGamePad.h"
#include "../math/Camera.h"

#include "../game/Scene_Game.h"
#include "../modules/Entity.h"
#include "../math/Transform.h"

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

			if (ImGui::Button("Set Size (1080, 720)"))
			{
				Window.SetSize(1080, 720);
				MAIN_CAMERA->updatePerspectiveAspectRatio(Window.GetAspectRatio());
			}

			if (ImGui::Button("Set Size (500, 500)"))
			{
				Window.SetSize(500, 500);
				MAIN_CAMERA->updatePerspectiveAspectRatio(1.0f);
			}

			if (ImGui::Button("Free Aspect Ratio"))
			{
				Window.SetCustomAspectRatio(false);
				MAIN_CAMERA->updatePerspectiveAspectRatio(Window.GetAspectRatio());
			}
			if (ImGui::Button("Lock Aspect Ratio [1:1]"))
			{
				Window.SetCustomAspectRatio(true, 1.0f);
				MAIN_CAMERA->updatePerspectiveAspectRatio(1.0f);
			}

			ImGui::Separator();

			ImGui::InputFloat("FOV", &CAMFOV, 5.0f, 5.0f, 1);
			ImGui::SliderFloat("ZNEAR", &CAM_ZNEAR, 0.01f, 5.0f);
			ImGui::SliderFloat("ZFAR", &CAM_ZFAR, 1.0, 50.0f);

			ImGui::Separator();

			ImGui::Checkbox("SHOW NORMALS", &SHOW_NORMALS);

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