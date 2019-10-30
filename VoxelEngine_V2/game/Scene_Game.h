// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../engine/modules/Scene.h"

#include "../engine/rendering/Gizmo.h"

static int SCREEN_WIDTH = 1280;
static int SCREEN_HEIGHT = 720;

namespace Vxl
{
	class ShaderProgram;
	class FramebufferObject;
	class Mesh;
	class Transform;
	class XGamePad;
	class Texture2D;
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
		// Real Assets
		Texture2DIndex tex_grid_test;
		Texture2DIndex tex_checkerboard;
		Texture2DIndex tex_beato;
		Texture2DIndex tex_crate_diffuse;
		Texture2DIndex tex_editor_camera;
		Texture2DIndex tex_editor_lightbulb;
		CubemapIndex cubemap_craterlake;

		// Assets Loaded

		// Assets Created
		Mesh* _mesh;

		// Assets Automated
		CameraObject* _cameraObject;

		FramebufferObject* _fbo_gbuffer;
		FramebufferObject* _fbo_editor;
		FramebufferObject* _fbo_colorpicker;
		FramebufferObject* _fbo_composite;
		FramebufferObject* _fbo_showRenderTarget;

		Material* material_skybox;
		Material* material_gbuffer;
		Material* material_transparent_gbuffer;
		Material* material_opaque_passthroughWorld;
		Material* material_opaque_billboard;
		Material* material_passthroughWorld;
		Material* material_billboard;
		Material* material_lines;
		Material* material_simpleLight;
		Material* material_colorPicker;
		Material* material_font;
		Material* material_showNormals;

		Gizmo gizmo;

		bool ShowNormal_DEV = false;
		bool ShowDepth_DEV = false;

		int renderTargetID = 0;

	public:
		void Setup() override;
		void Destroy() override;

		void Update() override;
		void UpdateFixed() override;
		void Draw() override;
	};
}