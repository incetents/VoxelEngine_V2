// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../engine/modules/Scene.h"

static int SCREEN_WIDTH = 1600;
static int SCREEN_HEIGHT = 900;

namespace Vxl
{
	class ShaderProgram;
	class FramebufferObject;
	//class Camera;
	class Mesh;
	class Transform;
	class XGamePad;
	class Texture;
	class Cubemap;
	class Material;
	class Clock;
	class GameObject;
	class LightObject;
	class CameraObject;
	class SkyboxObject;

	class Scene_Game : public Scene
	{
	public:
		// Assets Loaded
		ShaderProgram* _shader_gbuffer;
		ShaderProgram* _shader_gbuffer_no_model;
		ShaderProgram* _shader_passthrough;
		ShaderProgram* _shader_skybox;
		ShaderProgram* _shader_lines;
		ShaderProgram* _shader_billboard;
		// Non-material Shader
		ShaderProgram* _shader_colorPicker;
		ShaderProgram* _shader_showRenderTarget;

		Texture* _tex;
		Texture* _tex_crate;
		Texture* _tex_gridtest;
		Cubemap* _cubemap1;

		// Assets Created
		Mesh* _mesh;

		// Assets Automated
		//Camera* _camera;
		CameraObject* _cameraObject;

		FramebufferObject* _fbo;
		FramebufferObject* _fbo_colorpicker;

		Material* _material_gbuffer;
		Material* _material_gbuffer_no_model;
		Material* _material_passthrough;
		Material* _material_skybox;
		Material* _material_lines;
		Material* _material_billboard;

		bool ShowNormal_DEV = false;
		bool ShowDepth_DEV = false;

		int FBO_OVERRIDE = 0;

	public:
		void Setup() override;
		void Destroy() override;

		void Update() override;
		void UpdateFixed() override;
		void Draw() override;
		void DrawImGui() override;
	};
}