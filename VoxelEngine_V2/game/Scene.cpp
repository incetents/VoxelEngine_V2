// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Scene.h"

#include "../imgui/imgui.h"


//#include <stdio.h>

//#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
//#pragma comment(lib, "legacy_stdio_definitions")
//#endif

//#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
//#include <GL/gl3w.h>    // Initialize with gl3wInit()
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
//#include <GL/glew.h>    // Initialize with glewInit()
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
//#include <glad/glad.h>  // Initialize with gladLoadGL()
//#else
//#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
//#endif

// Include glfw3.h after our OpenGL definitions
//#include <GLFW/glfw3.h> 



#include "../engine/window/window.h"
#include "../engine/input/Input.h"

#include "../engine/utilities/Loader.h"
#include "../engine/utilities/logger.h"
#include "../engine/utilities/stringUtil.h"

#include "../engine/opengl/FramebufferObject.h"
#include "../engine/opengl/Geometry.h"
#include "../engine/opengl/Mesh.h"
#include "../engine/opengl/Shader.h"
#include "../engine/opengl/Texture.h"

#include "../engine/math/Camera.h"
#include "../engine/math/Color.h"
#include "../engine/math/Transform.h"
#include "../engine/math/Vector2.h"
#include "../engine/math/Vector3.h"
#include "../engine/math/Vector4.h"

#include "../engine/modules/Entity.h"
#include "../engine/modules/Material.h"



#include <iostream>


namespace Vxl
{
	void Scene::setup()
	{
		_camera = new Camera(Vector3(0, 0, 0), Vector3(0, 0, 1), 0.01f, 50.0f);
		//_camera->setOrthographic(-15, 15, -15, 15);
		_camera->setPerspective(110.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
		_camera->update();

		// FBO
		_fbo = new FramebufferObject();
		_fbo->addTexture(new RenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
		_fbo->addTexture(new RenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
		_fbo->addTexture(new RenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
		_fbo->addDepth(SCREEN_WIDTH, SCREEN_HEIGHT);
		_fbo->unbind();

		_shader_gbuffer = ShaderProgram::m_database.Get("gbuffer");
		_shader_gbuffer_instancing = ShaderProgram::m_database.Get("gbuffer_instancing");
		_shader_passthrough = ShaderProgram::m_database.Get("passthrough");

		_tex = Texture::m_database.Get("beato");

		_mesh = new Mesh();

		Vector3 pos[] = {
			Vector3(-0.5f, -0.5f, -1.0f),
			Vector3(+0.5f, -0.5f, -1.0f),
			Vector3(+0.5f, +0.5f, -1.0f),
			Vector3(-0.5f, +0.5f, -1.0f)
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
				Transform t(Vec3(x * 1.2f, y * 1.2f, -3.0f));
				m_models.push_back(t.getModel());
			}
		}

		_mesh->m_instances = m_models;

		_mesh->Bind();

		// Entity
		_entity1 = new Entity();
		_entity1->SetMaterial(_shader_gbuffer_instancing);
		_entity1->m_material.SetTexture(_tex, Active_Texture::LEVEL0);
		_entity1->m_mesh = _mesh;
		//_entity1->m_transform.setScale(+0.5f);

		_entity2 = new Entity();
		_entity2->SetMaterial(_shader_gbuffer);
		_entity1->m_material.SetTexture(_tex, Active_Texture::LEVEL0);
		_entity2->m_mesh = Geometry::GetCube();
		_entity2->m_transform.setPosition(Vector3(+1.5f, 0, -3.0f));
		
		_entity3 = new Entity();
		_entity3->SetMaterial(_shader_gbuffer);
		_entity1->m_material.SetTexture(_tex, Active_Texture::LEVEL0);
		_entity3->m_mesh = Geometry::GetCube();
		_entity3->m_transform.setPosition(Vector3(-1.5f, 0, -3.0f));

		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				for (int z = -1; z <= 1; z++)
				{
					if (x == 0 && y == 0 && z == 0)
						continue;

					Entity* ent = new Entity();
					ent->SetMaterial(_shader_gbuffer);
					ent->m_mesh = Geometry::GetCube();
					
					ent->m_transform.setPosition(Vector3(x * 1.1f, y * 1.1f, z * 1.1f));

					if (x == 1 || y == 1 || z == 1)
					{
						ent->m_transform.setRotationX(45.0f);
						ent->m_transform.setRotationY(45.0f);
					}

					_cubes.push_back(ent);
				}
			}
		}

		

	}

	void Scene::update()
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

		_camera->update();
	}

	void Scene::draw()
	{
		glUtil::clearBuffer();
		glUtil::clearColor(Color3F(0.1f, 0.1f, 0.3f));

		_fbo->bind();
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


		_shader_gbuffer_instancing->Bind();
		_shader_gbuffer_instancing->SetUniform("camForward", _camera->getForward());
		_shader_gbuffer_instancing->SetUniform("viewProjection", _camera->getViewProjection());
		
		_entity1->Draw();

		
		_shader_gbuffer->Bind();
		_shader_gbuffer->SetUniform("camForward", _camera->getForward());
		_shader_gbuffer->SetUniform("viewProjection", _camera->getViewProjection());
		
		//auto test = _entity2->m_transform.getModel();
		//_shader_gbuffer->SetUniform("model", _entity2->m_transform.getModel());
		_entity2->Draw();
		
		//_shader_gbuffer->SetUniform("model", _entity3->m_transform.getModel());
		_entity3->Draw();
		


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
		_fbo->bindTexture(2, Active_Texture::LEVEL0);
		Geometry::GetFullQuad()->Draw();
		
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
		
		if(ImGui::Begin("TEST WINDOW"))
		{
			ImGui::Separator();

			ImGui::InputFloat("FOV", &FOV, 5.0f, 5.0f, 1);
			ImGui::SliderFloat("ZNEAR", &ZNEAR, 0.01f, 5.0f);
			ImGui::SliderFloat("ZFAR", &ZFAR, 1.0, 50.0f);

			ImGui::Separator();
			//ImGui::Text("Dear ImGui, %s", ImGui::GetVersion());
			//ImGui::Separator();
		
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