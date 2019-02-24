// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderManager.h"

#include "Entity.h"
#include "Scene.h"
#include "Layer.h"
#include "Material.h"
#include "../math/Camera.h"
#include "../opengl/Texture.h"
#include "../imgui/imgui.h"

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
		assert(_entity && _entity->GetMaterial());
		m_entities[std::make_pair(_entity->GetMaterialOrder(), _entity->GetMaterial())].insert(_entity);
	}
	void RenderManager::RemoveEntity(Entity* _entity)
	{
		assert(_entity && _entity->GetMaterial());
		m_entities[std::make_pair(_entity->GetMaterialOrder(), _entity->GetMaterial())].erase(_entity);
	}

	// Reload Shader System
	void RenderManager::ReloadShaders()
	{
		Shader::ShaderErrorLog.clear();
		Shader::ShaderErrorLogSize = 0;

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

	// Behaviour
	void RenderManager::Reload()
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

	void RenderManager::RenderScene()
	{
		for (auto mat = m_entities.begin(); mat != m_entities.end(); mat++)
		{
			Material* _base = mat->first.second;
			_base->Bind();

			std::set<Entity*> _entites = mat->second;

			for (auto ent : _entites)
			{
				if(ent->m_isActive)
					ent->Draw();
			}
		}
	}
}
