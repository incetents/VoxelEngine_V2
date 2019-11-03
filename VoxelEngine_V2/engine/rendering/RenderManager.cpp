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
#include "../modules/GlobalData.h"

#include "../utilities/Util.h"
#include "../utilities/Time.h"
#include "../utilities/Macros.h"

#include "../textures/Cubemap.h"
#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"

#include "../window/window.h"
#include "../editor/Editor.h"
#include "../rendering/Gizmo.h"

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
#include "../editorGui/ShaderCodeViewer.h"

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

		for (auto Program : Programs)
		{
			Program.second->ReloadShaders();
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
		Gizmo::InitGLResources();

		GlobalData.InitGLResources();
	}
	void RenderManager::DestroyGlobalGLResources()
	{
		UBOManager.DestroyGLResources();
		Debug.DestroyGLResources();
		GlobalRenderText.DestoryGLResources();
		GUIViewport.DestroyGLResources();
		Gizmo::DestroyGLResources();

		GlobalAssets.DestroyAndEraseAll();
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
		m_gameObjectsSorted_Opaque.clear();
		m_gameObjectsSorted_Transparent.clear();

		// Clear Shader Error Log
		Shader::ShaderErrorLog.clear();
		Shader::ShaderErrorLogSize = 0;
		ShaderProgram::ProgramsFailed.clear();
		ShaderProgram::ProgramsFailedSize = 0;
		// Delete Shaders
		ShaderProgram::DeleteAllAssets();

		// Delete Textures
		//Texture2D::DeleteAllAssets();
		//Cubemap::DeleteAllAssets();
		//RenderTexture::DeleteAllAssets();

		// Delete extra buffers
		//RenderBuffer::DeleteAllAssets();

		// Delete Framebuffers
		FramebufferObject::DeleteAllAssets();

		// Delete Meshes
		Mesh::DeleteAllAssets();

		// Delete All Scene Assets
		SceneAssets.DestroyAndEraseAll();
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
		GUIViewport.SetOpen(false);
		ShaderCodeViewer.Init("ShaderCodeViewer", Vector2(700, 400), 0.9f);
		ShaderCodeViewer.SetOpen(false);

		m_guiWindows.push_back(&DevConsole.instanceRef);
		m_guiWindows.push_back(&ShaderErrors.instanceRef);
		m_guiWindows.push_back(&Inspector.instanceRef);
		m_guiWindows.push_back(&Hierarchy.instanceRef);
		m_guiWindows.push_back(&Performance.instanceRef);
		m_guiWindows.push_back(&GUIViewport.instanceRef);
		m_guiWindows.push_back(&ShaderCodeViewer.instanceRef);
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
				for (GuiWindow* window : m_guiWindows)
				{
					if (ImGui::MenuItem(window->GetName().c_str(), "", window->IsOpen()))
					{ 
						window->ToggleOpen();
					}
				}


				//	if (ImGui::MenuItem(DevConsole.GetName().c_str(), "", DevConsole.IsOpen()))
				//	{ 
				//		DevConsole.ToggleOpen();
				//	}
				//	if (ImGui::MenuItem(Inspector.GetName().c_str(), "", Inspector.IsOpen()))
				//	{
				//		Inspector.ToggleOpen();
				//	}
				//	if (ImGui::MenuItem(Hierarchy.GetName().c_str(), "", Hierarchy.IsOpen()))
				//	{
				//		Hierarchy.ToggleOpen();
				//	}
				//	if (ImGui::MenuItem(Performance.GetName().c_str(), "", Performance.IsOpen()))
				//	{
				//		Performance.ToggleOpen();
				//	}
				//	if (ImGui::MenuItem(GUIViewport.GetName().c_str(), "", GUIViewport.IsOpen()))
				//	{
				//		GUIViewport.ToggleOpen();
				//	}
				//	if (ImGui::MenuItem(ShaderCodeViewer.GetName().c_str(), "", ShaderCodeViewer.IsOpen()))
				//	{
				//		ShaderCodeViewer.ToggleOpen();
				//	}

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

	void RenderManager::SortGameObjects()
	{
		// If material order has been changed, re-sort them all
		if (Material::m_masterOrderDirty)
		{
			m_gameObjectsSorted_Opaque.clear();
			m_gameObjectsSorted_Transparent.clear();

			for (auto renderSet = m_gameObjectsPerMaterial.begin(); renderSet != m_gameObjectsPerMaterial.end(); renderSet++)
			{
				switch (renderSet->first->m_renderMode)
				{
				case MaterialRenderMode::Opaque:
					m_gameObjectsSorted_Opaque[renderSet->first->GetOrder()] = std::pair<Material*, std::set<GameObject*>*>(renderSet->first, renderSet->second);
					break;

				case MaterialRenderMode::Transparent:
					m_gameObjectsSorted_Transparent[renderSet->first->GetOrder()] = std::pair<Material*, std::set<GameObject*>*>(renderSet->first, renderSet->second);
					break;
				}
			}

			Material::m_masterOrderDirty = false;
		}
	}
	void RenderManager::Render(Material* _material, const std::set<GameObject*>& _objects)
	{
		if (_material->IsValid())
		{
			VXL_ASSERT(_material, "Material required to render objects");
			_material->BindProgram();
			_material->BindStates();
			_material->BindTextures(); // Only occurs if shared textures is true

			for (auto& ent : _objects)
			{
				if (ent->IsFamilyActive())
					ent->Draw();
			}
		}
	}

	void RenderManager::RenderSceneGameObjects_Opaque()
	{
		SortGameObjects();

		// Render all Opaque Objects
		for (auto renderOrder = m_gameObjectsSorted_Opaque.begin(); renderOrder != m_gameObjectsSorted_Opaque.end(); renderOrder++)
		{
			Render(renderOrder->second.first, *renderOrder->second.second);
		}
	}
	void RenderManager::RenderSceneGameObjects_Transparent()
	{
		SortGameObjects();

		auto fbo_gbuffer = FramebufferObject::GetAsset("gbuffer");
		fbo_gbuffer->DisableAttachment(1); // Don't draw normals

		// Render all Opaque Objects
		for (auto renderOrder = m_gameObjectsSorted_Transparent.begin(); renderOrder != m_gameObjectsSorted_Transparent.end(); renderOrder++)
		{
			Render(renderOrder->second.first, *renderOrder->second.second);
		}

		fbo_gbuffer->EnableAttachment(1); // Revert
	}

	// Effectively non-gameobject entities need to have their own color ID show up in the color ID attachment.
	// This function takes care of that without affecting albedo,normals,metallic,smoothness,etc...
	void RenderManager::RenderSceneObjectsWithOnlyColorID()
	{
		// Extra Objects for COLOR ID
		auto gbuffer_mat = Material::GetAsset("gbuffer");
		if (gbuffer_mat->IsValid())
		{
			gbuffer_mat->BindProgram();
			gbuffer_mat->BindStates();

			// Force specific fbo draw buffers
			auto fbo_gbuffer = FramebufferObject::GetAsset("gbuffer");

			// Only render to COLOR ID Texture
			fbo_gbuffer->DisableAttachment(0);
			fbo_gbuffer->DisableAttachment(1);

			// Default
			gbuffer_mat->m_property_useModel.SetProperty(true);
			gbuffer_mat->m_property_useInstancing.SetProperty(false);
			gbuffer_mat->m_property_useTexture.SetProperty(false);
			gbuffer_mat->m_property_tint.SetProperty(Color3F(1, 1, 1));
			gbuffer_mat->m_property_color.SetProperty(Color3F(1, 1, 1));

			// Render CameraObjects
			auto Cameras = CameraObject::GetAllNamedAssets();
			for (auto it = Cameras.begin(); it != Cameras.end(); it++)
			{
				gbuffer_mat->m_property_model.SetPropertyMatrix(it->second->m_transform.getModel(), true);
				gbuffer_mat->m_property_output.SetProperty(it->second->m_colorID);

				Geometry.GetSphere()->Draw();
			}
			// Render LightObjects
			auto Lights = LightObject::GetAllNamedAssets();
			for (auto it = Lights.begin(); it != Lights.end(); it++)
			{
				gbuffer_mat->m_property_model.SetPropertyMatrix(it->second->m_transform.getModel(), true);
				gbuffer_mat->m_property_output.SetProperty(it->second->m_colorID);

				Geometry.GetSphere()->Draw();
			}

			// Fix fbo draw buffers
			fbo_gbuffer->EnableAttachment(0);
			fbo_gbuffer->EnableAttachment(1);
		}
	}
	void RenderManager::RenderEditorObjects()
	{
		Graphics::SetDepthRead(true);
		Graphics::SetDepthWrite(true);

		Graphics::SetBlendState(true);
		Graphics::SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);

		// Draw Lights
		auto billboard = Material::GetAsset("billboard");
		if (billboard->IsValid())
		{
			billboard->BindProgram();

			auto AllLights = LightObject::GetAllNamedAssets();
			for (auto light = AllLights.begin(); light != AllLights.end(); light++)
			{
				billboard->m_property_model.SetPropertyMatrix(light->second->m_transform.getModel(), true);
				
				GlobalData.tex_editor_light->Bind(TextureLevel::LEVEL0);

				Geometry.GetQuadZ()->Draw();
			}

			// Draw Cameras
			auto AllCameras = CameraObject::GetAllNamedAssets();
			for (auto camera = AllCameras.begin(); camera != AllCameras.end(); camera++)
			{
				billboard->m_property_model.SetPropertyMatrix(camera->second->m_transform.getModel(), true);
				
				GlobalData.tex_editor_camera->Bind(TextureLevel::LEVEL0);

				Geometry.GetQuadZ()->Draw();
			}
		}

		// Draw Debug Lines
		auto lines = Material::GetAsset("lines");
		if (lines->IsValid())
		{
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
		}

		// Draw Cubes
		Graphics::SetCullMode(CullMode::NO_CULL);
		auto gbuffer = Material::GetAsset("gbuffer");
		if(gbuffer->IsValid())
		{
			gbuffer->BindProgram();

			gbuffer->m_property_useTexture.SetProperty(false);
			gbuffer->m_property_useModel.SetProperty(true);
			gbuffer->m_property_useInstancing.SetProperty(false);

			for (const auto& cube : Debug.m_cubes)
			{
				gbuffer->m_property_color.SetProperty(cube.m_color.getRGB());
				gbuffer->m_property_alpha.SetProperty(cube.m_color.a);
				gbuffer->m_property_model.SetPropertyMatrix(cube.m_model, true);

				Geometry.GetCube()->Draw();
			}
		}
		Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

		// Draw Debug Wireframe Sphere
		auto passthrough = Material::GetAsset("transparent_passthroughWorld");
		if (passthrough->IsValid())
		{
			passthrough->BindProgram();

			passthrough->m_property_useTexture.SetProperty(false);
			passthrough->m_property_useModel.SetProperty(true);
			passthrough->m_property_useInstancing.SetProperty(false);

			Graphics::SetWireframeState(true);
			Graphics::SetCullMode(CullMode::NO_CULL);
			Graphics::SetDepthWrite(false);
			Graphics::SetBlendMode(BlendSource::ONE, BlendDestination::ONE);

			Graphics::SetLineWidth(3.0f);
			for (const auto& sphere : Debug.m_wireframeSpheres)
			{
				passthrough->m_property_color.SetProperty(sphere.m_color.getRGB());
				passthrough->m_property_alpha.SetProperty(sphere.m_color.a);
				passthrough->m_property_model.SetPropertyMatrix(sphere.m_model, true);
				Geometry.GetIcoSphere()->Draw();
			}
			Graphics::SetLineWidth(1.0f);

			Graphics::SetWireframeState(false);
			Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
			Graphics::SetDepthWrite(true);
			Graphics::SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);
		}

	}
	void RenderManager::RenderEditorObjectsPostDepth()
	{
		// Draw Debug Lines
		auto lines = Material::GetAsset("lines");
		if (lines->IsValid())
		{
			lines->BindProgram();
			lines->m_property_viewport.SetProperty(Window.GetViewport());

			lines->SetProperty<bool>("passthrough", false);

			Graphics::SetDepthWrite(false);

			Debug.RenderWorldLinesNoDepth();

			Graphics::SetDepthWrite(true);
		}
	}
}
