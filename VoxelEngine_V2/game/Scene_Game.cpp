// Copyright (c) 2018 Emmanuel Lajeunesse
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
#include "../engine/opengl/DebugLines.h"

#include "../engine/math/Camera.h"
#include "../engine/math/Color.h"
#include "../engine/math/Transform.h"
#include "../engine/math/Vector2.h"
#include "../engine/math/Vector3.h"
#include "../engine/math/Vector4.h"
#include "../engine/math/MathCore.h"

#include "../engine/modules/Entity.h"
#include "../engine/modules/Material.h"
#include "../engine/modules/RenderManager.h"

#include "../game/terrain/TerrainManager.h"

#include <iostream>


namespace Vxl
{
	void Scene_Game::Setup()
	{
		_camera = new Camera(Vector3(3.5f, 2.8f, 0.3f), Vector3(-0.5f, -0.38f, -0.72f), 0.01f, 50.0f);
		//_camera->setOrthographic(-15, 15, -15, 15);
		_camera->setPerspective(110.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
		_camera->update();
		_camera->SetMain();

		// FBO
		_fbo = new FramebufferObject();
		_fbo->addTexture(new RenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
		_fbo->addTexture(new RenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
		_fbo->addTexture(new RenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
		_fbo->addDepth(SCREEN_WIDTH, SCREEN_HEIGHT);
		_fbo->unbind();

		_shader_skybox				= ShaderProgram::m_database.Get("skybox");
		_shader_gbuffer				= ShaderProgram::m_database.Get("gbuffer");
		_shader_gbuffer_no_model	= ShaderProgram::m_database.Get("gbuffer_no_model");
		_shader_debugLines			= ShaderProgram::m_database.Get("debugLines");
		_shader_passthrough			= ShaderProgram::m_database.Get("passthrough");

		_material_gbuffer				= new MaterialBase(_shader_gbuffer, 1);
		_material_gbuffer_no_model		= new MaterialBase(_shader_gbuffer_no_model, 2);
		_material_debugLines			= new MaterialBase(_shader_debugLines, 3);
		_material_passthrough			= new MaterialBase(_shader_passthrough, 999);
		_material_skybox				= new MaterialBase(_shader_skybox, 0);

		_tex = Texture::m_database.Get("beato");
		_tex_crate = Texture::m_database.Get("crate_diffuse");

		_cubemap1 = Cubemap::m_database.Get("craterlake");
		
		// Voxel Stuff
		BlockAtlas.Set(Texture::m_database.Get("TextureAtlas"), 16); // texture of all blocks
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
		Vector3 nor[] = {
			Vector3(0,0,-1),
			Vector3(0,0,-1),
			Vector3(0,0,-1),
			Vector3(0,0,-1)
		};
		GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };

		_mesh->m_positions.set(pos, 4);
		_mesh->m_uvs.set(uvs, 4);
		_mesh->m_normals.set(nor, 4);
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

		// Entity
		_entity1 = new Entity();
		_entity1->SetMaterialBase(_material_gbuffer);
		_entity1->m_material.SetTexture(_tex, Active_Texture::LEVEL0);
		_entity1->m_mesh = _mesh;
		_entity1->m_transform.setScale(+0.5f);

		_entity2 = new Entity();
		_entity2->SetMaterialBase(_material_gbuffer);
		_entity2->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_entity2->m_mesh = Geometry::GetCube();
		_entity2->m_transform.setPosition(Vector3(+1.5f, 0, -3.0f));
		
		_entity3 = new Entity();
		_entity3->SetMaterialBase(_material_gbuffer);
		_entity3->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_entity3->m_mesh = Geometry::GetCube();
		_entity3->m_transform.setPosition(Vector3(-1.5f, 0, -3.0f));

		_entity4 = new Entity();
		_entity4->SetMaterialBase(_material_skybox);
		_entity4->m_material.SetTexture(_cubemap1, Active_Texture::LEVEL0);
		_entity4->m_mesh = Geometry::GetInverseCube();

		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				for (int z = -1; z <= 1; z++)
				{
					if (x == 0 && y == 0 && z == 0)
						continue;

					/*
					Entity* ent = new Entity();
					ent->SetMaterialBase(_material_gbuffer);
					ent->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
					ent->m_mesh = Geometry::GetCube();
					
					ent->m_transform.setPosition(Vector3(x * 2.0f, y * 2.0f, z * 2.0f));

					if (x == 1 || y == 1 || z == 1)
					{
						ent->m_transform.setRotationX(45.0f);
						ent->m_transform.setRotationY(45.0f);
					}

					_cubes.push_back(ent);
					*/
				}
			}
		}

		//
		_crate1 = new Entity();
		_crate1->SetMaterialBase(_material_gbuffer);
		_crate1->m_mesh = Geometry::GetCube();
		_crate1->m_transform.setPosition(0, 0, 3);
		_crate1->SetTint(Color3F(0.4, 0.1, 0.9));

		_crate2 = new Entity();
		_crate2->SetMaterialBase(_material_gbuffer);
		_crate2->m_mesh = Geometry::GetCube();
		_crate2->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		_crate2->m_transform.setPosition(3, 4, 6);
		//
		//_crate1->m_transform.setParent(&_crate2->m_transform);
		_crate2->m_transform.addChild(&_crate1->m_transform);

		_octo1 = new Entity();
		_octo1->SetMaterialBase(_material_gbuffer);
		_octo1->m_mesh = Geometry::GetOctahedron();
		_octo1->m_transform.setPosition(0, 0, 0);
		_octo1->m_transform.setScale(0.5f);
		_octo1->SetColor(Color3F(1, 1, 1));

		_octo2 = new Entity();
		_octo2->SetMaterialBase(_material_gbuffer);
		_octo2->m_mesh = Geometry::GetOctahedron();
		_octo2->m_transform.setPosition(1, 0, 0);
		_octo2->m_transform.setScale(0.5f);
		_octo2->SetColor(Color3F(1, 0, 0));

		_octo3 = new Entity();
		_octo3->SetMaterialBase(_material_gbuffer);
		_octo3->m_mesh = Geometry::GetOctahedron();
		_octo3->m_transform.setPosition(0, 1, 0);
		_octo3->m_transform.setScale(0.5f);
		_octo3->SetColor(Color3F(0, 1, 0));

		_octo4 = new Entity();
		_octo4->SetMaterialBase(_material_gbuffer);
		_octo4->m_mesh = Geometry::GetOctahedron();
		_octo4->m_transform.setPosition(0, 0, 1);
		_octo4->m_transform.setScale(0.5f);
		_octo4->SetColor(Color3F(0, 0, 1));


		// TEST
		_entity2->SetMaterialBase(_material_gbuffer);
		_entity2->SetColor(Color3F(1, 0, 0));

		DebugLines.Setup();

		clock1 = new Clock(5.0);
	}
	void Scene_Game::Destroy()
	{
		delete _fbo;
		delete _camera;
		delete _mesh;

		delete clock1;

		TerrainManager.Destroy();
	}

	void Scene_Game::Update()
	{
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
		DebugLines.AddLine(
			Vector3(-1, -1, -1), Vector3(+1, +1, -1),
			Color3F(0, 1, 0), Color3F(1, 0, 1)
		);
		static float time = 0.0f;
		time += 0.2f;
		DebugLines.AddLine(
			Vector3(+1, +1, -1), Vector3(+1, +4 + cosf(time), -1),
			Color3F(1, 1, 0), Color3F(0, 1, 1)
		);

		// Rotate Stuff
		_crate2->m_transform.increaseRotation(1.0f, 0.2f, 0);

		// End Frame Updates
		TextureTracker.NewFrame();
		XGamePadManager.Update();
	}

	void Scene_Game::Draw()
	{
		glUtil::clearBuffer();
		glUtil::clearColor(Color3F(0.1f, 0.1f, 0.3f));

		_fbo->bind();
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		

		_material_debugLines->Bind();
		//
		glUtil::unbindVAO();
		glLineWidth(9.0f);

		DebugLines.DrawLines();

		glLineWidth(1.0f);


		RenderManager.TEST();
		{
			// GBUFFER Instancing
			//_material_gbuffer_instancing->Bind();
			//
			//_entity1->Draw();

			// GBUFFER
			//	_material_gbuffer->Bind();
			//	
			//	_entity2->Draw();
			//	_entity3->Draw();

			//for (int i = 0; i < _cubes.size(); i++)
			//	_cubes[i]->Draw();

			// GBUFFER Color
			//	_material_gbuffer_color->Bind();
			//	
			//	_entity5->m_transform.setScale(0.4f);
			//	
			//	_shader_gbuffer_color->SetUniform("color", vec3(1, 1, 1));
			//	_entity5->m_transform.setPosition(Vector3(0, 0, 0));
			//	_entity5->Draw();
			//	
			//	_shader_gbuffer_color->SetUniform("color", vec3(1, 0, 0));
			//	_entity5->m_transform.setPosition(Vector3(1, 0, 0));
			//	_entity5->Draw();
			//	
			//	_shader_gbuffer_color->SetUniform("color", vec3(0, 1, 0));
			//	_entity5->m_transform.setPosition(Vector3(0, 1, 0));
			//	_entity5->Draw();
			//	
			//	_shader_gbuffer_color->SetUniform("color", vec3(0, 0, 1));
			//	_entity5->m_transform.setPosition(Vector3(0, 0, 1));
			//	_entity5->Draw();
			//	
			//	// Skybox
			//	_material_skybox->Bind();
			//	
			//	_entity4->Draw();
			//
		}

		// GBUFFER No model
		_material_gbuffer_no_model->Bind();

		glUtil::setActiveTexture(Active_Texture::LEVEL0);
		BlockAtlas.BindAtlas();

		TerrainManager.Draw();


		


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
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		// ~~~~~~~~~~~~~~~~~ //
		glUtil::clearBuffer();
		glUtil::clearColor(Color3F(0, 0, 0));

		// Final Pass / Back Buffer
		_shader_passthrough->Bind();
		glDepthFunc(GL_ALWAYS);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		_fbo->bindTexture(0, Active_Texture::LEVEL0);
		Geometry::GetFullQuad()->Draw();
		
		// Normals test
		static bool SHOW_NORMAL_QUAD = false;

		if (SHOW_NORMAL_QUAD)
		{
			glViewport(0, 0, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
			_fbo->bindTexture(1, Active_Texture::LEVEL0);
			Geometry::GetFullQuad()->Draw();
		}

		// glViewport(0, 0, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
		// _fbo->bindDepth(Active_Texture::LEVEL0);
		// Geometry::GetFullQuad()->Draw();


		glDepthFunc(GL_LEQUAL);



		/*
		// SUBROUTINES
		glSubroutine Sub = Sp->GetSubroutine(ShaderType::FRAGMENT);
		Sub.set("Colour1", "ColorRed");
		Sub.set("Colour2", "ColorGreen");
		Sub.set("myNum", "t2");
		Sub.bind();

		// UNIFORM BLOCKS
		auto block = Sp->GetUniformBlock("ColorBlock_0");
		float myFloats[3] = { 0.5f, 0.0f, 1.0f };
		block->set(myFloats, 3);
		float r = 0.5f;
		float g = 1.0f;
		float b = 0.0f;
		block->set(&r, 1, 0);
		block->set(&g, 1, 1);

		// UNIFORMS
		Sp->SetUniform("value1", 0.1f);
		Sp->SetUniform("value2", 0.1f);
		Sp->SetUniform("value3", 0.1f);

		// Uniform a = Sp->GetUniform("value1");
		a.set<float>(1.0f);
		Uniform b = Sp->GetUniform("value2");
		b.set<float>(0.0f);
		Uniform c = Sp->GetUniform("value3");
		c.set<float>(0.5f);

		Shader_Gbuffer->SetUniform("model", model.getModel());

		Shader_Gbuffer->SetUniform("view", cam.getView());
		Shader_Gbuffer->SetUniform("projection", cam.getProjection());
		*/

		static float FOV = _camera->getFOV();
		static float NewFOV = 0.0f;

		static float ZNEAR = _camera->getZnear();
		static float N_ZNEAR = 0.0f;

		static float ZFAR = _camera->getZfar();;
		static float N_ZFAR = 0.0f;

		// IMGUI TEST
		ImGui::NewFrame();
		
		bool open;
		if(ImGui::Begin("ImGUI", &open, ImVec2(280, 380), 0.9f))
		{
			ImGui::Text("FPS: %f", Time.GetFPS());
			ImGui::Text("Time: %f", Time.GetTime());

			ImGui::Separator();

			ImGui::Text("CamPos: %f %f %f", _camera->getPosition().x, _camera->getPosition().y, _camera->getPosition().z);
			ImGui::Text("CamForward: %f %f %f", _camera->getForward().x, _camera->getForward().y, _camera->getForward().z);

			ImGui::Separator();

			ImGui::InputFloat("FOV", &FOV, 5.0f, 5.0f, 1);
			ImGui::SliderFloat("ZNEAR", &ZNEAR, 0.01f, 5.0f);
			ImGui::SliderFloat("ZFAR", &ZFAR, 1.0, 50.0f);

			ImGui::Separator();

			ImGui::Checkbox("SHOW NORMALS", &SHOW_NORMAL_QUAD);

			//ImGui::Text("Dear ImGui, %s", ImGui::GetVersion());

			if (GamePad1.IsConnected())
				ImGui::Text("CONNECTED");
			else
				ImGui::Text("NOT connected");

			//ImGui::Separator();
			if (GamePad1.GetButton(XGamePad::Buttons::A))
				ImGui::Text("[A] YES");
			else
				ImGui::Text("[A] NO");

			ImGui::Text("Left Analog Mag: %f", GamePad1.GetLeftAnalogMagnitude());
			ImGui::Text("Left Analog X: %f", GamePad1.GetLeftAnalogNormalized().x);
			ImGui::Text("Left Analog Y: %f", GamePad1.GetLeftAnalogNormalized().y);

			ImGui::Separator();

			ImGui::Text("Clock: %f", clock1->GetTimeLeft());
			if (ImGui::Button("Reset Clock at 5 sec"))
				clock1->Reset(5.0);
			ImGui::SameLine();
			if (ImGui::Button("Reset Clock at 3 sec"))
				clock1->Reset(3.0);
		
		}
		ImGui::End();
		
		ImGui::Render();
		
		if (fabs(FOV - NewFOV) > 0.01f)
		{
			NewFOV = FOV;
			_camera->updatePerspective(FOV, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
		}
		if (fabs(ZNEAR - N_ZNEAR) > 0.01f)
		{
			N_ZNEAR = ZNEAR;
			_camera->setZnear(ZNEAR);
		}
		if (fabs(ZFAR - N_ZFAR) > 0.01f)
		{
			N_ZFAR = ZFAR;
			_camera->setZfar(ZFAR);
		}

	}
}