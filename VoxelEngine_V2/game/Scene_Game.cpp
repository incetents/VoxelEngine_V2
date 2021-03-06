// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Scene_Game.h"

#include "../imgui/imgui.h"

#include "../engine/window/window.h"
#include "../engine/input/Input.h"
#include "../engine/input/XGamePad.h"

#include "../engine/utilities/Logger.h"
#include "../engine/utilities/stringUtil.h"
#include "../engine/utilities/Time.h"
#include "../engine/utilities/FileIO.h"
#include "../engine/utilities/Asset.h"

#include "../engine/rendering/FramebufferObject.h"
#include "../engine/rendering/Primitives.h"
#include "../engine/rendering/Mesh.h"
#include "../engine/rendering/Shader.h"
#include "../engine/rendering/Debug.h"
#include "../engine/rendering/UBO.h"
#include "../engine/rendering/Graphics.h"
#include "../engine/rendering/RenderManager.h"
#include "../engine/rendering/RenderBuffer.h"

#include "../engine/textures/Texture2D.h"
#include "../engine/textures/RenderTexture.h"
#include "../engine/textures/Cubemap.h"

#include "../engine/math/Color.h"
#include "../engine/math/Transform.h"
#include "../engine/math/Vector.h"
#include "../engine/math/MathCore.h"
#include "../engine/math/Lerp.h"
#include "../engine/math/Model.h"

#include "../engine/objects/LightObject.h"
#include "../engine/objects/Camera.h"
#include "../engine/objects/GameObject.h"
#include "../engine/objects/TextObject.h"

#include "../engine/modules/Material.h"

#include "../engine/window/window.h"

#include "../engine/editorGui/GUI_Viewport.h"

#include "../engine/editor/Editor.h"
#include "../engine/editorGui/GUI_ShaderErrors.h"
#include "../engine/editorGui/GUI_DevConsole.h"
#include "../engine/editorGui/GUI_Hierarchy.h"
#include "../engine/editorGui/GUI_Inspector.h"
#include "../engine/editorGui/GUI_Performance.h"



namespace Vxl
{
	void Scene_Game::Setup()
	{
		// Load Textures
		tex_grid_test = SceneAssets.loadTexture2D(
			"./assets/textures/grid_test.png",
			true,
			true,
			TextureWrapping::REPEAT,
			TextureFilter::LINEAR,
			TextureFormat::RGB8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::HIGH
			);
		tex_checkerboard = SceneAssets.loadTexture2D(
			"./assets/textures/checkerboard.jpg",
			true,
			true,
			TextureWrapping::REPEAT,
			TextureFilter::LINEAR,
			TextureFormat::RGB8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::HIGH
		);
		tex_beato = SceneAssets.loadTexture2D(
			"./assets/textures/beato.png",
			true,
			true,
			TextureWrapping::REPEAT,
			TextureFilter::LINEAR,
			TextureFormat::RGB8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::NONE
		);

		tex_crate_diffuse = SceneAssets.loadTexture2D(
			"./assets/textures/crate_diffuse.png",
			true,
			true,
			TextureWrapping::REPEAT,
			TextureFilter::LINEAR,
			TextureFormat::RGB8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::NONE
		);
		cubemap_craterlake = SceneAssets.loadCubemap(
			"./assets/cubemaps/craterlake_ft.tga",
			"./assets/cubemaps/craterlake_bk.tga",
			"./assets/cubemaps/craterlake_up.tga",
			"./assets/cubemaps/craterlake_dn.tga",
			"./assets/cubemaps/craterlake_rt.tga",
			"./assets/cubemaps/craterlake_lf.tga",
			false, true,
			TextureWrapping::CLAMP_STRETCH,
			TextureFilter::LINEAR,
			TextureFormat::RGB8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::NONE
		);

		// FBO Gbuffer
		{
			fboIndex_gbuffer = SceneAssets.createFramebuffer("gbuffer");
			FramebufferObject* fbo_gbuffer_ptr = Assets.getFramebufferObject(fboIndex_gbuffer);
			fbo_gbuffer_ptr->setSizeToViewportSize();

			// render targets
			fbotex_gbuffer_albedo = SceneAssets.createRenderTexture(
				"albedo",
				fbo_gbuffer_ptr->getWidth(), fbo_gbuffer_ptr->getHeight(),
				TextureFormat::SRGBA8, TexturePixelType::UNSIGNED_BYTE, false);

			fbotex_gbuffer_normal = SceneAssets.createRenderTexture(
				"normal",
				fbo_gbuffer_ptr->getWidth(), fbo_gbuffer_ptr->getHeight(),
				TextureFormat::RGBA16_SNORM, TexturePixelType::UNSIGNED_BYTE, false);
			Assets.getRenderTexture(fbotex_gbuffer_normal)->m_viewMode = RenderTexture::ViewMode::ABSOLUTE_VALUE;

			fbotex_gbuffer_reflection = SceneAssets.createRenderTexture(
				"reflection",
				fbo_gbuffer_ptr->getWidth(), fbo_gbuffer_ptr->getHeight(),
				TextureFormat::RGBA8, TexturePixelType::UNSIGNED_BYTE, false);

			fbotex_gbuffer_depth = SceneAssets.createRenderTextureDepth(
				fbo_gbuffer_ptr->getWidth(), fbo_gbuffer_ptr->getHeight(),
				TextureDepthFormat::DEPTH32
			);

			fbo_gbuffer_ptr->bind();
			fbo_gbuffer_ptr->setRenderTexture(0, fbotex_gbuffer_albedo);
			fbo_gbuffer_ptr->setRenderTexture(1, fbotex_gbuffer_normal);
			fbo_gbuffer_ptr->setRenderTexture(2, fbotex_gbuffer_reflection);
			fbo_gbuffer_ptr->setRenderTextureDepth(fbotex_gbuffer_depth);
			fbo_gbuffer_ptr->checkFBOStatus();
			fbo_gbuffer_ptr->unbind();
		}

		// FBO Editor Post
		{
			fboIndex_editor = SceneAssets.createFramebuffer("editor");
			FramebufferObject* fbo_editor_ptr = Assets.getFramebufferObject(fboIndex_editor);
			fbo_editor_ptr->setSizeToViewportSize();

			// render targets
			fbotex_editor_albedo = SceneAssets.createRenderTexture(
				"albedo",
				fbo_editor_ptr->getWidth(), fbo_editor_ptr->getHeight(),
				TextureFormat::SRGBA8, TexturePixelType::UNSIGNED_BYTE, false);

			fbotex_editor_depth = SceneAssets.createRenderBufferDepth(
				fbo_editor_ptr->getWidth(), fbo_editor_ptr->getHeight(),
				TextureDepthFormat::DEPTH32
			);

			fbo_editor_ptr->bind();
			fbo_editor_ptr->setRenderTexture(0, fbotex_editor_albedo);
			fbo_editor_ptr->setRenderBufferDepth(fbotex_editor_depth);
			fbo_editor_ptr->checkFBOStatus();
			fbo_editor_ptr->unbind();
		}

		// FBO Color Picker
		{
			fboIndex_colorpicker = SceneAssets.createFramebuffer("colorPicker");
			FramebufferObject* fbo_colorPicker_ptr = Assets.getFramebufferObject(fboIndex_colorpicker);
			fbo_colorPicker_ptr->setSizeToViewportSize();

			// render targets
			fbotex_colorPicker_albedo = SceneAssets.createRenderTexture(
				"albedo",
				fbo_colorPicker_ptr->getWidth(), fbo_colorPicker_ptr->getHeight(),
				TextureFormat::RGBA8, TexturePixelType::UNSIGNED_BYTE, false);
			// Default view mode
			Assets.getRenderTexture(fbotex_colorPicker_albedo)->m_viewMode = RenderTexture::ViewMode::COLORFUL;

			fbotex_colorPicker_depth = SceneAssets.createRenderBufferDepth(
				fbo_colorPicker_ptr->getWidth(), fbo_colorPicker_ptr->getHeight(),
				TextureDepthFormat::DEPTH32
			);

			fbo_colorPicker_ptr->bind();
			fbo_colorPicker_ptr->setRenderTexture(0, fbotex_colorPicker_albedo);
			fbo_colorPicker_ptr->setRenderBufferDepth(fbotex_colorPicker_depth);
			fbo_colorPicker_ptr->checkFBOStatus();
			fbo_colorPicker_ptr->unbind();
		}

		// FBO Composite
		{
			fboIndex_composite = SceneAssets.createFramebuffer("composite");
			FramebufferObject* fbo_composite_ptr = Assets.getFramebufferObject(fboIndex_composite);
			fbo_composite_ptr->setSizeToViewportSize();

			// render targets
			fbotex_composite_albedo = SceneAssets.createRenderTexture(
				"albedo",
				fbo_composite_ptr->getWidth(), fbo_composite_ptr->getHeight(),
				TextureFormat::RGBA8, TexturePixelType::UNSIGNED_BYTE, false);

			fbo_composite_ptr->bind();
			fbo_composite_ptr->setRenderTexture(0, fbotex_composite_albedo);
			fbo_composite_ptr->checkFBOStatus();
			fbo_composite_ptr->unbind();
		}

		// Shader Materials
		sMat_skybox = SceneAssets.createShaderMaterial("./assets/materials/skybox.material");
		sMat_gbuffer = SceneAssets.createShaderMaterial("./assets/materials/gbuffer.material");

		// Materials
		material_skybox = SceneAssets.createMaterial("skybox");
		{
			auto mat = Assets.getMaterial(material_skybox);
			mat->setShaderMaterial(sMat_skybox);
			mat->setTexture(cubemap_craterlake, TextureLevel::LEVEL0);
			mat->setSequenceID(1);
			mat->m_blendFunc.source = BlendSource::ONE;
			mat->m_blendFunc.destination = BlendDestination::ZERO;
		}
		material_gbuffer = SceneAssets.createMaterial("gbuffer");
		{
			auto mat = Assets.getMaterial(material_gbuffer);
			mat->setShaderMaterial(sMat_gbuffer);
			mat->m_sharedTextures = false;
			mat->setSequenceID(2);
			mat->m_blendFunc.source = BlendSource::ONE;
			mat->m_blendFunc.destination = BlendDestination::ZERO;
		}
		//	material_displayRenderTarget = SceneAssets.createMaterial("displayRenderTarget");
		//	{
		//		auto mat = Assets.getMaterial(material_displayRenderTarget);
		//		mat->setShaderMaterial(sMat_displayRenderTarget);
		//		mat->setSequenceID(10000);
		//		mat->m_blendFunc.source = BlendSource::ONE;
		//		mat->m_blendFunc.destination = BlendDestination::ZERO;
		//	}

		//	material_gbuffer_transparent = SceneAssets.createMaterial("gbuffer_transparent");
		//	{
		//		auto mat = Assets.getMaterial(material_gbuffer_transparent);
		//		mat->setSceneShader(shader_gbuffer);
		//		mat->m_renderMode = MaterialRenderMode::Transparent;
		//		mat->m_depthWrite = false;
		//		mat->m_blendFunc.source = BlendSource::ONE;
		//		mat->m_blendFunc.destination = BlendDestination::ONE;
		//		// ignore color ID layer
		//		mat->m_blendFuncAttachments[TextureLevel::LEVEL2].source = BlendSource::ONE;
		//		mat->m_blendFuncAttachments[TextureLevel::LEVEL2].destination = BlendDestination::ONE;
		//	}
		//	material_passthroughWorld = SceneAssets.createMaterial("passthrough");
		//	{
		//		auto mat = Assets.getMaterial(material_passthroughWorld);
		//		mat->setSceneShader(shader_passthroughWorld);
		//		mat->m_blendState = false;
		//	}
		//	material_passthroughWorld_transparent = SceneAssets.createMaterial("passthrough_transparent");
		//	{
		//		auto mat = Assets.getMaterial(material_passthroughWorld_transparent);
		//		mat->setSceneShader(shader_passthroughWorld);
		//		mat->m_depthWrite = false;
		//		mat->m_blendFunc.source = BlendSource::ONE;
		//		mat->m_blendFunc.destination = BlendDestination::ONE;
		//		mat->m_renderMode = MaterialRenderMode::Transparent;
		//	
		//	}
		
		//	material_lines = SceneAssets.createMaterial("lines");
		//	{
		//		auto mat = Assets.getMaterial(material_lines);
		//		mat->setSceneShader(shader_lines);
		//	}
		//	material_colorPicker = SceneAssets.createMaterial("colorPicker");
		//	{
		//		auto mat = Assets.getMaterial(material_colorPicker);
		//		mat->setSceneShader(shader_colorPicker);
		//	}
		//	material_gizmo = SceneAssets.createMaterial("gizmo");
		//	{
		//		auto mat = Assets.getMaterial(material_gizmo);
		//		mat->setSceneShader(shader_gizmo);
		//	}
		//	material_showRenderTarget = SceneAssets.createMaterial("showRenderTarget");
		//	{
		//		auto mat = Assets.getMaterial(material_showRenderTarget);
		//		mat->setSceneShader(shader_showRenderTarget);
		//	}
		//	material_billboard = SceneAssets.createMaterial("billboard");
		//	{
		//		auto mat = Assets.getMaterial(material_billboard);
		//		mat->setSceneShader(shader_billboard);
		//		mat->m_blendState = false;
		//	}
		//	material_font = SceneAssets.createMaterial("font");
		//	{
		//		auto mat = Assets.getMaterial(material_font);
		//		mat->setSceneShader(shader_font);
		//		mat->m_depthRead = false;
		//		mat->m_depthWrite = false;
		//	}

		//material_skybox = Material::Create("skybox", 0);
		//material_skybox->SetProgram(*_shader_skybox);

		// Gbuffer Materials [Should not do any blending]

		//	material_gbuffer = Material::Create("gbuffer", 1);
		//	material_gbuffer->SetProgram(*_shader_gbuffer);
		//	material_gbuffer->m_BlendState = false;
		//	
		//	material_transparent_gbuffer = Material::Create("transparent_gbuffer", 1000);
		//	material_transparent_gbuffer->SetProgram(*_shader_gbuffer);
		//	material_transparent_gbuffer->SetRenderMode(MaterialRenderMode::Transparent);
		//	material_transparent_gbuffer->m_DepthWrite = false;
		//	material_transparent_gbuffer->m_BlendFunc.source = BlendSource::ONE;
		//	material_transparent_gbuffer->m_BlendFunc.destination = BlendDestination::ONE;
		//	// Color ID still passes normally by ignoring blend mode
		//	material_transparent_gbuffer->SetBlendFuncAttachment(3, BlendSource::ONE, BlendDestination::ZERO);
		//	
		//	material_opaque_passthroughWorld = Material::Create("opaque_passthroughWorld", 2);
		//	material_opaque_passthroughWorld->SetProgram(*_shader_passthroughWorld);
		//	material_opaque_passthroughWorld->m_BlendState = false;
		//	
		//	material_opaque_billboard = Material::Create("opaque_billboard", 3);
		//	material_opaque_billboard->SetProgram(*_shader_billboard);
		//	material_opaque_billboard->m_BlendState = false;
		//	
		//	material_passthroughWorld = Material::Create("transparent_passthroughWorld", 1001);
		//	material_passthroughWorld->SetProgram(*_shader_passthroughWorld);
		//	material_passthroughWorld->SetRenderMode(MaterialRenderMode::Transparent);
		//	material_passthroughWorld->m_DepthWrite = false;
		//	material_passthroughWorld->m_BlendFunc.source = BlendSource::ONE;
		//	material_passthroughWorld->m_BlendFunc.destination = BlendDestination::ONE;
		//	// Color ID still passes normally by ignoring blend mode
		//	material_passthroughWorld->SetBlendFuncAttachment(3, BlendSource::ONE, BlendDestination::ZERO);
		//	
		//	material_billboard = Material::Create("billboard", 3);
		//	material_billboard->SetProgram(*_shader_billboard);
		//	
		//	material_lines = Material::Create("lines", 4);
		//	material_lines->SetProgram(*_shader_lines);
		//	
		//	material_simpleLight = Material::Create("gizmo", 5);
		//	material_simpleLight->SetProgram(*_shader_gizmo);
		//	
		//	material_colorPicker = Material::Create("colorPicker", 6);
		//	material_colorPicker->SetProgram(*_shader_colorPicker);
		//	material_colorPicker->m_BlendState = false;
		//	
		//	material_showNormals = Material::Create("showNormals", 20);
		//	material_showNormals->SetProgram(*_shader_showNormals);
		//	material_showNormals->m_BlendState = false;
		//	
		//	material_font = Material::Create("font", 100);
		//	material_font->SetProgram(*_shader_font);
		//	material_font->m_DepthRead = false;
		//	material_font->m_DepthWrite = false;

		// Voxel Stuff
		//BlockAtlas.Set(Texture::Get("TextureAtlas"), 16); // texture of all blocks
		//BlockDictionary.Setup(); // info of all blocks
		//TerrainManager.Setup(); // keeps track of terrain info
		//

		// Import Mesh
		mesh_jiggy = Model::LoadMesh("jiggy", "./assets/models/jiggy.obj", true, 0.2f);

		mesh_manyQuads = SceneAssets.createMesh(DrawType::TRIANGLES);
		Mesh* _mesh = Assets.getMesh(mesh_manyQuads);
		
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
		uint32_t indices[6] = { 0, 1, 2, 0, 2, 3 };
		
		_mesh->m_positions = std::vector<Vector3>(pos, pos + 4);
		_mesh->m_uvs = std::vector<Vector2>(uvs, uvs + 4);
		_mesh->m_indices = std::vector<uint32_t>(indices, indices + 6);

		_mesh->generateNormals(false);
		_mesh->generateTangents();
		
		std::vector<Matrix4x4> m_models;
		for (float x = 0; x < 5.0f; x++)
		{
			for (float y = 0; y < 5.0f; y++)
			{
				Transform t(Vec3(x * 1.2f, y * 1.2f, 1.0f));
				m_models.push_back(t.getModel().Transpose());
			}
		}
		
		_mesh->m_instances = m_models;
		
		_mesh->bind();
		
		// Camera
		camera_main = SceneAssets.createCamera("_camera_editor", 0.01f, 5000.0f);
		{
			Camera* camera_ptr = Assets.getCamera(camera_main);
			camera_ptr->m_transform.setPosition(2.635f, 2.8f, 2.3f);
			camera_ptr->m_transform.setCameraForward(Vector3(-1, 0, -1));
			camera_ptr->SetPerspectiveWindowAspect(110.0f);
			camera_ptr->update();
		}
		camera_side = SceneAssets.createCamera("_camera_side", 0.5, 10.0f);
		{
			Camera* camera_ptr = Assets.getCamera(camera_side);
			camera_ptr->m_transform.setPosition(2.9f, 2.1f, -2.5f);
			camera_ptr->m_transform.setForward(Vector3(0, 0, 1));
			camera_ptr->SetPerspectiveWindowAspect(140.0f);
			camera_ptr->update();
		}

		// Main Camera
		RenderManager.m_mainCamera = camera_main;

		// Entities
		entity_skybox = SceneAssets.createEntity("_skybox");
		{
			Entity* entity_ptr = Assets.getEntity(entity_skybox);
			entity_ptr->setMaterial(material_skybox);
			entity_ptr->setMesh(Primitives.GetInverseCube());
			entity_ptr->m_useTransform = false;
			entity_ptr->m_isSelectable = false;
		}
		entity_error_cube = SceneAssets.createEntity("_errorCube");
		{
			Entity* entity_ptr = Assets.getEntity(entity_error_cube);
			entity_ptr->setMaterial(material_gbuffer);
			entity_ptr->setMesh(Primitives.GetCube());
			entity_ptr->m_transform.setPosition(Vector3(0, 2, 0));
			entity_ptr->m_transform.setRotation(Vector3(-60.0f, -35.7f, 0.0f));
			entity_ptr->m_transform.setScaleY(2);
		}
		entity_beato_cube = SceneAssets.createEntity("_beato_cube");
		{
			Entity* entity_ptr = Assets.getEntity(entity_beato_cube);
			entity_ptr->setMaterial(material_gbuffer);
			entity_ptr->setMesh(Primitives.GetCube());
			entity_ptr->setTexture(tex_beato, TextureLevel::LEVEL0);
			entity_ptr->m_transform.setPosition(Vector3(-3, 0, 0));
			entity_ptr->m_transform.setRotation(Vector3(0, 0, 48.0f));
		
			entity_ptr->m_transform.setParent(&Assets.getEntity(entity_error_cube)->m_transform);
		}
		entity_beato_crate = SceneAssets.createEntity("_beato_crate");
		{
			Entity* entity_ptr = Assets.getEntity(entity_beato_crate);
			entity_ptr->setMaterial(material_gbuffer);
			entity_ptr->setMesh(Primitives.GetCube());
			entity_ptr->setTexture(tex_crate_diffuse, TextureLevel::LEVEL0);
			entity_ptr->m_transform.setPosition(Vector3(-3, 0, 0));
			entity_ptr->m_transform.setRotation(Vector3(0, 0, 48.0f));
		
			entity_ptr->m_transform.setParent(&Assets.getEntity(entity_beato_cube)->m_transform);
		}
		entity_jiggy = SceneAssets.createEntity("_jiggy");
		{
			Entity* entity_ptr = Assets.getEntity(entity_jiggy);
			entity_ptr->setMaterial(material_gbuffer);
			entity_ptr->setMesh(mesh_jiggy);
			entity_ptr->m_Color = Color3F(1, 1, 0);
			entity_ptr->m_useTextures = false;
			entity_ptr->m_transform.setPosition(Vector3(0, 0, -7));
		
			
		}
		entity_instance_beato = SceneAssets.createEntity("_instance_beato");
		{
			Entity* entity_ptr = Assets.getEntity(entity_instance_beato);
			entity_ptr->setMaterial(material_gbuffer);
			entity_ptr->setMesh(mesh_manyQuads);
			entity_ptr->setTexture(tex_beato, TextureLevel::LEVEL0);
			entity_ptr->m_transform.setPosition(Vector3(-2, 2, -6.6f));
		
			//entity_ptr->m_transform.setParent(&Assets.getEntity(entity_jiggy)->m_transform);
		}
		
		entity_arrowX = SceneAssets.createEntity("_arrowX");
		{
			Entity* entity_ptr = Assets.getEntity(entity_arrowX);
			entity_ptr->setMaterial(GlobalAssets.get_MaterialDebugRenderID());
			entity_ptr->setMesh(Primitives.GetArrowX());
			entity_ptr->m_Color = Color3F::RED;
			entity_ptr->m_useTextures = false;
			entity_ptr->m_transform.setPosition(Vector3(0, 3, 0));
		}
		entity_arrowY = SceneAssets.createEntity("_arrowY");
		{
			Entity* entity_ptr = Assets.getEntity(entity_arrowY);
			entity_ptr->setMaterial(GlobalAssets.get_MaterialDebugRenderID());
			entity_ptr->setMesh(Primitives.GetArrowY());
			entity_ptr->m_Color = Color3F::GREEN;
			entity_ptr->m_useTextures = false;
			entity_ptr->m_transform.setPosition(Vector3(0, 3, 0));
		}
		entity_arrowZ = SceneAssets.createEntity("_arrowZ");
		{
			Entity* entity_ptr = Assets.getEntity(entity_arrowZ);
			entity_ptr->setMaterial(GlobalAssets.get_MaterialDebugRenderID());
			entity_ptr->setMesh(Primitives.GetArrowZ());
			entity_ptr->m_Color = Color3F::BLUE;
			entity_ptr->m_useTextures = false;
			entity_ptr->m_transform.setPosition(Vector3(0, 3, 0));
		}
		
		

		//			_errorCube2->m_transform.setParent(&_errorCube->m_transform);
		//			
		//			GameObject* _entity1 = GameObject::Create("_entity1");
		//			_entity1->SetMaterial(material_gbuffer);
		//			_entity1->SetTexture(Assets.getTexture2D(tex_beato), TextureLevel::LEVEL0);
		//			_entity1->SetMesh(_mesh);
		//			_entity1->m_transform.setScale(1.0f);
		//			_entity1->m_transform.setPositionZ(-7.0f);
		//			
		//			//Loader::Load_Model("jiggy1", "./assets/models/jiggy.obj", false, true);
		//			Mesh* jiggyMesh = Assets.getMesh(mesh_jiggy);
		//			
		//			GameObject* _entity2 = GameObject::Create("_entity2");
		//			_entity2->SetMaterial(material_gbuffer);
		//			//_entity2->m_material.SetTexture(_tex_crate, ActiveTexture::LEVEL0);
		//			_entity2->SetMesh(jiggyMesh);// Primitives.GetIcoSphere();
		//			_entity2->m_transform.setPosition(Vector3(+1.5f, 0, -3.0f));
		//			_entity2->SetColor(Color3F(1, 1, 0));
		//			
		//			
		//			
		//			GameObject* _alphaCube1 = GameObject::Create("_alphaCube1");
		//			_alphaCube1->SetMaterial(material_transparent_gbuffer);
		//			_alphaCube1->SetColor(Color3F(1, 0, 0));
		//			_alphaCube1->SetAlpha(0.5f);
		//			_alphaCube1->SetMesh(Primitives.GetCube());// Primitives.GetIcoSphere();
		//			_alphaCube1->m_transform.setPosition(Vector3(-0.5f, -2, -3.0f));
		//			
		//			GameObject* _alphaCube2 = GameObject::Create("_alphaCube2");
		//			_alphaCube2->SetMaterial(material_transparent_gbuffer);
		//			_alphaCube2->SetColor(Color3F(0, 1, 0));
		//			_alphaCube2->SetAlpha(0.5f);
		//			_alphaCube2->SetMesh(Primitives.GetCube());// Primitives.GetIcoSphere();
		//			_alphaCube2->m_transform.setPosition(Vector3(-1.5f, -2, -3.0f));
		//			
		//			GameObject* _alphaCube3 = GameObject::Create("_alphaCube3");
		//			_alphaCube3->SetMaterial(material_transparent_gbuffer);
		//			_alphaCube3->SetColor(Color3F(0, 0, 1));
		//			_alphaCube3->SetAlpha(0.5f);
		//			_alphaCube3->SetMesh(Primitives.GetCube());// Primitives.GetIcoSphere();
		//			_alphaCube3->m_transform.setPosition(Vector3(-2.5f, -2, -3.0f));
		//			
		//			
		//			GameObject* _entity3 = GameObject::Create("_entity3");
		//			_entity3->SetTexture(Assets.getTexture2D(tex_crate_diffuse), TextureLevel::LEVEL0);
		//			_entity3->SetMaterial(material_gbuffer);
		//			_entity3->SetMesh(Primitives.GetIcosahedron());
		//			_entity3->m_transform.setPosition(Vector3(-2.5f, 0, -3.0f));
		//			
		//			
		//			
		//			GameObject* _entity5 = GameObject::Create("_entity5");
		//			_entity5->SetMaterial(material_gbuffer);
		//			_entity5->SetTexture(Assets.getTexture2D(tex_grid_test), TextureLevel::LEVEL0);
		//			_entity5->SetMesh(Primitives.GetSphereUV_Good());
		//			_entity5->m_transform.setPosition(Vector3(0, -4, 0));
		//			
		//			GameObject* _entity6 = GameObject::Create("_entity6");
		//			_entity6->SetMaterial(material_gbuffer);
		//			_entity6->SetTexture(Assets.getTexture2D(tex_grid_test), TextureLevel::LEVEL0);
		//			_entity6->SetMesh(Primitives.GetCube());
		//			_entity6->m_transform.setPosition(Vector3(5, -4, 0));
		//			
		//			GameObject* _entity6_b = GameObject::Create("_entity6_normals");
		//			_entity6_b->SetMaterial(material_showNormals);
		//			_entity6_b->SetTexture(Assets.getTexture2D(tex_grid_test), TextureLevel::LEVEL0);
		//			_entity6_b->SetMesh(Primitives.GetCube());
		//			_entity6_b->m_transform.setPosition(Vector3(5, -4, 0));
		//			
		//			
		//			GameObject* _entity7 = GameObject::Create("_entity7");
		//			_entity7->SetMaterial(material_gbuffer);
		//			_entity7->SetTexture(Assets.getTexture2D(tex_grid_test), TextureLevel::LEVEL0);
		//			_entity7->SetMesh(Primitives.GetQuadY());
		//			_entity7->m_transform.setPosition(Vector3(0, -10, 0));
		//			_entity7->m_transform.setScale(Vector3(20, 1, 20));
		//			//_entity5->SetColor(Color3F(1, 1, 1));
		//			
		//			
		//			
		//			
		//			//
		//			GameObject* _crate1 = GameObject::Create("_crate1");
		//			_crate1->SetMaterial(material_gbuffer);
		//			_crate1->SetTexture(Assets.getTexture2D(tex_crate_diffuse), TextureLevel::LEVEL0);
		//			_crate1->SetMesh(Primitives.GetCylinderX());
		//			_crate1->m_transform.setPosition(1, 3, 2);
		//			_crate1->SetTint(Color3F(0.4f, 0.1f, 0.9f));
		//			
		//			
		//			GameObject* _crate2 = GameObject::Create("_crate2");
		//			_crate2->SetMaterial(material_gbuffer);
		//			_crate2->SetTexture(Assets.getTexture2D(tex_crate_diffuse), TextureLevel::LEVEL0);
		//			_crate2->SetMesh(Primitives.GetCylinderY());
		//			//_crate2->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		//			_crate2->m_transform.setPosition(0, 2, 0);
		//			
		//			
		//			GameObject* _crate3 = GameObject::Create("_crate3");
		//			_crate3->SetMaterial(material_gbuffer);
		//			_crate3->SetTexture(Assets.getTexture2D(tex_crate_diffuse), TextureLevel::LEVEL0);
		//			_crate3->SetMesh(Primitives.GetCylinderZ());
		//			//_crate3->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		//			_crate3->m_transform.setPosition(1, 2, 0);
		//			
		//			// Parent Test
		//			_crate3->m_transform.addChild(&_crate2->m_transform);
		//			_crate2->m_transform.addChild(&_crate1->m_transform);
		//			
		//			_crate1->m_transform.setRotation(45, -45, 0);
		//			_crate2->m_transform.setRotation(65, 12, 3);
		//			_crate3->m_transform.setRotation(45, 12, 3);
		//			// World Position
		//			_crate1->m_transform.setWorldPosition(-5, -5, 0);
		//			_crate1->m_transform.increaseWorldPosition(1, 2, 3);
		//			
		//			GameObject* _octo1 = GameObject::Create("_octo1");
		//			_octo1->SetMaterial(material_gbuffer);
		//			_octo1->SetMesh(Primitives.GetOctahedron());
		//			_octo1->m_transform.setPosition(0, 0, 0);
		//			_octo1->m_transform.setScale(0.5f);
		//			_octo1->SetColor(Color3F(1, 1, 1));
		//			
		//			GameObject* _octo2 = GameObject::Create("_octo2");
		//			_octo2->SetMaterial(material_gbuffer);
		//			_octo2->SetMesh(Primitives.GetOctahedron());
		//			_octo2->m_transform.setPosition(1, 0, 0);
		//			_octo2->m_transform.setScale(0.5f);
		//			_octo2->SetColor(Color3F(1, 0, 0));
		//			
		//			GameObject* _octo3 = GameObject::Create("_octo3");
		//			_octo3->SetMaterial(material_gbuffer);
		//			_octo3->SetMesh(Primitives.GetOctahedron());
		//			_octo3->m_transform.setPosition(0, 1, 0);
		//			_octo3->m_transform.setScale(0.5f);
		//			_octo3->SetColor(Color3F(0, 1, 0));
		//			
		//			GameObject* _octo4 = GameObject::Create("_octo4");
		//			_octo4->SetMaterial(material_gbuffer);
		//			_octo4->SetMesh(Primitives.GetOctahedron());
		//			_octo4->m_transform.setPosition(0, 0, 1);
		//			_octo4->m_transform.setScale(0.5f);
		//			_octo4->SetColor(Color3F(0, 0, 1));
		//			
		//			LightObject* _light1 = LightObject::Create("_light1", Light::Type::POINT);
		//			_light1->m_transform.setPosition(5, 0, 0);
		//			
		//			GameObject* _billboard1 = GameObject::Create("_quad1");
		//			_billboard1->SetMaterial(material_opaque_billboard);
		//			_billboard1->SetTexture(Assets.getTexture2D(tex_beato), TextureLevel::LEVEL0);
		//			_billboard1->SetMesh(Primitives.GetQuadZ());
		//			_billboard1->m_transform.setPosition(7, 3, -3);
		//			
		//			TextObject::LoadFont("arial", "assets/fonts/arial.ttf");
		//			
		//			TextObject* _text1 = TextObject::Create("_text1");
		//			_text1->m_transform.setPosition(-2, -3, 0);
		//			_text1->SetFont("arial");
		//			_text1->SetText("abcdefghijkl\nmnopqrstuvwxyz\nquick brown fox jumped over the moon\n12345678900987654321");
		//			
		//			//_text1->SetTexture(Texture::GetAsset("beato"), TextureLevel::LEVEL0);
		//			
		//			//material_gbuffer
		//			//material_billboard
		//			
		//			//GameObject::Delete(_entity5);
		//			//GameObject::Delete("_octo3");
		//			
		//			//_light1 = LightObject::Create("light1");


		
	}
	void Scene_Game::Destroy()
	{
		//TerrainManager.Destroy();
	}

	void Scene_Game::Update()
	{
		DEV_SHOW_BOOL("show_bool", false);
		DEV_SHOW_FLOAT("show_float", 5.0f);
		DEV_SHOW_DOUBLE("show_float", 8.0);
		DEV_SHOW_VECTOR("show_vec3", Vector3(1,2,3));
		DEV_SHOW_VECTOR("show_vec3i", Vector3i(1,2,3));
		DEV_SHOW_VECTOR("show_vec3d", Vector3d(1,2,3));
		DEV_SHOW_VECTOR("show_vec4", Vector4(10,20,30,40));
		DEV_SHOW_VECTOR("show_vec4i", Vector4i(190, 2, 3, 4));
		DEV_SHOW_VECTOR("show_vec4d", Vector4d(190, 2, 3, 4));
		DEV_SHOW_COLOR("show_color4", Color4F(1,0,0,1));
		
		DEV_GET_BOOL("edit_bool", false);
		DEV_GET_FLOAT("edit_float", 2.0f);
		DEV_GET_DOUBLE("edit_double", 2.0);
		DEV_GET_FLOAT_RANGE("edit_float_range", 2.0f, -10.0f, 10.0f);
		DEV_GET_DOUBLE_RANGE("edit_double_range", 1.0, 0.0, 2.0);
		DEV_GET_VECTOR("edit_vec3", vec3(0, 1, 2));
		DEV_GET_VECTOR("edit_vec3i", vec3i(0, 1, 2));
		DEV_GET_VECTOR("edit_vec3d", vec3d(0, 1, 2));
		DEV_GET_VECTOR("edit_vec4", vec4(0, 1, 2, 3));
		DEV_GET_VECTOR("edit_vec4i", vec4i(0, 1, 2, 3));
		DEV_GET_VECTOR("edit_vec4d", vec4d(0, 1, 2, 3));
		DEV_GET_COLOR("edit_color4f", Color4F(0, 1, 0.5f, 1));

		CPUTimer::StartTimer("UPDATE");

		if (Input.getKeyDown(KeyCode::ESCAPE))
			Window.Close();

		if (DEV_GET_BOOL("Camera Keyboard Controls", true))
		{
			// Camera Movement
			Vector3 CamMove;
			float CamSpeed = 0.1f;

			Camera* _cameraObject = Assets.getCamera(camera_main);

			if (Input.getKey(KeyCode::W))
				CamMove += _cameraObject->m_transform.getCameraForward();
			if (Input.getKey(KeyCode::S))
				CamMove += _cameraObject->m_transform.getCameraBackwards();
			if (Input.getKey(KeyCode::A))
				CamMove += _cameraObject->m_transform.getLeft();
			if (Input.getKey(KeyCode::D))
				CamMove += _cameraObject->m_transform.getRight();

			if (Input.getKey(KeyCode::SPACE))
				CamMove += Vector3::UP;
			if (Input.getKey(KeyCode::LEFT_CONTROL))
				CamMove += Vector3::DOWN;

			if(CamMove.LengthSqr() > FLT_EPSILON)
				_cameraObject->m_transform.increasePosition(CamMove.Normalize() * CamSpeed);

			// Camera Rotation
			Vector3 CamRotation;
			float CamRotationSpeed = 2.25f;

			if (Input.getKey(KeyCode::UP))
				CamRotation.x -= CamRotationSpeed;
			if (Input.getKey(KeyCode::DOWN))
				CamRotation.x += CamRotationSpeed;
			if (Input.getKey(KeyCode::LEFT))
				CamRotation.y -= CamRotationSpeed;
			if (Input.getKey(KeyCode::RIGHT))
				CamRotation.y += CamRotationSpeed;

			if (CamRotation.LengthSqr() > FLT_EPSILON)
				_cameraObject->m_transform.increaseRotation(CamRotation);

			// Camera Lock
			if (Input.getKeyDown(KeyCode::Z))
			{
				if (Window.GetCursor() == CursorMode::LOCKED)
					Window.SetCursor(CursorMode::NORMAL);
				else
					Window.SetCursor(CursorMode::LOCKED);
			}

			float CameraHorizontalRotation = DEV_GET_FLOAT("Camera Horizontal Rotation", 0.2f);
			float CameraVerticalRotation = DEV_GET_FLOAT("Camera Vertical Rotation", 0.2f);

			// Lock = mouse rotates camera
			if (Window.GetCursor() == CursorMode::LOCKED)
			{
				float deltax = Input.getMouseDeltaX() * (float)Time.GetDeltaTime() * CameraVerticalRotation;
				float deltay = Input.getMouseDeltaY() * (float)Time.GetDeltaTime() * CameraHorizontalRotation;
				// clamp delta for safety
				deltax = MacroClamp(deltax, -100, +100);
				deltay = MacroClamp(deltay, -100, +100);

				if(fabs(deltax) > FLT_EPSILON || fabs(deltay) > FLT_EPSILON)
					_cameraObject->m_transform.increaseRotation(deltay, deltax, 0);
			}

			// Edge case for vertical rotation
			float OrigXrot = _cameraObject->m_transform.getRotationEuler().x;
			float Xrot = MacroClamp(_cameraObject->m_transform.getRotationEuler().x, -89.9f, 89.9f);
			if(fabs(OrigXrot - Xrot) > FLT_EPSILON)
				_cameraObject->m_transform.setRotationX(Xrot);
			//

			// Update Cam
			_cameraObject->update();
		}


		// Debug Lines (screen space)
		//	auto t = Time.GetTime() / 2.0;
		//	Debug.DrawScreenSpaceLine(
		//		Vector2::ZERO, Vector2(sinf(t), cosf(t)),
		//		4.0f,
		//		Color4F::RED, Color4F::BLUE
		//	);

		//	// Selection
		//	auto SelectedEntities = Editor.m_selectedEntities;
		//	for (const auto& _Entity : SelectedEntities)
		//	{
		//		if (_Entity->m_isSelectable)
		//			_Entity->DrawSelection();
		//	}

		// Debug Lines
		Debug.DrawLine(
			Vector3(-1, -1, -1), Vector3(+1, +1, -1),
			10.0f,
			Color3F(0, 1, 0), Color3F(1, 0, 1)
		);
		static float time = 0.0f;
		time += 0.2f;
		Debug.DrawLine(
			Vector3(+3, +1, -1), Vector3(+3, +4 + cosf(time), -1),
			10.0f,
			Color3F(1, 1, 0), Color3F(0, 1, 1)
		);
		Debug.DrawLineNoDepth(
			Vector3(+3, +1, -1), Vector3(+3, +4 + cosf(time), -1),
			10.0f,
			Color3F(0, 0, 0), Color3F(0, 0, 0)
		);
		
		//	Debug.DrawLineSquareScreenSpace(
		//		Vector2(0, 0), Vector2(1, 1),
		//		1.0f,
		//		Color3F(1, 0, 0)
		//	);

		Debug.DrawLineCube(
			Vector3(4, 0,0),
			5.0f,
			Vector3(1,1,1),
			Vector3(time, 0, time*2.0f),
			Color3F::PURPLE
		);
		Debug.DrawLineSphere(
			Vector3(-4, 0, 0),
			5.0f,
			Vector3(cosf(Time.GetTimef()) + 2),
			Color3F::YELLOW
		);


		Debug.DrawCube(Vector3(4, 0, 0), Vector3::ONE, Vector3::ZERO, Color3F::BLUE);
		Debug.DrawSphere(Vector3(-4, 0, 0), Vector3::ONE, Vector3::ZERO, Color3F::RED);

		Debug.DrawLineArrow(Vector3(0, 0, 0), Vector3(sinf(Time.GetTimef()), sinf(Time.GetTimef() * 0.5f) * 2.0f * 1, cosf(Time.GetTimef())), 5.0f, 2.0f, Color3F::RED);

		// Draw Jiggy outline
		//	Entity* jiggy_entity = Assets.getEntity(entity_jiggy);
		//	if (jiggy_entity)
		//	{
		//		// AABB
		//		Debug.DrawLineAABB(
		//			jiggy_entity->GetAABB(),
		//			2.0f,
		//			Color3F::RED
		//		);
		//		// OBB
		//		Debug.DrawLineOBB(jiggy_entity->GetOBB(), 5.0f, Color3F::BLUE);
		//	//
		//	}

		// Draw Outline for all Selected Objects
		for (SceneNodeIndex sceneNodeIndex : Editor.m_selectedNodes)
		{
			SceneNode* sceneNode = Assets.getSceneNode(sceneNodeIndex);
			// Ignore if non selectable
			if (!sceneNode->m_isSelectable)
				continue;
			//
			switch (sceneNode->getType())
			{
			//
			case SceneNodeType::ENTITY:
			{
				Entity* entity = sceneNode->getEntity();
				// Instanced
				Mesh* mesh = Assets.getMesh(entity->getMesh());
				if (mesh->m_instances.getDrawCount() > 0)
				{
					// Base Directions
					Vector3 right = entity->m_transform.getRight();
					Vector3 up = entity->m_transform.getUp();
					Vector3 forward = entity->m_transform.getForward();

					for (const Matrix4x4& instance : mesh->m_instances.vertices)
					{
						Mesh* _mesh = Assets.getMesh(entity->getMesh());

						// Base model
						Matrix4x4 model = entity->m_transform.getModel() * instance.Transpose();
						// Rotation/Direction
						Matrix3x3 normal = Matrix3x3(instance).Inverse();
						
						// New Calculated OBB
						OBB instanceOBB = OBB(
							model,
							normal * right,
							normal * up,
							normal * forward,
							_mesh->getVertexMin(),
							_mesh->getVertexMax()
						);

						// OBB
						Debug.DrawLineOBB(instanceOBB, 5.0f, Color3F::BLUE);
						// AABB
						Debug.DrawLineAABB(instanceOBB.generateAABB(), 2.0f, Color3F::RED);
					}
				}
				// Non-instance
				else
				{
					// OBB
					Debug.DrawLineOBB(entity->GetOBB(), 5.0f, Color3F::BLUE);
					// AABB
					Debug.DrawLineAABB(entity->GetAABB(), 2.0f, Color3F::RED);

					// Good Axis
					Debug.DrawLine(entity->m_transform.getWorldPosition(), entity->m_transform.getWorldPosition() + entity->m_transform.getRight() * 5.0f, 5.0f, Color3F::RED, Color3F::RED);
					Debug.DrawLine(entity->m_transform.getWorldPosition(), entity->m_transform.getWorldPosition() + entity->m_transform.getUp() * 5.0f, 5.0f, Color3F::GREEN, Color3F::GREEN);
					Debug.DrawLine(entity->m_transform.getWorldPosition(), entity->m_transform.getWorldPosition() + entity->m_transform.getForward() * 5.0f, 5.0f, Color3F::BLUE, Color3F::BLUE);
				}
				break;
			}
			//
			}
		}


		// End Frame Updates
		

		// Imgui Specials
		//	if (material_gbuffer)
		//	{
		//		material_gbuffer->m_Wireframe = DEVCONSOLE_GET_BOOL("Gbuffer Wireframe", false);
		//	}

		CPUTimer::EndTimer("UPDATE");

		gizmo.m_pivotAxisAligned = DEV_GET_BOOL("PivotAxisAligned", false);
		gizmo.m_translateSnapping = DEV_GET_BOOL("TranslateSnapping", false);
		gizmo.m_rotateSnapping = DEV_GET_BOOL("RotateSnapping", false);

		// GizmoMode
		if (Input.getKeyDown(KeyCode::Num_1))
			gizmo.m_mode = Gizmo::Mode::TRANSLATE;
		if (Input.getKeyDown(KeyCode::Num_2))
			gizmo.m_mode = Gizmo::Mode::SCALE;
		if (Input.getKeyDown(KeyCode::Num_3))
			gizmo.m_mode = Gizmo::Mode::ROTATE;

		// Gizmo
		std::vector<SceneNode*> selectedNodes;
		selectedNodes.reserve(Editor.m_selectedNodes.size());
		for (const auto& id : Editor.m_selectedNodes)
		{
			selectedNodes.push_back(Assets.getSceneNode(id));
		}

		gizmo.Update(selectedNodes);

		// Delete Selection
		if (Window.GetWindowFocused())
		{
			if (ImGui::GetIO().KeysDown[(int)KeyCode::DELETEKEY] || ImGui::GetIO().KeysDown[(int)KeyCode::BACKSPACE])
			{
				Editor.deleteSelection();
			}
		}
	}

	// DO NOT DRAW IN HERE //
	void Scene_Game::UpdateFixed()
	{

	}

	void Scene_Game::Draw()
	{
		

		//ShaderProgram* _shader_gbuffer = ShaderProgram::GetAsset("gbuffer");
		//	auto& sub = _shader_gbuffer->GetSubroutine(ShaderType::FRAGMENT);
		//	
		//	if(Time.GetFrameCount() % 100 < 50)
		//		sub.Connect("Colour1", "ColorRed");
		//	else
		//		sub.Connect("Colour1", "ColorGreen");
		//	

		// FBOS
		auto* fbo_gbuffer = Assets.getFramebufferObject(fboIndex_gbuffer);
		auto* fbo_editor = Assets.getFramebufferObject(fboIndex_editor);
		auto* fbo_colorPicker = Assets.getFramebufferObject(fboIndex_colorpicker);
		auto* fbo_composite = Assets.getFramebufferObject(fboIndex_composite);

		// GUI Setup [needs to be changed]
#ifdef GLOBAL_IMGUI
		//GUI_Viewport.fboIndex_gbuffer = this->fboIndex_gbuffer;
		//GUI_Viewport.fboIndex_editor = this->fboIndex_editor;
#endif

		// UBO BINDING per frame
		auto _cameraObject = Assets.getCamera(RenderManager.m_mainCamera);

		if(_cameraObject)
			UBOManager.BindCamera(*_cameraObject);

		UBOManager.BindTime();

		// Render Gbuffer Information
		{
			GPUTimer::StartTimer("Gbuffer");
			CPUTimer::StartTimer("Gbuffer");
			//
			fbo_gbuffer->bind();
			fbo_gbuffer->clearBuffers();
			//
			RenderManager.sortMaterials();
			RenderManager.sortEntities();
			//
			RenderManager.renderOpaque(ShaderMaterialType::CORE);
			RenderManager.renderTransparent(ShaderMaterialType::CORE);
			//
			CPUTimer::EndTimer("Gbuffer");
			GPUTimer::EndTimer();
		}

		// Render Debugging Information
		if(RenderManager.m_editorMode)
		{
			GPUTimer::StartTimer("Editor");
			CPUTimer::StartTimer("Editor");
			//
			fbo_editor->bind();
			fbo_editor->clearBuffers();
			// Get Gbuffer depth
			fbo_gbuffer->blitDepth(*fbo_editor);

			// Shapes
			{
				ShaderProgram* programDebugRender = GlobalAssets.get_ProgramDebugRender();
				programDebugRender->bind();

				// States
				Graphics::SetDepthWrite(true);
				Graphics::SetDepthRead(true);

				if(programDebugRender->m_uniform_useModel.has_value())
					programDebugRender->m_uniform_useModel.value().send(true);
				
				// Spheres
				Mesh* meshSphere = Assets.getMesh(Primitives.GetSphereUV_Cheap());
				if (meshSphere)
				{
					for (const auto& object : Debug.m_spheres)
					{
						if(programDebugRender->m_uniform_color.has_value())
							programDebugRender->m_uniform_color.value().send(object.color);

						if (programDebugRender->m_uniform_mvp.has_value())
						{
							Camera* camera = Assets.getCamera(RenderManager.m_mainCamera);
							if(camera)
								programDebugRender->m_uniform_mvp.value().sendMatrix(camera->getViewProjection() * object.model, true);
						}
						
						meshSphere->draw();
					}
				}

				// Cubes
				Mesh* meshCube = Assets.getMesh(Primitives.GetCube());
				if (meshCube)
				{
					for (const auto& object : Debug.m_cubes)
					{
						if (programDebugRender->m_uniform_color.has_value())
							programDebugRender->m_uniform_color.value().send(object.color);

						if (programDebugRender->m_uniform_mvp.has_value())
						{
							Camera* camera = Assets.getCamera(RenderManager.m_mainCamera);
							if (camera)
								programDebugRender->m_uniform_mvp.value().sendMatrix(camera->getViewProjection() * object.model, true);
						}

						meshCube->draw();
					}
				}

				// Lines
				Mesh* mesh_ArrowZ = Assets.getMesh(Primitives.GetArrowZNoTail());
				if (mesh_ArrowZ)
				{
					for (const auto& object : Debug.m_arrowLines)
					{
						if (programDebugRender->m_uniform_color.has_value())
							programDebugRender->m_uniform_color.value().send(object.color);

						if (programDebugRender->m_uniform_mvp.has_value())
						{
							Camera* camera = Assets.getCamera(RenderManager.m_mainCamera);
							if (camera)
								programDebugRender->m_uniform_mvp.value().sendMatrix(camera->getViewProjection() * object.model, true);
						}

						mesh_ArrowZ->draw();
					}
				}

			}
			// Lines
			{
				ShaderProgram* programLineRender = GlobalAssets.get_ProgramLineRender();
				programLineRender->bind();
				
				// State
				if(programLineRender->m_uniform_useModel.has_value())
					programLineRender->m_uniform_useModel.value().send(false);
			
				// State
				programLineRender->sendUniform("useViewProjection", true);
				programLineRender->sendUniform("useVertexColors", true);
				programLineRender->sendUniform("useVertexWidth", true);
				programLineRender->sendUniform("globalWidth", 1.0f);
				Graphics::SetDepthRead(false);
			
				// Ignore current depth
				Debug.RenderWorldLinesNoDepth();
			
				// State
				Graphics::SetDepthRead(true);
			
				// Normal lines
				Debug.RenderWorldLines();

				// Objects
				if (programLineRender->m_uniform_useModel.has_value())
					programLineRender->m_uniform_useModel.value().send(true);

				programLineRender->sendUniform("useViewProjection", true);
				programLineRender->sendUniform("useVertexColors", false);
				programLineRender->sendUniform("useVertexWidth", false);
				
				// Spheres Outline
				Mesh* meshLines_CircleAllAxis = Assets.getMesh(Primitives.GetLines_CircleAllAxis_Unit());
				if (meshLines_CircleAllAxis)
				{
					for (const auto& object : Debug.m_spheresLines)
					{
						if (programLineRender->m_uniform_color.has_value())
							programLineRender->m_uniform_color.value().send(object.color);

						if (programLineRender->m_uniform_mvp.has_value())
						{
							Camera* camera = Assets.getCamera(RenderManager.m_mainCamera);
							if (camera)
								programLineRender->m_uniform_mvp.value().sendMatrix(camera->getViewProjection() * object.model, true);
						}

						programLineRender->sendUniform("globalWidth", object.width);

						meshLines_CircleAllAxis->draw();
					}
				}

				// Cubes Outline
				Mesh* meshLines_Cube = Assets.getMesh(Primitives.GetLines_Cube());
				if (meshLines_Cube)
				{
					for (const auto& object : Debug.m_cubesLines)
					{
						if (programLineRender->m_uniform_color.has_value())
							programLineRender->m_uniform_color.value().send(object.color);

						if (programLineRender->m_uniform_mvp.has_value())
						{
							Camera* camera = Assets.getCamera(RenderManager.m_mainCamera);
							if (camera)
								programLineRender->m_uniform_mvp.value().sendMatrix(camera->getViewProjection() * object.model, true);
						}

						programLineRender->sendUniform("globalWidth", object.width);

						meshLines_Cube->draw();
					}
				}

				// Screenspace Lines
				Debug.RenderScreenLines();

				// State
				Graphics::SetDepthRead(true);
			}

			//
			CPUTimer::EndTimer("Editor");
			GPUTimer::EndTimer();
		}

		//	// Remember Depth gbuffer depth
		//	fbo_gbuffer->GetDepthRenderTexture()->Copy(*fbo_colorPicker->GetDepthRenderBuffer());
		//	
		//	// RENDER
		//	fbo_gbuffer->DisableAttachment(1);
		//	RenderManager.RenderSceneGameObjects_Transparent();
		//	fbo_gbuffer->EnableAttachment(1);
		//	
		//	// RENDER
		//	fbo_gbuffer->DisableAttachment(0);
		//	fbo_gbuffer->DisableAttachment(1);
		//	RenderManager.RenderSceneObjectsWithOnlyColorID();
		//	fbo_gbuffer->EnableAttachment(0);
		//	fbo_gbuffer->EnableAttachment(1);
		//	
		//	// Revert Gbuffer Depth [Opaque only depth]
		//	fbo_colorPicker->GetDepthRenderBuffer()->Copy(*fbo_gbuffer->GetDepthRenderTexture());
		//	
		//	
		//	//	if (Input.getKeyDown(KeyCode::K))
		//	//	{
		//	//		RawArray<uint8_t> test = _fbo_gbuffer->readDepthPixelsFromMouse(1, 1);
		//	//		std::cout << (uint32_t)test[0] << std::endl;
		//	//		std::cout << (uint32_t)test[1] << std::endl;
		//	//		std::cout << "~~~" << std::endl;
		//	//		test.Deallocate();
		//	//	}
		//	
		//	//Graphics::SetBlendState(false);
		//	
		//	// FONT TEST END
		//	
		//	
		//	//
		
		//	
		//	// Render GIZMO ID [Changes FBO bound]
		//	gizmo.RenderIDCapture();
		//	
		//	fbo_editor->bind();
		//	fbo_editor->ClearBuffers();
		//	
		//	
		//	//
		//	GPUTimer::StartTimer("EditorObjects");
		//	//
		//	
		//	
		//	// Editor objects needs gbuffer depth for proper overlay
		//	//_fbo_gbuffer->blitDepth(*_fbo_editor);
		//	fbo_gbuffer->GetDepthRenderTexture()->Copy(*fbo_editor->GetDepthRenderBuffer());
		//	
		//	//Graphics::SetDepthRead(false);
		//	
		//	RenderManager.RenderEditorObjects();
		//	
		//	Graphics::ClearBuffer(BufferBit::DEPTH);
		//	
		//	gizmo.RenderOnScreen();
		//	
		//	RenderManager.RenderEditorObjectsPostDepth();


		//
		//GPUTimer::EndTimer();

		// Selection Data
		//if (RenderManager.GetAllEntities().size() > 0)
		//	Editor.UpdateSelectionTransformModel();
		
		//Editor.UpdateSelectionInfo();


		// Render ColorID Selection Information
		if (Input.getMouseButtonDown(MouseButton::LEFT) && !Window.IsCursorOnImguiWindow() && Window.GetCursor() == CursorMode::NORMAL)
		{
			fbo_colorPicker->bind();
			fbo_colorPicker->clearBuffers();
			//
			RenderManager.sortMaterials();
			RenderManager.sortEntities();
			//
			RenderManager.renderOpaque(ShaderMaterialType::COLORID);
			RenderManager.renderTransparent(ShaderMaterialType::COLORID);

			// Read Selected Pixel
			RawArray<uint8_t> data = fbo_colorPicker->readPixelsFromMouse(0, 1, 1);
			if (!data.isEmpty())
			{
				uint32_t sceneNodeIndex = Util::Conversion::uchars_to_uint(data.start);
				data.deallocate();

				// Toggle selection based on node state
				SceneNode* sceneNode = Assets.getSceneNode(sceneNodeIndex);
				if (sceneNode)
				{
					if (sceneNode->m_isSelectable)
					{
						if (Input.getKey(KeyCode::LEFT_CONTROL))
						{
							if (!sceneNode->IsSelected())
								Editor.addSelection(sceneNodeIndex);
							else
								Editor.removeSelection(sceneNodeIndex);
						}
						else
						{
							Editor.clearSelection();
							Editor.addSelection(sceneNodeIndex);
						}
					}
					else
						Editor.clearSelection();
				}
				else
					Editor.clearSelection();
			}
			else
				Editor.clearSelection();
		}

		// ~~ //

		// ~~~~
		Graphics::SetBlendState(false);
		Graphics::SetWireframeState(false);
		Graphics::SetDepthWrite(true);
		Graphics::SetDepthRead(true);
		Graphics::SetDepthPassRule(DepthPassRule::ALWAYS);
		Graphics::SetBlendMode(BlendSource::ONE, BlendDestination::ZERO);

		//////////////////////
		fbo_composite->bind();
		fbo_composite->clearBuffers();

		// Display Final Image
		ShaderProgram* shaderShowRenderTarget = GlobalAssets.get_ProgramShowRenderTarget();
		shaderShowRenderTarget->bind();
		shaderShowRenderTarget->sendUniform("channelOutput", 0);
		shaderShowRenderTarget->sendUniform("outputMode", RenderManager.m_editorMode ? 3 : 0);

		fbo_gbuffer->bindTexture(0, TextureLevel::LEVEL0);

		if(RenderManager.m_editorMode)
			fbo_editor->bindTexture(0, TextureLevel::LEVEL1);
		
		RenderManager.RenderFullScreen();
		//////////////////////
	
		//_fbo_composite->SetClearColor(Color4F(1, 0, 0, 1));
		//_fbo_composite->ClearBuffers();

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Display to backbuffer
		FramebufferObject::unbind();

		Window.BindWindowViewport();

		Graphics::SetBlendState(false);
		Graphics::SetWireframeState(false);
		Graphics::SetDepthPassRule(DepthPassRule::ALWAYS);

		Graphics::SetClearColor(0, 0, 0, 0);
		Graphics::ClearAllBuffers();

		ShaderProgram* programPassthrough = GlobalAssets.get_ProgramPassthrough();
		if (programPassthrough)
		{
			programPassthrough->bind();
		}

		fbo_composite->bindTexture(0, TextureLevel::LEVEL0);
		RenderManager.RenderFullScreen();

		// Screenshot
		if (Input.getKeyDown(KeyCode::F1))
		{
			Texture2D::ScreenshotBMP("screenshots/gbuffer");
		}

		//	// SUBROUTINES
		//	ShaderProgram* _shader_gbuffer = ShaderProgram::Get("gbuffer");
		//	auto& sub = _shader_gbuffer->GetSubroutine(ShaderType::FRAGMENT);
		//	
		//	if(Time.GetFrameCount() % 100 < 50)
		//		sub.Connect("Colour1", "ColorRed");
		//	else
		//		sub.Connect("Colour1", "ColorGreen");
		//	
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

	}
}