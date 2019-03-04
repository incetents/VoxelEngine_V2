// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <assert.h>
#include <set>
#include <unordered_map>
#include <map>
#include <vector>

#include "../utilities/singleton.h"

#define MAX_LAYERS 32

namespace Vxl
{
	class Scene;
	class Layer;
	class Entity;
	class Material;

	static class RenderManager : public Singleton<class RenderManager>
	{
	private:
		Scene* m_currentScene = nullptr;

		Layer* m_layers;
		UINT m_layerToRender = ~0;

		// Masterlist of entities
		std::set<Entity*> m_allEntities;

		// Rendering stuff // UINT = Material Order
		std::map<std::pair<UINT, Material*>, std::set<Entity*>> m_entities; 
		
	public:
		RenderManager();

		// Utility
		void SetNewScene(Scene* _scene);
		const Layer& GetLayer(UINT index);

		void AddEntity(Entity* _entity);
		void RemoveEntity(Entity* _entity);

		// Reload Systems
		void ReloadShaders();
		void ReloadWindow();
		void ReloadFBOS();

		// Behaviour
		void ReloadScene();
		void Destroy();
		void Update();
		void Draw();
		void DrawImGui();

		void RenderScene();

	} SingletonInstance(RenderManager);
}
