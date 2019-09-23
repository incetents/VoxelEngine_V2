// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Scene_Game.h"

#include "../imgui/imgui.h"

#include "../engine/window/window.h"
#include "../engine/input/Input.h"
#include "../engine/input/XGamePad.h"

#include "../engine/utilities/Loader.h"
#include "../engine/utilities/Logger.h"
#include "../engine/utilities/stringUtil.h"
#include "../engine/utilities/Time.h"
#include "../engine/utilities/FileIO.h"

#include "../engine/rendering/FramebufferObject.h"
#include "../engine/rendering/Geometry.h"
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
#include "../engine/objects/CameraObject.h"
#include "../engine/objects/GameObject.h"
#include "../engine/objects/TextObject.h"

#include "../engine/modules/Material.h"

#include "../engine/window/window.h"

#include "../engine/editor/Editor.h"
#include "../engine/editorGui/ShaderErrors.h"
#include "../engine/editorGui/DevConsole.h"
#include "../engine/editorGui/Hierarchy.h"
#include "../engine/editorGui/Inspector.h"
#include "../engine/editorGui/Performance.h"

#include "../game/terrain/TerrainManager.h"



namespace Vxl
{
	void Scene_Game::Setup()
	{
		GlobalFiles.LoadFile("_UBO.glsl", "./assets/files/_UBO.glsl");
		GlobalFiles.LoadFile("_Math.glsl", "./assets/files/_Math.glsl");
		GlobalFiles.LoadFile("_Uniforms.glsl", "./assets/files/_Uniforms.glsl");

		//ShaderBuilder builder;
		//builder.Create("");

		//Text::Init();
		//Text::LoadFont("arial", "assets/fonts/arial.ttf");

		Loader::LoadScript_ImportFiles("./assets/scripts/ImportFiles.txt");

		//_camera = Camera::Create("main", Vector3(3.5f, 2.8f, 0.3f), Vector3(-0.5f, -0.38f, -0.72f), 0.01f, 50.0f);

		_cameraObject = CameraObject::Create("_camera_editor", 0.01f, 5000.0f);
		_cameraObject->m_transform.setPosition(3.5f, 2.8f, 0.3f);
		_cameraObject->m_transform.setCameraForward(Vector3(-1, 0, -1));
		_cameraObject->SetPerspectiveWindowAspect(110.0f);

		//_cameraObject->TransformChanged();

		CameraObject* _c2 = CameraObject::Create("_camera2", 0.5f, 10.0f);
		_c2->m_transform.setPosition(2.9f, 2.1f, -2.5f);
		_c2->m_transform.setForward(Vector3(0, 0, 1));
		_c2->SetPerspectiveWindowAspect(140.0f);


		RenderManager.SetMainCamera(_cameraObject);

		//_camera->setOrthographic(-15, 15, -15, 15);
		//_camera->setPerspective(110.0f, 1.0f);
		//_camera->setPerspectiveWindowAspectLock(110.0f);
		//_camera->update();
		//_camera->SetMain();


		// FBO
		_fbo_gbuffer = FramebufferObject::Create("gbuffer");
		_fbo_gbuffer->SetClearColor(Color4F(-1, -1, 0, 1));
		_fbo_gbuffer->SetSizeToWindowSize();
		_fbo_gbuffer->Bind();
		_fbo_gbuffer->NewAttachment(0, RenderTarget::Type::TEXTURE, "albedo", TextureFormat::RGBA8);
		_fbo_gbuffer->NewAttachment(1, RenderTarget::Type::TEXTURE, "normal", TextureFormat::RGBA16_SNORM);
		_fbo_gbuffer->NewAttachment(2, RenderTarget::Type::TEXTURE, "reflection", TextureFormat::RGBA8);
		_fbo_gbuffer->NewAttachment(3, RenderTarget::Type::TEXTURE, "colorID", TextureFormat::RGBA8);
		_fbo_gbuffer->SetDepth(TextureDepthFormat::DEPTH16, RenderTarget::Type::TEXTURE);
		_fbo_gbuffer->checkFBOStatus();
		_fbo_gbuffer->Unbind();

		_fbo_editor = FramebufferObject::Create("EditorPost");
		_fbo_editor->SetSizeToWindowSize();
		_fbo_editor->Bind();
		_fbo_editor->NewAttachment(0, RenderTarget::Type::TEXTURE, "albedo", TextureFormat::RGBA8);
		_fbo_editor->SetDepth(TextureDepthFormat::DEPTH16, RenderTarget::Type::BUFFER);
		_fbo_editor->checkFBOStatus();
		_fbo_editor->Unbind();

		_fbo_colorpicker = FramebufferObject::Create("ColorPicker");
		_fbo_colorpicker->SetSizeToWindowSize();
		_fbo_colorpicker->Bind();
		_fbo_colorpicker->NewAttachment(0, RenderTarget::Type::TEXTURE, "color", TextureFormat::RGBA8);
		_fbo_colorpicker->SetDepth(TextureDepthFormat::DEPTH16, RenderTarget::Type::BUFFER);
		_fbo_colorpicker->checkFBOStatus();
		_fbo_colorpicker->Unbind();

		_fbo_composite = FramebufferObject::Create("composite");
		_fbo_composite->SetSizeToWindowSize();
		_fbo_composite->Bind();
		_fbo_composite->NewAttachment(0, RenderTarget::Type::TEXTURE, "albedo", TextureFormat::RGBA8);
		_fbo_composite->checkFBOStatus();
		_fbo_composite->Unbind();

		_fbo_showRenderTarget = FramebufferObject::Create("showRenderTarget");
		_fbo_showRenderTarget->SetSizeToWindowSize();
		_fbo_showRenderTarget->Bind();
		_fbo_showRenderTarget->NewAttachment(0, RenderTarget::Type::TEXTURE, "albedo", TextureFormat::RGBA8);
		_fbo_showRenderTarget->checkFBOStatus();
		_fbo_showRenderTarget->Unbind();


		ShaderProgram* _shader_skybox			= ShaderProgram::GetAsset("skybox");
		ShaderProgram* _shader_gbuffer			= ShaderProgram::GetAsset("gbuffer");
		ShaderProgram* _shader_lines			= ShaderProgram::GetAsset("lines");
		ShaderProgram* _shader_passthroughWorld	= ShaderProgram::GetAsset("passthroughWorld");
		ShaderProgram* _shader_colorPicker		= ShaderProgram::GetAsset("colorPicker");
		ShaderProgram* _shader_showRenderTarget = ShaderProgram::GetAsset("showRenderTarget");
		ShaderProgram* _shader_billboard		= ShaderProgram::GetAsset("billboard");
		ShaderProgram* _shader_simpleLight		= ShaderProgram::GetAsset("simpleLight");
		ShaderProgram* _shader_font				= ShaderProgram::GetAsset("font");
		ShaderProgram* _shader_showNormals		= ShaderProgram::GetAsset("showNormals");


		material_skybox = Material::Create("skybox", 0);
		material_skybox->SetProgram(*_shader_skybox);

		// Gbuffer Materials [Should not do any blending]

		material_gbuffer = Material::Create("gbuffer", 1);
		material_gbuffer->SetProgram(*_shader_gbuffer);
		material_gbuffer->m_BlendState = false;

		material_transparent_gbuffer = Material::Create("transparent_gbuffer", 1000);
		material_transparent_gbuffer->SetProgram(*_shader_gbuffer);
		material_transparent_gbuffer->SetRenderMode(MaterialRenderMode::Transparent);
		material_transparent_gbuffer->m_DepthWrite = false;
		material_transparent_gbuffer->m_BlendFunc.source = BlendSource::ONE;
		material_transparent_gbuffer->m_BlendFunc.destination = BlendDestination::ONE;
		// Color ID still passes normally by ignoring blend mode
		material_transparent_gbuffer->SetBlendFuncAttachment(3, BlendSource::ONE, BlendDestination::ZERO);

		material_opaque_passthroughWorld = Material::Create("opaque_passthroughWorld", 2);
		material_opaque_passthroughWorld->SetProgram(*_shader_passthroughWorld);
		material_opaque_passthroughWorld->m_BlendState = false;

		material_opaque_billboard = Material::Create("opaque_billboard", 3);
		material_opaque_billboard->SetProgram(*_shader_billboard);
		material_opaque_billboard->m_BlendState = false;

		material_passthroughWorld = Material::Create("transparent_passthroughWorld", 1001);
		material_passthroughWorld->SetProgram(*_shader_passthroughWorld);
		material_passthroughWorld->SetRenderMode(MaterialRenderMode::Transparent);
		material_passthroughWorld->m_DepthWrite = false;
		material_passthroughWorld->m_BlendFunc.source = BlendSource::ONE;
		material_passthroughWorld->m_BlendFunc.destination = BlendDestination::ONE;
		// Color ID still passes normally by ignoring blend mode
		material_passthroughWorld->SetBlendFuncAttachment(3, BlendSource::ONE, BlendDestination::ZERO);

		material_billboard = Material::Create("billboard", 3);
		material_billboard->SetProgram(*_shader_billboard);

		material_lines = Material::Create("lines", 4);
		material_lines->SetProgram(*_shader_lines);

		material_simpleLight = Material::Create("simpleLight", 5);
		material_simpleLight->SetProgram(*_shader_simpleLight);

		material_colorPicker = Material::Create("colorPicker", 6);
		material_colorPicker->SetProgram(*_shader_colorPicker);
		material_colorPicker->m_BlendState = false;

		material_showNormals = Material::Create("showNormals", 20);
		material_showNormals->SetProgram(*_shader_showNormals);
		material_showNormals->m_BlendState = false;

		material_font = Material::Create("font", 100);
		material_font->SetProgram(*_shader_font);
		material_font->m_DepthRead = false;
		material_font->m_DepthWrite = false;

		_tex = Texture2D::GetAsset("beato");
		_tex_crate = Texture2D::GetAsset("crate_diffuse");
		_tex_gridtest = Texture2D::GetAsset("grid_test");

		_cubemap1 = Cubemap::GetAsset("craterlake");
		
		// Voxel Stuff
		//BlockAtlas.Set(Texture::Get("TextureAtlas"), 16); // texture of all blocks
		//BlockDictionary.Setup(); // info of all blocks
		//TerrainManager.Setup(); // keeps track of terrain info
		//

		_mesh = Mesh::Create("test1");
		
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
		
		_mesh->m_positions.set(pos, 4);
		_mesh->m_uvs.set(uvs, 4);
		_mesh->m_indices.set(indices, 6);
		_mesh->GenerateNormals(false);
		_mesh->GenerateTangents();
		
		std::vector<Matrix4x4> m_models;
		for (float x = 0; x < 5.0f; x++)
		{
			for (float y = 0; y < 5.0f; y++)
			{
				Transform t(Vec3(x * 1.2f, y * 1.2f, 0.0f));
				m_models.push_back(t.getModel().Transpose());
			}
		}
		
		_mesh->m_instances = m_models;
		
		_mesh->Bind();
		
		
		// Entities
		GameObject* _entity1 = GameObject::Create("_entity1");
		_entity1->SetMaterial(material_gbuffer);
		_entity1->SetTexture(_tex, TextureLevel::LEVEL0);
		_entity1->SetMesh(_mesh);
		_entity1->m_transform.setScale(1.0f);
		_entity1->m_transform.setPositionZ(-7.0f);

		//Loader::Load_Model("jiggy1", "./assets/models/jiggy.obj", false, true);
		Mesh* jiggyMesh = Mesh::GetAsset("jiggy");
		
		GameObject* _entity2 = GameObject::Create("_entity2");
		_entity2->SetMaterial(material_gbuffer);
		//_entity2->m_material.SetTexture(_tex_crate, ActiveTexture::LEVEL0);
		_entity2->SetMesh(jiggyMesh);// Geometry.GetIcoSphere();
		_entity2->m_transform.setPosition(Vector3(+1.5f, 0, -3.0f));
		_entity2->SetColor(Color3F(1, 1, 0));

		GameObject* _errorCube = GameObject::Create("_errorCube");
		_errorCube->SetMaterial(material_gbuffer);
		//_errorCube->m_material.SetTexture(_tex_crate, ActiveTexture::LEVEL0);
		//_errorCube->SetMesh(jiggyMesh);// Geometry.GetIcoSphere();
		_errorCube->SetMesh(Geometry.GetCube());// Geometry.GetIcoSphere();
		_errorCube->m_transform.setPosition(Vector3(-0.5f, 0, -3.0f));
		//_errorCube->SetColor(Color3F(1, 1, 0));

		GameObject* _alphaCube1 = GameObject::Create("_alphaCube1");
		_alphaCube1->SetMaterial(material_transparent_gbuffer);
		_alphaCube1->SetColor(Color3F(1, 0, 0));
		_alphaCube1->SetAlpha(0.5f);
		_alphaCube1->SetMesh(Geometry.GetCube());// Geometry.GetIcoSphere();
		_alphaCube1->m_transform.setPosition(Vector3(-0.5f, -2, -3.0f));

		GameObject* _alphaCube2 = GameObject::Create("_alphaCube2");
		_alphaCube2->SetMaterial(material_transparent_gbuffer);
		_alphaCube2->SetColor(Color3F(0, 1, 0));
		_alphaCube2->SetAlpha(0.5f);
		_alphaCube2->SetMesh(Geometry.GetCube());// Geometry.GetIcoSphere();
		_alphaCube2->m_transform.setPosition(Vector3(-1.5f, -2, -3.0f));

		GameObject* _alphaCube3 = GameObject::Create("_alphaCube3");
		_alphaCube3->SetMaterial(material_transparent_gbuffer);
		_alphaCube3->SetColor(Color3F(0, 0, 1));
		_alphaCube3->SetAlpha(0.5f);
		_alphaCube3->SetMesh(Geometry.GetCube());// Geometry.GetIcoSphere();
		_alphaCube3->m_transform.setPosition(Vector3(-2.5f, -2, -3.0f));

		
		GameObject* _entity3 = GameObject::Create("_entity3");
		_entity3->SetTexture(_tex_crate, TextureLevel::LEVEL0);
		_entity3->SetMaterial(material_gbuffer);
		_entity3->SetMesh(Geometry.GetIcosahedron());
		_entity3->m_transform.setPosition(Vector3(-2.5f, 0, -3.0f));
		
		
		
		GameObject* _entity5 = GameObject::Create("_entity5");
		_entity5->SetMaterial(material_gbuffer);
		_entity5->SetTexture(_tex_gridtest, TextureLevel::LEVEL0);
		_entity5->SetMesh(Geometry.GetSphereUV_Good());
		_entity5->m_transform.setPosition(Vector3(0, -4, 0));

		GameObject* _entity6 = GameObject::Create("_entity6");
		_entity6->SetMaterial(material_gbuffer);
		_entity6->SetTexture(_tex_gridtest, TextureLevel::LEVEL0);
		_entity6->SetMesh(Geometry.GetCube());
		_entity6->m_transform.setPosition(Vector3(5, -4, 0));

		GameObject* _entity6_b = GameObject::Create("_entity6_normals");
		_entity6_b->SetMaterial(material_showNormals);
		_entity6_b->SetTexture(_tex_gridtest, TextureLevel::LEVEL0);
		_entity6_b->SetMesh(Geometry.GetCube());
		_entity6_b->m_transform.setPosition(Vector3(5, -4, 0));


		GameObject* _entity7 = GameObject::Create("_entity7");
		_entity7->SetMaterial(material_gbuffer);
		_entity7->SetTexture(Texture2D::GetAsset("grid_test"), TextureLevel::LEVEL0);
		_entity7->SetMesh(Geometry.GetQuadY());
		_entity7->m_transform.setPosition(Vector3(0, -10, 0));
		_entity7->m_transform.setScale(Vector3(20, 1, 20));
		//_entity5->SetColor(Color3F(1, 1, 1));
		
		
		GameObject* _skybox = GameObject::Create("_skybox");
		_skybox->SetMaterial(material_skybox);
		_skybox->SetTexture(_cubemap1, TextureLevel::LEVEL0);
		_skybox->SetMesh(Geometry.GetInverseCube());
		_skybox->m_useTransform = false;
		_skybox->SetSelectable(false);

		//_skybox->SetMaterial(material_gbuffer);

		//
		GameObject* _crate1 = GameObject::Create("_crate1");
		_crate1->SetMaterial(material_gbuffer);
		_crate1->SetTexture(_tex_crate, TextureLevel::LEVEL0);
		_crate1->SetMesh(Geometry.GetCylinderX());
		_crate1->m_transform.setPosition(1, 3, 2);
		_crate1->SetTint(Color3F(0.4f, 0.1f, 0.9f));
		
		
		GameObject* _crate2 = GameObject::Create("_crate2");
		_crate2->SetMaterial(material_gbuffer);
		_crate2->SetTexture(_tex_crate, TextureLevel::LEVEL0);
		_crate2->SetMesh(Geometry.GetCylinderY());
		//_crate2->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		_crate2->m_transform.setPosition(0, 2, 0);
		

		GameObject* _crate3 = GameObject::Create("_crate3");
		_crate3->SetMaterial(material_gbuffer);
		_crate3->SetTexture(_tex_crate, TextureLevel::LEVEL0);
		_crate3->SetMesh(Geometry.GetCylinderZ());
		//_crate3->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		_crate3->m_transform.setPosition(1, 2, 0);

		// Parent Test
		_crate3->m_transform.addChild(&_crate2->m_transform);
		_crate2->m_transform.addChild(&_crate1->m_transform);

		//_crate2->m_transform.setRotation(65, 12, 3);
		//_crate3->m_transform.setRotation(45, 12, 3);
		// World Position
		_crate1->m_transform.setWorldPosition(-5, -5, 0);
		
		GameObject* _octo1 = GameObject::Create("_octo1");
		_octo1->SetMaterial(material_gbuffer);
		_octo1->SetMesh(Geometry.GetOctahedron());
		_octo1->m_transform.setPosition(0, 0, 0);
		_octo1->m_transform.setScale(0.5f);
		_octo1->SetColor(Color3F(1, 1, 1));
		
		GameObject* _octo2 = GameObject::Create("_octo2");
		_octo2->SetMaterial(material_gbuffer);
		_octo2->SetMesh(Geometry.GetOctahedron());
		_octo2->m_transform.setPosition(1, 0, 0);
		_octo2->m_transform.setScale(0.5f);
		_octo2->SetColor(Color3F(1, 0, 0));
		
		GameObject* _octo3 = GameObject::Create("_octo3");
		_octo3->SetMaterial(material_gbuffer);
		_octo3->SetMesh(Geometry.GetOctahedron());
		_octo3->m_transform.setPosition(0, 1, 0);
		_octo3->m_transform.setScale(0.5f);
		_octo3->SetColor(Color3F(0, 1, 0));
		
		GameObject* _octo4 = GameObject::Create("_octo4");
		_octo4->SetMaterial(material_gbuffer);
		_octo4->SetMesh(Geometry.GetOctahedron());
		_octo4->m_transform.setPosition(0, 0, 1);
		_octo4->m_transform.setScale(0.5f);
		_octo4->SetColor(Color3F(0, 0, 1));
		
		LightObject* _light1 = LightObject::Create("_light1", Light::Type::POINT);
		_light1->m_transform.setPosition(5, 0, 0);

		GameObject* _billboard1 = GameObject::Create("_quad1");
		_billboard1->SetMaterial(material_opaque_billboard);
		_billboard1->SetTexture(Texture2D::GetAsset("beato"), TextureLevel::LEVEL0);
		_billboard1->SetMesh(Geometry.GetQuadZ());
		_billboard1->m_transform.setPosition(7, 3, -3);

		TextObject::LoadFont("arial", "assets/fonts/arial.ttf");
		
		TextObject* _text1 = TextObject::Create("_text1");
		_text1->m_transform.setPosition(-2, -3, 0);
		_text1->SetFont("arial");
		_text1->SetText("abcdefghijkl\nmnopqrstuvwxyz\nquick brown fox jumped over the moon\n12345678900987654321");
		
		//_text1->SetTexture(Texture::GetAsset("beato"), TextureLevel::LEVEL0);

		//material_gbuffer
		//material_billboard

		//GameObject::Delete(_entity5);
		//GameObject::Delete("_octo3");
		
		//_light1 = LightObject::Create("light1");


		
	}
	void Scene_Game::Destroy()
	{
		TerrainManager.Destroy();
	}

	void Scene_Game::Update()
	{
		//	auto _entity5 = GameObject::GetAsset("_entity5");
		//	_entity5->m_transform.rotateAroundAxis(Vector3(-1,-1,-1), 1);

		CPUTimer::StartTimer("UPDATE");

		if (Input.getKeyDown(KeyCode::ESCAPE))
			Window.Close();

		if (DEVCONSOLE_GET_BOOL("Camera Keyboard Controls", true))
		{
			// Camera Movement
			Vector3 CamMove;
			float CamSpeed = 0.1f;

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

			float CameraHorizontalRotation = DEVCONSOLE_GET_FLOAT("Camera Horizontal Rotation", 0.2f);
			float CameraVerticalRotation = DEVCONSOLE_GET_FLOAT("Camera Vertical Rotation", 0.2f);

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
			_cameraObject->Update();
		}

		// GizmoMode
		if (Input.getKeyDown(KeyCode::Num_1))
			Editor.m_controlMode = Editor::GizmoMode::TRANSLATE;
		if (Input.getKeyDown(KeyCode::Num_2))
			Editor.m_controlMode = Editor::GizmoMode::SCALE;
		if (Input.getKeyDown(KeyCode::Num_3))
			Editor.m_controlMode = Editor::GizmoMode::ROTATE;

		// Debug Lines (screen space)
		//	auto t = Time.GetTime() / 2.0;
		//	Debug.DrawScreenSpaceLine(
		//		Vector2::ZERO, Vector2(sinf(t), cosf(t)),
		//		4.0f,
		//		Color4F::RED, Color4F::BLUE
		//	);

		// Selection
		auto SelectedEntities = Editor.GetSelectedEntities();
		for (const auto& Entity : SelectedEntities)
		{
			// If appears unorthodox, selection might be disabled [ex: skybox cube]
			if (Entity->IsSelectable())
			{
				static Vector3 Epsilon = Vector3(0.01f, 0.01f, 0.01f);

				Color4F AABB_Color = Entity->IsFamilyActive() ? Color4F::YELLOW : Color4F::GREY;
				Color4F OBB_Color = Entity->IsFamilyActive() ? Color4F::GREEN : Color4F::GREY;

				// Selection for GameObjects
				if (Entity->GetType() == EntityType::GAMEOBJECT)
				{
					Mesh* _mesh = dynamic_cast<GameObject*>(Entity)->GetMesh();
					Vector3 EntityWorld = Entity->m_transform.getWorldPosition();

					// Draw Outline around Entity
					if (_mesh)
					{
						if (_mesh->m_instances.Empty())
						{
							Debug.DrawLineOBB(*Entity, EntityWorld, 3.0f, OBB_Color);
							Debug.DrawLineAABB(Entity->GetAABBMin() - Epsilon, Entity->GetAABBMax() + Epsilon, EntityWorld, 3.0f, AABB_Color);
						}
						// Draw outline around all instances
						else
						{
							//Vector4 WPosition = Vector4(Entity->m_transform.getWorldPosition(), 1);
							auto Instances = _mesh->m_instances.GetVertices();
							auto EModel = Entity->m_transform.getModel();
							for (Matrix4x4& instanceMatrix : *Instances)
							{
								//Vector3 Pos = Vector3(instanceMatrix.Transpose() * Vector4(EntityWorld, 1));
								Vector4 Pos = Vector4(instanceMatrix[12], instanceMatrix[13], instanceMatrix[14], 1);

								Vector3 PosWorld = Vector3(EModel * Pos);

								Debug.DrawLineOBB(*Entity, PosWorld, 3.0f, OBB_Color);
								Debug.DrawLineAABB(Entity->GetAABBMin() - Epsilon, Entity->GetAABBMax() + Epsilon, PosWorld, 3.0f, AABB_Color);
							}
						}
					}

					// Draw Axis Directions
					Debug.DrawLine(EntityWorld, EntityWorld + Entity->m_transform.getForward() * 8.0f, 5.0f, Color4F::BLUE, Color4F::BLUE);
					Debug.DrawLine(EntityWorld, EntityWorld + Entity->m_transform.getUp() * 8.0f, 5.0f, Color4F::GREEN, Color4F::GREEN);
					Debug.DrawLine(EntityWorld, EntityWorld + Entity->m_transform.getRight() * 8.0f, 5.0f, Color4F::RED, Color4F::RED);
				}
				// Selection for Cameras
				else if (Entity->GetType() == EntityType::CAMERA)
				{
					//Debug.DrawOBB(*Entity, Vector3::ZERO, 5.0f, OBB_Color);

					CameraObject* C = dynamic_cast<CameraObject*>(Entity);
					C->DrawFrustum(5.0f, OBB_Color);
				}
				// Selection for Lights
				else if (Entity->GetType() == EntityType::LIGHT)
				{
					LightObject* LObject = dynamic_cast<LightObject*>(Entity);
					
					switch (LObject->GetLightType())
					{
						case Light::Type::POINT:
						{
							Light_Point* LightPoint = dynamic_cast<Light_Point*>(LObject->GetLight());
							float lightRadius = LightPoint->m_radius;

							Debug.DrawWireframeSphere(LObject->m_transform.getWorldPosition(), Vector3(lightRadius), Color4F(LightPoint->m_color, 0.1f));
							break;
						}
					}
				}
			}
		}

		// End Frame Updates
		XGamePadManager.Update();

		// Imgui Specials
		if (material_gbuffer)
		{
			material_gbuffer->m_Wireframe = DEVCONSOLE_GET_BOOL("Gbuffer Wireframe", false);
		}

		CPUTimer::EndTimer("UPDATE");
	}

	// DO NOT DRAW IN HERE //
	void Scene_Game::UpdateFixed()
	{

	}

	void Scene_Game::Draw()
	{
		// Debug Lines
		Debug.DrawLine(
			Vector3(-1, -1, -1), Vector3(+1, +1, -1),
			10.0f,
			Color4F(0, 1, 0, 0.25f), Color4F(1, 0, 1, 1)
		);
		static float time = 0.0f;
		time += 0.2f;
		Debug.DrawLine(
			Vector3(+3, +1, -1), Vector3(+3, +4 + cosf(time), -1),
			10.0f,
			Color4F(1, 1, 0, 0.25f), Color4F(0, 1, 1, 1)
		);

		// Mouse pos [-1 to +1]
		//Vector4 clickPoint = RenderManager.GetMainCamera()->ScreenSpaceToWorldSpace(Vector3(Input.getMousePosScreenspaceX(), Input.getMousePosScreenspaceY(true), 0.5f));
		//Debug.DrawCube(clickPoint.GetVector3(), Vector3(0.01f), Color4F(1, 0, 1, 1));
		// Screenspace raycast direction
		//std::cout << RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true)) << std::endl;
		//Debug.DrawLine(Editor.m_GizmoTransform.WorldPosition, Editor.m_GizmoTransform.WorldPosition + Editor.m_dragStart * 5.0f, 5.0f);
		//Debug.DrawLine(Editor.m_GizmoTransform.WorldPosition, Editor.m_GizmoTransform.WorldPosition + Editor.m_dragEnd * 5.0f, 5.0f);

		//ShaderProgram* _shader_gbuffer = ShaderProgram::GetAsset("gbuffer");
		//	auto& sub = _shader_gbuffer->GetSubroutine(ShaderType::FRAGMENT);
		//	
		//	if(Time.GetFrameCount() % 100 < 50)
		//		sub.Connect("Colour1", "ColorRed");
		//	else
		//		sub.Connect("Colour1", "ColorGreen");
		//	

		// UBO BINDING per frame
		UBOManager.BindCamera(*RenderManager.GetMainCamera());
		UBOManager.BindTime();

		_fbo_gbuffer->Bind();
		_fbo_gbuffer->ClearBuffers();
		//
		GPUTimer::StartTimer("Gbuffer");
		CPUTimer::StartTimer("Gbuffer");
		//
		RenderManager.RenderSceneGameObjects_Opaque();

		// Remember Depth gbuffer depth
		_fbo_gbuffer->GetDepthRenderTexture()->Copy(*_fbo_colorpicker->GetDepthRenderBuffer());

		RenderManager.RenderSceneGameObjects_Transparent();
		RenderManager.RenderSceneObjectsWithOnlyColorID();

		// Revert Gbuffer Depth [Opaque only depth]
		_fbo_colorpicker->GetDepthRenderBuffer()->Copy(*_fbo_gbuffer->GetDepthRenderTexture());


		//	if (Input.getKeyDown(KeyCode::K))
		//	{
		//		RawArray<uint8_t> test = _fbo_gbuffer->readDepthPixelsFromMouse(1, 1);
		//		std::cout << (uint32_t)test[0] << std::endl;
		//		std::cout << (uint32_t)test[1] << std::endl;
		//		std::cout << "~~~" << std::endl;
		//		test.Deallocate();
		//	}

		//Graphics::SetBlendState(false);

		// FONT TEST END
		

		//
		CPUTimer::EndTimer("Gbuffer");
		GPUTimer::EndTimer();

		

		_fbo_editor->Bind();
		_fbo_editor->ClearBuffers();


		//
		GPUTimer::StartTimer("EditorObjects");
		//

		
		// Editor objects needs gbuffer depth for proper overlay
		//_fbo_gbuffer->blitDepth(*_fbo_editor);
		_fbo_gbuffer->GetDepthRenderTexture()->Copy(*_fbo_editor->GetDepthRenderBuffer());

		//Graphics::SetDepthRead(false);
		RenderManager.RenderEditorObjects();

		Graphics::ClearBuffer(BufferBit::DEPTH);
		//RenderManager.RenderEditorObjectsPostDepth(); // gizmo rn

		static Gizmo gizmo;

		// GizmoMode
		if (Input.getKeyDown(KeyCode::Num_1))
			gizmo.m_mode = Gizmo::Mode::TRANSLATE;
		if (Input.getKeyDown(KeyCode::Num_2))
			gizmo.m_mode = Gizmo::Mode::SCALE;
		if (Input.getKeyDown(KeyCode::Num_3))
			gizmo.m_mode = Gizmo::Mode::ROTATE;

		gizmo.UpdateModel(*GameObject::GetAsset("_crate3"));
		gizmo.RenderOnScreen();
		gizmo.RenderIDCapture();
		gizmo.Update();

		//
		GPUTimer::EndTimer();

		// Selection Data
		//if (RenderManager.GetAllEntities().size() > 0)
		//	Editor.UpdateSelectionTransformModel();
		
		//Editor.UpdateSelectionInfo();


		

		// Select Object in scene
		// ~~ //
		if (!Editor.m_GizmoSelected && !Editor.m_GizmoClicked && Input.getMouseButtonDown(MouseButton::LEFT) && !Window.IsCursorOnImguiWindow() && Window.GetCursor() == CursorMode::NORMAL)
		{
			if (DEVCONSOLE_GET_BOOL("Objects are Clickable", true))
			{
				_fbo_gbuffer->Bind();

				RawArray<uint8_t> data = _fbo_gbuffer->readPixelsFromMouse(3, 1, 1);
				if (!data.IsEmpty())
				{

					unsigned int EntityIndex = Util::Conversion::uchars_to_uint(data.start);
					//std::cout << "Unique ID: " << EntityIndex << std::endl;
					data.Deallocate();

					Entity* SelectedEntity = Entity::GetEntityByID(EntityIndex);

					// Found an entity
					if (SelectedEntity)
					{
						if (Input.getKey(KeyCode::LEFT_CONTROL))
						{
							if (!SelectedEntity->IsSelected())
								Editor.AddSelection(SelectedEntity);
							else
								Editor.RemoveSelection(SelectedEntity);
						}
						else
						{
							Editor.ClearSelection();
							Editor.AddSelection(SelectedEntity);
						}
					}
					else
					{
						Editor.ClearSelection();
					}

					//if ()
					//	std::cout << mem.ui_ID << ", Entity: " << Entities[mem.ui_ID]->m_name << std::endl;

				}
			}
		}

		// ~~ //


		// ~~~~
		Graphics::SetBlendState(false);
		Graphics::SetWireframeState(false);
		Graphics::SetDepthPassRule(DepthPassRule::ALWAYS);

		//////////////////////
		_fbo_composite->Bind();
		_fbo_composite->ClearBuffers();

		// Display Final Image
		ShaderProgram* _shader_showRenderTarget = ShaderProgram::GetAsset("showRenderTarget");
		_shader_showRenderTarget->Bind();
		_shader_showRenderTarget->SetUniform("channelOutput", 0);
		_shader_showRenderTarget->SetUniform("outputMode", 3);

		_fbo_gbuffer->bindTexture(0, TextureLevel::LEVEL0);
		_fbo_editor->bindTexture(0, TextureLevel::LEVEL1);
		
		RenderManager.RenderFullScreen();
		//////////////////////
	
		//_fbo_composite->SetClearColor(Color4F(1, 0, 0, 1));
		//_fbo_composite->ClearBuffers();

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Display to backbuffer
		FramebufferObject::Unbind();

		Window.BindWindowViewport();

		Graphics::SetBlendState(false);
		Graphics::SetWireframeState(false);
		Graphics::SetDepthPassRule(DepthPassRule::ALWAYS);

		Graphics::SetClearColor(0, 0, 0, 0);
		Graphics::ClearAllBuffers();

		_fbo_composite->bindTexture(0, TextureLevel::LEVEL0);
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