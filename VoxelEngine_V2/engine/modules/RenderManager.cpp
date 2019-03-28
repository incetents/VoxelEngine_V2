// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderManager.h"

#include "Scene.h"
#include "Layer.h"
#include "Material.h"
#include "../utilities/Util.h"
#include "../utilities/Time.h"
#include "../imgui/imgui.h"
#include "../opengl/Texture.h"
#include "../opengl/Debug.h"
#include "../opengl/Geometry.h"
#include "../opengl/Mesh.h"
#include "../opengl/FramebufferObject.h"
#include "../window/window.h"
#include "../window/Hierarchy.h"
#include "../objects/GameObject.h"
#include "../objects/LightObject.h"
#include "../objects/CameraObject.h"

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
		assert(index < MAX_LAYERS);
		return m_layers[index];
	}

	void RenderManager::UpdateAllWindowAspectCameras()
	{
		for (auto Camera : m_allCameraObjects)
		{
			if (Camera->isPerspectiveWindowAspect())
			{
				Camera->UpdateAspect(Window.GetAspectRatio());
				Camera->CreatePerspective();
			}
		}
	}

	void RenderManager::AddEntity(Entity* _entity)
	{
		assert(_entity);

		switch (_entity->GetType())
		{
			case EntityType::GAMEOBJECT:
			{
				auto _GameObject = (GameObject*)_entity;

				// Check if entity already exists
				if (Util::IsInVector(m_allGameObjects, _GameObject))
					return;

				// Key
				auto Key = _GameObject->GetMaterialOrder();

				// New Material = New Set
				if (m_gbufferObjects.find(Key) == m_gbufferObjects.end())
					m_gbufferObjects[Key] = std::make_pair(_GameObject->GetMaterial(), std::set<GameObject*>());

				// Add
				m_gbufferObjects[Key].second.insert(_GameObject);
				m_allGameObjects.push_back(_GameObject);
				m_allEntities.push_back(_entity);

				break;
			}
			case EntityType::CAMERA:
			{
				auto _Camera = (CameraObject*)_entity;

				// Check if entity already exists
				if (Util::IsInVector(m_allCameraObjects, _Camera))
					return;

				// Add
				m_allCameraObjects.push_back(_Camera);
				m_allEntities.push_back(_entity);

				break;
			}
		}
	}

	void RenderManager::RemoveEntity(Entity* _entity)
	{
		assert(_entity);

		switch (_entity->GetType())
		{
			case EntityType::GAMEOBJECT:
			{
				auto _GameObject = dynamic_cast<GameObject*>(_entity);
				assert(_GameObject);

				// Makes sure entity has a material/shader
				if (_GameObject->GetMaterial())
				{
					// Remove from Material Render
					m_gbufferObjects[_GameObject->GetMaterialOrder()].second.erase(_GameObject);
				}

				// Remove
				Util::RemoveFromVector(m_allGameObjects, _GameObject);
				Util::RemoveFromVector(m_allEntities, _entity);

				break;
			}
			case EntityType::CAMERA:
			{
				auto _Camera = dynamic_cast<CameraObject*>(_entity);
				assert(_Camera);

				// Remove
				Util::RemoveFromVector(m_allCameraObjects, _Camera);
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

		auto Shaders = Shader::GetDatabase();
		for (auto Shader : Shaders)
			Shader.second->reload();

		auto Programs = ShaderProgram::GetDatabase();
		for (auto Program : Programs)
			Program.second->reload();

		// Materials are part of Shaders
		auto Materials = Material::GetDatabase();
		for (auto Material : Materials)
			Material.second->ReloadPackages();

		// Entity Materials are part of Shaders
		auto Entities = Entity::GetDatabase();
		for (auto Entity : Entities)
		{
			if (Entity.second->GetType() == EntityType::GAMEOBJECT)
			{
				GameObject* G_Object = dynamic_cast<GameObject*>(Entity.second);
				G_Object->m_material.UpdateMaterialPackages();
			}
		}
	}
	void RenderManager::ReloadWindow()
	{
		glUtil::initHints(); // important for reset reasons

		Window.Reload();

		ReloadScene();

		Debug.CreateDebugTextures();
	}
	void RenderManager::ReloadFBOS()
	{
		// ???
	}

	// Behaviour
	void RenderManager::ReloadScene()
	{
		Destroy();

		if (m_currentScene)
			m_currentScene->Reload();
	}
	// Destroy all data for next Window context
	void RenderManager::Destroy()
	{
		// Delete Materials and Entities
		Material::DeleteAndClearAll();
		Entity::DeleteAndClearAll(); // Since deleting entities changes

		// Clear Shader Error Log
		Shader::ShaderErrorLog.clear();
		Shader::ShaderErrorLogSize = 0;
		ShaderProgram::ProgramsFailed.clear();
		ShaderProgram::ProgramsFailedSize = 0;
		// Delete Shaders
		Shader::DeleteAndClearAll();
		ShaderProgram::DeleteAndClearAll();

		// Delete Textures
		Texture::DeleteAndClearAll();
		Cubemap::DeleteAndClearAll();

		// Delete Framebuffers
		FramebufferObject::DeleteAndClearAll();

		// Delete Meshes
		Mesh::DeleteAndClearAll();

		// Clear Render List
		m_allEntities.clear();
		m_allGameObjects.clear();
		m_gbufferObjects.clear();

		// Remove selected entity
		Hierarchy._selectedEntity = nullptr;

		// Recreate Geometry Shapes
		Geometry.Reload();

	}
	void RenderManager::Update()
	{
		m_currentScene->Update();

		// Update all entities
		for (auto it = m_allEntities.begin(); it != m_allEntities.end(); it++)
			(*it)->Update();

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
	}
	void RenderManager::DrawImGui()
	{
		ImGui::NewFrame();

		m_currentScene->DrawImGui();

		ImGui::Render();
	}

	void RenderManager::RenderFullScreen()
	{
		if(m_FSQMode)
			Geometry.GetFullQuad()->Draw();
		else
			Geometry.GetFullTriangle()->Draw();
	}

	void RenderManager::RenderSceneGameObjects()
	{
		for (auto renderList = m_gbufferObjects.begin(); renderList != m_gbufferObjects.end(); renderList++)
		{
			Material* _base = renderList->second.first;
			_base->Bind();

			std::set<GameObject*> _entites = renderList->second.second;

			for (auto ent : _entites)
			{
				if(ent->IsFamilyActive())
					ent->Draw();
			}
		}
	}
}
