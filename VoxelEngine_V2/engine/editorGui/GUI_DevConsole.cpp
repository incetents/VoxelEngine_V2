// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GUI_DevConsole.h"

#ifdef GLOBAL_IMGUI

#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"

#include "../input/Input.h"
#include "../input/XGamePad.h"

#include "../editor/Editor.h"

#include "../game/Scene_Game.h"

#include "../math/Transform.h"

#include "../modules/Entity.h"

#include "../objects/Camera.h"

#include "../rendering/RenderManager.h"
#include "../rendering/Mesh.h"

#include "../textures/Texture2D.h"

#include "../utilities/Asset.h"
#include "../utilities/Time.h"
#include "../utilities/Util.h"

#include "../window/window.h"

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

		if (ImGui::Button(RenderManager.m_editorMode ? "Editor Mode [ON]" : "Editor Mode [OFF]"))
		{
			RenderManager.m_editorMode = !RenderManager.m_editorMode;
		}
		if (ImGui::Button(RenderManager.m_globalVAO ? "GLOBAL VAO [ON]" : "GLOBAL VAO [OFF]"))
		{
			RenderManager.m_globalVAO = !RenderManager.m_globalVAO;
			// Rebind all meshes with new VAO state
			{
				auto& meshes = Assets.getAllMesh();
				for (auto& mesh : meshes)
					mesh.second->bind();

				auto& lineMeshes3D = Assets.getAllLineMesh3D();
				for (auto& mesh : lineMeshes3D)
					mesh.second->bind();

				auto& lineMeshes2D = Assets.getAllLineMesh2D();
				for (auto& mesh : lineMeshes2D)
					mesh.second->bind();
			}
		}
		if (ImGui::Button(RenderManager.m_fullScreenRender == FullScreenRender::TRIANGLE ? "FullScreenRender = [Triangle]" : "FullScreenRender = [Quad]"))
		{
			if (RenderManager.m_fullScreenRender == FullScreenRender::TRIANGLE)
				RenderManager.m_fullScreenRender = FullScreenRender::QUAD;
			else
				RenderManager.m_fullScreenRender = FullScreenRender::TRIANGLE;
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Camera"))
		{
			Camera* camera = Assets.getCamera(Game->camera_main);

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
	void DevConsole::Draw_ShowValues()
	{
		//
		ImGui::PushItemWidth(-1);

		// Stored Values
		ImGui::Text("Show Values:");
		for (const auto& container : m_showData)
		{
			ImGui::TextColored(ImGuiColor::Orange, container.first.c_str());
			ImGui::SameLine();

			const RawData* rawData = &container.second;

			switch (container.second.type)
			{
				case UniformType::BOOL:
				{
					bool value = *rawData->getData<bool>();
					if(value)
						ImGui::TextColored(ImGuiColor::GreenLight, "TRUE");
					else
						ImGui::TextColored(ImGuiColor::RedLight, "FALSE");

					break;
				}

				case UniformType::INT:
				{
					ImGui::Text(std::to_string(*rawData->getData<int>()).c_str());
					break;
				}

				case UniformType::FLOAT:
				{
					ImGui::Text(std::to_string(*rawData->getData<float>()).c_str());
					break;
				}

				case UniformType::DOUBLE:
				{
					ImGui::Text(std::to_string(*rawData->getData<double>()).c_str());
					break;
				}

				case UniformType::INT_VEC2:
				{
					Vector2i* data = rawData->getData<Vector2i>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}

				case UniformType::INT_VEC3:
				{
					Vector3i* data = rawData->getData<Vector3i>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}

				case UniformType::INT_VEC4:
				{
					Vector4i* data = rawData->getData<Vector4i>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}

				case UniformType::FLOAT_VEC2:
				{
					Vector2* data = rawData->getData<Vector2>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}
				
				case UniformType::FLOAT_VEC3:
				{
					Vector3* data = rawData->getData<Vector3>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}

				case UniformType::FLOAT_VEC4:
				{
					Vector4* data = rawData->getData<Vector4>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}

				case UniformType::DOUBLE_VEC2:
				{
					Vector2d* data = rawData->getData<Vector2d>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}

				case UniformType::DOUBLE_VEC3:
				{
					Vector3d* data = rawData->getData<Vector3d>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}

				case UniformType::DOUBLE_VEC4:
				{
					Vector4d* data = rawData->getData<Vector4d>();
					ImGui::Text(data->ToString('|').c_str());
					break;
				}
			}
		}

		//
		ImGui::PopItemWidth();
	}
	void DevConsole::Draw_EditValues()
	{
		//
		ImGui::PushItemWidth(-1);

		// Stored Values
		ImGui::Text("Edit Values:");
		for (auto& container : m_editData)
		{
			ImGui::TextColored(ImGuiColor::Orange, container.first.c_str());
			//ImGui::SameLine();

			RawData* rawData = &container.second.data;

			switch (container.second.data.type)
			{
			case UniformType::BOOL:
			{
				bool value = *rawData->getData<bool>();
				if(ImGui::Checkbox(container.first.c_str(), &value))
					rawData->setData<bool>(value);
				break;
			}

			case UniformType::INT:
			{
				int value = *rawData->getData<int>();
				if (ImGui::DragInt(container.first.c_str(), &value))
					rawData->setData<int>(value);
				break;
			}

			case UniformType::FLOAT:
			{
				float value = *rawData->getData<float>();
				if (ImGui::DragFloat(container.first.c_str(), &value))
					rawData->setData<float>(value);
				break;
			}

			case UniformType::DOUBLE:
			{
				double value = *rawData->getData<double>();
				if (ImGui::DragScalar(container.first.c_str(), ImGuiDataType_Double, &value, 1))
					rawData->setData<double>(value);
				break;
			}

			case UniformType::INT_VEC2:
			{
				Vector2i value = *rawData->getData<Vector2i>();
				if (ImGui::DragInt2(container.first.c_str(), value.GetStartPointer()))
					rawData->setData<Vector2i>(value);
				break;
			}

			case UniformType::INT_VEC3:
			{
				Vector3i value = *rawData->getData<Vector3i>();

				// Range Case
				if (container.second.isRange)
				{
					float avg_speed = (value.z - value.y) / 1000.0f;
					if (ImGui::DragInt(container.first.c_str(), &value.x, avg_speed, value.y, value.z))
						rawData->setData<Vector3i>(value);
				}
				// Normal Vec3
				else
				{
					if (ImGui::DragInt3(container.first.c_str(), value.GetStartPointer()))
						rawData->setData<Vector3i>(value);
				}
				break;
			}

			case UniformType::INT_VEC4:
			{
				Vector4i value = *rawData->getData<Vector4i>();
				if (ImGui::DragInt4(container.first.c_str(), value.GetStartPointer()))
					rawData->setData<Vector4i>(value);
				break;
			}

			case UniformType::FLOAT_VEC2:
			{
				Vector2 value = *rawData->getData<Vector2>();
				if (ImGui::DragFloat3(container.first.c_str(), value.GetStartPointer()))
					rawData->setData<Vector2>(value);
				break;
			}

			case UniformType::FLOAT_VEC3:
			{
				Vector3 value = *rawData->getData<Vector3>();

				// Color Case
				if (container.second.isColor)
				{
					if (ImGui::ColorEdit3(container.first.c_str(), value.GetStartPointer()))
						rawData->setData<Vector3>(value);
				}
				// Range Case
				else if (container.second.isRange)
				{
					float avg_speed = (value.z - value.y) / 1000.0f;
					if(ImGui::DragFloat(container.first.c_str(), &value.x, avg_speed, value.y, value.z))
						rawData->setData<Vector3>(value);
				}
				// Normal Vec3
				else
				{
					if (ImGui::DragFloat3(container.first.c_str(), value.GetStartPointer()))
						rawData->setData<Vector3>(value);
				}
				break;
			}

			case UniformType::FLOAT_VEC4:
			{
				Vector4 value = *rawData->getData<Vector4>();

				// Color Case
				if (container.second.isColor)
				{
					if (ImGui::ColorEdit4(container.first.c_str(), value.GetStartPointer()))
						rawData->setData<Vector4>(value);
				}
				else
				{
					if (ImGui::DragFloat4(container.first.c_str(), value.GetStartPointer()))
						rawData->setData<Vector4>(value);
				}

				break;
			}

			case UniformType::DOUBLE_VEC2:
			{
				Vector2d value = *rawData->getData<Vector2d>();
				Vector2 fixed((float)value.x, (float)value.y);
				if (ImGui::DragFloat2(container.first.c_str(), fixed.GetStartPointer()))
					rawData->setData<Vector2d>(Vector2d(fixed.x, fixed.y));
				break;
			}

			case UniformType::DOUBLE_VEC3:
			{
				Vector3d value = *rawData->getData<Vector3d>();
				Vector3 fixed((float)value.x, (float)value.y, (float)value.z);

				// Range Case
				if (container.second.isRange)
				{
					float avg_speed = (float)((value.z - value.y) / 1000.0);
					if (ImGui::DragFloat(container.first.c_str(), &fixed.x, avg_speed, fixed.y, fixed.z))
						rawData->setData<Vector3d>(Vector3d(fixed.x, fixed.y, fixed.z));
				}
				// Normal Vec3
				else
				{
					if (ImGui::DragFloat3(container.first.c_str(), fixed.GetStartPointer()))
						rawData->setData<Vector3d>(Vector3d(fixed.x, fixed.y, fixed.z));
				}
				break;
			}

			case UniformType::DOUBLE_VEC4:
			{
				Vector4d value = *rawData->getData<Vector4d>();
				Vector4 fixed((float)value.x, (float)value.y, (float)value.z, (float)value.w);
				if (ImGui::DragFloat4(container.first.c_str(), fixed.GetStartPointer()))
					rawData->setData<Vector4d>(Vector4d(fixed.x, fixed.y, fixed.z, fixed.w));
				break;
			}

			}
			ImGui::Separator();
		}
	

		//
		ImGui::PopItemWidth();
	}

	void DevConsole::Draw()
	{
		// Menu
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Mode"))
			{
				if (ImGui::MenuItem("Master", NULL, m_State == MenuState::MASTER, m_State != MenuState::MASTER))
					m_State = MenuState::MASTER;
				if (ImGui::MenuItem("Edit Data", NULL, m_State == MenuState::EDIT_DATA, m_State != MenuState::EDIT_DATA))
					m_State = MenuState::EDIT_DATA;
				if (ImGui::MenuItem("Show Data", NULL, m_State == MenuState::SHOW_DATA, m_State != MenuState::SHOW_DATA))
					m_State = MenuState::SHOW_DATA;

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
		case MenuState::EDIT_DATA:
			Draw_EditValues();
			break;
		case MenuState::SHOW_DATA:
			Draw_ShowValues();
			break;
		}

	}
}
#endif