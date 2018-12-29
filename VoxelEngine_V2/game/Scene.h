// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

static int SCREEN_WIDTH = 1080;
static int SCREEN_HEIGHT = 720;

namespace Vxl
{
	class ShaderProgram;
	class FramebufferObject;
	class Camera;
	class Mesh;
	class Transform;
	class Entity;
	class XGamePad;
	class Texture;
	class Cubemap;
	class Light;
	class MaterialBase;

	class Scene
	{
	private:
		ShaderProgram* _shader_gbuffer;
		ShaderProgram* _shader_gbuffer_instancing;
		ShaderProgram* _shader_gbuffer_color;
		ShaderProgram* _shader_gbuffer_no_model;
		ShaderProgram* _shader_passthrough;
		ShaderProgram* _shader_skybox;

		MaterialBase* _material_gbuffer;
		MaterialBase* _material_gbuffer_instancing;
		MaterialBase* _material_gbuffer_color;
		MaterialBase* _material_gbuffer_no_model;
		MaterialBase* _material_passthrough;
		MaterialBase* _material_skybox;

		FramebufferObject * _fbo;
		Camera* _camera;
		Mesh* _mesh;
		Texture* _tex;
		Texture* _tex_crate;
		Cubemap* _cubemap1;

		Entity* _entity1;
		Entity* _entity2;
		Entity* _entity3;
		Entity* _entity4;
		Entity* _entity5;
		
		std::vector<Entity*> _cubes;

	public:
		void setup();

		void update();
		void draw();
	};
}