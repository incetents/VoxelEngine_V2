// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "../engine/modules/Scene.h"

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
	class Material;
	class Clock;

	class Scene_Game : public Scene
	{
	private:
		// Assets Loaded
		ShaderProgram* _shader_gbuffer;
		ShaderProgram* _shader_gbuffer_no_model;
		ShaderProgram* _shader_passthrough;
		ShaderProgram* _shader_skybox;
		ShaderProgram* _shader_debugLines;

		Texture* _tex;
		Texture* _tex_crate;
		Cubemap* _cubemap1;

		// Assets Created
		FramebufferObject* _fbo;
		Mesh* _mesh;

		// Assets Automated
		Camera* _camera;

		Material* _material_gbuffer;
		Material* _material_gbuffer_no_model;
		Material* _material_passthrough;
		Material* _material_skybox;
		Material* _material_debugLines;

		Entity* _entity1;
		Entity* _entity2;
		Entity* _entity3;
		Entity* _entity4;

		Entity* _crate1;
		Entity* _crate2;

		Entity* _octo1;
		Entity* _octo2;
		Entity* _octo3;
		Entity* _octo4;
		
		std::vector<Entity*> _cubes;

		Clock* clock1;

	public:
		void Setup() override;
		void Destroy() override;

		void Update() override;
		void Draw() override;
	};
}