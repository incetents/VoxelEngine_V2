// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Scene_Game.h"

#include "../imgui/imgui.h"

#include "../engine/window/window.h"
#include "../engine/input/Input.h"
#include "../engine/input/XGamePad.h"

#include "../engine/utilities/Loader.h"
#include "../engine/utilities/logger.h"
#include "../engine/utilities/stringUtil.h"
#include "../engine/utilities/Time.h"

#include "../engine/opengl/FramebufferObject.h"
#include "../engine/opengl/Geometry.h"
#include "../engine/opengl/Mesh.h"
#include "../engine/opengl/Shader.h"
#include "../engine/opengl/Texture.h"
#include "../engine/opengl/Debug.h"
#include "../engine/opengl/UBO.h"

#include "../engine/math/Color.h"
#include "../engine/math/Transform.h"
#include "../engine/math/Vector2.h"
#include "../engine/math/Vector3.h"
#include "../engine/math/Vector4.h"
#include "../engine/math/MathCore.h"
#include "../engine/math/Lerp.h"

#include "../engine/objects/LightObject.h"
#include "../engine/objects/CameraObject.h"
#include "../engine/objects/GameObject.h"
#include "../engine/modules/Material.h"
#include "../engine/modules/RenderManager.h"

#include "../engine/window/window.h"

#include "../engine/window/ShaderErrors.h"
#include "../engine/window/DevConsole.h"
#include "../engine/window/Hierarchy.h"
#include "../engine/window/Inspector.h"
#include "../engine/window/Performance.h"

#include "../game/terrain/TerrainManager.h"

#include <iostream>

#include "../engine/math/Model.h"

namespace Vxl
{
	void Scene_Game::Setup()
	{
		Loader::LoadScript_ImportFiles("./scripts/ImportFiles.txt");

		//_camera = Camera::Create("main", Vector3(3.5f, 2.8f, 0.3f), Vector3(-0.5f, -0.38f, -0.72f), 0.01f, 50.0f);

		_cameraObject = CameraObject::Create("_camera_special", 0.01f, 50.0f);
		_cameraObject->m_transform.setPosition(3.5f, 2.8f, 0.3f);
		_cameraObject->m_transform.setForward(Vector3(0, 0, 1));

		_cameraObject->SetPerspectiveWindowAspect(110.0f);

		RenderManager.SetMainCamera(_cameraObject);

		//_camera->setOrthographic(-15, 15, -15, 15);
		//_camera->setPerspective(110.0f, 1.0f);
		//_camera->setPerspectiveWindowAspectLock(110.0f);
		//_camera->update();
		//_camera->SetMain();

		// FBO
		_fbo = FramebufferObject::Create("Gbuffer", Window.GetResolutionWidth(), Window.GetResolutionHeight(), Color4F(0.1f, 0.1f, 0.3f, 1.0f));
		_fbo->addTexture("albedo");
		_fbo->addTexture("normal", Wrap_Mode::CLAMP_STRETCH, Filter_Min::NEAREST, Filter_Mag::NEAREST, Format_Type::RGBA16_SNORM, Channel_Type::RGBA, Data_Type::FLOAT);
		_fbo->addTexture("test");
		_fbo->addDepth();

		_fbo_colorpicker = FramebufferObject::Create("ColorPicker", Window.GetResolutionWidth(), Window.GetResolutionHeight(), Color4F(0,0,0,0));
		_fbo_colorpicker->addTexture("color");
		_fbo_colorpicker->addDepth();

		_shader_skybox				= ShaderProgram::Get("skybox");
		_shader_gbuffer				= ShaderProgram::Get("gbuffer");
		_shader_gbuffer_no_model	= ShaderProgram::Get("gbuffer_no_model");
		_shader_lines				= ShaderProgram::Get("lines");
		_shader_passthrough			= ShaderProgram::Get("passthrough");
		_shader_colorPicker			= ShaderProgram::Get("colorPicker");
		_shader_showRenderTarget	= ShaderProgram::Get("showRenderTarget");

		_material_skybox			= Material::Create("skybox", _shader_skybox, 0);
		_material_gbuffer			= Material::Create("gbuffer", _shader_gbuffer, 1);
		_material_gbuffer_no_model	= Material::Create("gbuffer_no_model", _shader_gbuffer_no_model, 2);
		_material_lines				= Material::Create("debug_lines", _shader_lines, 3);
		_material_passthrough		= Material::Create("passthrough", _shader_passthrough, 999);

		_material_gbuffer->m_BlendState = false;
		_material_gbuffer->m_BlendSource = Blend_Source::ONE;
		_material_gbuffer->m_BlendDest = Blend_Destination::ZERO;

		_tex = Texture::Get("beato");
		_tex_crate = Texture::Get("crate_diffuse");
		_tex_gridtest = Texture::Get("grid_test");

		_cubemap1 = Cubemap::Get("craterlake");
		
		// Voxel Stuff
		//BlockAtlas.Set(Texture::Get("TextureAtlas"), 16); // texture of all blocks
		//BlockDictionary.Setup(); // info of all blocks
		//TerrainManager.Setup(); // keeps track of terrain info
		//

		_mesh = Mesh::Create("test1");
		
		Vector3 pos[] = {
			Vector3(-0.5f, -0.5f, 0.0f),
			Vector3(+0.5f, -0.5f, 0.0f),
			Vector3(+0.5f, +0.5f, 0.0f),
			Vector3(-0.5f, +0.5f, 0.0f)
		};
		Vector2 uvs[] = {
			Vector2(0,0),
			Vector2(1,0),
			Vector2(1,1),
			Vector2(0,1)
		};
		GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };
		
		_mesh->m_positions.set(pos, 4);
		_mesh->m_uvs.set(uvs, 4);
		_mesh->m_indices.set(indices, 6);
		
		std::vector<Matrix4x4> m_models;
		for (float x = 0; x < 5.0f; x++)
		{
			for (float y = 0; y < 5.0f; y++)
			{
				Transform t(Vec3(x * 1.2f, y * 1.2f, -4.0f));
				m_models.push_back(t.getModel());
			}
		}
		
		_mesh->m_instances = m_models;
		
		_mesh->Bind();
		
		
		// Entities
		GameObject* _entity1 = GameObject::Create("_entity1");
		_entity1->SetMaterial(_material_gbuffer);
		_entity1->m_material.SetTexture(_tex, Active_Texture::LEVEL0);
		_entity1->SetMesh(_mesh);
		_entity1->m_transform.setScale(+0.5f);
		
		//Loader::Load_Model("jiggy1", "./assets/models/jiggy.obj", false, true);
		Mesh* jiggyMesh = Mesh::Get("jiggy");
		
		GameObject* _entity2 = GameObject::Create("_entity2");
		_entity2->SetMaterial(_material_gbuffer);
		//_entity2->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_entity2->SetMesh(jiggyMesh);// Geometry.GetIcoSphere();
		_entity2->m_transform.setPosition(Vector3(+1.5f, 0, -3.0f));
		_entity2->SetColor(Color3F(1, 1, 0));
		
		GameObject* _entity3 = GameObject::Create("_entity3");
		_entity3->SetMaterial(_material_gbuffer);
		_entity3->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_entity3->SetMesh(Geometry.GetIcosahedron());
		_entity3->m_transform.setPosition(Vector3(-2.5f, 0, -3.0f));
		
		
		
		GameObject* _entity5 = GameObject::Create("_entity5");
		_entity5->SetMaterial(_material_gbuffer);
		_entity5->SetMesh(Geometry.GetSphereUV_Good());
		_entity5->m_transform.setPosition(Vector3(0, -4, 0));
		_entity5->m_material.SetTexture(_tex_gridtest, Active_Texture::LEVEL0);
		//_entity5->SetColor(Color3F(1, 1, 1));
		

		GameObject* _skybox = GameObject::Create("_skybox");
		_skybox->SetMaterial(_material_skybox);
		_skybox->m_material.SetTexture(_cubemap1, Active_Texture::LEVEL0);
		_skybox->SetMesh(Geometry.GetInverseCube());
		_skybox->m_isClickable = false;
		_skybox->m_useTransform = false;
		
		//
		GameObject* _crate1 = GameObject::Create("_crate1");
		_crate1->SetMaterial(_material_gbuffer);
		_crate1->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_crate1->SetMesh(Geometry.GetCube());
		_crate1->m_transform.setPosition(0, 2, 0);
		_crate1->SetTint(Color3F(0.4f, 0.1f, 0.9f));
		
		
		GameObject* _crate2 = GameObject::Create("_crate2");
		_crate2->SetMaterial(_material_gbuffer);
		_crate2->SetMesh(Geometry.GetCube());
		_crate2->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		_crate2->m_transform.setPosition(0, 2, 0);
		// Parent Test
		//_crate1->m_transform.setParent(&_crate2->m_transform);
		_crate2->m_transform.addChild(&_crate1->m_transform);
		
		_crate1->m_transform.setWorldPosition(0, 0, 0);
		
		GameObject* _octo1 = GameObject::Create("_octo1");
		_octo1->SetMaterial(_material_gbuffer);
		_octo1->SetMesh(Geometry.GetOctahedron());
		_octo1->m_transform.setPosition(0, 0, 0);
		_octo1->m_transform.setScale(0.5f);
		_octo1->SetColor(Color3F(1, 1, 1));
		
		GameObject* _octo2 = GameObject::Create("_octo2");
		_octo2->SetMaterial(_material_gbuffer);
		_octo2->SetMesh(Geometry.GetOctahedron());
		_octo2->m_transform.setPosition(1, 0, 0);
		_octo2->m_transform.setScale(0.5f);
		_octo2->SetColor(Color3F(1, 0, 0));
		
		GameObject* _octo3 = GameObject::Create("_octo3");
		_octo3->SetMaterial(_material_gbuffer);
		_octo3->SetMesh(Geometry.GetOctahedron());
		_octo3->m_transform.setPosition(0, 1, 0);
		_octo3->m_transform.setScale(0.5f);
		_octo3->SetColor(Color3F(0, 1, 0));
		
		GameObject* _octo4 = GameObject::Create("_octo4");
		_octo4->SetMaterial(_material_gbuffer);
		_octo4->SetMesh(Geometry.GetOctahedron());
		_octo4->m_transform.setPosition(0, 0, 1);
		_octo4->m_transform.setScale(0.5f);
		_octo4->SetColor(Color3F(0, 0, 1));
		
		//GameObject::Delete(_entity5);
		

		//GameObject::Delete("_octo3");
		//
		//_light1 = LightObject::Create("light1");


		Debug.Setup();
	}
	void Scene_Game::Destroy()
	{
		//delete _mesh;

		TerrainManager.Destroy();
	}

	void Scene_Game::Update()
	{
		// Setup Debug
		Debug.UpdateStart();

		if (Input.getKeyDown(KeyCode::ESCAPE))
			Window.Close();

		// Camera Movement
		Vector3 CamMove;
		float CamSpeed = 0.1f;

		if (Input.getKey(KeyCode::W))
			CamMove += _cameraObject->m_transform.getCameraForward();
		if (Input.getKey(KeyCode::S))
			CamMove += _cameraObject->m_transform.getCameraBackwards();
		if (Input.getKey(KeyCode::A))
			CamMove += _cameraObject->m_transform.getLeft();
		if (Input.getKey(KeyCode::D))
			CamMove += _cameraObject->m_transform.getRight();

		if (Input.getKey(KeyCode::SPACE))
			CamMove += Vector3::UP;
		if (Input.getKey(KeyCode::LEFT_CONTROL))
			CamMove += Vector3::DOWN;

		_cameraObject->m_transform.increasePosition(CamMove.Normalize() * CamSpeed);
		//_camera->increasePosition(CamMove.Normalize() * CamSpeed);

		// Camera Rotation
		Vector3 CamRotation;
		float CamRotationSpeed = 2.25f;

		if (Input.getKey(KeyCode::UP))
			CamRotation.x += CamRotationSpeed;
		if (Input.getKey(KeyCode::DOWN))
			CamRotation.x -= CamRotationSpeed;
		if (Input.getKey(KeyCode::LEFT))
			CamRotation.y += CamRotationSpeed;
		if (Input.getKey(KeyCode::RIGHT))
			CamRotation.y -= CamRotationSpeed;

		_cameraObject->m_transform.increaseRotation(CamRotation);
		//_camera->increaseRotation(CamRotation);

		// Camera Lock
		if (Input.getKeyDown(KeyCode::Z))
		{
			if (Window.GetCursor() == CursorMode::LOCKED)
				Window.SetCursor(CursorMode::NORMAL);
			else
				Window.SetCursor(CursorMode::LOCKED);
		}

		float CameraHorizontalRotation = DevConsole.GetFloat("Camera Horizontal Rotation", 0.2f);
		float CameraVerticalRotation = DevConsole.GetFloat("Camera Vertical Rotation", 0.2f);

		// Lock = mouse rotates camera
		if (Window.GetCursor() == CursorMode::LOCKED)
		{
			float deltax = Input.getMouseDeltaX() * (float)Time.GetDeltaTime() * CameraVerticalRotation;
			float deltay = Input.getMouseDeltaY() * (float)Time.GetDeltaTime() * CameraHorizontalRotation;
			// clamp delta for safety
			deltax = MacroClamp(deltax, -100, +100);
			deltay = MacroClamp(deltay, -100, +100);

			_cameraObject->m_transform.increaseRotation(-deltay, -deltax, 0);
			//_camera->increaseRotation(-deltay, -deltax, 0);
		}

		// Edge case for vertical rotation
		float Xrot = MacroClamp(_cameraObject->m_transform.getRotationEuler().x, -89.9f, 89.9f);
		_cameraObject->m_transform.setRotationX(Xrot);
		//

		// Update Cam
		_cameraObject->Update();
		//_camera->update();

		// Debug Lines
		Debug.DrawLine(
			Vector3(-1, -1, -1), Vector3(+1, +1, -1),
			10.0f,
			Color4F(0, 1, 0, 1), Color4F(1, 0, 1, 0)
		);
		static float time = 0.0f;
		time += 0.2f;
		Debug.DrawLine(
			Vector3(+3, +1, -1), Vector3(+3, +4 + cosf(time), -1),
			10.0f,
			Color4F(1, 1, 0, 1), Color4F(0, 1, 1, 0)
		);

		// Selection
		if (Hierarchy._selectedEntity != nullptr && Hierarchy._selectedEntity->m_useTransform)
		{
			auto Entity = Hierarchy._selectedEntity;
			static Vector3 Epsilon = Vector3(0.01f, 0.01f, 0.01f);

			Color4F AABB_Color = Entity->m_isActive ? Color4F::YELLOW : Color4F::GREY;
			Color4F OBB_Color = Entity->m_isActive ? Color4F::GREEN : Color4F::GREY;

			// Draw Outline around Entity
			if (Entity->GetMesh())
			{
				if (Entity->GetMesh()->m_instances.Empty())
				{
					Debug.DrawAABB(Entity->GetAABBMin() - Epsilon, Entity->GetAABBMax() + Epsilon, Vector3::ZERO, 5.0f, AABB_Color);
					Debug.DrawOBB(*Entity, Vector3::ZERO, 5.0f, OBB_Color);
				}
				// Draw outline around all instances
				else
				{
					//Vector4 WPosition = Vector4(Entity->m_transform.getWorldPosition(), 1);
					auto Instances = Entity->GetMesh()->m_instances.GetVertices();
					for (Matrix4x4 instanceMatrix : *Instances)
					{
						Vector3 Pos = Vector3(instanceMatrix[12], instanceMatrix[13], instanceMatrix[14]);
						Debug.DrawAABB(Entity->GetAABBMin() - Epsilon, Entity->GetAABBMax() + Epsilon, Pos, 5.0f, AABB_Color);
						Debug.DrawOBB(*Entity, Pos, 5.0f, OBB_Color);
					}
				}
			}
		}

		//	// Draw OBB for entity 3
		//	Vector3 p = _entity3->m_transform.getPosition();
		//	Vector3 pmin = _entity3->GetAABBMin();
		//	Vector3 pmax = _entity3->GetAABBMax();
		//	std::vector<Vector3> OBB = _entity3->GetOBB();
		//	
		//	//DebugLines.AddAABB(pmin, pmax, p);
		//	
		//	// Bot
		//	Debug.DrawLine(p + OBB[0], p + OBB[1]);
		//	Debug.DrawLine(p + OBB[1], p + OBB[5]);
		//	Debug.DrawLine(p + OBB[5], p + OBB[4]);
		//	Debug.DrawLine(p + OBB[4], p + OBB[0]);
		//	
		//	// Top
		//	Debug.DrawLine(p + OBB[2], p + OBB[3]);
		//	Debug.DrawLine(p + OBB[3], p + OBB[7]);
		//	Debug.DrawLine(p + OBB[7], p + OBB[6]);
		//	Debug.DrawLine(p + OBB[6], p + OBB[2]);
		//	
		//	// Mid
		//	Debug.DrawLine(p + OBB[0], p + OBB[2]);
		//	Debug.DrawLine(p + OBB[1], p + OBB[3]);
		//	Debug.DrawLine(p + OBB[4], p + OBB[6]);
		//	Debug.DrawLine(p + OBB[5], p + OBB[7]);
		//	
		//	// Transforms
		//	Debug.DrawLine(p, p + _entity3->m_transform.getForward() * 4.0f, Color3F::BLUE, Color3F::BLUE);
		//	Debug.DrawLine(p, p + _entity3->m_transform.getUp() * 4.0f, Color3F::GREEN, Color3F::GREEN);
		//	Debug.DrawLine(p, p + _entity3->m_transform.getRight() * 4.0f, Color3F::RED, Color3F::RED);
		//	
		//	Debug.DrawAABB(pmin, pmax, p, Color3F::BLACK);
		//	
		//	// Rotate Stuff
		//	//_crate2->m_transform.setRotation(sin(Time.GetTime() / 2) * 90.0, cos(Time.GetTime() / 2) * 90.0, 0);
		//	//_crate2->m_transform.increaseRotation(1.0f, 0.2f, 0);
		//	_entity3->m_transform.increaseRotation(0.2f, 1, 0);

		// End Frame Updates
		XGamePadManager.Update();

		//_camera->updatePerspective(_camera->getFOV(), Window.GetAspectRatio());

		// Imgui Specials
		if (_material_gbuffer)
		{
			_material_gbuffer->m_Wireframe = DevConsole.GetBool("Gbuffer Wireframe", false);
		}
	}

	void Scene_Game::UpdateFixed()
	{

	}
	void Scene_Game::Draw()
	{
		//
		_shader_gbuffer->SetProgramUniform<int>("TESTMODE", DevConsole.GetInt("TESTMODE", 0));
		//

		GPUTimer::StartTimer("Fbo1_Bind");

		_fbo->bind();

		GPUTimer::EndTimer("Fbo1_Bind");

		GPUTimer::StartTimer("Gbuffer");

		RenderManager.GetMainCamera()->BindUBO();
		//Camera::GetMain()->BindUBO();
		RenderManager.RenderSceneGameObjects();

		GPUTimer::EndTimer("Gbuffer");

		//	// GBUFFER No model
		//	_material_gbuffer_no_model->Bind();
		//	
		//	glUtil::setActiveTexture(Active_Texture::LEVEL0);
		//	BlockAtlas.BindAtlas();
		//	
		//	TerrainManager.Draw();

		// ~~ //
		_material_lines->Bind();
		
		_shader_lines->SetUniform<Vector4>("_viewport", Vector4(
			(float)Window.GetScreenOffsetX(),
			(float)Window.GetScreenOffsetY(),
			(float)Window.GetScreenWidth(),
			(float)Window.GetScreenHeight()
		));

		Debug.RenderLines();
		// ~~ //

		//for (int i = 0; i < _cubes.size(); i++)
		//	_cubes[i]->Draw();

		//_entity1->m_transform.increaseRotationX(rot);

		//glActiveTexture(GL_TEXTURE0);
		//_tex->Bind();

		//Transform model1 = Transform(Vector3(0, 0, -5));
		//Transform model2 = Transform(Vector3(5, 0, -5));
		//Transform model3 = Transform(Vector3(-5, 0, -5));
		//Transform model4 = Transform(Vector3(0, 5, -5));
		//Transform model5 = Transform(Vector3(0, -5, -5));

		//model1.setRotationX(rot / 2);
		//model1.setRotationY(rot);
		//
		//model2.setRotationX(+rot / 2);
		//model2.setRotationY(-rot);
		//rot++;


		//	_shader_gbuffer->SetUniform("model", model1.getModel());
		//	Geometry::GetCube()->Draw();
		//	
		//	_shader_gbuffer->SetUniform("model", model2.getModel());
		//	Geometry::GetCube()->Draw();
		//	
		//	_shader_gbuffer->SetUniform("model", model3.getModel());
		//	Geometry::GetCube()->Draw();
		//	
		//	_shader_gbuffer->SetUniform("model", model4.getModel());
		//	Geometry::GetCube()->Draw();
		//	
		//	_shader_gbuffer->SetUniform("model", model5.getModel());
		//	Geometry::GetCube()->Draw();

		// ~~ //
		if (DevConsole.GetBool("Objects are Clickable", true) && Window.GetCursor() == CursorMode::NORMAL)
		{
			_fbo_colorpicker->bind();
			glUtil::blendState(false);

			_shader_colorPicker->Bind();

			auto Entities = RenderManager.GetAllEntities();
			unsigned int EntityCount = (unsigned int)Entities.size();
			union FloatChar
			{
				unsigned int ui_ID = 0;
				unsigned char f_ID[4];
			};
			FloatChar mem;
			for (unsigned int i = 0; i < EntityCount; i++)
			{
				auto Entity = Entities[i];

				// Ignore if not active or not clickable
				if (!Entity->IsFamilyActive() || !Entity->m_isClickable || !Entity->GetMesh())
					continue;

				_shader_colorPicker->SetUniform("VXL_model", Entity->m_transform.getModel());
				_shader_colorPicker->SetUniform("VXL_useInstancing", !Entity->GetMesh()->m_instances.Empty());
				_shader_colorPicker->SetUniform("VXL_useModel", Entity->m_useTransform);

				// Convert ID into color
				mem.ui_ID = i + 1; // offset (0 = nothing instead of first value)
				float x = (float)mem.f_ID[0] / 255.0f;
				float y = (float)mem.f_ID[1] / 255.0f;
				float z = (float)mem.f_ID[2] / 255.0f;
				float w = (float)mem.f_ID[3] / 255.0f;

				_shader_colorPicker->SetUniform<Vector4>("colorID", vec4(x, y, z, w));
				Entity->Draw();
			}

			GLubyte *data = new GLubyte[4];
			glReadPixels(
				(GLint)Input.getMouseX(),
				Window.GetResolutionHeight() - (GLint)Input.getMouseY(),
				1,
				1,
				(GLenum)Format_Type::RGBA,
				(GLenum)Data_Type::UNSIGNED_BYTE,
				data
			);
			//	std::cout <<
			//		(unsigned int)data[0] << ' ' <<
			//		(unsigned int)data[1] << ' ' <<
			//		(unsigned int)data[2] << ' ' <<
			//		(unsigned int)data[3] << ' ' <<
			//		std::endl;

			mem.f_ID[0] = data[0];
			mem.f_ID[1] = data[1];
			mem.f_ID[2] = data[2];
			mem.f_ID[3] = data[3];
			mem.ui_ID--;//offset (0 = nothing instead of first value)

			if (Input.getMouseButton(MouseButton::LEFT) && !Window.IsCursorOnImguiWindow())
			{
				if (mem.ui_ID < EntityCount)
					Hierarchy._selectedEntity = Entities[mem.ui_ID];
				else
					Hierarchy._selectedEntity = nullptr;
			}

			//if ()
			//	std::cout << mem.ui_ID << ", Entity: " << Entities[mem.ui_ID]->m_name << std::endl;

			delete data;
		}
		// ~~ //

		// Backbuffer
		FramebufferObject::unbind();
		Window.BindWindowViewport();

		glUtil::blendState(false);
		glUtil::wireframe(false);
		glUtil::depthTest(Depth_Pass_Rule::ALWAYS);

		// ~~~~~~~~~~~~~~~~~ //
		glUtil::clearColor(Color4F(0, 0, 0, 0));
		glUtil::clearBuffer();

		// Final Pass / Back Buffer
		switch (FBO_OVERRIDE)
		{
			// Output Normally
			case 0:
			{
				_shader_passthrough->Bind();

				_fbo->bindTexture(0, Active_Texture::LEVEL0);
				break;
			}
			// Show Albedo
			case 1:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 0);

				_fbo->bindTexture(0, Active_Texture::LEVEL0);
				break;
			}
			// Show Normal
			case 2:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 1);

				_fbo->bindTexture(1, Active_Texture::LEVEL0);
				break;
			}
			// Show Depth
			case 3:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 2);
				_shader_showRenderTarget->SetUniform("zNear", RenderManager.GetMainCamera()->getZnear());
				_shader_showRenderTarget->SetUniform("zFar", RenderManager.GetMainCamera()->getZfar());

				_fbo->bindDepth(Active_Texture::LEVEL0);
				break;
			}
		}
		RenderManager.RenderFullScreen();
		
		
		
		//	// Normals test
		//	if (DevConsole.GetBool("Show Normals", false))
		//	{
		//		
		//		
		//		glUtil::viewport(0, 0, Window.GetScreenWidth() / 4, Window.GetScreenHeight() / 4);
		//		
		//		Geometry.GetFullQuad()->Draw();
		//	}
		//	// Depth test
		//	if (DevConsole.GetBool("Show Depth", false))
		//	{
		//		_shader_showRenderTarget->SetUniform("outputMode", 2);
		//		_shader_showRenderTarget->SetUniform("zNear", Camera::GetMain()->getZnear());
		//		_shader_showRenderTarget->SetUniform("zFar", Camera::GetMain()->getZfar());
		//	
		//		glUtil::viewport(Window.GetScreenWidth() / 4, 0, Window.GetScreenWidth() / 4, Window.GetScreenHeight() / 4);
		//		_fbo->bindDepth(Active_Texture::LEVEL0);
		//		Geometry.GetFullQuad()->Draw();
		//	}

		// _fbo->bindDepth(Active_Texture::LEVEL0);
		// Geometry::GetFullQuad()->Draw();


		//glDepthFunc(GL_LEQUAL);
		//Window.ViewportToWindowSize();


		
		//	// SUBROUTINES
		//	glSubroutine Sub = *_shader_gbuffer->GetSubroutine(ShaderType::FRAGMENT);
		//	Sub.Set("Colour1", "ColorRed");
		//	Sub.Set("Colour2", "ColorGreen");
		//	Sub.Set("myNum", "t2");
		//	Sub.Bind();
		//	
		//	// UNIFORM BLOCKS
		//	auto block = _shader_gbuffer->GetUniformBlock("ColorBlock_0");
		//	float myFloats[3] = { 0.5f, 0.0f, 1.0f };
		//	block->Set(myFloats, 3);
		//	float r = 0.5f;
		//	float g = 1.0f;
		//	float b = 0.0f;
		//	block->Set(&r, 1, 0);
		//	block->Set(&g, 1, 1);

		//	// UNIFORMS
		//	_shader_gbuffer->SetUniform("value1", 0.1f);
		//	_shader_gbuffer->SetUniform("value2", 0.1f);
		//	_shader_gbuffer->SetUniform("value3", 0.1f);
		//	
		//	// Uniform a = Sp->GetUniform("value1");
		//	a.set<float>(1.0f);
		//	Uniform b = Sp->GetUniform("value2");
		//	b.set<float>(0.0f);
		//	Uniform c = Sp->GetUniform("value3");
		//	c.set<float>(0.5f);
		//	
		//	Shader_Gbuffer->SetUniform("model", model.getModel());
		//	
		//	Shader_Gbuffer->SetUniform("view", cam.getView());
		//	Shader_Gbuffer->SetUniform("projection", cam.getProjection());

	}
	void Scene_Game::DrawImGui()
	{
		// Shader Errors
		ShaderErrors.Draw();
		// Inspector
		Inspector.Draw();
		// Hierarchy
		Hierarchy.Draw();
		// Performance
		Performance.Draw();
		// Dev Console
		DevConsole.Draw(this);
	}
}