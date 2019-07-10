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
		Texture* _tex;
		Texture* _tex_crate;
		Texture* _tex_gridtest;
		Cubemap* _cubemap1;

		// Assets Created
		Mesh* _mesh;

		// Assets Automated
		//Camera* _camera;
		CameraObject* _cameraObject;

		FramebufferObject* _fbo_gbuffer;
		FramebufferObject* _fbo_editor;
		FramebufferObject* _fbo_colorpicker;

		Material* material_skybox;
		Material* material_gbuffer;
		Material* material_passthrough;
		Material* material_billboard;
		Material* material_lines;
		Material* material_simpleLight;
		Material* material_colorPicker;

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