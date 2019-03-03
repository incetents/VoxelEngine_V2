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
#include "../engine/opengl/TextureTracker.h"
#include "../engine/opengl/Debug.h"
#include "../engine/opengl/UBO.h"

#include "../engine/math/Camera.h"
#include "../engine/math/Color.h"
#include "../engine/math/Transform.h"
#include "../engine/math/Vector2.h"
#include "../engine/math/Vector3.h"
#include "../engine/math/Vector4.h"
#include "../engine/math/MathCore.h"
#include "../engine/math/Lerp.h"

#include "../engine/modules/Entity.h"
#include "../engine/modules/Material.h"
#include "../engine/modules/RenderManager.h"

#include "../engine/window/window.h"

#include "../engine/imgui_wrappers/Imgui_ShaderErrors.h"
#include "../engine/imgui_wrappers/ImGui_DevConsole.h"

#include "../game/terrain/TerrainManager.h"

#include <iostream>

#include "../engine/math/Model.h"

namespace Vxl
{
	void Scene_Game::Setup()
	{
		Loader::LoadScript_ImportFiles("./scripts/ImportFiles.txt");

		_camera = Camera::Create("main", Vector3(3.5f, 2.8f, 0.3f), Vector3(-0.5f, -0.38f, -0.72f), 0.01f, 50.0f);
		//_camera->setOrthographic(-15, 15, -15, 15);
		_camera->setPerspective(110.0f, Window.GetAspectRatio());
		_camera->update();
		_camera->SetMain();

		// FBO
		_fbo = new FramebufferObject();
		_fbo->addTexture(Window.GetResolutionWidth(), Window.GetResolutionHeight());
		_fbo->addTexture(Window.GetResolutionWidth(), Window.GetResolutionHeight());
		_fbo->addTexture(Window.GetResolutionWidth(), Window.GetResolutionHeight());
		_fbo->addDepth(Window.GetResolutionWidth(), Window.GetResolutionHeight());
		_fbo->unbind();

		_shader_skybox				= ShaderProgram::Get("skybox");
		_shader_gbuffer				= ShaderProgram::Get("gbuffer");
		_shader_gbuffer_no_model	= ShaderProgram::Get("gbuffer_no_model");
		_shader_debugLines			= ShaderProgram::Get("debugLines");
		_shader_passthrough			= ShaderProgram::Get("passthrough");

		_material_skybox			= Material::Create("skybox", _shader_skybox, 0);
		_material_gbuffer			= Material::Create("gbuffer", _shader_gbuffer, 1);
		_material_gbuffer_no_model	= Material::Create("gbuffer_no_model", _shader_gbuffer_no_model, 2);
		_material_debugLines		= Material::Create("debug_lines", _shader_debugLines, 3);
		_material_passthrough		= Material::Create("passthrough", _shader_passthrough, 999);

		_tex = Texture::Get("beato");
		_tex_crate = Texture::Get("crate_diffuse");

		_cubemap1 = Cubemap::Get("craterlake");
		
		// Voxel Stuff
		BlockAtlas.Set(Texture::Get("TextureAtlas"), 16); // texture of all blocks
		BlockDictionary.Setup(); // info of all blocks
		TerrainManager.Setup(); // keeps track of terrain info
		//

		_mesh = new Mesh();

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
		_mesh->GenerateNormals(pos, 4, indices, 6);

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
		_entity1 = Entity::Create();
		_entity1->SetMaterial(_material_gbuffer);
		_entity1->m_material.SetTexture(_tex, Active_Texture::LEVEL0);
		_entity1->m_mesh = _mesh;
		_entity1->m_transform.setScale(+0.5f);
		
		//Loader::Load_Model("jiggy1", "./assets/models/jiggy.obj", false, true);
		Model* jiggy = Model::Get("jiggy");
		Mesh* jiggyMesh = new Mesh();
		jiggyMesh->m_positions.set(jiggy->positions);
		jiggyMesh->m_uvs.set(jiggy->uvs);
		jiggyMesh->m_normals.set(jiggy->normals);
		//jiggyMesh->m_tangents.set(jiggy->tangents);
		//jiggyMesh->m_bitangents.set(jiggy->bitangents);
		jiggyMesh->m_indices.set(jiggy->indices);
		jiggyMesh->Bind();

		_entity2 = Entity::Create();
		_entity2->SetMaterial(_material_gbuffer);
		//_entity2->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_entity2->m_mesh = jiggyMesh;// Geometry.GetIcoSphere();
		_entity2->m_transform.setPosition(Vector3(+1.5f, 0, -3.0f));
		// TEST
		_entity2->SetColor(Color3F(1, 1, 0));
		
		_entity3 = Entity::Create();
		_entity3->SetMaterial(_material_gbuffer);
		_entity3->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_entity3->m_mesh = Geometry.GetIcosahedron();
		_entity3->m_transform.setPosition(Vector3(-2.5f, 0, -3.0f));
		
		_entity4 = Entity::Create();
		_entity4->SetMaterial(_material_skybox);
		_entity4->m_material.SetTexture(_cubemap1, Active_Texture::LEVEL0);
		_entity4->m_mesh = Geometry.GetInverseCube();
		

		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				for (int z = -1; z <= 1; z++)
				{
					if (x == 0 && y == 0 && z == 0)
						continue;
					
					//	Entity* ent = new Entity();
					//	ent->SetMaterial(_material_gbuffer);
					//	ent->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
					//	ent->m_mesh = Geometry::GetCube();
					//	
					//	ent->m_transform.setPosition(Vector3(x * 4.0f, y * 4.0f, z * 4.0f));
					//	
					//	if (x == 1 || y == 1 || z == 1)
					//	{
					//		ent->m_transform.setRotationX(45.0f);
					//		ent->m_transform.setRotationY(45.0f);
					//	}
					//	
					//	_cubes.push_back(ent);
				}
			}
		}

		//
		_crate1 = Entity::Create();
		_crate1->SetMaterial(_material_gbuffer);
		_crate1->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_crate1->m_mesh = Geometry.GetCube();
		_crate1->m_transform.setPosition(0, 2, 0);
		_crate1->SetTint(Color3F(0.4f, 0.1f, 0.9f));
		
		
		_crate2 = Entity::Create();
		_crate2->SetMaterial(_material_gbuffer);
		_crate2->m_mesh = Geometry.GetCube();
		_crate2->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		_crate2->m_transform.setPosition(0, 2, 0);
		// Parent Test
		//_crate1->m_transform.setParent(&_crate2->m_transform);
		_crate2->m_transform.addChild(&_crate1->m_transform);
		
		_crate1->m_transform.setWorldPosition(0, 0, 0);
		
		_octo1 = Entity::Create();
		_octo1->SetMaterial(_material_gbuffer);
		_octo1->m_mesh = Geometry.GetOctahedron();
		_octo1->m_transform.setPosition(0, 0, 0);
		_octo1->m_transform.setScale(0.5f);
		_octo1->SetColor(Color3F(1, 1, 1));
		
		_octo2 = Entity::Create();
		_octo2->SetMaterial(_material_gbuffer);
		_octo2->m_mesh = Geometry.GetOctahedron();
		_octo2->m_transform.setPosition(1, 0, 0);
		_octo2->m_transform.setScale(0.5f);
		_octo2->SetColor(Color3F(1, 0, 0));
		
		_octo3 = Entity::Create();
		_octo3->SetMaterial(_material_gbuffer);
		_octo3->m_mesh = Geometry.GetOctahedron();
		_octo3->m_transform.setPosition(0, 1, 0);
		_octo3->m_transform.setScale(0.5f);
		_octo3->SetColor(Color3F(0, 1, 0));
		
		_octo4 = Entity::Create();
		_octo4->SetMaterial(_material_gbuffer);
		_octo4->m_mesh = Geometry.GetOctahedron();
		_octo4->m_transform.setPosition(0, 0, 1);
		_octo4->m_transform.setScale(0.5f);
		_octo4->SetColor(Color3F(0, 0, 1));
		

		Debug.Setup();

		// TEST
		//UniformBufferObject* UBO = new UniformBufferObject(12, 0, "Ubo1");
		//UBO->sendFloat(0.0f, 0);
		//UBO->sendFloat(0.0f, 4);
		//UBO->sendFloat(1.0f, 8);
	}
	void Scene_Game::Destroy()
	{
		delete _fbo;
		delete _mesh;

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
			CamMove += _camera->getForward();
		if (Input.getKey(KeyCode::S))
			CamMove += _camera->getBackwards();
		if (Input.getKey(KeyCode::A))
			CamMove += _camera->getLeft();
		if (Input.getKey(KeyCode::D))
			CamMove += _camera->getRight();

		if (Input.getKey(KeyCode::SPACE))
			CamMove += Vector3::UP;
		if (Input.getKey(KeyCode::LEFT_CONTROL))
			CamMove += Vector3::DOWN;

		_camera->increasePosition(CamMove.Normalize() * CamSpeed);

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

		_camera->increaseRotation(CamRotation);

		// Edge case for vertical rotation
		float Xrot = _camera->getRotationEuler().x;
		Xrot = MacroClamp(Xrot, -89.9f, 89.9f);
		_camera->setRotationX(Xrot);
		//

		_camera->update();

		// Debug Lines
		Debug.DrawLine(
			Vector3(-1, -1, -1), Vector3(+1, +1, -1),
			Color3F(0, 1, 0), Color3F(1, 0, 1)
		);
		static float time = 0.0f;
		time += 0.2f;
		Debug.DrawLine(
			Vector3(+1, +1, -1), Vector3(+1, +4 + cosf(time), -1),
			Color3F(1, 1, 0), Color3F(0, 1, 1)
		);

		// Draw OBB for entity 3
		Vector3 p = _entity3->m_transform.getPosition();
		Vector3 pmin = _entity3->GetAABBMin();
		Vector3 pmax = _entity3->GetAABBMax();
		std::vector<Vector3> OBB = _entity3->GetOBB();

		//DebugLines.AddAABB(pmin, pmax, p);
		
		// Bot
		Debug.DrawLine(p + OBB[0], p + OBB[1]);
		Debug.DrawLine(p + OBB[1], p + OBB[5]);
		Debug.DrawLine(p + OBB[5], p + OBB[4]);
		Debug.DrawLine(p + OBB[4], p + OBB[0]);
		
		// Top
		Debug.DrawLine(p + OBB[2], p + OBB[3]);
		Debug.DrawLine(p + OBB[3], p + OBB[7]);
		Debug.DrawLine(p + OBB[7], p + OBB[6]);
		Debug.DrawLine(p + OBB[6], p + OBB[2]);

		// Mid
		Debug.DrawLine(p + OBB[0], p + OBB[2]);
		Debug.DrawLine(p + OBB[1], p + OBB[3]);
		Debug.DrawLine(p + OBB[4], p + OBB[6]);
		Debug.DrawLine(p + OBB[5], p + OBB[7]);

		// Transforms
		Debug.DrawLine(p, p + _entity3->m_transform.getForward() * 4.0f, Color3F::BLUE, Color3F::BLUE);
		Debug.DrawLine(p, p + _entity3->m_transform.getUp() * 4.0f, Color3F::GREEN, Color3F::GREEN);
		Debug.DrawLine(p, p + _entity3->m_transform.getRight() * 4.0f, Color3F::RED, Color3F::RED);

		Debug.DrawAABB(pmin, pmax, p, Color3F::BLACK);

		// Rotate Stuff
		//_crate2->m_transform.setRotation(sin(Time.GetTime() / 2) * 90.0, cos(Time.GetTime() / 2) * 90.0, 0);
		//_crate2->m_transform.increaseRotation(1.0f, 0.2f, 0);
		_entity3->m_transform.increaseRotation(0.2f, 1, 0);

		// End Frame Updates
		TextureTracker.NewFrame();
		XGamePadManager.Update();

		//_camera->updatePerspective(_camera->getFOV(), Window.GetAspectRatio());
	}

	void Scene_Game::Draw()
	{
		Camera::GetMain()->BindUBO();
		//

		glUtil::clearBuffer();
		glUtil::clearColor(Color3F(0.1f, 0.1f, 0.3f));

		_fbo->bind();
		Window.ViewportToWindowResolution();

		// ~~ //
		_material_debugLines->Bind();

		glLineWidth(9.0f);
		Debug.RenderLines();
		glLineWidth(1.0f);
		// ~~ //

		RenderManager.RenderScene();
		{
			/*
			// GBUFFER Instancing
			_material_gbuffer_instancing->Bind();
			
			_entity1->Draw();

			// GBUFFER
			_material_gbuffer->Bind();
			
			_entity2->Draw();
			_entity3->Draw();

			for (int i = 0; i < _cubes.size(); i++)
				_cubes[i]->Draw();

			//GBUFFER Color
				_material_gbuffer_color->Bind();
				
				_entity5->m_transform.setScale(0.4f);
				
				_shader_gbuffer_color->SetUniform("color", vec3(1, 1, 1));
				_entity5->m_transform.setPosition(Vector3(0, 0, 0));
				_entity5->Draw();
				
				_shader_gbuffer_color->SetUniform("color", vec3(1, 0, 0));
				_entity5->m_transform.setPosition(Vector3(1, 0, 0));
				_entity5->Draw();
				
				_shader_gbuffer_color->SetUniform("color", vec3(0, 1, 0));
				_entity5->m_transform.setPosition(Vector3(0, 1, 0));
				_entity5->Draw();
				
				_shader_gbuffer_color->SetUniform("color", vec3(0, 0, 1));
				_entity5->m_transform.setPosition(Vector3(0, 0, 1));
				_entity5->Draw();
				
				// Skybox
				_material_skybox->Bind();
				
				_entity4->Draw();
			
			*/
		}

		//	// GBUFFER No model
		//	_material_gbuffer_no_model->Bind();
		//	
		//	glUtil::setActiveTexture(Active_Texture::LEVEL0);
		//	BlockAtlas.BindAtlas();
		//	
		//	TerrainManager.Draw();


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

		

		_fbo->unbind();
		Window.ViewportToWindowSize();
		glUtil::wireframe(false);

		// ~~~~~~~~~~~~~~~~~ //
		glUtil::clearBuffer();
		glUtil::clearColor(Color3F(0, 0, 0));

		// Final Pass / Back Buffer
		_shader_passthrough->Bind();
		glDepthFunc(GL_ALWAYS);

		_fbo->bindTexture(0, Active_Texture::LEVEL0);
		Geometry.GetFullQuad()->Draw();
		
		// Normals test
		if (ShowNormal_DEV)
		{
			glViewport(0, 0, Window.GetScreenWidth() / 4, Window.GetScreenHeight() / 4);
			_fbo->bindTexture(1, Active_Texture::LEVEL0);
			Geometry.GetFullQuad()->Draw();
		}
		// Depth test
		if (ShowDepth_DEV)
		{
			glViewport(Window.GetScreenWidth() / 4, 0, Window.GetScreenWidth() / 4, Window.GetScreenHeight() / 4);
			_fbo->bindDepth(Active_Texture::LEVEL0);
			Geometry.GetFullQuad()->Draw();
		}

		// _fbo->bindDepth(Active_Texture::LEVEL0);
		// Geometry::GetFullQuad()->Draw();


		glDepthFunc(GL_LEQUAL);
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
		Imgui_ShaderErrors.Draw();

		// Setup
		Imgui_DevConsole.GBUFFER_WIREFRAME = _material_gbuffer->m_wireframe;
		Imgui_DevConsole.MAIN_CAMERA = _camera;
		Imgui_DevConsole.CAMFOV = _camera->getFOV();
		Imgui_DevConsole.CAM_ZNEAR = _camera->getZnear();
		Imgui_DevConsole.CAM_ZFAR = _camera->getZfar();
		// Draw
		Imgui_DevConsole.Draw(this);
		// Update Values from dev console
		_material_gbuffer->m_wireframe = Imgui_DevConsole.GBUFFER_WIREFRAME;
		_camera = Imgui_DevConsole.MAIN_CAMERA;
		ShowNormal_DEV = Imgui_DevConsole.SHOW_NORMALS;
		ShowDepth_DEV = Imgui_DevConsole.SHOW_DEPTH;

		if (fabs(_camera->getFOV() - Imgui_DevConsole.CAMFOV) > 0.01f)
		{
			_camera->updatePerspective(Imgui_DevConsole.CAMFOV, Window.GetAspectRatio());
		}
		if (fabs(_camera->getZnear() - Imgui_DevConsole.CAM_ZNEAR) > 0.01f)
		{
			_camera->setZnear(Imgui_DevConsole.CAM_ZNEAR);
		}
		if (fabs(_camera->getZfar() - Imgui_DevConsole.CAM_ZFAR) > 0.01f)
		{
			_camera->setZfar(Imgui_DevConsole.CAM_ZFAR);
		}
	}
}