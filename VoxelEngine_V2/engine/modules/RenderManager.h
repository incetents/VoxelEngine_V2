// Copyright (c) 2018 Emmanuel Lajeunesse
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
	class MaterialBase;

	static class RenderManager : public Singleton<class RenderManager>
	{
	private:
		Scene* m_currentScene = nullptr;

		Layer* m_layers;
		UINT m_layerToRender = ~0;

		// Rendering stuff // UINT = Material Order
		std::map<std::pair<UINT, MaterialBase*>, std::set<Entity*>> m_entities; 
		
	public:
		RenderManager();

		// Utility
		void SetNewScene(Scene* _scene);
		const Layer& GetLayer(UINT index);

		void AddEntity(Entity* _entity);
		void RemoveEntity(Entity* _entity);

		// Behaviour
		void Reload();
		void Destroy();
		void Update();
		void Draw();

		void RenderScene();

	} SingletonInstance(RenderManager);
}
