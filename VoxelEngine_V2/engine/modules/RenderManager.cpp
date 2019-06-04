// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderManager.h"

#include "Scene.h"
#include "Layer.h"
#include "Material.h"
#include "../utilities/Util.h"
#include "../utilities/Time.h"
#include "../utilities/GlobalMacros.h"
#include "../textures/Texture.h"
#include "../opengl/Debug.h"
#include "../opengl/Geometry.h"
#include "../opengl/Mesh.h"
#include "../opengl/FramebufferObject.h"
#include "../window/window.h"
#include "../editor/Editor.h"
#include "../objects/GameObject.h"
#include "../objects/LightObject.h"
#include "../objects/CameraObject.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#endif

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
			case EntityType::LIGHT:
			{
				auto _light = (LightObject*)_entity;

				// Check if entity already exists
				if (Util::IsInVector(m_allLightObjects, _light))
					return;

				// Add
				m_allLightObjects.push_back(_light);
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
			case EntityType::LIGHT:
			{
				auto _Light = dynamic_cast<LightObject*>(_entity);
				assert(_Light);

				// Remove
				Util::RemoveFromVector(m_allLightObjects, _Light);
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

		auto Programs = ShaderProgram::GetDatabase();
		auto Shaders = Shader::GetDatabase();

		for (auto Program : Programs)
			Program.second->detachShaders();

		for (auto Shader : Shaders)
			Shader.second->reload();

		for (auto Program : Programs)
		{
			Program.second->destroyProgram();
			Program.second->createProgram();
			Program.second->attachShaders();
			Program.second->Link();
		}

		auto Materials = Material::GetDatabase();
		for (auto Mat : Materials)
			Mat.second->UpdateMaterialPackages();
	}
	void RenderManager::ReloadWindow()
	{
		glUtil::initHints(); // important for reset reasons

		Destroy();

		Window.Reload();

		CreateSpecialManagers();

		if (m_currentScene)
			m_currentScene->Reload();
	}
	void RenderManager::ReloadFBOS()
	{
		// ???
	}

	// Behaviour
	void RenderManager::CreateSpecialManagers()
	{
		UBOManager.Setup();
		Debug.Setup();
		Geometry.Setup();
	}
	// Destroy all data for next Window context
	void RenderManager::Destroy()
	{
		// Remove selected entity
		Editor.ClearSelection();

		// Delete special
		GPUTimer::DestroyTimers();

		// Delete Materials and Entities
		Material::DeleteAndClearAll();
		Entity::DeleteAndClearAll();

		// Clear Shader Error Log
		Shader::ShaderErrorLog.clear();
		Shader::ShaderErrorLogSize = 0;
		ShaderProgram::ProgramsFailed.clear();
		ShaderProgram::ProgramsFailedSize = 0;
		// Delete Shaders
		ShaderProgram::DeleteAndClearAll();
		Shader::DeleteAndClearAll();

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
		Debug.End();
	}
	void RenderManager::DrawImGui()
	{
#ifdef GLOBAL_IMGUI
		ImGui::NewFrame();

		m_currentScene->DrawImGui();

		ImGui::Render();
#endif
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
		// render all gameobjects with their corresponding materials
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
	void RenderManager::RenderEditorObjects()
	{
		glUtil::blendState(true);
		glUtil::blendMode(Blend_Source::SRC_ALPHA, Blend_Destination::ONE_MINUS_SRC_ALPHA);

		// Draw Lights
		auto billboard = ShaderProgram::Get("billboard");
		billboard->Bind();
		
		Texture* LightTexture = Texture::Get("editor_lightbulb");
		for (auto light : m_allLightObjects)
		{
			billboard->SetUniformMatrix<Matrix4x4>("VXL_model", light->m_transform.getWorldModel(), true);
		
			if (LightTexture)
				LightTexture->Bind(Active_Texture::LEVEL0);
			else
				Debug.GetNullTexture()->Bind(Active_Texture::LEVEL0);

			Geometry.GetQuadZ()->Draw();
		}

		// Draw Cameras
		Texture* CameraTexture = Texture::Get("editor_camera");
		for (auto camera : m_allCameraObjects)
		{
			billboard->SetUniformMatrix<Matrix4x4>("VXL_model", camera->m_transform.getWorldModel(), true);

			if (LightTexture)
				CameraTexture->Bind(Active_Texture::LEVEL0);
			else
				Debug.GetNullTexture()->Bind(Active_Texture::LEVEL0);

			Geometry.GetQuadZ()->Draw();
		}

		// Draw Debug Lines
		auto lines = ShaderProgram::Get("lines");
		lines->Bind();
		lines->SetUniform<Vector4>("_viewport", Vector4(
			(float)Window.GetScreenOffsetX(),
			(float)Window.GetScreenOffsetY(),
			(float)Window.GetScreenWidth(),
			(float)Window.GetScreenHeight()
		));

		lines->SetUniform<bool>("passthrough", false);
		Debug.RenderWorldLines();
		lines->SetUniform<bool>("passthrough", true);
		Debug.RenderScreenLines();

		// Draw Debug Wireframe Sphere
		auto passthrough = ShaderProgram::Get("passthrough");
		passthrough->Bind();
		passthrough->SetUniform<bool>("VXL_useTexture", false);
		passthrough->SetUniform<bool>("VXL_useModel", true);
		
		glUtil::wireframe(true);
		glUtil::cullMode(Cull_Type::NO_CULL);
		glLineWidth(5.0f);
		for (const auto& sphere : Debug.m_wireframeSpheres)
		{
			passthrough->SetUniform<Color4F>("VXL_color", sphere.color);
			passthrough->SetUniformMatrix<Matrix4x4>("VXL_model", sphere.model, true);
			Geometry.GetIcoSphere()->Draw();
		}
		glLineWidth(1.0f);
		glUtil::cullMode(Cull_Type::COUNTER_CLOCKWISE);
		glUtil::wireframe(false);

	}

	void RenderManager::RenderEditorObjectsPostDepth()
	{
		// Draw Editor Arrows (if selection applies)
		if (Editor.HasSelection())
		{
			auto simpleLight = ShaderProgram::Get("simpleLight");
			simpleLight->Bind();

			simpleLight->SetUniform<bool>("VXL_useModel", true);
			simpleLight->SetUniformMatrix<Matrix4x4>("VXL_model", Editor.GetSelectionTransformModel(), true);

			// Movement //
			if (Editor.m_controlMode == Editor.ControlMode::TRANSLATE)
			{
				// X Axis //
				if (Editor.m_controlAxis == Axis::X)
					if (Editor.m_controlAxisClicked)
						simpleLight->SetUniform<Color4F>("VXL_color", Color4F::WHITE);
					else
						simpleLight->SetUniform<Color4F>("VXL_color", Color4F::YELLOW);
				else
					simpleLight->SetUniform<Color4F>("VXL_color", Color4F(1, 0, 0, 1));

				Geometry.GetArrowX()->Draw();

				// Y Axis //
				if (Editor.m_controlAxis == Axis::Y)
					if (Editor.m_controlAxisClicked)
						simpleLight->SetUniform<Color4F>("VXL_color", Color4F::WHITE);
					else
						simpleLight->SetUniform<Color4F>("VXL_color", Color4F::YELLOW);
				else
					simpleLight->SetUniform<Color4F>("VXL_color", Color4F(0, 1, 0, 1));

				Geometry.GetArrowY()->Draw();

				// Z Axis //
				if (Editor.m_controlAxis == Axis::Z)
					if (Editor.m_controlAxisClicked)
						simpleLight->SetUniform<Color4F>("VXL_color", Color4F::WHITE);
					else
						simpleLight->SetUniform<Color4F>("VXL_color", Color4F::YELLOW);
				else
					simpleLight->SetUniform<Color4F>("VXL_color", Color4F(0, 0, 1, 1));

				Geometry.GetArrowZ()->Draw();

				// Small cube in the middle //
				simpleLight->SetUniform<Color4F>("VXL_color", Color4F(1, 1, 1, 0.85f));
				Geometry.GetCubeSmall()->Draw();

				//	// X Plane
				//	simpleLight->SetUniform<Color4F>("VXL_color", Color4F(1, 0, 0, 0.5f));
				//	Geometry.GetQuadX()->Draw();
				//	
				//	// Y Plane
				//	simpleLight->SetUniform<Color4F>("VXL_color", Color4F(0, 1, 0, 0.5f));
				//	Geometry.GetQuadY()->Draw();
				//	
				//	// Z Plane
				//	simpleLight->SetUniform<Color4F>("VXL_color", Color4F(0, 0, 1, 0.5f));
				//	Geometry.GetQuadZ()->Draw();

			}
			// TEST //


		}
	}
}
