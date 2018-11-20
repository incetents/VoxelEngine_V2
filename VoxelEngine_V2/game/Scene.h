// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

static int SCREEN_WIDTH = 800;
static int SCREEN_HEIGHT = 800;

namespace Vxl
{
	class ShaderProgram;
	class FramebufferObject;
	class Camera;
	class Mesh;
	class Transform;
	class Entity;

	class Scene
	{
	private:
		ShaderProgram* _shader_gbuffer;
		ShaderProgram* _shader_gbuffer_instancing;
		ShaderProgram* _shader_passthrough;
		FramebufferObject * _fbo;
		Camera* _camera;
		Mesh* _mesh;
		Texture* _tex;

		Entity* _entity1;
		Entity* _entity2;
		Entity* _entity3;
		
		std::vector<Entity*> _cubes;

	public:
		void setup();

		void update();
		void draw();
	};
}