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
	class GuiWindow;

	static class RenderManager : public Singleton<class RenderManager>
	{
		friend class Hierarchy;
	private:
	public: // TEMP PUBLIC
		Scene* m_currentScene = nullptr;
	private:

		// [True = FullQuad] or [False = Fulltriangle]
		bool m_FSQMode = false;
		Layer* m_layers;
		uint32_t m_layerToRender = ~0;

		// Main Camera
		CameraObject* m_mainCamera = nullptr;

		// Masterlist of entities
		std::vector<Entity*> m_allEntities;

		// GameObjects per Material

		std::map<Material*, std::set<GameObject*>*> m_gameObjectsPerMaterial;
		// All Materials with their gameobjects, sorted by their OrderID
		std::map<uint32_t, std::pair< Material*, std::set<GameObject*>* >> m_gameObjectsSorted_Opaque;
		std::map<uint32_t, std::pair< Material*, std::set<GameObject*>* >> m_gameObjectsSorted_Transparent;
		void SortGameObjects();
		void Render(Material* _material, const std::set<GameObject*>& _objects);

		// Imgui render window
		std::vector<GuiWindow*> m_guiWindows;

	public:
		RenderManager();

		// Utility
		void SetNewScene(Scene* _scene);
		const Layer& GetLayer(uint32_t index);
		inline const std::vector<Entity*> GetAllEntities(void) const
		{
			return m_allEntities;
		}

		// Camera
		void SetMainCamera(CameraObject* _camera)
		{
			m_mainCamera = _camera;
		}
		CameraObject* GetMainCamera(void) const
		{
			return m_mainCamera;
		}
		void UpdateAllWindowAspectCameras();

		// Entity
		void AddEntity(Entity* _entity);
		void RemoveEntity(Entity* _entity);

		// Reload Systems
		void ReloadShaders();
		void ReloadWindow();
		void ReloadFBOS();

		// Behaviour
		void InitSceneGLResources();
		void DestroySceneGLResources();
		void Update();
		void UpdateFixed();
		void Draw();
		void InitImGui();
		void DrawImGui();
		// Special Resources allocated untraditionally
		void InitGlobalGLResources();
		void DestroyGlobalGLResources();

		// Render
		void RenderFullScreen();
		void RenderSceneGameObjects_Opaque();
		void RenderSceneGameObjects_Transparent();
		void RenderSceneObjectsWithOnlyColorID();
		void RenderEditorObjects();
		void RenderEditorObjectsPostDepth();

	} SingletonInstance(RenderManager);
}
