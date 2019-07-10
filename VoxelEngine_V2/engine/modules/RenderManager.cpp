// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderManager.h"

#include "Scene.h"
#include "Layer.h"
#include "Material.h"
#include "../utilities/Util.h"
#include "../utilities/Time.h"
#include "../utilities/Macros.h"
#include "../textures/Texture.h"
#include "../textures/Cubemap.h"
#include "../rendering/Debug.h"
#include "../rendering/Geometry.h"
#include "../rendering/Mesh.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/Graphics.h"
#include "../window/window.h"
#include "../editor/Editor.h"
#include "../objects/GameObject.h"
#include "../objects/LightObject.h"
#include "../objects/CameraObject.h"

#include "../editor/DevConsole.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
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
			Destroy();
		}

		m_currentScene = _scene;

		m_currentScene->Setup();
	}
	const Layer& RenderManager::GetLayer(UINT index)
	{
		VXL_ASSERT(index < MAX_LAYERS, "Layer index out of bounds");
		return m_layers[index];
	}

	void RenderManager::UpdateAllWindowAspectCameras()
	{
		auto AllCameras = CameraObject::GetDatabase();
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

				// Check if entity already exists
				//if (Util::IsInVector(m_allGameObjects, _GameObject))
				//	return;

				// Material/GameObjects set
				auto key = _GameObject->GetMaterial();

				// Empty Check
				if (m_gameObjectsPerMaterial[key] == nullptr)
					m_gameObjectsPerMaterial[key] = new std::set<GameObject*>();

				m_gameObjectsPerMaterial[key]->insert(_GameObject);

				//m_allGameObjects.push_back(_GameObject);
				m_allEntities.push_back(_entity);

				break;
			}
			case EntityType::CAMERA:
			{
				auto _Camera = (CameraObject*)_entity;

				// Check if entity already exists
				//if (Util::IsInVector(m_allCameraObjects, _Camera))
				//	return;

				// Add
				//m_allCameraObjects.push_back(_Camera);
				m_allEntities.push_back(_entity);

				break;
			}
			case EntityType::LIGHT:
			{
				auto _light = (LightObject*)_entity;

				// Check if entity already exists
				//if (Util::IsInVector(m_allLightObjects, _light))
				//	return;

				// Add
				//m_allLightObjects.push_back(_light);
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

		auto Programs = ShaderProgram::GetDatabase();
		auto Shaders = Shader::GetDatabase();

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

		auto Materials = Material::GetDatabase();
		for (auto Mat : Materials)
			Mat.second->UpdateProperties();
	}
	void RenderManager::ReloadWindow()
	{
		Graphics::initHints(); // Newer version

		Destroy();

		Window.Reload();

		CreateSpecialManagers();

		if (m_currentScene)
			m_currentScene->Reload();
	}
	void RenderManager::ReloadFBOS()
	{
		auto fbos = FramebufferObject::GetDatabase();
		for (auto& fbo : fbos)
		{
			if (fbo.second->m_fullscreen)
			{
				fbo.second->unload();
				// Adjust Size based on current window render size [effectively resizing FBO resolution]
				fbo.second->m_width = Window.GetViewportWidth();
				fbo.second->m_height = Window.GetViewportHeight();
				//
				fbo.second->load();
			}
		}
		FramebufferObject::unbind();
	}

	// Behaviour
	void RenderManager::CreateSpecialManagers()
	{
		UBOManager.Setup();
		Debug.Setup();
		Geometry.Setup();
	}
	// Destroy all data for next Window context
	void RenderManager::Destroy()
	{
		// Remove selected entity
		Editor.ClearSelection();

		// Delete special
		GPUTimer::DestroyTimers();

		// Destroy all Entities
		GameObject::DeleteAndClearAll();
		LightObject::DeleteAndClearAll();
		CameraObject::DeleteAndClearAll();

		// Delete Materials
		Material::DeleteAndClearAll();

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
		ShaderProgram::DeleteAndClearAll();
		Shader::DeleteAndClearAll();

		// Delete Textures
		Texture::DeleteAndClearAll();
		Cubemap::DeleteAndClearAll();

		// Delete Framebuffers
		FramebufferObject::DeleteAndClearAll();

		// Delete Meshes
		Mesh::DeleteAndClearAll();
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
	void RenderManager::DrawImGui()
	{
#ifdef GLOBAL_IMGUI
		ImGui::NewFrame();

		m_currentScene->DrawImGui();

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
			std::set<GameObject*> BENT = *entities;

			for (auto& ent : *entities)
			{
				if (ent->IsFamilyActive())
					ent->Draw();
			}
		}
	}
	void RenderManager::RenderEditorObjects()
	{
		Graphics::SetBlendState(true);
		Graphics::SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);

		// Draw Lights
		auto billboard = Material::Get("billboard");
		billboard->BindProgram();
		
		Texture* LightTexture = Texture::Get("editor_lightbulb");
		auto AllLights = LightObject::GetDatabase();
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
		Texture* CameraTexture = Texture::Get("editor_camera");
		auto AllCameras = CameraObject::GetDatabase();
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
		auto lines = Material::Get("lines");
		lines->BindProgram();
		lines->m_property_viewport.SetProperty(Window.GetViewport());

		lines->SetProperty<bool>("passthrough", false);

		Graphics::SetDepthMask(false);
		Graphics::SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);

		Debug.RenderWorldLines();

		Graphics::SetDepthPassRule(DepthPassRule::GREATER);
		// ???

		Graphics::SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);

		lines->SetProperty<bool>("passthrough", true);

		Debug.RenderScreenLines();

		Graphics::SetDepthMask(true);

		// Draw Debug Wireframe Sphere
		auto passthrough = Material::Get("passthrough");
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
			auto simpleLight = Material::Get("simpleLight");
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
