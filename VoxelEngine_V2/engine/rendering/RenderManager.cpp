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
#include "../rendering/Shader.h"

#include "../objects/GameObject.h"
#include "../objects/LightObject.h"
#include "../objects/Camera.h"
#include "../objects/TextObject.h"

#include "../editorGui/DevConsole.h"
#include "../editorGui/GUIViewport.h"
#include "../editorGui/Hierarchy.h"
#include "../editorGui/Inspector.h"
#include "../editorGui/Performance.h"
#include "../editorGui/ShaderErrors.h"
#include "../editorGui/ShaderCodeViewer.h"

#include "../utilities/Asset.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_colors.h"
#endif

#include <algorithm>

namespace Vxl
{
	RenderManager::RenderManager()
	{
		//	m_layers = new Layer[MAX_LAYERS];
		//	for (UINT i = 0; i < MAX_LAYERS; i++)
		//		m_layers[i].m_id = (1 << i);
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
	//	const Layer& RenderManager::GetLayer(uint32_t index)
	//	{
	//		VXL_ASSERT(index < MAX_LAYERS, "Layer index out of bounds");
	//		return m_layers[index];
	//	}

	void RenderManager::UpdateAllWindowAspectCameras()
	{
		auto AllCameras = Assets::getAllCamera();

		for (auto camera = AllCameras.begin(); camera != AllCameras.end(); camera++)
		{
			if (camera->second->isPerspectiveWindowAspect())
			{
				camera->second->UpdateAspect(Window.GetAspectRatio());
				camera->second->CreatePerspective();
			}
		}
	}

	// Reload Shader System
	void RenderManager::ReloadShaders()
	{
		//	auto& Programs = Assets::getAllShaderProgram();
		//	for (auto& Program : Programs)
		//	{
		//		Program.second->ReloadShaders();
		//	}

		//	auto Materials = Material::GetAllNamedAssets();
		//	for (auto Mat : Materials)
		//		Mat.second->UpdateProperties();
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
		auto fbos = SceneAssets.getAllFramebufferObject();
		for (auto& fbo : fbos)
		{
			if (fbo.second->isFullscreen())
			{
				// Adjust Size based on current window render size [effectively resizing FBO resolution]
				fbo.second->setSizeToViewportSize();
				fbo.second->bind();
				fbo.second->reloadAttachments();
				fbo.second->reloadDepth();
				//
			}
		}
		FramebufferObject::unbind();
	}

	// 
	void RenderManager::InitGlobalGLResources()
	{
		UBOManager.InitGLResources();
		Debug.InitGLResources();
		//GlobalRenderText.InitGLResources();
#ifdef GLOBAL_IMGUI
		GUIViewport.InitGLResources();
#endif

		Gizmo::InitGLResources();
		
		Geometry.InitGLResources();

		GlobalAssets.InitGLResources();
	}
	void RenderManager::DestroyGlobalGLResources()
	{
		UBOManager.DestroyGLResources();
		Debug.DestroyGLResources();
		//GlobalRenderText.DestoryGLResources();

#ifdef GLOBAL_IMGUI
		GUIViewport.DestroyGLResources();
#endif

		Gizmo::DestroyGLResources();

		GlobalAssets.DestroyAndEraseAll();
	}

	//
	void RenderManager::InitSceneGLResources()
	{
		
	}
	void RenderManager::DestroySceneGLResources()
	{
		// Remove selected entity
		Editor.ClearSelection();

		// Delete special
		GPUTimer::DestroyTimers();

		// Destroy all Entities
		//GameObject::DeleteAllAssets();
		//LightObject::DeleteAllAssets();
		//Camera::DeleteAllAssets();

		// Delete Materials
		//Material::DeleteAllAssets();

		//Material::m_masterOrder.clear();
		//Material::m_masterOrderDirty = true;

		// Clear Render List
		//m_allEntities.clear();

		//m_gameObjectsPerMaterial.clear();
		//m_gameObjectsSorted_Opaque.clear();
		//m_gameObjectsSorted_Transparent.clear();

		// Clear Shader Error Log
		//Shader::ShaderErrorLog.clear();
		//Shader::ShaderErrorLogSize = 0;
		//ShaderProgram::ProgramsFailed.clear();
		//ShaderProgram::ProgramsFailedSize = 0;
		// Delete Shaders
		//ShaderProgram::DeleteAllAssets();

		// Delete Textures
		//Texture2D::DeleteAllAssets();
		//Cubemap::DeleteAllAssets();
		//RenderTexture::DeleteAllAssets();

		// Delete extra buffers
		//RenderBuffer::DeleteAllAssets();

		// Delete Framebuffers
		//FramebufferObject::DeleteAllAssets();

		// Delete Meshes
		//Mesh::DeleteAllAssets();

		// Delete All Scene Assets
		SceneAssets.DestroyAndEraseAll();
	}
	void RenderManager::Update()
	{
		m_currentScene->Update();

		// Update all entities
		//	for (auto it = m_allEntities.begin(); it != m_allEntities.end(); it++)
		//		(*it)->update();

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
#ifdef GLOBAL_IMGUI
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
#endif
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
			Assets::getMesh(Geometry.GetFullQuad())->Draw();
		else
			Assets::getMesh(Geometry.GetFullTriangle())->Draw();
	}

	void RenderManager::sortEntities()
	{
		// Data
		const auto& materials = Assets::getAllMaterial();
		const auto& entities = Assets::getAllEntity();

		// Material Sequence
		m_materialSequence.clear();
		for (const auto& material : materials)
		{
			m_materialSequence[material.second->getSequenceID()] = material.first;
		}

		// Material/Entity match
		m_renderlist_opaque.clear();
		m_renderlist_transparent.clear();
		
		// Setup Slots
		for (const auto& material : materials)
		{
			if(material.second->m_renderMode == MaterialRenderMode::Opaque)
				m_renderlist_opaque[material.first] = std::vector<Entity*>();
			else
				m_renderlist_transparent[material.first] = std::vector<Entity*>();

		}

		// Fill Slots
		for (const auto& entity : entities)
		{
			_Material* mat = Assets::getMaterial(entity.second->m_material);
			MaterialRenderMode mode = mat->m_renderMode;

			if (mode == MaterialRenderMode::Opaque)
				m_renderlist_opaque[entity.second->m_material].push_back(entity.second);
			else
				m_renderlist_transparent[entity.second->m_material].push_back(entity.second);

		}
	}

	void RenderManager::render(MaterialIndex _material, const std::vector<Entity*>& _entities)
	{
		_Material* material = Assets::getMaterial(_material);
		if (material)
		{
			material->bindProgram();
			material->bindStates();
			material->bindTextures();
			
			for (auto& ent : _entities)
			{
				if (ent->IsFamilyActive())
				{
					Mesh* mesh = Assets::getMesh(ent->m_mesh);
					if (mesh)
					{
						material->bindCommonUniforms(ent);
						mesh->Draw();
					}
				}
			}
		}
	}
	void RenderManager::renderOpaque()
	{
		// Check all materials in order
		for (const auto& data : m_materialSequence)
		{
			MaterialIndex _materialIndex = data.second;
			// Render all associated entities tied to that material
			if (m_renderlist_opaque.find(_materialIndex) != m_renderlist_opaque.end())
			{
				render(_materialIndex, m_renderlist_opaque[_materialIndex]);
			}
		}
	}
	void RenderManager::renderTransparent()
	{
		// Check all materials in order
		for (const auto& data : m_materialSequence)
		{
			MaterialIndex _materialIndex = data.second;
			// Render all associated entities tied to that material
			if (m_renderlist_transparent.find(_materialIndex) != m_renderlist_transparent.end())
			{
				render(_materialIndex, m_renderlist_transparent[_materialIndex]);
			}
		}
	}


	//	void RenderManager::SortGameObjects()
	//	{
	//		// If material order has been changed, re-sort them all
	//		if (Material::m_masterOrderDirty)
	//		{
	//			m_gameObjectsSorted_Opaque.clear();
	//			m_gameObjectsSorted_Transparent.clear();
	//	
	//			for (auto renderSet = m_gameObjectsPerMaterial.begin(); renderSet != m_gameObjectsPerMaterial.end(); renderSet++)
	//			{
	//				switch (renderSet->first->m_renderMode)
	//				{
	//				case MaterialRenderMode::Opaque:
	//					m_gameObjectsSorted_Opaque[renderSet->first->GetOrder()] = std::pair<Material*, std::set<GameObject*>*>(renderSet->first, renderSet->second);
	//					break;
	//	
	//				case MaterialRenderMode::Transparent:
	//					m_gameObjectsSorted_Transparent[renderSet->first->GetOrder()] = std::pair<Material*, std::set<GameObject*>*>(renderSet->first, renderSet->second);
	//					break;
	//				}
	//			}
	//	
	//			Material::m_masterOrderDirty = false;
	//		}
	//	}
	//	void RenderManager::Render(Material* _material, const std::set<GameObject*>& _objects)
	//	{
	//		if (_material->IsValid())
	//		{
	//			VXL_ASSERT(_material, "Material required to render objects");
	//			_material->BindProgram();
	//			_material->BindStates();
	//			_material->BindTextures(); // Only occurs if shared textures is true
	//	
	//			for (auto& ent : _objects)
	//			{
	//				if (ent->IsFamilyActive())
	//					ent->Draw();
	//			}
	//		}
	//	}
	//	
	//	void RenderManager::RenderSceneGameObjects_Opaque()
	//	{
	//		SortGameObjects();
	//	
	//		// Render all Opaque Objects
	//		for (auto renderOrder = m_gameObjectsSorted_Opaque.begin(); renderOrder != m_gameObjectsSorted_Opaque.end(); renderOrder++)
	//		{
	//			Render(renderOrder->second.first, *renderOrder->second.second);
	//		}
	//	}
	//	void RenderManager::RenderSceneGameObjects_Transparent()
	//	{
	//		SortGameObjects();
	//	
	//		// Render all Opaque Objects
	//		for (auto renderOrder = m_gameObjectsSorted_Transparent.begin(); renderOrder != m_gameObjectsSorted_Transparent.end(); renderOrder++)
	//		{
	//			Render(renderOrder->second.first, *renderOrder->second.second);
	//		}
	//	}
	//	
	//	// Effectively non-gameobject entities need to have their own color ID show up in the color ID attachment.
	//	// This function takes care of that without affecting albedo,normals,metallic,smoothness,etc...
	//	void RenderManager::RenderSceneObjectsWithOnlyColorID()
	//	{
	//		// Extra Objects for COLOR ID
	//		auto gbuffer_mat = Material::GetAsset("gbuffer");
	//		if (gbuffer_mat->IsValid())
	//		{
	//			gbuffer_mat->BindProgram();
	//			gbuffer_mat->BindStates();
	//	
	//			// Default
	//			gbuffer_mat->m_property_useModel.SetProperty(true);
	//			gbuffer_mat->m_property_useInstancing.SetProperty(false);
	//			gbuffer_mat->m_property_useTexture.SetProperty(false);
	//			gbuffer_mat->m_property_tint.SetProperty(Color3F(1, 1, 1));
	//			gbuffer_mat->m_property_color.SetProperty(Color3F(1, 1, 1));
	//	
	//			// Render CameraObjects
	//			auto Cameras = Camera::GetAllNamedAssets();
	//			for (auto it = Cameras.begin(); it != Cameras.end(); it++)
	//			{
	//				gbuffer_mat->m_property_model.SetPropertyMatrix(it->second->m_transform.getModel(), true);
	//				gbuffer_mat->m_property_output.SetProperty(it->second->m_colorID);
	//	
	//				Geometry.GetSphere()->Draw();
	//			}
	//			// Render LightObjects
	//			auto Lights = LightObject::GetAllNamedAssets();
	//			for (auto it = Lights.begin(); it != Lights.end(); it++)
	//			{
	//				gbuffer_mat->m_property_model.SetPropertyMatrix(it->second->m_transform.getModel(), true);
	//				gbuffer_mat->m_property_output.SetProperty(it->second->m_colorID);
	//	
	//				Geometry.GetSphere()->Draw();
	//			}
	//		}
	//	}
	//	void RenderManager::RenderEditorObjects()
	//	{
	//		Graphics::SetDepthRead(true);
	//		Graphics::SetDepthWrite(true);
	//	
	//		Graphics::SetBlendState(true);
	//		Graphics::SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);
	//	
	//		// Draw Lights
	//		auto billboard = Material::GetAsset("billboard");
	//		if (billboard->IsValid())
	//		{
	//			billboard->BindProgram();
	//	
	//			auto AllLights = LightObject::GetAllNamedAssets();
	//			for (auto light = AllLights.begin(); light != AllLights.end(); light++)
	//			{
	//				billboard->m_property_model.SetPropertyMatrix(light->second->m_transform.getModel(), true);
	//				
	//				GlobalData.tex_editor_light->Bind(TextureLevel::LEVEL0);
	//	
	//				Geometry.GetQuadZ()->Draw();
	//			}
	//	
	//			// Draw Cameras
	//			auto AllCameras = Camera::GetAllNamedAssets();
	//			for (auto camera = AllCameras.begin(); camera != AllCameras.end(); camera++)
	//			{
	//				billboard->m_property_model.SetPropertyMatrix(camera->second->m_transform.getModel(), true);
	//				
	//				GlobalData.tex_editor_camera->Bind(TextureLevel::LEVEL0);
	//	
	//				Geometry.GetQuadZ()->Draw();
	//			}
	//		}
	//	
	//		// Draw Debug Lines
	//		auto lines = Material::GetAsset("lines");
	//		if (lines->IsValid())
	//		{
	//			lines->BindProgram();
	//			lines->m_property_viewport.SetProperty(Window.GetViewport());
	//	
	//			lines->SetProperty<bool>("passthrough", false);
	//	
	//			Graphics::SetDepthWrite(false);
	//			Graphics::SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);
	//	
	//			Debug.RenderWorldLines();
	//	
	//			Graphics::SetDepthPassRule(DepthPassRule::GREATER);
	//			// ???
	//	
	//			Graphics::SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);
	//	
	//			lines->SetProperty<bool>("passthrough", true);
	//	
	//			Debug.RenderScreenLines();
	//	
	//			Graphics::SetDepthWrite(true);
	//		}
	//	
	//		// Draw Cubes
	//		Graphics::SetCullMode(CullMode::NO_CULL);
	//		auto gbuffer = Material::GetAsset("gbuffer");
	//		if(gbuffer->IsValid())
	//		{
	//			gbuffer->BindProgram();
	//	
	//			gbuffer->m_property_useTexture.SetProperty(false);
	//			gbuffer->m_property_useModel.SetProperty(true);
	//			gbuffer->m_property_useInstancing.SetProperty(false);
	//	
	//			for (const auto& cube : Debug.m_cubes)
	//			{
	//				gbuffer->m_property_color.SetProperty(cube.m_color.getRGB());
	//				gbuffer->m_property_alpha.SetProperty(cube.m_color.a);
	//				gbuffer->m_property_model.SetPropertyMatrix(cube.m_model, true);
	//	
	//				Geometry.GetCube()->Draw();
	//			}
	//		}
	//		Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
	//	
	//		// Draw Debug Wireframe Sphere
	//		auto passthrough = Material::GetAsset("transparent_passthroughWorld");
	//		if (passthrough->IsValid())
	//		{
	//			passthrough->BindProgram();
	//	
	//			passthrough->m_property_useTexture.SetProperty(false);
	//			passthrough->m_property_useModel.SetProperty(true);
	//			passthrough->m_property_useInstancing.SetProperty(false);
	//	
	//			Graphics::SetWireframeState(true);
	//			Graphics::SetCullMode(CullMode::NO_CULL);
	//			Graphics::SetDepthWrite(false);
	//			Graphics::SetBlendMode(BlendSource::ONE, BlendDestination::ONE);
	//	
	//			Graphics::SetLineWidth(3.0f);
	//			for (const auto& sphere : Debug.m_wireframeSpheres)
	//			{
	//				passthrough->m_property_color.SetProperty(sphere.m_color.getRGB());
	//				passthrough->m_property_alpha.SetProperty(sphere.m_color.a);
	//				passthrough->m_property_model.SetPropertyMatrix(sphere.m_model, true);
	//				Geometry.GetIcoSphere()->Draw();
	//			}
	//			Graphics::SetLineWidth(1.0f);
	//	
	//			Graphics::SetWireframeState(false);
	//			Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
	//			Graphics::SetDepthWrite(true);
	//			Graphics::SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);
	//		}
	//	
	//	}
	//	void RenderManager::RenderEditorObjectsPostDepth()
	//	{
	//		// Draw Debug Lines
	//		auto lines = Material::GetAsset("lines");
	//		if (lines->IsValid())
	//		{
	//			lines->BindProgram();
	//			lines->m_property_viewport.SetProperty(Window.GetViewport());
	//	
	//			lines->SetProperty<bool>("passthrough", false);
	//	
	//			Graphics::SetDepthWrite(false);
	//	
	//			Debug.RenderWorldLinesNoDepth();
	//	
	//			Graphics::SetDepthWrite(true);
	//		}
	//	}
}
