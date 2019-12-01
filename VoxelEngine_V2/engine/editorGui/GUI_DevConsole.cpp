// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_DevConsole.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../utilities/Time.h"
#include "../utilities/Util.h"

#include "../window/window.h"

#include "../rendering/RenderManager.h"
#include "../modules/Entity.h"

#include "../objects/Camera.h"

#include "../input/Input.h"
#include "../input/XGamePad.h"

#include "../math/Transform.h"

#include "../game/Scene_Game.h"

#include "../editor/Editor.h"

#include "../textures/Texture2D.h"

#include "../utilities/Asset.h"

#include "windows.h"

namespace Vxl
{
	void DevConsole::Draw_Master(Scene* scene)
	{
		Scene_Game* Game = dynamic_cast<Scene_Game*>(scene);
		if (!Game)
			return;

		ImGui::Text("FPS: %f", Time.GetFPS());
		ImGui::Text("Time: %f", Time.GetTime());

		const float* _fpsGraph = Time.GetFPSHistogram();
		UINT _fpsGraphSize = Time.GetFPSHistogramSize();
		ImGui::PlotHistogram("FPS Histogram", _fpsGraph, _fpsGraphSize, 0, NULL, 0.0f, 100.0f, ImVec2(0, 70));

		ImGui::Separator();

		if (ImGui::CollapsingHeader("GPU Memory"))
		{
			std::string m1 = "GPU TOTAL: " + std::to_string(Graphics::VRAM_Maximum_KB / 1000) + " [MB]";
			ImGui::Text(m1.c_str());

			std::string m2 = "GPU Current: " + std::to_string(Graphics::VRAM_Current_KB / 1000) + " [MB]";
			ImGui::Text(m2.c_str());
		}
		if (ImGui::CollapsingHeader("CPU Memory"))
		{
			std::string c1 = "CPU Virtual Memory [Total] : " + std::to_string(Graphics::CPU::VirtualMemory::Total_KB / 1000) + " [MB]";
			ImGui::Text(c1.c_str());

			std::string c2 = "CPU Virtual Memory [Used] : " + std::to_string(Graphics::CPU::VirtualMemory::Used_KB / 1000) + " [MB]";
			ImGui::Text(c2.c_str());

			std::string c3 = "CPU Virtual Memory [UsedByProcess] : " + std::to_string(Graphics::CPU::VirtualMemory::UsedByProcess_KB / 1000) + " [MB]";
			ImGui::Text(c3.c_str());

			std::string r1 = "CPU RAM [Total] : " + std::to_string(Graphics::CPU::RAM::Total_KB / 1000) + " [MB]";
			ImGui::Text(r1.c_str());

			std::string r2 = "CPU RAM [Used] : " + std::to_string(Graphics::CPU::RAM::Used_KB / 1000) + " [MB]";
			ImGui::Text(r2.c_str());

			std::string r3 = "CPU RAM [UsedByProcess] : " + std::to_string(Graphics::CPU::RAM::UsedByProcess_KB / 1000) + " [MB]";
			ImGui::Text(r3.c_str());
		}

		ImGui::Separator();

		ImGui::Text("Window Position: X=[%d], Y=[%d]", Window.GetPositionX(), Window.GetPositionY());
		ImGui::Text("Window Size: W=[%d], H=[%d]", Window.GetWindowWidth(), Window.GetWindowHeight());
		ImGui::Text("Window Viewport: X=[%d], Y=[%d], W=[%d], H=[%d]", Window.GetViewportOffsetX(), Window.GetViewportOffsetY(), Window.GetViewportWidth(), Window.GetViewportHeight());

		ImGui::Text("Reload Window = [F5]");
		ImGui::Text("Reload Fullscreen FBOs = [F6]");
		ImGui::Text("Reload Shaders = [R]");

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Camera"))
		{
			Camera* camera = Assets::getCamera(Game->camera_main);

			if (camera)
			{
				Vector3 CamPos = camera->m_transform.getPosition();
				Vector3 CamForward = camera->m_transform.getForward();
				ImGui::Text("CamPos: %f %f %f", CamPos.x, CamPos.y, CamPos.z);
				ImGui::Text("CamForward: %f %f %f", CamForward.x, CamForward.y, CamForward.z);

				float cameraFOV = camera->getFOV();
				float cameraZNear = camera->getZnear();
				float cameraZFar = camera->getZfar();

				if (ImGui::SliderFloat("FOV", &cameraFOV, 60.0f, 180.0f))
					camera->SetPerspective(cameraFOV, camera->getAspect());

				if (ImGui::SliderFloat("ZNEAR", &cameraZNear, 0.01f, 5.0f))
					camera->SetZNear(cameraZNear);

				if (ImGui::SliderFloat("ZFAR", &cameraZFar, 1.0f, 50.0f))
					camera->SetZFar(cameraZFar);

				ImGui::Separator();
			}
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Resolutions"))
		{
			ImGui::BeginChild("Resolutions", ImVec2(0, 150), true);
			{
				for (unsigned int i = 0; i < Util::m_commonResolutionsCount; i++)
				{
					std::string Name = "Set Size: " + std::string(Util::m_commonResolutionsStr[i]);
					if (ImGui::Button(Name.c_str()))
					{
						Window.SetSize(
							Util::m_commonResolutions[i].first,
							Util::m_commonResolutions[i].second
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

				for(uint32_t i = 0; i < Util::m_commonAspectRatiosCount; i++)
				{
					if (ImGui::Button(Util::m_commonAspectRatiosStr[i]))
					{
						Window.SetCustomAspectRatio(true, Util::m_commonAspectRatios[i]);
					}
				}
			}
			ImGui::EndChild();
		}

		ImGui::Separator();

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
		if (m_editData.m_bools.size() > 0)
		{
			for (auto it = m_editData.m_bools.begin(); it != m_editData.m_bools.end(); it++)
			{
				ImGui::Checkbox(it->first.c_str(), &it->second);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Integers
		ImGui::Text("Integers:");
		if (m_editData.m_integers.size() > 0)
		{
			for (auto it = m_editData.m_integers.begin(); it != m_editData.m_integers.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				ImGui::InputInt(it->first.c_str(), &it->second);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Integers Ranged
		ImGui::Text("Integers Ranged:");
		if (m_editData.m_integersRanged.size() > 0)
		{
			for (auto it = m_editData.m_integersRanged.begin(); it != m_editData.m_integersRanged.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				ImGui::SliderInt(it->first.c_str(), &it->second.x, it->second.y, it->second.z);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Floats
		ImGui::Text("Floats:");
		if (m_editData.m_floats.size() > 0)
		{
			for (auto it = m_editData.m_floats.begin(); it != m_editData.m_floats.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				ImGui::InputFloat(it->first.c_str(), &it->second);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Floats Ranged
		ImGui::Text("Floats Ranged:");
		if (m_editData.m_floatsRanged.size() > 0)
		{
			for (auto it = m_editData.m_floatsRanged.begin(); it != m_editData.m_floatsRanged.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				ImGui::SliderFloat(it->first.c_str(), &it->second.x, it->second.y, it->second.z);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Double
		ImGui::Text("Doubles:");
		if (m_editData.m_doubles.size() > 0)
		{
			for (auto it = m_editData.m_doubles.begin(); it != m_editData.m_doubles.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				ImGui::InputDouble(it->first.c_str(), &it->second);
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Double
		ImGui::Text("Doubles Ranged:");
		if (m_editData.m_doublesRanged.size() > 0)
		{
			for (auto it = m_editData.m_doublesRanged.begin(); it != m_editData.m_doublesRanged.end(); it++)
			{
				ImGui::Text(it->first.c_str());
				ImGui::SameLine();
				float v = (float)it->second.x;
				ImGui::SliderFloat(it->first.c_str(), &v, (float)it->second.y, (float)it->second.z);
				it->second.x = (double)v;
			}
		}
		else
			ImGui::TextColored(ImGuiColor::Orange, "~None~");
		ImGui::Separator();

		// Vector2
		ImGui::Text("Vector2s:");
		if (m_editData.m_vec2.size() > 0)
		{
			for (auto it = m_editData.m_vec2.begin(); it !=m_editData. m_vec2.end(); it++)
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
		if (m_editData.m_vec3.size() > 0)
		{
			for (auto it = m_editData.m_vec3.begin(); it != m_editData.m_vec3.end(); it++)
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
		if (m_editData.m_vec4.size() > 0)
		{
			for (auto it = m_editData.m_vec4.begin(); it != m_editData.m_vec4.end(); it++)
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
		if (m_editData.m_color3.size() > 0)
		{
			for (auto it = m_editData.m_color3.begin(); it != m_editData.m_color3.end(); it++)
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
		if (m_editData.m_color4.size() > 0)
		{
			for (auto it = m_editData.m_color4.begin(); it != m_editData.m_color4.end(); it++)
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
	void DevConsole::Draw_Statistics()
	{
		//
		ImGui::PushItemWidth(-1);

		// Stored Values
		ImGui::Text("Statistics:");
		for (const auto& container : m_showData)
		{
			ImGui::TextColored(ImGuiColor::Orange, container.first.c_str());
			ImGui::SameLine();

			switch (container.second.type)
			{
				case GenericDataType::BOOL:
				{
					bool value = *container.second.GetData<bool>();
					if(value)
						ImGui::TextColored(ImGuiColor::GreenLight, "TRUE");
					else
						ImGui::TextColored(ImGuiColor::RedLight, "FALSE");

					break;
				}

				case GenericDataType::INT:
				{
					ImGui::Text(std::to_string(*container.second.GetData<int>()).c_str());
					break;
				}

				case GenericDataType::FLOAT:
				{
					ImGui::Text(std::to_string(*container.second.GetData<float>()).c_str());
					break;
				}

				case GenericDataType::DOUBLE:
				{
					ImGui::Text(std::to_string(*container.second.GetData<double>()).c_str());
					break;
				}

				case GenericDataType::VEC2:
				{
					Vector2* data = container.second.GetData<Vector2>();
					ImGui::Text(data->ToString().c_str());
					break;
				}
				
				case GenericDataType::VEC3:
				{
					Vector3* data = container.second.GetData<Vector3>();
					ImGui::Text(data->ToString().c_str());
					break;
				}

				case GenericDataType::VEC4:
				{
					Vector4* data = container.second.GetData<Vector4>();
					ImGui::Text(data->ToString().c_str());
					break;
				}

				case GenericDataType::COLOR3:
				{
					Color3F* data = container.second.GetData<Color3F>();
					ImGui::Text(data->ToString().c_str());
					break;
				}

				case GenericDataType::COLOR4:
				{
					Color4F* data = container.second.GetData<Color4F>();
					ImGui::Text(data->ToString().c_str());
					break;
				}
			}
		}

		//	if (m_display_values.size() > 0)
		//	{
		//		for (auto it = m_display_values.begin(); it != m_display_values.end(); it++)
		//		{
		//			ImGui::TextColored(ImGuiColor::Orange, it->first.c_str());	ImGui::SameLine();
		//			
		//			ImGui::Text(": ");				ImGui::SameLine();
		//	
		//			if (it->second.first)
		//				ImGui::Text(it->second.second.c_str());
		//			else
		//				ImGui::TextColored(ImGuiColor::Grey, it->second.second.c_str());
		//		}
		//	}
		//	else
		//		ImGui::TextColored(ImGuiColor::Orange, "~None~");
		//	ImGui::Separator();

		//
		ImGui::PopItemWidth();
	}

	void DevConsole::Draw()
	{
		//	static bool open = true;
		//	
		//	if (Input.getKeyDown(KeyCode::F1))
		//		open = !open;
		//	
		//	if (!open)
		//		return;

		//if (ImGui::Begin("[F1] DevConsole", &open, ImVec2(280, 380), 0.9f, ImGuiWindowFlags_MenuBar))
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
					if (ImGui::MenuItem("Statistics", NULL, m_State == MenuState::STATISTICS, m_State != MenuState::STATISTICS))
						m_State = MenuState::STATISTICS;

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// TEST IMAGE
			//	Texture2D* tex = Texture2D::GetAsset("beato");
			//	ImGui::Image((void*)tex->getID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

			switch (m_State)
			{
			case MenuState::MASTER:
				Draw_Master(m_scene);
				break;
			case MenuState::CUSTOM_VALUES:
				Draw_CustomValues();
				break;
			case MenuState::STATISTICS:
				Draw_Statistics();
				break;
			}

		}
		//ImGui::End();

	}
}
#endif