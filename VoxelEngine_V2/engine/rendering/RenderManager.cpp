// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderManager.h"

#include "Debug.h"
#include "Primitives.h"
#include "Mesh.h"
#include "FramebufferObject.h"
#include "Graphics.h"
#include "RenderBuffer.h"

#include "../modules/Scene.h"
#include "../modules/Layer.h"
#include "../modules/Material.h"

#include "../utilities/Util.h"
#include "../utilities/Time.h"
#include "../utilities/Macros.h"

#include "../textures/Cubemap.h"
#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"

#include "../window/window.h"
#include "../editor/Editor.h"
#include "../rendering/Gizmo.h"
#include "../rendering/Shader.h"

#include "../objects/GameObject.h"
#include "../objects/LightObject.h"
#include "../objects/Camera.h"
#include "../objects/TextObject.h"

#include "../editorGui/GUI_Viewport.h"

#include "../utilities/Asset.h"

#include <algorithm>

namespace Vxl
{
	RenderManager::RenderManager()
	{
		//	m_layers = new Layer[MAX_LAYERS];
		//	for (UINT i = 0; i < MAX_LAYERS; i++)
		//		m_layers[i].m_id = (1 << i);
	}

	// Utility
	void RenderManager::SetNewScene(Scene* _scene)
	{
		if (m_currentScene)
		{
			m_currentScene->Destroy();
			DestroySceneGLResources();
		}

		m_currentScene = _scene;

		if(m_currentScene)
			m_currentScene->Setup();
	}
	//	const Layer& RenderManager::GetLayer(uint32_t index)
	//	{
	//		VXL_ASSERT(index < MAX_LAYERS, "Layer index out of bounds");
	//		return m_layers[index];
	//	}

	void RenderManager::UpdateAllWindowAspectCameras()
	{
		auto AllCameras = Assets::getAllCamera();

		for (auto camera = AllCameras.begin(); camera != AllCameras.end(); camera++)
		{
			if (camera->second->isPerspectiveWindowAspect())
			{
				camera->second->UpdateAspect(Window.GetAspectRatio());
				camera->second->CreatePerspective();
			}
		}
	}

	// Reload Shader System
	void RenderManager::ReloadShaders()
	{
		// Reload Global Shaders
		{
			GlobalAssets.deleteAllShaderProgram();

			const auto& shaderMaterials = GlobalAssets.getAllShaderMaterial();
			for (auto& shaderMaterial : shaderMaterials)
			{
				shaderMaterial.second->reload(false);
			}
		}
		// Reload Scene Shaders
		{
			SceneAssets.deleteAllShaderProgram();

			const auto& shaderMaterials = SceneAssets.getAllShaderMaterial();
			for (auto& shaderMaterial : shaderMaterials)
			{
				shaderMaterial.second->reload(false);
			}
		}
	}
	void RenderManager::ReloadWindow()
	{
		Graphics::initHints(); // Newer version

		DestroyGlobalGLResources();
		DestroySceneGLResources();

		Window.Reload();

		InitGlobalGLResources();

		if (m_currentScene)
			m_currentScene->Reload();
	}
	void RenderManager::ReloadViewportFBOS()
	{
		auto fbos = SceneAssets.getAllFramebufferObject();
		for (auto& fbo : fbos)
		{
			if (fbo.second->isFullscreen())
			{
				// Adjust Size based on current window render size [effectively resizing FBO resolution]
				fbo.second->setSizeToViewportSize();
				fbo.second->bind();
				fbo.second->reloadAttachments();
				fbo.second->reloadDepth();
				//
			}
		}
		FramebufferObject::unbind();
	}

	// 
	void RenderManager::InitGlobalGLResources()
	{
		UBOManager.InitGLResources();
		Debug.InitGLResources();
		//GlobalRenderText.InitGLResources();
#ifdef GLOBAL_IMGUI
		GUI_Viewport.InitGLResources();
#endif

		Gizmo::InitGLResources();
		
		Primitives.InitGLResources();

		GlobalAssets.InitGLResources();
	}
	void RenderManager::DestroyGlobalGLResources()
	{
		UBOManager.DestroyGLResources();
		Debug.DestroyGLResources();
		//GlobalRenderText.DestoryGLResources();

//#ifdef GLOBAL_IMGUI
//		GUI_Viewport.DestroyGLResources();
//#endif

		Gizmo::DestroyGLResources();

		GlobalAssets.DestroyAndEraseAll();
	}

	//
	void RenderManager::DestroySceneGLResources()
	{
		// Remove selected entity
		Editor.ClearSelection();

		// Delete special
		GPUTimer::DestroyTimers();

		// Delete All Scene Assets
		SceneAssets.DestroyAndEraseAll();
	}
	void RenderManager::Update()
	{
		m_currentScene->Update();

		// Update all entities
		//	for (auto it = m_allEntities.begin(); it != m_allEntities.end(); it++)
		//		(*it)->update();

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

	void RenderManager::RenderFullScreen()
	{
#define FULLSCREEN_TRIANGLE

#ifdef FULLSCREEN_TRIANGLE
			Assets::getMesh(Primitives.GetFullTriangle())->Draw();
#else
			Assets::getMesh(Primitives.GetFullQuad())->Draw();
#endif
	
	}

	void RenderManager::sortMaterials()
	{
		if (!m_materialSequenceDirty)
			return;

		m_materialSequenceDirty = false;

		// Data
		const auto& materials = Assets::getAllMaterial();

		// Material Sequence
		m_materialSequence.clear();
		for (const auto& material : materials)
		{
			uint32_t SequenceID = material.second->getSequenceID();
			if (SequenceID == -1)
				continue;

			m_materialSequence[SequenceID] = material.first;
		}
	}

	void RenderManager::sortEntities()
	{
		if (!m_renderlistDirty)
			return;

		m_renderlistDirty = false;

		// Data
		const auto& materials = Assets::getAllMaterial();
		const auto& entities = Assets::getAllEntity();

		// Material/Entity match
		m_renderlist_opaque.clear();
		m_renderlist_transparent.clear();
		
		// Setup Slots
		for (const auto& material : materials)
		{
			if(material.second->m_renderMode == MaterialRenderMode::Opaque)
				m_renderlist_opaque[material.first] = std::vector<Entity*>();
			else
				m_renderlist_transparent[material.first] = std::vector<Entity*>();
		}

		// Fill Slots
		for (const auto& entity : entities)
		{
			if (entity.second->getMaterial() == -1)
				continue;

			Material* mat = Assets::getMaterial(entity.second->m_material);
			MaterialRenderMode mode = mat->m_renderMode;

			if (mode == MaterialRenderMode::Opaque)
				m_renderlist_opaque[entity.second->m_material].push_back(entity.second);
			else
				m_renderlist_transparent[entity.second->m_material].push_back(entity.second);

		}

		// Sort each Material/Entity slots based on their VAO Id to make sure similar meshes render one after another
		for (auto& set : m_renderlist_opaque)
		{
			std::sort(set.second.begin(), set.second.end());
		}
		for (auto& set : m_renderlist_transparent)
		{
			std::sort(set.second.begin(), set.second.end());
		}
	}

	void RenderManager::render(MaterialIndex _material, const std::vector<Entity*>& _entities)
	{
		Material* material = Assets::getMaterial(_material);

		// If material program didn't link, use error material instead
		if (material)
		{
			if (!material->bindCoreProgram())
			{
				material = GlobalAssets.get_MaterialError();
				if (!material->bindCoreProgram())
					VXL_ERROR("Material used for Error doesn't work");
			}

			material->bindStates();

			if(material->m_sharedTextures)
				material->bindTextures();
			
			for (auto& ent : _entities)
			{
				if (ent->IsFamilyActive())
				{
					Mesh* mesh = Assets::getMesh(ent->m_mesh);
					if (mesh)
					{
						if (!material->m_sharedTextures)
							material->bindTextures(ent);

						material->bindCoreProgramCommonUniforms(ent->m_uniqueID);

						mesh->Draw();
					}
				}
			}
		}
	}
	void RenderManager::renderOpaque()
	{
		// Check all materials in order
		for (const auto& data : m_materialSequence)
		{
			MaterialIndex _materialIndex = data.second;
			// Render all associated entities tied to that material
			if (m_renderlist_opaque.find(_materialIndex) != m_renderlist_opaque.end())
			{
				render(_materialIndex, m_renderlist_opaque[_materialIndex]);
			}
		}
	}
	void RenderManager::renderTransparent()
	{
		// Check all materials in order
		for (const auto& data : m_materialSequence)
		{
			MaterialIndex _materialIndex = data.second;
			// Render all associated entities tied to that material
			if (m_renderlist_transparent.find(_materialIndex) != m_renderlist_transparent.end())
			{
				render(_materialIndex, m_renderlist_transparent[_materialIndex]);
			}
		}
	}


	//	void RenderManager::SortGameObjects()
	//	{
	//		// If material order has been changed, re-sort them all
	//		if (Material::m_masterOrderDirty)
	//		{
	//			m_gameObjectsSorted_Opaque.clear();
	//			m_gameObjectsSorted_Transparent.clear();
	//	
	//			for (auto renderSet = m_gameObjectsPerMaterial.begin(); renderSet != m_gameObjectsPerMaterial.end(); renderSet++)
	//			{
	//				switch (renderSet->first->m_renderMode)
	//				{
	//				case MaterialRenderMode::Opaque:
	//					m_gameObjectsSorted_Opaque[renderSet->first->GetOrder()] = std::pair<Material*, std::set<GameObject*>*>(renderSet->first, renderSet->second);
	//					break;
	//	
	//				case MaterialRenderMode::Transparent:
	//					m_gameObjectsSorted_Transparent[renderSet->first->GetOrder()] = std::pair<Material*, std::set<GameObject*>*>(renderSet->first, renderSet->second);
	//					break;
	//				}
	//			}
	//	
	//			Material::m_masterOrderDirty = false;
	//		}
	//	}
	//	void RenderManager::Render(Material* _material, const std::set<GameObject*>& _objects)
	//	{
	//		if (_material->IsValid())
	//		{
	//			VXL_ASSERT(_material, "Material required to render objects");
	//			_material->BindProgram();
	//			_material->BindStates();
	//			_material->BindTextures(); // Only occurs if shared textures is true
	//	
	//			for (auto& ent : _objects)
	//			{
	//				if (ent->IsFamilyActive())
	//					ent->Draw();
	//			}
	//		}
	//	}
	//	
	//	void RenderManager::RenderSceneGameObjects_Opaque()
	//	{
	//		SortGameObjects();
	//	
	//		// Render all Opaque Objects
	//		for (auto renderOrder = m_gameObjectsSorted_Opaque.begin(); renderOrder != m_gameObjectsSorted_Opaque.end(); renderOrder++)
	//		{
	//			Render(renderOrder->second.first, *renderOrder->second.second);
	//		}
	//	}
	//	void RenderManager::RenderSceneGameObjects_Transparent()
	//	{
	//		SortGameObjects();
	//	
	//		// Render all Opaque Objects
	//		for (auto renderOrder = m_gameObjectsSorted_Transparent.begin(); renderOrder != m_gameObjectsSorted_Transparent.end(); renderOrder++)
	//		{
	//			Render(renderOrder->second.first, *renderOrder->second.second);
	//		}
	//	}
	//	
	//	// Effectively non-gameobject entities need to have their own color ID show up in the color ID attachment.
	//	// This function takes care of that without affecting albedo,normals,metallic,smoothness,etc...
	//	void RenderManager::RenderSceneObjectsWithOnlyColorID()
	//	{
	//		// Extra Objects for COLOR ID
	//		auto gbuffer_mat = Material::GetAsset("gbuffer");
	//		if (gbuffer_mat->IsValid())
	//		{
	//			gbuffer_mat->BindProgram();
	//			gbuffer_mat->BindStates();
	//	
	//			// Default
	//			gbuffer_mat->m_property_useModel.SetProperty(true);
	//			gbuffer_mat->m_property_useInstancing.SetProperty(false);
	//			gbuffer_mat->m_property_useTexture.SetProperty(false);
	//			gbuffer_mat->m_property_tint.SetProperty(Color3F(1, 1, 1));
	//			gbuffer_mat->m_property_color.SetProperty(Color3F(1, 1, 1));
	//	
	//			// Render CameraObjects
	//			auto Cameras = Camera::GetAllNamedAssets();
	//			for (auto it = Cameras.begin(); it != Cameras.end(); it++)
	//			{
	//				gbuffer_mat->m_property_model.SetPropertyMatrix(it->second->m_transform.getModel(), true);
	//				gbuffer_mat->m_property_output.SetProperty(it->second->m_colorID);
	//	
	//				Primitives.GetSphere()->Draw();
	//			}
	//			// Render LightObjects
	//			auto Lights = LightObject::GetAllNamedAssets();
	//			for (auto it = Lights.begin(); it != Lights.end(); it++)
	//			{
	//				gbuffer_mat->m_property_model.SetPropertyMatrix(it->second->m_transform.getModel(), true);
	//				gbuffer_mat->m_property_output.SetProperty(it->second->m_colorID);
	//	
	//				Primitives.GetSphere()->Draw();
	//			}
	//		}
	//	}
	//	void RenderManager::RenderEditorObjects()
	//	{
	//		Graphics::SetDepthRead(true);
	//		Graphics::SetDepthWrite(true);
	//	
	//		Graphics::SetBlendState(true);
	//		Graphics::SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);
	//	
	//		// Draw Lights
	//		auto billboard = Material::GetAsset("billboard");
	//		if (billboard->IsValid())
	//		{
	//			billboard->BindProgram();
	//	
	//			auto AllLights = LightObject::GetAllNamedAssets();
	//			for (auto light = AllLights.begin(); light != AllLights.end(); light++)
	//			{
	//				billboard->m_property_model.SetPropertyMatrix(light->second->m_transform.getModel(), true);
	//				
	//				GlobalData.tex_editor_light->bind(TextureLevel::LEVEL0);
	//	
	//				Primitives.GetQuadZ()->Draw();
	//			}
	//	
	//			// Draw Cameras
	//			auto AllCameras = Camera::GetAllNamedAssets();
	//			for (auto camera = AllCameras.begin(); camera != AllCameras.end(); camera++)
	//			{
	//				billboard->m_property_model.SetPropertyMatrix(camera->second->m_transform.getModel(), true);
	//				
	//				GlobalData.tex_editor_camera->bind(TextureLevel::LEVEL0);
	//	
	//				Primitives.GetQuadZ()->Draw();
	//			}
	//		}
	//	
	//		// Draw Debug Lines
	//		auto lines = Material::GetAsset("lines");
	//		if (lines->IsValid())
	//		{
	//			lines->BindProgram();
	//			lines->m_property_viewport.SetProperty(Window.GetViewport());
	//	
	//			lines->SetProperty<bool>("passthrough", false);
	//	
	//			Graphics::SetDepthWrite(false);
	//			Graphics::SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);
	//	
	//			Debug.RenderWorldLines();
	//	
	//			Graphics::SetDepthPassRule(DepthPassRule::GREATER);
	//			// ???
	//	
	//			Graphics::SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);
	//	
	//			lines->SetProperty<bool>("passthrough", true);
	//	
	//			Debug.RenderScreenLines();
	//	
	//			Graphics::SetDepthWrite(true);
	//		}
	//	
	//		// Draw Cubes
	//		Graphics::SetCullMode(CullMode::NO_CULL);
	//		auto gbuffer = Material::GetAsset("gbuffer");
	//		if(gbuffer->IsValid())
	//		{
	//			gbuffer->BindProgram();
	//	
	//			gbuffer->m_property_useTexture.SetProperty(false);
	//			gbuffer->m_property_useModel.SetProperty(true);
	//			gbuffer->m_property_useInstancing.SetProperty(false);
	//	
	//			for (const auto& cube : Debug.m_cubes)
	//			{
	//				gbuffer->m_property_color.SetProperty(cube.m_color.getRGB());
	//				gbuffer->m_property_alpha.SetProperty(cube.m_color.a);
	//				gbuffer->m_property_model.SetPropertyMatrix(cube.m_model, true);
	//	
	//				Primitives.GetCube()->Draw();
	//			}
	//		}
	//		Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
	//	
	//		// Draw Debug Wireframe Sphere
	//		auto passthrough = Material::GetAsset("transparent_passthroughWorld");
	//		if (passthrough->IsValid())
	//		{
	//			passthrough->BindProgram();
	//	
	//			passthrough->m_property_useTexture.SetProperty(false);
	//			passthrough->m_property_useModel.SetProperty(true);
	//			passthrough->m_property_useInstancing.SetProperty(false);
	//	
	//			Graphics::SetWireframeState(true);
	//			Graphics::SetCullMode(CullMode::NO_CULL);
	//			Graphics::SetDepthWrite(false);
	//			Graphics::SetBlendMode(BlendSource::ONE, BlendDestination::ONE);
	//	
	//			Graphics::SetLineWidth(3.0f);
	//			for (const auto& sphere : Debug.m_wireframeSpheres)
	//			{
	//				passthrough->m_property_color.SetProperty(sphere.m_color.getRGB());
	//				passthrough->m_property_alpha.SetProperty(sphere.m_color.a);
	//				passthrough->m_property_model.SetPropertyMatrix(sphere.m_model, true);
	//				Primitives.GetIcoSphere()->Draw();
	//			}
	//			Graphics::SetLineWidth(1.0f);
	//	
	//			Graphics::SetWireframeState(false);
	//			Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
	//			Graphics::SetDepthWrite(true);
	//			Graphics::SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);
	//		}
	//	
	//	}
	//	void RenderManager::RenderEditorObjectsPostDepth()
	//	{
	//		// Draw Debug Lines
	//		auto lines = Material::GetAsset("lines");
	//		if (lines->IsValid())
	//		{
	//			lines->BindProgram();
	//			lines->m_property_viewport.SetProperty(Window.GetViewport());
	//	
	//			lines->SetProperty<bool>("passthrough", false);
	//	
	//			Graphics::SetDepthWrite(false);
	//	
	//			Debug.RenderWorldLinesNoDepth();
	//	
	//			Graphics::SetDepthWrite(true);
	//		}
	//	}
}
