// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderManager.h"

#include "Debug.h"
#include "Geometry.h"
#include "Mesh.h"
#include "FramebufferObject.h"
#include "Graphics.h"
#include "RenderBuffer.h"

#include "../modules/Scene.h"
#include "../modules/Layer.h"
#include "../modules/Material.h"

#include "../utilities/Util.h"
#include "../utilities/Time.h"
#include "../utilities/Macros.h"

#include "../textures/Cubemap.h"
#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"

#include "../window/window.h"
#include "../editor/Editor.h"

#include "../objects/GameObject.h"
#include "../objects/LightObject.h"
#include "../objects/CameraObject.h"
#include "../objects/TextObject.h"

#include "../editorGui/DevConsole.h"
#include "../editorGui/GUIViewport.h"
#include "../editorGui/Hierarchy.h"
#include "../editorGui/Inspector.h"
#include "../editorGui/Performance.h"
#include "../editorGui/ShaderErrors.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"
#endif

#include <algorithm>

namespace Vxl
{
	RenderManager::RenderManager()
	{
		m_layers = new Layer[MAX_LAYERS];
		for (UINT i = 0; i < MAX_LAYERS; i++)
			m_layers[i].m_id = (1 << i);
	}

	// Utility
	void RenderManager::SetNewScene(Scene* _scene)
	{
		if (m_currentScene)
		{
			m_currentScene->Destroy();
			DestroySceneGLResources();
		}

		InitSceneGLResources();

		m_currentScene = _scene;

		m_currentScene->Setup();
	}
	const Layer& RenderManager::GetLayer(uint32_t index)
	{
		VXL_ASSERT(index < MAX_LAYERS, "Layer index out of bounds");
		return m_layers[index];
	}

	void RenderManager::UpdateAllWindowAspectCameras()
	{
		auto AllCameras = CameraObject::GetAllNamedAssets();
		for (auto camera = AllCameras.begin(); camera != AllCameras.end(); camera++)
		{
			if (camera->second->isPerspectiveWindowAspect())
			{
				camera->second->UpdateAspect(Window.GetAspectRatio());
				camera->second->CreatePerspective();
			}
		}
	}

	void RenderManager::AddEntity(Entity* _entity)
	{
		VXL_ASSERT(_entity, "RenderManager cannot add nullptr");

		switch (_entity->GetType())
		{
			case EntityType::GAMEOBJECT:
			{
				auto _GameObject = (GameObject*)_entity;
				VXL_ASSERT(_GameObject->GetMaterial() != nullptr, "GameObject has no material");

				// Material/GameObjects set
				auto key = _GameObject->GetMaterial();

				// Empty Check
				if (m_gameObjectsPerMaterial[key] == nullptr)
					m_gameObjectsPerMaterial[key] = new std::set<GameObject*>();

				m_gameObjectsPerMaterial[key]->insert(_GameObject);

				m_allEntities.push_back(_entity);

				break;
			}
			// Other Entities (Cameras, Lights, Text, etc...)
			default:
			{
				m_allEntities.push_back(_entity);
				break;
			}
		}
	}

	void RenderManager::RemoveEntity(Entity* _entity)
	{
		VXL_ASSERT(_entity, "RenderManager cannot remove nullptr");

		switch (_entity->GetType())
		{
			case EntityType::GAMEOBJECT:
			{
				auto _GameObject = (GameObject*)_entity;
				VXL_ASSERT(_GameObject->GetMaterial() != nullptr, "GameObject has no material");

				// Material/GameObjects set
				auto key = _GameObject->GetMaterial();

				// Empty or Missing Check
				if (m_gameObjectsPerMaterial[key] == nullptr || m_gameObjectsPerMaterial[key]->find(_GameObject) == m_gameObjectsPerMaterial[key]->end())
					return;

				m_gameObjectsPerMaterial[key]->erase(_GameObject);

				// Remove
				Util::RemoveFromVector(m_allEntities, _entity);

				break;
			}
			case EntityType::CAMERA:
			{
				auto _Camera = (CameraObject*)_entity;
				VXL_ASSERT(_Camera, "CameraObject cast failed");

				// Remove
				Util::RemoveFromVector(m_allEntities, _entity);

				break;
			}
			case EntityType::LIGHT:
			{
				auto _Light = (LightObject*)_entity;
				VXL_ASSERT(_Light, "LightObject cast failed");

				// Remove
				Util::RemoveFromVector(m_allEntities, _entity);

				break;
			}
		}
	}

	// Reload Shader System
	void RenderManager::ReloadShaders()
	{
		Shader::ShaderErrorLog.clear();
		Shader::ShaderErrorLogSize = 0;
		ShaderProgram::ProgramsFailed.clear();
		ShaderProgram::ProgramsFailedSize = 0;

		auto Programs = ShaderProgram::GetAllNamedAssets();
		auto Shaders = Shader::GetAllNamedAssets();

		for (auto Program : Programs)
			Program.second->detachShaders();

		for (auto Shader : Shaders)
			Shader.second->reload();

		for (auto Program : Programs)
		{
			Program.second->destroyProgram();
			Program.second->createProgram();
			Program.second->attachShaders();
			Program.second->Link();
		}

		auto Materials = Material::GetAllNamedAssets();
		for (auto Mat : Materials)
			Mat.second->UpdateProperties();
	}
	void RenderManager::ReloadWindow()
	{
		Graphics::initHints(); // Newer version

		DestroyGlobalGLResources();
		DestroySceneGLResources();

		Window.Reload();

		InitGlobalGLResources();
		InitSceneGLResources();

		if (m_currentScene)
			m_currentScene->Reload();
	}
	void RenderManager::ReloadFBOS()
	{
		auto fbos = FramebufferObject::GetAllNamedAssets();
		for (auto& fbo : fbos)
		{
			if (fbo.second->m_fullscreen)
			{
				// Adjust Size based on current window render size [effectively resizing FBO resolution]
				fbo.second->m_width = Window.GetViewportWidth();
				fbo.second->m_height = Window.GetViewportHeight();
				fbo.second->Bind();
				fbo.second->ReloadAttachments();
				fbo.second->ReloadDepth();
				//
			}
		}
		FramebufferObject::Unbind();
	}

	// 
	void RenderManager::InitGlobalGLResources()
	{
		UBOManager.InitGLResources();
		Debug.InitGLResources();
		GlobalRenderText.InitGLResources();
		GUIViewport.InitGLResources();
	}
	void RenderManager::DestroyGlobalGLResources()
	{
		UBOManager.DestroyGLResources();
		Debug.DestroyGLResources();
		GlobalRenderText.DestoryGLResources();
		GUIViewport.DestroyGLResources();
	}

	//
	void RenderManager::InitSceneGLResources()
	{
		Geometry.InitGLResources();
	}
	void RenderManager::DestroySceneGLResources()
	{
		// Remove selected entity
		Editor.ClearSelection();

		// Delete special
		GPUTimer::DestroyTimers();

		// Destroy all Entities
		GameObject::DeleteAllAssets();
		LightObject::DeleteAllAssets();
		CameraObject::DeleteAllAssets();

		// Delete Materials
		Material::DeleteAllAssets();

		Material::m_masterOrder.clear();
		Material::m_masterOrderDirty = true;

		// Clear Render List
		m_allEntities.clear();

		m_gameObjectsPerMaterial.clear();
		m_gameObjectsSorted.clear();

		// Clear Shader Error Log
		Shader::ShaderErrorLog.clear();
		Shader::ShaderErrorLogSize = 0;
		ShaderProgram::ProgramsFailed.clear();
		ShaderProgram::ProgramsFailedSize = 0;
		// Delete Shaders
		ShaderProgram::DeleteAllAssets();
		Shader::DeleteAllAssets();

		// Delete Textures
		Texture2D::DeleteAllAssets();
		Cubemap::DeleteAllAssets();
		RenderTexture::DeleteAllAssets();

		// Delete extra buffers
		RenderBuffer::DeleteAllAssets();

		// Delete Framebuffers
		FramebufferObject::DeleteAllAssets();

		// Delete Meshes
		Mesh::DeleteAllAssets();
	}
	void RenderManager::Update()
	{
		m_currentScene->Update();

		// Update all entities
		for (auto it = m_allEntities.begin(); it != m_allEntities.end(); it++)
			(*it)->Update();

		// Update all CPU Timers
		for (auto it = CPUTimer::m_timers.begin(); it != CPUTimer::m_timers.end(); it++)
			it->second->Update();

		// Update all GPU Timers
		for (auto it = GPUTimer::m_timers.begin(); it != GPUTimer::m_timers.end(); it++)
			it->second->Update();
	}
	void RenderManager::UpdateFixed()
	{
		m_currentScene->UpdateFixed();
	}
	void RenderManager::Draw()
	{
		m_currentScene->Draw();
		Debug.End();
	}
	void RenderManager::InitImGui()
	{
		// Imgui Initial Values
		DevConsole.Init("DevConsole", Vector2(280.f, 380.f), 0.9f, ImGuiWindowFlags_MenuBar);
		ShaderErrors.Init("ShaderErrors", Vector2(700.f, 400.f), 0.9f);
		ShaderErrors.SetOpen(false);
		Inspector.Init("Inspector", Vector2(380, 280), 0.9f);
		Hierarchy.Init("Hierarchy", Vector2(280, 380), 0.9f);
		Performance.Init("Performance", Vector2(280, 680), 0.9f);
		GUIViewport.Init("Viewport", Vector2(500, 500), 0.9f, ImGuiWindowFlags_MenuBar);
		GUIViewport.SetPadding(false);

		m_guiWindows.push_back(&DevConsole.instanceRef);
		m_guiWindows.push_back(&ShaderErrors.instanceRef);
		m_guiWindows.push_back(&Inspector.instanceRef);
		m_guiWindows.push_back(&Hierarchy.instanceRef);
		m_guiWindows.push_back(&Performance.instanceRef);
		m_guiWindows.push_back(&GUIViewport.instanceRef);
	}
	void RenderManager::DrawImGui()
	{
#ifdef GLOBAL_IMGUI
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			// VIEW //
			if (ImGui::BeginMenu("View"))
			{

				if (ImGui::MenuItem(DevConsole.GetName().c_str(), "", DevConsole.IsOpen()))
				{ 
					DevConsole.ToggleOpen();
				}
				if (ImGui::MenuItem(Inspector.GetName().c_str(), "", Inspector.IsOpen()))
				{
					Inspector.ToggleOpen();
				}
				if (ImGui::MenuItem(Hierarchy.GetName().c_str(), "", Hierarchy.IsOpen()))
				{
					Hierarchy.ToggleOpen();
				}
				if (ImGui::MenuItem(Performance.GetName().c_str(), "", Performance.IsOpen()))
				{
					Performance.ToggleOpen();
				}
				if (ImGui::MenuItem(GUIViewport.GetName().c_str(), "", GUIViewport.IsOpen()))
				{
					GUIViewport.ToggleOpen();
				}

				ImGui::EndMenu();
			}

			// SHADER ERRORS //
			{
				bool hasErrors = ShaderErrors.HasErrors();

				if (hasErrors)
					ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::RedLight);
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::Grey);

				if (ImGui::MenuItem(hasErrors ? "Shader Errors !" : "Shader Errors"))
				{
					ShaderErrors.ToggleOpen();
				}

				ImGui::PopStyleColor();
			}
		}
		ImGui::EndMainMenuBar();

		// Drawing All Imgui Windows here
		for (auto& GUI : m_guiWindows)
		{
			if (!GUI->IsOpen())
				continue;

			GUI->SetScene(m_currentScene);
			if (GUI->Begin())
			{
				GUI->Draw();
			}
			GUI->End();
		}

		//m_currentScene->DrawImGui();

		ImGui::Render();
#endif
	}

	void RenderManager::RenderFullScreen()
	{
		if(m_FSQMode)
			Geometry.GetFullQuad()->Draw();
		else
			Geometry.GetFullTriangle()->Draw();
	}

	// render all gameobjects with their corresponding materials
	void RenderManager::RenderSceneGameObjects()
	{
		// If material order has been changed, re-sort them all
		if (Material::m_masterOrderDirty)
		{
			m_gameObjectsSorted.clear();

			for (auto renderSet = m_gameObjectsPerMaterial.begin(); renderSet != m_gameObjectsPerMaterial.end(); renderSet++)
			{
				m_gameObjectsSorted[renderSet->first->GetOrder()] = std::pair<Material*, std::set<GameObject*>*>(renderSet->first, renderSet->second);
			}

			Material::m_masterOrderDirty = false;
		}


		// Map that contains all sets of material/gameobjects in order
		for (auto renderOrder = m_gameObjectsSorted.begin(); renderOrder != m_gameObjectsSorted.end(); renderOrder++)
		{
			// Set of material/gameobjects
			auto materialSet = renderOrder->second;

			// Get Material for binding
			Material* material = materialSet.first;
			VXL_ASSERT(material, "Storing nullptr in renderSet");
			material->BindProgram();
			material->BindStates();
			material->BindTextures(); // Only occurs if shared textures is true

			// Get GameObject for rendering
			std::set<GameObject*>* entities = materialSet.second;

			for (auto& ent : *entities)
			{
				if (ent->IsFamilyActive())
					ent->Draw();
			}
		}
	}

	// Effectively non-gameobject entities need to have their own color ID show up in the color ID attachment.
	// This function takes care of that without affecting albedo,normals,metallic,smoothness,etc...
	void RenderManager::RenderSceneObjectsWithOnlyColorID()
	{
		// Extra Objects for COLOR ID
		auto gbuffer_mat = Material::GetAsset("gbuffer");
		gbuffer_mat->BindProgram();
		gbuffer_mat->BindStates();

		// Force specific fbo draw buffers
		auto fbo_gbuffer = FramebufferObject::GetAsset("gbuffer");
		// Store Depth for later usage
		auto fbo_colorpicker = FramebufferObject::GetAsset("ColorPicker");
		
		fbo_gbuffer->blitDepth(*fbo_colorpicker);

		// Only render to COLOR ID Texture
		fbo_gbuffer->DisableAttachment(0);
		fbo_gbuffer->DisableAttachment(1);

		// Default
		gbuffer_mat->m_property_useModel.SetProperty(true);
		gbuffer_mat->m_property_useInstancing.SetProperty(false);
		gbuffer_mat->m_property_useTexture.SetProperty(false);
		gbuffer_mat->m_property_tint.SetProperty(Color3F(1,1,1));
		gbuffer_mat->m_property_color.SetProperty(Color3F(1,1,1));
		
		// Render CameraObjects
		auto Cameras = CameraObject::GetAllNamedAssets();
		for (auto it = Cameras.begin(); it != Cameras.end(); it++)
		{
			gbuffer_mat->m_property_model.SetPropertyMatrix(it->second->m_transform.getWorldModel(), true);
			gbuffer_mat->m_property_output.SetProperty(it->second->m_colorID);

			Geometry.GetSphere()->Draw();
		}
		// Render LightObjects
		auto Lights = LightObject::GetAllNamedAssets();
		for (auto it = Lights.begin(); it != Lights.end(); it++)
		{
			gbuffer_mat->m_property_model.SetPropertyMatrix(it->second->m_transform.getWorldModel(), true);
			gbuffer_mat->m_property_output.SetProperty(it->second->m_colorID);

			Geometry.GetSphere()->Draw();
		}

		// Fix fbo draw buffers
		fbo_gbuffer->EnableAttachment(0);
		fbo_gbuffer->EnableAttachment(1);

		// Revert Depth
		fbo_colorpicker->blitDepth(*fbo_gbuffer);
	}
	void RenderManager::RenderEditorObjects()
	{
		Graphics::SetDepthRead(true);
		Graphics::SetDepthWrite(true);

		Graphics::SetBlendState(true);
		Graphics::SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);

		// Draw Lights
		auto billboard = Material::GetAsset("billboard");
		billboard->BindProgram();
		
		Texture2D* LightTexture = Texture2D::GetAsset("editor_lightbulb");
		auto AllLights = LightObject::GetAllNamedAssets();
		for (auto light = AllLights.begin(); light != AllLights.end(); light++)
		{
			billboard->m_property_model.SetPropertyMatrix(light->second->m_transform.getWorldModel(), true);
		
			if (LightTexture)
				LightTexture->Bind(TextureLevel::LEVEL0);
			else
				Debug.GetNullTexture()->Bind(TextureLevel::LEVEL0);

			Geometry.GetQuadZ()->Draw();
		}

		// Draw Cameras
		Texture2D* CameraTexture = Texture2D::GetAsset("editor_camera");
		auto AllCameras = CameraObject::GetAllNamedAssets();
		for (auto camera = AllCameras.begin(); camera != AllCameras.end(); camera++)
		{
			billboard->m_property_model.SetPropertyMatrix(camera->second->m_transform.getWorldModel(), true);

			if (LightTexture)
				CameraTexture->Bind(TextureLevel::LEVEL0);
			else
				Debug.GetNullTexture()->Bind(TextureLevel::LEVEL0);

			Geometry.GetQuadZ()->Draw();
		}

		// Draw Debug Lines

		auto lines = Material::GetAsset("lines");
		lines->BindProgram();
		lines->m_property_viewport.SetProperty(Window.GetViewport());

		lines->SetProperty<bool>("passthrough", false);

		Graphics::SetDepthWrite(false);
		Graphics::SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);

		Debug.RenderWorldLines();

		Graphics::SetDepthPassRule(DepthPassRule::GREATER);
		// ???

		Graphics::SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);

		lines->SetProperty<bool>("passthrough", true);

		Debug.RenderScreenLines();

		Graphics::SetDepthWrite(true);



		// Draw Debug Wireframe Sphere
		auto passthrough = Material::GetAsset("passthroughWorld");
		passthrough->BindProgram();
		passthrough->m_property_useTexture.SetProperty(false);
		passthrough->m_property_useModel.SetProperty(true);
		
		Graphics::SetWireframeState(true);
		Graphics::SetCullMode(CullMode::NO_CULL);

		Graphics::SetLineWidth(5.0f);
		for (const auto& sphere : Debug.m_wireframeSpheres)
		{
			passthrough->m_property_color.SetProperty(sphere.color.getRGB());
			passthrough->m_property_model.SetPropertyMatrix(sphere.model, true);
			Geometry.GetIcoSphere()->Draw();
		}
		Graphics::SetLineWidth(1.0f);

		Graphics::SetWireframeState(false);
		Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

	}

	void RenderManager::RenderEditorObjectsPostDepth()
	{
		// Draw Editor Arrows (if selection applies)
		if (Editor.HasSelection())
		{
			auto simpleLight = Material::GetAsset("simpleLight");
			simpleLight->BindProgram();

			simpleLight->m_property_useModel.SetProperty(true);
			simpleLight->m_property_model.SetPropertyMatrix(Editor.GetSelectionTransform().Model, true);

			// Movement //
			if (Editor.m_controlMode == Editor.ControlMode::TRANSLATE)
			{
				// X Axis //
				if (Editor.m_controlAxis == Axis::X)
					if (Editor.m_controlAxisClicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::RED);

				Geometry.GetArrowX()->Draw();

				// Y Axis //
				if (Editor.m_controlAxis == Axis::Y)
					if (Editor.m_controlAxisClicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::GREEN);

				Geometry.GetArrowY()->Draw();

				// Z Axis //
				if (Editor.m_controlAxis == Axis::Z)
					if (Editor.m_controlAxisClicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::BLUE);

				Geometry.GetArrowZ()->Draw();

				// Small cube in the middle //
				simpleLight->m_property_color.SetProperty(Color3F::WHITE);
				simpleLight->m_property_alpha.SetProperty(0.85f);
				Geometry.GetCubeSmall()->Draw();

				Graphics::SetCullMode(CullMode::NO_CULL);

				// X Plane
				simpleLight->m_property_model.SetPropertyMatrix(Editor.GetAxisSelectionTransform().X_Model, true);
				if (Editor.m_controlPlane == Axis::X)
					if (Editor.m_controlAxisClicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::RED);
				Geometry.GetHalfQuadX()->Draw();
				
				// Y Plane
				simpleLight->m_property_model.SetPropertyMatrix(Editor.GetAxisSelectionTransform().Y_Model, true);
				if (Editor.m_controlPlane == Axis::Y)
					if (Editor.m_controlAxisClicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::GREEN);
				Geometry.GetHalfQuadY()->Draw();

				// Z Plane
				simpleLight->m_property_model.SetPropertyMatrix(Editor.GetAxisSelectionTransform().Z_Model, true);
				if (Editor.m_controlPlane == Axis::Z)
					if (Editor.m_controlAxisClicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::BLUE);
				Geometry.GetHalfQuadZ()->Draw();

				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

				// revert
				simpleLight->m_property_alpha.SetProperty(1.0f);
			}
			// TEST //


		}
	}
}
