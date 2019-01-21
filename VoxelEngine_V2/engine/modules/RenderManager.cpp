// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderManager.h"

#include "Entity.h"
#include "Scene.h"
#include "Layer.h"
#include "Material.h"

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
		assert(_entity && _entity->GetMaterialBase());
		m_entities[std::make_pair(_entity->GetMaterialOrder(), _entity->GetMaterialBase())].insert(_entity);
	}
	void RenderManager::RemoveEntity(Entity* _entity)
	{
		assert(_entity && _entity->GetMaterialBase());
		m_entities[std::make_pair(_entity->GetMaterialOrder(), _entity->GetMaterialBase())].erase(_entity);
	}

	// Behaviour
	void RenderManager::Reload()
	{
		Destroy();

		if (m_currentScene)
			m_currentScene->Reload();
	}
	void RenderManager::Destroy()
	{
		for (auto mat = m_entities.begin(); mat != m_entities.end(); mat++)
		{
			MaterialBase* _base = mat->first.second;

			std::set<Entity*> _entites = mat->second;

			for (auto ent : _entites)
			{
				delete ent;
			}

			delete _base;
		}

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

	void RenderManager::RenderScene()
	{
		for (auto mat = m_entities.begin(); mat != m_entities.end(); mat++)
		{
			MaterialBase* _base = mat->first.second;
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
