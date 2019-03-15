// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderManager.h"

#include "Entity.h"
#include "Scene.h"
#include "Layer.h"
#include "Material.h"
#include "../math/Camera.h"
#include "../opengl/Texture.h"
#include "../imgui/imgui.h"
#include "../window/window.h"
#include "../opengl/Debug.h"
#include "../opengl/Geometry.h"

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

	void RenderManager::AddEntity(Entity* _entity)
	{
		// Makes sure entity has a material/shader
		assert(_entity && _entity->GetMaterial());

		// Check duplicate
		auto Key = std::make_pair(_entity->GetMaterialOrder(), _entity->GetMaterial());
		if (m_entities.find(Key) != m_entities.end())
		{
			if (m_entities[Key].find(_entity) != m_entities[Key].end())
			{
				// Duplicate found, don't add anything
				return;
			}
		}

		m_entities[Key].insert(_entity);
		m_allEntities.push_back(_entity);
	}
	void RenderManager::RemoveEntity(Entity* _entity)
	{
		// Makes sure entity has a material/shader
		assert(_entity && _entity->GetMaterial());
		m_entities[std::make_pair(_entity->GetMaterialOrder(), _entity->GetMaterial())].erase(_entity);
		m_allEntities.erase(std::remove(m_allEntities.begin(), m_allEntities.end(), _entity), m_allEntities.end());
	}

	// Reload Shader System
	void RenderManager::ReloadShaders()
	{
		Shader::ShaderErrorLog.clear();
		Shader::ShaderErrorLogSize = 0;
		ShaderProgram::ProgramsFailed.clear();
		ShaderProgram::ProgramsFailedSize = 0;

		auto Shaders = Shader::m_database.Get();
		for (auto Shader : Shaders)
			Shader.second->reload();

		auto Programs = ShaderProgram::m_database.Get();;
		for (auto Program : Programs)
			Program.second->reload();

		// Materials are part of Shaders
		auto Materials = Material::m_database.Get();
		for (auto Material : Materials)
			Material.second->ReloadPackages();

		// Entity Materials are part of Shaders
		auto Entities = Entity::m_database.Get();
		for (auto Entity : Entities)
			Entity->m_material.UpdateMaterialPackages();
	}
	void RenderManager::ReloadWindow()
	{
		Window.Reload();
		Geometry.Reload();

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
		// Delete Cameras
		Camera::m_database.DeleteAndClear();
		
		// Delete Materials and Entities
		Material::m_database.DeleteAndClear();
		Entity::m_database.DeleteAndClear();

		// Clear Shader Error Log
		Shader::ShaderErrorLog.clear();
		Shader::ShaderErrorLogSize = 0;
		ShaderProgram::ProgramsFailed.clear();
		ShaderProgram::ProgramsFailedSize = 0;
		// Delete Shaders
		Shader::m_database.DeleteAndClear();
		ShaderProgram::m_database.DeleteAndClear();

		// Delete Textures
		Texture::m_database.DeleteAndClear();
		Cubemap::m_database.DeleteAndClear();

		// Clear Render List
		m_entities.clear();

	}
	void RenderManager::Update()
	{
		m_currentScene->Update();
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

	void RenderManager::RenderScene_ByMaterial()
	{
		for (auto mat = m_entities.begin(); mat != m_entities.end(); mat++)
		{
			Material* _base = mat->first.second;
			_base->Bind();

			std::set<Entity*> _entites = mat->second;

			for (auto ent : _entites)
			{
				if(ent->IsFamilyActive())
					ent->Draw();
			}
		}
	}
}
