// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Scene_Game.h"

#include "../imgui/imgui.h"

#include "../engine/window/window.h"
#include "../engine/input/Input.h"
#include "../engine/input/XGamePad.h"

#include "../engine/utilities/Loader.h"
#include "../engine/utilities/logger.h"
#include "../engine/utilities/stringUtil.h"
#include "../engine/utilities/Time.h"

#include "../engine/opengl/FramebufferObject.h"
#include "../engine/opengl/Geometry.h"
#include "../engine/opengl/Mesh.h"
#include "../engine/opengl/Shader.h"
#include "../engine/opengl/Texture.h"
#include "../engine/opengl/Debug.h"
#include "../engine/opengl/UBO.h"

#include "../engine/math/Color.h"
#include "../engine/math/Transform.h"
#include "../engine/math/Vector2.h"
#include "../engine/math/Vector3.h"
#include "../engine/math/Vector4.h"
#include "../engine/math/MathCore.h"
#include "../engine/math/Lerp.h"
#include "../engine/math/Model.h"

#include "../engine/objects/LightObject.h"
#include "../engine/objects/CameraObject.h"
#include "../engine/objects/GameObject.h"
#include "../engine/modules/Material.h"
#include "../engine/modules/RenderManager.h"

#include "../engine/window/window.h"

#include "../engine/editor/Editor.h"
#include "../engine/editor/ShaderErrors.h"
#include "../engine/editor/DevConsole.h"
#include "../engine/editor/Hierarchy.h"
#include "../engine/editor/Inspector.h"
#include "../engine/editor/Performance.h"

#include "../game/terrain/TerrainManager.h"


//#define GL_CLAMP_READ_COLOR 0x891C

namespace Vxl
{
	void Scene_Game::Setup()
	{
		Loader::LoadScript_ImportFiles("./assets/scripts/ImportFiles.txt");

		//_camera = Camera::Create("main", Vector3(3.5f, 2.8f, 0.3f), Vector3(-0.5f, -0.38f, -0.72f), 0.01f, 50.0f);

		_cameraObject = CameraObject::Create("_camera_editor", 0.01f, 50.0f);
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
		_fbo = FramebufferObject::Create("Gbuffer", Window.GetResolutionWidth(), Window.GetResolutionHeight(), Color4F(-1, -1, 0, 1));
		_fbo->addTexture("albedo");
		_fbo->addTexture("normal" , Wrap_Mode::CLAMP_STRETCH, Min_Filter::NEAREST, Mag_Filter::NEAREST, Format_Type::RGBA16_SNORM, Channel_Type::RGBA, Data_Type::FLOAT);
		_fbo->addTexture("test");
		_fbo->addDepth();

		_fbo_editor = FramebufferObject::Create("EditorPost", Window.GetResolutionWidth(), Window.GetResolutionHeight(), Color4F(0,0,0,0));
		_fbo_editor->addTexture("albedo");
		_fbo_editor->addDepth();

		_fbo_colorpicker = FramebufferObject::Create("ColorPicker", Window.GetResolutionWidth(), Window.GetResolutionHeight(), Color4F(0,0,0,0));
		_fbo_colorpicker->addTexture("color", Wrap_Mode::CLAMP_STRETCH, Min_Filter::NEAREST, Mag_Filter::NEAREST, Format_Type::RGBA, Channel_Type::RGBA, Data_Type::FLOAT);
		_fbo_colorpicker->addDepth();

		_shader_skybox				= ShaderProgram::Get("skybox");
		_shader_gbuffer				= ShaderProgram::Get("gbuffer");
		_shader_lines				= ShaderProgram::Get("lines");
		_shader_passthrough			= ShaderProgram::Get("passthrough");
		_shader_colorPicker			= ShaderProgram::Get("colorPicker");
		_shader_showRenderTarget	= ShaderProgram::Get("showRenderTarget");
		_shader_billboard			= ShaderProgram::Get("billboard");

		_material_skybox			= Material::Create("skybox", _shader_skybox, 0);
		_material_gbuffer			= Material::Create("gbuffer", _shader_gbuffer, 1);
		_material_passthrough		= Material::Create("passthrough", _shader_passthrough, 999);
		_material_billboard			= Material::Create("billboard", _shader_billboard, 4);

		_material_gbuffer->m_BlendState = false;
		_material_gbuffer->m_BlendSource = Blend_Source::ONE;
		_material_gbuffer->m_BlendDest = Blend_Destination::ZERO;

		_tex = Texture::Get("beato");
		_tex_crate = Texture::Get("crate_diffuse");
		_tex_gridtest = Texture::Get("grid_test");

		_cubemap1 = Cubemap::Get("craterlake");
		
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
		GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };
		
		_mesh->m_positions.set(pos, 4);
		_mesh->m_uvs.set(uvs, 4);
		_mesh->m_indices.set(indices, 6);
		
		std::vector<Matrix4x4> m_models;
		for (float x = 0; x < 5.0f; x++)
		{
			for (float y = 0; y < 5.0f; y++)
			{
				Transform t(Vec3(x * 1.2f, y * 1.2f, -7.0f));
				m_models.push_back(t.getLocalModel().Transpose());
			}
		}
		
		_mesh->m_instances = m_models;
		
		_mesh->Bind();
		
		
		// Entities
		GameObject* _entity1 = GameObject::Create("_entity1");
		_entity1->SetMaterial(_material_gbuffer);
		_entity1->m_material.SetTexture(_tex, Active_Texture::LEVEL0);
		_entity1->SetMesh(_mesh);
		_entity1->m_transform.setScale(+0.5f);
		
		//Loader::Load_Model("jiggy1", "./assets/models/jiggy.obj", false, true);
		Mesh* jiggyMesh = Mesh::Get("jiggy");
		
		GameObject* _entity2 = GameObject::Create("_entity2");
		_entity2->SetMaterial(_material_gbuffer);
		//_entity2->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_entity2->SetMesh(jiggyMesh);// Geometry.GetIcoSphere();
		_entity2->m_transform.setPosition(Vector3(+1.5f, 0, -3.0f));
		//_entity2->SetColor(Color3F(1, 1, 0));
		
		GameObject* _entity3 = GameObject::Create("_entity3");
		_entity3->SetMaterial(_material_gbuffer);
		_entity3->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_entity3->SetMesh(Geometry.GetIcosahedron());
		_entity3->m_transform.setPosition(Vector3(-2.5f, 0, -3.0f));
		
		
		
		GameObject* _entity5 = GameObject::Create("_entity5");
		_entity5->SetMaterial(_material_gbuffer);
		_entity5->SetMesh(Geometry.GetSphereUV_Good());
		_entity5->m_transform.setPosition(Vector3(0, -4, 0));
		_entity5->m_material.SetTexture(_tex_gridtest, Active_Texture::LEVEL0);
		//_entity5->SetColor(Color3F(1, 1, 1));
		
		
		
		GameObject* _skybox = GameObject::Create("_skybox");
		_skybox->SetMaterial(_material_skybox);
		_skybox->m_material.SetTexture(_cubemap1, Active_Texture::LEVEL0);
		_skybox->SetMesh(Geometry.GetInverseCube());
		_skybox->m_useTransform = false;
		
		//
		GameObject* _crate1 = GameObject::Create("_crate1");
		_crate1->SetMaterial(_material_gbuffer);
		_crate1->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_crate1->SetMesh(Geometry.GetCylinderX());
		_crate1->m_transform.setPosition(1, 0, 0);
		_crate1->SetTint(Color3F(0.4f, 0.1f, 0.9f));
		
		
		GameObject* _crate2 = GameObject::Create("_crate2");
		_crate2->SetMaterial(_material_gbuffer);
		_crate2->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_crate2->SetMesh(Geometry.GetCylinderY());
		//_crate2->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		_crate2->m_transform.setPosition(0, 2, 0);
		

		GameObject* _crate3 = GameObject::Create("_crate3");
		_crate3->SetMaterial(_material_gbuffer);
		_crate3->m_material.SetTexture(_tex_crate, Active_Texture::LEVEL0);
		_crate3->SetMesh(Geometry.GetCylinderZ());
		//_crate3->SetColor(Color3F(0.4f, 0.7f, 0.3f));
		_crate3->m_transform.setPosition(1, 2, 0);

		// Parent Test
		_crate3->m_transform.addChild(&_crate2->m_transform);
		_crate2->m_transform.addChild(&_crate1->m_transform);

		_crate2->m_transform.setRotation(65, 12, 3);
		_crate3->m_transform.setRotation(45, 12, 3);
		// World Position
		_crate1->m_transform.setWorldPosition(-5, 0, 0);
		
		GameObject* _octo1 = GameObject::Create("_octo1");
		_octo1->SetMaterial(_material_gbuffer);
		_octo1->SetMesh(Geometry.GetOctahedron());
		_octo1->m_transform.setPosition(0, 0, 0);
		_octo1->m_transform.setScale(0.5f);
		_octo1->SetColor(Color3F(1, 1, 1));
		
		GameObject* _octo2 = GameObject::Create("_octo2");
		_octo2->SetMaterial(_material_gbuffer);
		_octo2->SetMesh(Geometry.GetOctahedron());
		_octo2->m_transform.setPosition(1, 0, 0);
		_octo2->m_transform.setScale(0.5f);
		_octo2->SetColor(Color3F(1, 0, 0));
		
		GameObject* _octo3 = GameObject::Create("_octo3");
		_octo3->SetMaterial(_material_gbuffer);
		_octo3->SetMesh(Geometry.GetOctahedron());
		_octo3->m_transform.setPosition(0, 1, 0);
		_octo3->m_transform.setScale(0.5f);
		_octo3->SetColor(Color3F(0, 1, 0));
		
		GameObject* _octo4 = GameObject::Create("_octo4");
		_octo4->SetMaterial(_material_gbuffer);
		_octo4->SetMesh(Geometry.GetOctahedron());
		_octo4->m_transform.setPosition(0, 0, 1);
		_octo4->m_transform.setScale(0.5f);
		_octo4->SetColor(Color3F(0, 0, 1));
		
		LightObject* _light1 = LightObject::Create("_light1", Light::Type::POINT);
		_light1->m_transform.setPosition(5, 0, 0);

		GameObject* _billboard1 = GameObject::Create("_quad1");
		_billboard1->SetMaterial(_material_gbuffer);
		_billboard1->SetMesh(Geometry.GetQuadX());
		_billboard1->m_transform.setPosition(7, 3, -3);
		_billboard1->m_material.SetTexture(Texture::Get("beato"), Active_Texture::LEVEL0);

		GameObject* _billboard2 = GameObject::Create("_quad2");
		_billboard2->SetMaterial(_material_gbuffer);
		_billboard2->SetMesh(Geometry.GetQuadY());
		_billboard2->m_transform.setPosition(7, 3, -3);
		_billboard2->m_material.SetTexture(Texture::Get("beato"), Active_Texture::LEVEL0);

		GameObject* _billboard3 = GameObject::Create("_quad3");
		_billboard3->SetMaterial(_material_gbuffer);
		_billboard3->SetMesh(Geometry.GetQuadZ());
		_billboard3->m_transform.setPosition(7, 3, -3);
		_billboard3->m_material.SetTexture(Texture::Get("beato"), Active_Texture::LEVEL0);

		//GameObject::Delete(_entity5);
		

		//GameObject::Delete("_octo3");
		//
		//_light1 = LightObject::Create("light1");


		
	}
	void Scene_Game::Destroy()
	{
		TerrainManager.Destroy();
	}

	void Scene_Game::Update()
	{

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
							Debug.DrawOBB(*Entity, EntityWorld, 3.0f, OBB_Color);
							Debug.DrawAABB(Entity->GetAABBMin() - Epsilon, Entity->GetAABBMax() + Epsilon, EntityWorld, 3.0f, AABB_Color);
						}
						// Draw outline around all instances
						else
						{
							//Vector4 WPosition = Vector4(Entity->m_transform.getWorldPosition(), 1);
							auto Instances = _mesh->m_instances.GetVertices();
							for (Matrix4x4 instanceMatrix : *Instances)
							{
								Vector3 Pos = Vector3(instanceMatrix[12], instanceMatrix[13], instanceMatrix[14]);
								Debug.DrawOBB(*Entity, Pos, 3.0f, OBB_Color);
								Debug.DrawAABB(Entity->GetAABBMin() - Epsilon, Entity->GetAABBMax() + Epsilon, Pos, 3.0f, AABB_Color);
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

							Debug.DrawWireframeSphere(LObject->m_transform.getWorldPosition(), Vector3(lightRadius), Color4F(LightPoint->m_color, 0.45f));
							break;
						}
					}
				}
			}
		}

		// End Frame Updates
		XGamePadManager.Update();

		// Imgui Specials
		if (_material_gbuffer)
		{
			_material_gbuffer->m_Wireframe = DEVCONSOLE_GET_BOOL("Gbuffer Wireframe", false);
		}
	}

	// DO NOT DRAW IN HERE //
	void Scene_Game::UpdateFixed()
	{

	}
	void Scene_Game::Draw()
	{

		_shader_gbuffer->SetProgramUniform<int>("TESTMODE", DEVCONSOLE_GET_INT("TESTMODE", 0));


		_fbo->bind();
		_fbo->clearBuffers();
		//
		GPUTimer::StartTimer("Gbuffer");
		//
		UBOManager.BindCamera(RenderManager.GetMainCamera());
		RenderManager.RenderSceneGameObjects();
		//
		GPUTimer::EndTimer("Gbuffer");

		

		_fbo_editor->bind();
		_fbo_editor->clearBuffers();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo->GetID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo_editor->GetID());
		glBlitFramebuffer(
			0, 0, _fbo->GetWidth(), _fbo->GetHeight(),
			0, 0, _fbo_editor->GetWidth(), _fbo_editor->GetHeight(),
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST
		);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo_editor->GetID());

		//
		GPUTimer::StartTimer("EditorObjects");
		//
		RenderManager.RenderEditorObjects();
		glUtil::clearBuffer(Buffer_Bit_Type::DEPTH);
		RenderManager.RenderEditorObjectsPostDepth();
		//
		GPUTimer::EndTimer("EditorObjects");

		//	if (Input.getKeyDown(KeyCode::K))
		//	{
		//		_fbo->bindTexture(1, Active_Texture::LEVEL0);
		//		GLubyte* pixels = new GLubyte[Window.GetWindowWidth() * Window.GetScreenHeight() * 4];
		//		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		//	
		//		GLuint r, g, b, a; // or GLbyte r, g, b, a;
		//	
		//		size_t x = 1;
		//		size_t y = 1;
		//	
		//		size_t elmes_per_line = 256 * 4; // elements per line = 256 * "RGBA"
		//	
		//		size_t row = y * elmes_per_line;
		//		size_t col = x * 4;
		//	
		//		r = pixels[row + col];
		//		g = pixels[row + col + 1];
		//		b = pixels[row + col + 2];
		//		a = pixels[row + col + 3];
		//	
		//	
		//		delete[] pixels;
		//	}

		//	// GBUFFER No model
		//	_material_gbuffer_no_model->Bind();
		//	
		//	glUtil::setActiveTexture(Active_Texture::LEVEL0);
		//	BlockAtlas.BindAtlas();
		//	
		//	TerrainManager.Draw();


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


		//	_shader_gbuffer->SetUniform("model", model1.getWorldModel());
		//	Geometry::GetCube()->Draw();
		//	
		//	_shader_gbuffer->SetUniform("model", model2.getWorldModel());
		//	Geometry::GetCube()->Draw();
		//	
		//	_shader_gbuffer->SetUniform("model", model3.getWorldModel());
		//	Geometry::GetCube()->Draw();
		//	
		//	_shader_gbuffer->SetUniform("model", model4.getWorldModel());
		//	Geometry::GetCube()->Draw();
		//	
		//	_shader_gbuffer->SetUniform("model", model5.getWorldModel());
		//	Geometry::GetCube()->Draw();

		

		// Editor Axis Objects
		// ~~ //
		if (!Window.IsCursorOnImguiWindow() && Window.GetCursor() == CursorMode::NORMAL)
		{
			auto SelectedEntities = Editor.GetSelectedEntities();

			// Highlighting over an Editor Axis not selected
			if (!Input.getMouseButton(MouseButton::LEFT) && !Editor.m_controlAxisClicked && Editor.HasSelection())
			{
				_fbo_colorpicker->bind();
				_fbo_colorpicker->clearBuffers();
				glUtil::blendState(false);

				_shader_colorPicker->Bind();

				_shader_colorPicker->SetUniform<bool>("VXL_useModel", true);
				_shader_colorPicker->SetUniformMatrix<Matrix4x4>("VXL_model", Editor.GetSelectionTransformModel(), true);

				Vector4 color;

				// Cube (ignored)
				_shader_colorPicker->SetUniform<Vector4>("VXL_colorID", Vector4(0, 0, 0, 0));
				Geometry.GetCubeSmall()->Draw();

				color = Util::DataConversion::uint_to_vec4(1u);
				_shader_colorPicker->SetUniform<Vector4>("VXL_colorID", color);
				Geometry.GetArrowX()->Draw();

				color = Util::DataConversion::uint_to_vec4(2u);
				_shader_colorPicker->SetUniform<Vector4>("VXL_colorID", color);
				Geometry.GetArrowY()->Draw();

				color = Util::DataConversion::uint_to_vec4(3u);
				_shader_colorPicker->SetUniform<Vector4>("VXL_colorID", color);
				Geometry.GetArrowZ()->Draw();

				// read pixel
				GLubyte *data = _fbo_colorpicker->readPixels(
					0,
					Input.getMouseX(),
					Window.GetResolutionHeight() - Input.getMouseY(),
					1,
					1
				);

				unsigned int EditorIndex;
				Util::DataConversion::uchars_to_uint(data, EditorIndex);
				delete[] data;

				switch (EditorIndex)
				{
				case 1u:
					Editor.m_controlAxis = Axis::X;
					break;
				case 2u:
					Editor.m_controlAxis = Axis::Y;
					break;
				case 3u:
					Editor.m_controlAxis = Axis::Z;
					break;
				default:
					Editor.m_controlAxis = Axis::NONE;
				}
			}

			
			// Drag Editor Axis around
			if (Input.getMouseButton(MouseButton::LEFT) && Editor.m_controlAxis != Axis::NONE && Editor.HasSelection())
			{
				Vector2 MouseChange = Vector2(
					(float)+Input.getMouseDragDeltaX(),
					(float)-Input.getMouseDragDeltaY()
				);

				// Clip space axis should only be calculated on the initial click
				static Vector2 Axis_ScreenDirection;
				static Vector2 CenterAxis_ScreenPos;
				static Vector2 Axis_ScreenPos;
				static float DragSpeed;

				if (Editor.m_controlAxisClicked == false)
				{
					// Object MVP
					Matrix4x4 MVP = RenderManager.GetMainCamera()->getViewProjection() * Editor.GetSelectionTransformModel();

					// Calculate Center Axis in ClipSpace
					Vector4 ClipSpaceAxis = MVP * Vector4(0, 0, 0, 1);
					ClipSpaceAxis /= ClipSpaceAxis.w; // [-1 to +1] Range
					CenterAxis_ScreenPos = ClipSpaceAxis.GetVector2(0, 1);

					// Calculate Selected Axis in ClipSpace
					switch (Editor.m_controlAxis)
					{
					case Axis::X:
						ClipSpaceAxis = MVP * Vector4(1, 0, 0, 1);
						break;

					case Axis::Y:
						ClipSpaceAxis = MVP * Vector4(0, 1, 0, 1);
						break;

					case Axis::Z:
						ClipSpaceAxis = MVP * Vector4(0, 0, 1, 1);
						break;
					}

					ClipSpaceAxis /= ClipSpaceAxis.w; // [-1 to +1] Range
					Axis_ScreenPos = ClipSpaceAxis.GetVector2(0, 1);

					//std::cout << Axis_ScreenPos << std::endl;
					//std::cout << CenterAxis_ScreenPos << std::endl;
					//std::cout << "~~~" << std::endl;

					Axis_ScreenDirection = (Axis_ScreenPos - CenterAxis_ScreenPos).Normalize();

					// Update Drag Speed
					DragSpeed = Editor.GetAverageSelectionDistanceFromEditorCamera() / 3.0f;
				}

				assert(Axis_ScreenDirection.LengthSqr() != 0.0f);

				// Project Mouse Change onto Xaxis in screenspace, now we'll see how far the mouse drags across the axis
				static float PreviousDragAmount = 0.0f;
				float DragAmount = MouseChange.ProjectLength(Axis_ScreenDirection) * DragSpeed;
				// Drag Multiply

				if (Editor.m_controlAxisClicked == false)
					PreviousDragAmount = DragAmount;

				// No movement occurs if mouse change is empty
				if (!MouseChange.Is_Empty())
				{
					Vector3 MoveDirection;

					switch (Editor.m_controlAxis)
					{
					case Axis::X:
						if (Editor.m_controlAxisLocal)
							MoveDirection = SelectedEntities[0]->m_transform.getRight();
						else
							MoveDirection = Vector3::RIGHT;
						break;
					case Axis::Y:
						if (Editor.m_controlAxisLocal)
							MoveDirection = SelectedEntities[0]->m_transform.getUp();
						else
							MoveDirection = Vector3::UP;
						break;
					case Axis::Z:
						if (Editor.m_controlAxisLocal)
							MoveDirection = SelectedEntities[0]->m_transform.getForward();
						else
							MoveDirection = Vector3::FORWARD;
						break;
					}

					for (auto& Entity : SelectedEntities)
					{
						Entity->m_transform.increaseWorldPosition(MoveDirection * 0.01f * (DragAmount - PreviousDragAmount));
					}
				}

				
				//std::cout << (MouseChange) << std::endl;
				//std::cout << (Axis_ScreenDirection) << std::endl;
				//std::cout << (DragAmount) << std::endl;
				//std::cout << (PreviousDragAmount) << std::endl;
				//std::cout << "~~~" << std::endl;

				PreviousDragAmount = DragAmount;

				Editor.m_controlAxisClicked = true;
			}
			else
				Editor.m_controlAxisClicked = false;
		}

		// Select Object in scene
		// ~~ //
		if (!Editor.m_controlAxisClicked && Input.getMouseButtonDown(MouseButton::LEFT) && !Window.IsCursorOnImguiWindow() && Window.GetCursor() == CursorMode::NORMAL)
		{
			if (DEVCONSOLE_GET_BOOL("Objects are Clickable", true))
			{
				_fbo_colorpicker->bind();
				_fbo_colorpicker->clearBuffers();
				glUtil::blendState(false);

				_shader_colorPicker->Bind();

				auto Entities = RenderManager.GetAllEntities();
				unsigned int EntityCount = (unsigned int)Entities.size();

				for (unsigned int i = 0; i < EntityCount; i++)
				{
					auto Entity = Entities[i];

					// Ignore if not active or not clickable
					if (!Entity->IsFamilyActive() || !Entity->IsSelectable())
						continue;

					// Convert ID into color
					Vector4 color = Util::DataConversion::uint_to_vec4(i + 1u);

					// Vert
					_shader_colorPicker->SetUniformMatrix<Matrix4x4>("VXL_model", Entity->m_transform.getWorldModel(), true);
					_shader_colorPicker->SetUniform("VXL_useInstancing", false);
					_shader_colorPicker->SetUniform("VXL_useModel", Entity->m_useTransform);
					// Frag
					_shader_colorPicker->SetUniform<Vector4>("VXL_colorID", color);

					// Different Effect based on type
					if (Entity->GetType() == EntityType::GAMEOBJECT)
					{
						GameObject* GameObj = dynamic_cast<GameObject*>(Entity);

						_shader_colorPicker->SetUniform("VXL_useInstancing", !GameObj->GetMesh()->m_instances.Empty());

						GameObj->GetMesh()->Draw();
					}
					else if (Entity->GetType() == EntityType::CAMERA)
					{
						Geometry.GetSphere()->Draw();
					}
					else if (Entity->GetType() == EntityType::LIGHT)
					{
						Geometry.GetSphere()->Draw();
					}

				}

				GLubyte *data = _fbo_colorpicker->readPixels(
					0,
					Input.getMouseX(),
					Window.GetResolutionHeight() - Input.getMouseY(),
					1,
					1
				);

				//	std::cout <<
				//	(unsigned int)data[0] << ' ' <<
				//	(unsigned int)data[1] << ' ' <<
				//	(unsigned int)data[2] << ' ' <<
				//	(unsigned int)data[3] << ' ' <<
				//	std::endl;

				unsigned int EntityIndex;
				Util::DataConversion::uchars_to_uint(data, EntityIndex);
				EntityIndex--;
				delete[] data;

				// Found an entity
				if (EntityIndex < EntityCount)
				{
					if (Input.getKey(KeyCode::LEFT_CONTROL))
					{
						if (!Entities[EntityIndex]->IsSelected())
							Editor.AddSelection(Entities[EntityIndex]);
						else
							Editor.RemoveSelection(Entities[EntityIndex]);
					}
					else
					{
						Editor.ClearSelection();
						Editor.AddSelection(Entities[EntityIndex]);
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

		// ~~ //

		if (RenderManager.GetAllEntities().size() > 0)
			Editor.UpdateSelectionTransformModel();
		
		// ~~ //

		// Backbuffer
		FramebufferObject::unbind();
		//glDrawBuffer(GL_BACK);

		Window.BindWindowViewport();

		glUtil::blendState(false);
		glUtil::wireframe(false);
		glUtil::depthTest(Depth_Pass_Rule::ALWAYS);

		// ~~~~~~~~~~~~~~~~~ //
		glUtil::clearColor(Color4F(0, 0, 0, 0));
		glUtil::clearBuffer();

		
		// Final Pass / Back Buffer
		switch (FBO_OVERRIDE)
		{
			// Output Normally
			case 0:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 3);

				_fbo->bindTexture(0, Active_Texture::LEVEL0);
				_fbo_editor->bindTexture(0, Active_Texture::LEVEL1);
				break;
			}
			// Show Albedo
			case 1:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 0);

				_fbo->bindTexture(0, Active_Texture::LEVEL0);
				break;
			}
			// Show Normal
			case 2:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 1);

				_fbo->bindTexture(1, Active_Texture::LEVEL0);
				break;
			}
			// Show Depth
			case 3:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 2);
				_shader_showRenderTarget->SetUniform("zNear", RenderManager.GetMainCamera()->getZnear());
				_shader_showRenderTarget->SetUniform("zFar", RenderManager.GetMainCamera()->getZfar());

				_fbo->bindDepth(Active_Texture::LEVEL0);
				break;
			}
			// Show Editor
			case 4:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 0);

				_fbo_editor->bindTexture(0, Active_Texture::LEVEL0);
				break;
			}
			// Show Editor Depth
			case 5:
			{
				_shader_showRenderTarget->Bind();
				_shader_showRenderTarget->SetUniform("outputMode", 2);
				_shader_showRenderTarget->SetUniform("zNear", RenderManager.GetMainCamera()->getZnear());
				_shader_showRenderTarget->SetUniform("zFar", RenderManager.GetMainCamera()->getZfar());

				_fbo_editor->bindDepth(Active_Texture::LEVEL0);
				break;
			}
		}
		RenderManager.RenderFullScreen();
		//
	
		
		//	// Normals test
		//	if (DevConsole.GetBool("Show Normals", false))
		//	{
		//		
		//		
		//		glUtil::viewport(0, 0, Window.GetScreenWidth() / 4, Window.GetScreenHeight() / 4);
		//		
		//		Geometry.GetFullQuad()->Draw();
		//	}
		//	// Depth test
		//	if (DevConsole.GetBool("Show Depth", false))
		//	{
		//		_shader_showRenderTarget->SetUniform("outputMode", 2);
		//		_shader_showRenderTarget->SetUniform("zNear", Camera::GetMain()->getZnear());
		//		_shader_showRenderTarget->SetUniform("zFar", Camera::GetMain()->getZfar());
		//	
		//		glUtil::viewport(Window.GetScreenWidth() / 4, 0, Window.GetScreenWidth() / 4, Window.GetScreenHeight() / 4);
		//		_fbo->bindDepth(Active_Texture::LEVEL0);
		//		Geometry.GetFullQuad()->Draw();
		//	}

		// _fbo->bindDepth(Active_Texture::LEVEL0);
		// Geometry::GetFullQuad()->Draw();


		//glDepthFunc(GL_LEQUAL);
		//Window.ViewportToWindowSize();


		
		//	// SUBROUTINES
		//	glSubroutine Sub = *_shader_gbuffer->GetSubroutine(ShaderType::FRAGMENT);
		//	Sub.Set("Colour1", "ColorRed");
		//	Sub.Set("Colour2", "ColorGreen");
		//	Sub.Set("myNum", "t2");
		//	Sub.Bind();
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

		//	// UNIFORMS
		//	_shader_gbuffer->SetUniform("value1", 0.1f);
		//	_shader_gbuffer->SetUniform("value2", 0.1f);
		//	_shader_gbuffer->SetUniform("value3", 0.1f);
		//	
		//	// Uniform a = Sp->GetUniform("value1");
		//	a.set<float>(1.0f);
		//	Uniform b = Sp->GetUniform("value2");
		//	b.set<float>(0.0f);
		//	Uniform c = Sp->GetUniform("value3");
		//	c.set<float>(0.5f);
		//	
		//	Shader_Gbuffer->SetUniform("model", model.getModel());
		//	
		//	Shader_Gbuffer->SetUniform("view", cam.getView());
		//	Shader_Gbuffer->SetUniform("projection", cam.getProjection());

	}
	void Scene_Game::DrawImGui()
	{
		// Shader Errors
		ShaderErrors.Draw();
		// Inspector
		Inspector.Draw();
		// Hierarchy
		Hierarchy.Draw();
		// Performance
		Performance.Draw();
		// Dev Console
		DevConsole.Draw(this);
	}
}