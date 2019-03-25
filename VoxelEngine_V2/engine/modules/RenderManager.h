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
	class GameObject;
	class CameraObject;
	class LightObject;
	class SkyboxObject;

	static class RenderManager : public Singleton<class RenderManager>
	{
		friend class Hierarchy;
	private:
		Scene* m_currentScene = nullptr;

		// [True = FullQuad] or [False = Fulltriangle]
		bool m_FSQMode = false;
		Layer* m_layers;
		UINT m_layerToRender = ~0;

		// Masterlist of entities
		std::vector<Entity*> m_allEntities;
		// Masterlist of specific entities
		std::vector<GameObject*>	m_allGameObjects;
		std::vector<CameraObject*>	m_allCameraObjects;
		std::vector<LightObject*>	m_allLightObjects;

		// Rendering Gbuffer stuff // UINT = Material Order
		std::map<UINT, std::pair<Material*, std::set<GameObject*>>> m_gbufferObjects;
		
	public:
		RenderManager();

		// Utility
		void SetNewScene(Scene* _scene);
		const Layer& GetLayer(UINT index);
		inline const std::vector<Entity*> GetAllEntities(void) const
		{
			return m_allEntities;
		}

		// Entity
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
		void UpdateFixed();
		void Draw();
		void DrawImGui();

		// Render
		void RenderFullScreen();
		void RenderSceneGameObjects();

	} SingletonInstance(RenderManager);
}
