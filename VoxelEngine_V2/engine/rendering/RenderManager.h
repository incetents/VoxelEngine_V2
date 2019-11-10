// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <assert.h>
#include <set>
#include <unordered_map>
#include <map>
#include <vector>

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"
#include "../utilities/Types.h"

#define MAX_LAYERS 32

namespace Vxl
{
	class Scene;
	class Layer;
	class Entity;
	class Camera;
	class GuiWindow;

	static class RenderManager : public Singleton<class RenderManager>
	{
		DISALLOW_COPY_AND_ASSIGN(RenderManager);
		friend class Hierarchy;
	private:
		Scene* m_currentScene = nullptr;
	private:

		// [True = FullQuad] or [False = Fulltriangle]
		bool m_FSQMode = false;

		// Associate Materials with rendering sequence
		std::map<uint32_t, MaterialIndex> m_materialSequence;
		// Associate entities to materials
		std::map<MaterialIndex, std::vector<Entity*>> m_renderlist_opaque;
		std::map<MaterialIndex, std::vector<Entity*>> m_renderlist_transparent;

		// Imgui render window
		std::vector<GuiWindow*> m_guiWindows;

	public:
		RenderManager();

		// Main Camera
		CameraIndex m_mainCamera = -1;

		// Utility
		void sortEntities();
		void render(MaterialIndex _material, const std::vector<Entity*>& _entities);
		void renderOpaque();
		void renderTransparent();

		// Utility
		void SetNewScene(Scene* _scene);
		//const Layer& GetLayer(uint32_t index);

		void UpdateAllWindowAspectCameras();

		// Reload Systems
		void ReloadShaders();
		void ReloadWindow();
		void ReloadFBOS();

		// Behaviour
		void Update();
		void UpdateFixed();
		void Draw();
		void InitImGui();
		void DrawImGui();
		// Special Resources allocated untraditionally
		void InitGlobalGLResources();
		void DestroyGlobalGLResources();
		void DestroySceneGLResources();

		// Render
		void RenderFullScreen();
		//	void RenderSceneGameObjects_Opaque();
		//	void RenderSceneGameObjects_Transparent();
		//	void RenderSceneObjectsWithOnlyColorID();
		//	void RenderEditorObjects();
		//	void RenderEditorObjectsPostDepth();

	} SingletonInstance(RenderManager);
}
