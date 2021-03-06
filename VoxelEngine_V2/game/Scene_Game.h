// Copyright (c) 2020 Emmanuel Lajeunesse
#pragma once

#include "../engine/modules/Scene.h"

#include "../engine/rendering/Gizmo.h"

static int SCREEN_WIDTH = 1080;
static int SCREEN_HEIGHT = 720;

namespace Vxl
{
	class XGamePad;
	class Clock;

	class Scene_Game : public Scene
	{
	public:
		//	// Shader Assets
		//	ShaderIndex gbuffer_vert;
		//	ShaderIndex gbuffer_frag;
		//	ShaderIndex passthrough_vert;
		//	ShaderIndex passthrough_frag;
		//	ShaderIndex skybox_vert;
		//	ShaderIndex skybox_frag;
		//	ShaderIndex lines_vert;
		//	ShaderIndex lines_geom;
		//	ShaderIndex lines_frag;
		//	ShaderIndex colorPicker_frag;
		//	ShaderIndex gizmo_vert;
		//	ShaderIndex gizmo_frag;
		//	ShaderIndex showRenderTarget;
		//	ShaderIndex billboard_vert;
		//	ShaderIndex billboard_frag;
		//	ShaderIndex font_vert;
		//	ShaderIndex font_frag;
		//	// Shader Program Assets
		//	ShaderProgramIndex shader_gbuffer;
		//	ShaderProgramIndex shader_passthroughWorld;
		//	ShaderProgramIndex shader_skybox;
		//	ShaderProgramIndex shader_lines;
		//	ShaderProgramIndex shader_colorPicker;
		//	ShaderProgramIndex shader_gizmo;
		//	ShaderProgramIndex shader_showRenderTarget;
		//	ShaderProgramIndex shader_billboard;
		//	ShaderProgramIndex shader_font;
		// Shader Material Assets
		ShaderMaterialIndex sMat_gbuffer;
		ShaderMaterialIndex sMat_skybox;
		//ShaderMaterialIndex sMat_displayRenderTarget;

		// Materials
		MaterialIndex material_gbuffer = -1;
		MaterialIndex material_skybox = -1;
		//MaterialIndex material_displayRenderTarget = -1;


		//	MaterialIndex material_gbuffer_transparent;
		//	MaterialIndex material_passthroughWorld;
		//	MaterialIndex material_passthroughWorld_transparent;
		//	MaterialIndex material_lines;
		//	MaterialIndex material_colorPicker;
		//	MaterialIndex material_gizmo;
		//	MaterialIndex material_showRenderTarget;
		//	MaterialIndex material_billboard;
		//	MaterialIndex material_font;
		//MaterialIndex material_showNormals;

		// Textures
		TextureIndex tex_grid_test;
		TextureIndex tex_checkerboard;
		TextureIndex tex_beato;
		TextureIndex tex_crate_diffuse;
		TextureIndex cubemap_craterlake;
		// FBOS
		FramebufferObjectIndex fboIndex_gbuffer;
		FramebufferObjectIndex fboIndex_editor;
		FramebufferObjectIndex fboIndex_editor2;
		FramebufferObjectIndex fboIndex_colorpicker;
		FramebufferObjectIndex fboIndex_composite;
		//FramebufferObjectIndex fboIndex_showRenderTarget;
		// Render Textures/Bufers
		RenderTextureIndex fbotex_gbuffer_albedo;
		RenderTextureIndex fbotex_gbuffer_normal;
		RenderTextureIndex fbotex_gbuffer_reflection;
		RenderTextureDepthIndex fbotex_gbuffer_depth;

		RenderTextureIndex fbotex_editor_albedo;
		RenderBufferDepthIndex fbotex_editor_depth;

		RenderTextureIndex fbotex_colorPicker_albedo;
		RenderBufferDepthIndex fbotex_colorPicker_depth;

		RenderTextureIndex fbotex_composite_albedo;

		//RenderTextureIndex fbotex_showRenderTarget_albedo;

		// Cameras
		CameraIndex camera_main;
		CameraIndex camera_side;
		// Entities
		EntityIndex entity_skybox;
		EntityIndex entity_error_cube;
		EntityIndex entity_beato_cube;
		EntityIndex entity_beato_crate;
		EntityIndex entity_instance_beato;
		EntityIndex entity_jiggy;
		EntityIndex entity_arrowX;
		EntityIndex entity_arrowY;
		EntityIndex entity_arrowZ;

		// Assets Created
		MeshIndex mesh_manyQuads;
		MeshIndex mesh_jiggy;

		// Assets ?
		//Camera* _cameraObject;

		

		//FramebufferObject* _fbo_gbuffer;
		//FramebufferObject* _fbo_editor;
		//FramebufferObject* _fbo_colorpicker;
		//FramebufferObject* _fbo_composite;
		//FramebufferObject* _fbo_showRenderTarget;

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