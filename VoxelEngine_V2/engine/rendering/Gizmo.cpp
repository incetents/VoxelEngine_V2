// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Gizmo.h"

#include "../input/Input.h"
#include "../math/Raycast.h"
#include "../modules/Entity.h"
#include "../modules/Material.h"
#include "../objects/Camera.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/Geometry.h"
#include "../rendering/Mesh.h"
#include "../rendering/Debug.h"
#include "../utilities/Asset.h"
#include "../window/window.h"
#include "RenderManager.h"

namespace Vxl
{
	// Static
	FramebufferObject*	Gizmo::m_fbo;
	MeshIndex			Gizmo::m_mesh_HalfQuadAxisX[4];
	MeshIndex			Gizmo::m_mesh_HalfQuadAxisY[4];
	MeshIndex			Gizmo::m_mesh_HalfQuadAxisZ[4];
	MeshIndex			Gizmo::m_mesh_ScaleCube[3];

	// Utility
	Vector3 Gizmo::CameraRayHitPlane(const Vector3& normal)
	{
		// Main Camera
		Camera* mainCamera = Assets::getCamera(RenderManager.m_mainCamera);
		VXL_ASSERT(mainCamera, "missing camera");

		// Make a raycast with the Mouse's ScreenPosition
		Vector3 ScreenAim = mainCamera->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));

		// Calculate where it hits
		Plane p(normal, m_transform.WorldPosition);
		Ray r(mainCamera->m_transform.getWorldPosition(), ScreenAim);
		
		// Return hit location // Guaranteed intersection
		RayHit hit = Intersection(r, p); 
		return hit.m_location;
	}
	Vector3 Gizmo::GetGizmoDirection(Axis axis)
	{
		switch (axis)
		{
		case Axis::X:
			return m_transform.Right;
		case Axis::Y:
			return m_transform.Up;
		case Axis::Z:
			return m_transform.Forward;
		default:
			VXL_ERROR("GizmoDirection Doesn't Support NONE/ALL as a parameter");
		}
		return Vector3::ZERO;
	}

	// Global GL Resources
	void Gizmo::InitGLResources()
	{
		// Mesh
		m_mesh_HalfQuadAxisX[0] = Geometry.GenerateQuad("", Axis::X, 0.5f, Vector3(0, -0.25f, -0.25f));
		m_mesh_HalfQuadAxisX[1] = Geometry.GenerateQuad("", Axis::X, 0.5f, Vector3(0, +0.25f, -0.25f));
		m_mesh_HalfQuadAxisX[2] = Geometry.GenerateQuad("", Axis::X, 0.5f, Vector3(0, -0.25f, +0.25f));
		m_mesh_HalfQuadAxisX[3] = Geometry.GenerateQuad("", Axis::X, 0.5f, Vector3(0, +0.25f, +0.25f));
		//
		m_mesh_HalfQuadAxisY[0] = Geometry.GenerateQuad("", Axis::Y, 0.5f, Vector3(-0.25f, 0, -0.25f));
		m_mesh_HalfQuadAxisY[1] = Geometry.GenerateQuad("", Axis::Y, 0.5f, Vector3(+0.25f, 0, -0.25f));
		m_mesh_HalfQuadAxisY[2] = Geometry.GenerateQuad("", Axis::Y, 0.5f, Vector3(-0.25f, 0, +0.25f));
		m_mesh_HalfQuadAxisY[3] = Geometry.GenerateQuad("", Axis::Y, 0.5f, Vector3(+0.25f, 0, +0.25f));
		//
		m_mesh_HalfQuadAxisZ[0] = Geometry.GenerateQuad("", Axis::Z, 0.5f, Vector3(-0.25f, -0.25f, 0));
		m_mesh_HalfQuadAxisZ[1] = Geometry.GenerateQuad("", Axis::Z, 0.5f, Vector3(+0.25f, -0.25f, 0));
		m_mesh_HalfQuadAxisZ[2] = Geometry.GenerateQuad("", Axis::Z, 0.5f, Vector3(-0.25f, +0.25f, 0));
		m_mesh_HalfQuadAxisZ[3] = Geometry.GenerateQuad("", Axis::Z, 0.5f, Vector3(+0.25f, +0.25f, 0));
		//
		m_mesh_ScaleCube[0] = Geometry.GenerateCube("", 0.25f, Vector3(1, 0, 0));
		m_mesh_ScaleCube[1] = Geometry.GenerateCube("", 0.25f, Vector3(0, 1, 0));
		m_mesh_ScaleCube[2] = Geometry.GenerateCube("", 0.25f, Vector3(0, 0, 1));

		// FBO
		auto fbo_id = GlobalAssets.createFramebuffer();
		m_fbo = GlobalAssets.getFramebufferObject(fbo_id);
		m_fbo->setSizeToViewportSize();
		m_fbo->bind();
		//
		auto id = SceneAssets.createRenderTexture(
			m_fbo->getWidth(), m_fbo->getHeight(), 
			TextureFormat::RGBA8, TexturePixelType::UNSIGNED_BYTE, false
		);
		auto id_depth = SceneAssets.createRenderBufferDepth(
			m_fbo->getWidth(), m_fbo->getHeight(),
			TextureDepthFormat::DEPTH16
		);

		m_fbo->setAttachment(0, SceneAssets.getRenderTexture(id));
		m_fbo->setDepth(SceneAssets.getRenderBufferDepth(id_depth));
		//
		m_fbo->checkFBOStatus();
		m_fbo->unbind();
	}
	void Gizmo::DestroyGLResources()
	{

		//FramebufferObject::DeleteNamedAsset(m_fbo);
		m_fbo = nullptr;
	}

	// Update
	void Gizmo::Update(const std::vector<Entity*> _entities)
	{
		Camera* mainCamera = Assets::getCamera(RenderManager.m_mainCamera);
		VXL_ASSERT(mainCamera, "Missing camera for Gizmo");

		// If only 1 Entity and its the EditorCamera, ignore it
		if (_entities.size() == 0)
		{
			m_show = false;
			return;
		}

		// Show
		m_show = true;

		// Update Distance from Camera as a matrix4x4
		if (!m_clicked)
		{
			m_distanceToCamera = (m_transform.WorldPosition - mainCamera->m_transform.getWorldPosition()).Length();
			m_constantScale = m_distanceToCamera * 0.5f;
			m_constantScaleMatrix = Matrix4x4::GetScale(Vector3(m_constantScale));
		}

		// Don't move Gizmo while clicking [unless it's translation]
		if (!m_clicked || m_mode == Mode::TRANSLATE)
		{
			// Update Average Selection Position
			Vector3 AverageSelection = Vector3::ZERO;
			uint32_t count = (uint32_t)_entities.size();
			for (const auto& _Entity : _entities)
			{
				AverageSelection += _Entity->m_transform.getWorldPosition();
			}

			if (count)
			{
				float invaTotal = 1.0f / (float)count;
				AverageSelection *= invaTotal;
			}

			// Store World Position
			m_transform.WorldPosition = AverageSelection;// _entity.m_transform.getWorldPosition();
		}

		// Set Axis-Aligned Pivot
		if ((m_pivotAxisAligned && m_mode != Mode::SCALE))
		{
			m_transform.Model = Matrix4x4::GetTranslate(m_transform.WorldPosition) * m_constantScaleMatrix;
			m_transform.Forward = Vector3::FORWARD;
			m_transform.Up = Vector3::UP;
			m_transform.Right = Vector3::RIGHT;
		}

		// Set Local Pivot
		else
		{
			Matrix3x3 RotationMatrix = _entities[0]->m_transform.getWorldRotation().GetMatrix3x3();

			m_transform.Model = Matrix4x4(RotationMatrix, m_transform.WorldPosition) * m_constantScaleMatrix;
			m_transform.Forward = _entities[0]->m_transform.getForward();
			m_transform.Up = _entities[0]->m_transform.getUp();
			m_transform.Right = _entities[0]->m_transform.getRight();
		}

		// Translate XYZ Planes to be in correct position
		if (m_mode == Mode::TRANSLATE)
		{
			Vector3 SelectionTransformToCamera = (mainCamera->m_transform.getWorldPosition() - m_transform.WorldPosition);

			// Which side of the axis is the camera on
			m_camAxisSide[0] = SelectionTransformToCamera.Dot(m_transform.Right) > 0.0f;
			m_camAxisSide[1] = SelectionTransformToCamera.Dot(m_transform.Up) > 0.0f;
			m_camAxisSide[2] = SelectionTransformToCamera.Dot(m_transform.Forward) > 0.0f;
		}
		// Scale Boxes move along with the cursor when dragging
		else if (m_mode == Mode::SCALE)
		{
			// Default position
			if (!m_clicked)
			{
				X_Offset = Vector3::ZERO;
				Y_Offset = Vector3::ZERO;
				Z_Offset = Vector3::ZERO;
			}
			// Moving Based on Drag Amount
			else
			{
				X_Offset = (m_selectedAxis == Axis::X || m_selectedAxis == Axis::ALL) ? Vector3(m_dragAmount / m_constantScale, 0, 0) : Vector3::ZERO;
				Y_Offset = (m_selectedAxis == Axis::Y || m_selectedAxis == Axis::ALL) ? Vector3(0, m_dragAmount / m_constantScale, 0) : Vector3::ZERO;
				Z_Offset = (m_selectedAxis == Axis::Z || m_selectedAxis == Axis::ALL) ? Vector3(0, 0, m_dragAmount / m_constantScale) : Vector3::ZERO;
			}
		}
		// Rotation, figure out if axis can be seen
		else
		{
			// If Camera can see Axis
			Vector3 ModelToCamera = (mainCamera->m_transform.getWorldPosition() - m_transform.WorldPosition).Normalize();

			// X Circle //
			m_showScaleGizmo[0] = (fabs(m_transform.Right.Dot(ModelToCamera)) > 0.05f);
			m_showScaleGizmo[1] = (fabs(m_transform.Up.Dot(ModelToCamera)) > 0.05f);
			m_showScaleGizmo[2] = (fabs(m_transform.Forward.Dot(ModelToCamera)) > 0.05f);
		}

		// No Selection
		if (!Input.getMouseButton(MouseButton::LEFT))
		{
			m_worldPositionStorage.clear();
			m_worldScaleStorage.clear();
			m_clicked = false;
			m_dragAmount = 0.0f;
			return;
		}

		// On Click on a Gizmo Portion
		if (Input.getMouseButtonDown(MouseButton::LEFT) && (m_selectedAxis != Axis::NONE || m_selectedPlane != Axis::NONE))
		{
			switch (m_mode)
			{
			case Mode::TRANSLATE:
			{
				// Translate based on Camera's Forward Plane
				if (m_selectedPlane == Axis::ALL)
				{
					m_targetNormal = mainCamera->m_transform.getForward();
					m_dragStart = CameraRayHitPlane(m_targetNormal);
				}
				// Translate based on Plane
				else if (m_selectedPlane != Axis::NONE)
				{
					m_targetNormal = GetGizmoDirection(m_selectedPlane);
					m_dragStart = CameraRayHitPlane(m_targetNormal);
				}
				// Translate based on Axis
				else
				{
					m_dragStart = m_transform.WorldPosition;
					Vector3 Direction = GetGizmoDirection(m_selectedAxis);
					// Make a raycast with the Mouse's ScreenPosition
					Vector3 ScreenAim = mainCamera->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
					Ray		ViewRay(mainCamera->m_transform.getWorldPosition(), ScreenAim);
					Ray		DirectionRay(m_dragStart, Direction);

					// Find shortest distance between Axis and Camera Ray
					ShortestDistance(ViewRay, DirectionRay);
					m_dragAmount = DirectionRay.m_t;
				}

				// Store all world positions
				m_worldPositionStorage.clear();
				m_worldPositionStorage.reserve(_entities.size());
				uint32_t _entityCount = (uint32_t)_entities.size();
				for (uint32_t i = 0; i < _entityCount; i++)
					m_worldPositionStorage.push_back(_entities[i]->m_transform.getWorldPosition());
				m_worldPositionStorage.shrink_to_fit();
				
			}
			break;

			case Mode::SCALE:
			{
				VXL_ASSERT(m_selectedAxis != Axis::NONE, "GizmoScale Axis::NONE");

				// Global Scale
				if (m_selectedAxis == Axis::ALL)
				{
					m_dragAmount = 0.0f;

					// Get Mouse position in -1 to +1 range of screen
					m_dragStart = Input.getMousePosScreenspace(true);
				}
				// Selecting Axis
				else if (m_selectedAxis != Axis::NONE)
				{
					m_dragStart = m_transform.WorldPosition;

					Vector3 Direction = GetGizmoDirection(m_selectedAxis);
					// Make a raycast with the Mouse's ScreenPosition
					Vector3 ScreenAim = mainCamera->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
					Ray		ViewRay(mainCamera->m_transform.getWorldPosition(), ScreenAim);
					Ray		DirectionRay(m_dragStart, Direction);
					
					// Find shortest distance between Axis and Camera Ray
					ShortestDistance(ViewRay, DirectionRay);
					m_dragPrev = DirectionRay.m_t;

					// Store all original scales to avoid precision loss
					uint32_t selectionCount = (uint32_t)_entities.size();
					m_worldScaleStorage.clear();
					m_worldScaleStorage.reserve(selectionCount);
					
					for (uint32_t i = 0; i < selectionCount; i++)
						m_worldScaleStorage.push_back(_entities[i]->m_transform.getScale()[(int)m_selectedAxis]);

					m_worldScaleStorage.shrink_to_fit();
				}
			}
			break;

			case Mode::ROTATE:
			{
				// Direction Click
				m_targetNormal = GetGizmoDirection(m_selectedPlane);
				m_dragStart = (CameraRayHitPlane(m_targetNormal) - m_transform.WorldPosition).NormalizeAccurate();
				m_rotationDragStart = m_dragStart;
			}
			break;
			}

			m_clicked = true;
		}

		// On Update Gizmo based on selection and mouse movements
		if (Input.getMouseButton(MouseButton::LEFT) && m_clicked)
		{
			switch (m_mode)
			{
			case Mode::TRANSLATE:
			{
				// Translate Plane or All of em
				if (m_selectedPlane != Axis::NONE)
				{
					m_dragEnd = CameraRayHitPlane(m_targetNormal);

					// Move Selected Objects
					uint32_t _entityCount = (uint32_t)_entities.size();
					for (uint32_t i = 0; i < _entityCount; i++)
					{
						Vector3 Target = m_worldPositionStorage[i] + (m_dragEnd - m_dragStart);
						_entities[i]->m_transform.setWorldPosition(Target);
					}
				}
				// Translate based on Axis
				else if (m_selectedAxis != Axis::NONE)
				{
					Vector3 Direction = GetGizmoDirection(m_selectedAxis);
					// Make a raycast with the Mouse's ScreenPosition
					Vector3 ScreenAim = mainCamera->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
					Ray		ViewRay(mainCamera->m_transform.getWorldPosition(), ScreenAim);
					Ray		DirectionRay(m_dragStart, Direction);

					// Find shortest distance between Axis and Camera Ray
					ShortestDistance(ViewRay, DirectionRay);

					// Check if snapping should happen
					float totalDrag = DirectionRay.m_t - m_dragAmount;

					// Fix for snapping
					if(m_translateSnapping)
						totalDrag = round(totalDrag / m_translateSnapAmount) * m_translateSnapAmount;

					uint32_t _entityCount = (uint32_t)_entities.size();
					for (uint32_t i = 0; i < _entityCount; i++)
					{
						_entities[i]->m_transform.setWorldPosition(m_worldPositionStorage[i] - Direction.NormalizeAccurate() * totalDrag);
					}
				}

			}
			break;

			case Mode::SCALE:
			{
				VXL_ASSERT(m_selectedAxis != Axis::NONE, "GizmoScale Axis::NONE");

				// Global Scale
				if (m_selectedAxis == Axis::ALL)
				{
					// Get Mouse position in -1 to +1 range of screen
					Vector2 MouseScreenSpace = Input.getMousePosScreenspace(true);

					// Distance from mouse to center point of Gizmo
					Vector2 MouseDelta = MouseScreenSpace - Vector2(m_dragStart);
					float Distance = MouseDelta.x + MouseDelta.y;

					// Scale everything based on distance
					uint32_t selectionCount = (uint32_t)_entities.size();
					for (uint32_t i = 0; i < selectionCount; i++)
					{
						_entities[i]->m_transform.increaseScale(Distance - m_dragAmount);
					}
					// Use "m_totalDrag1" similar to a "lastTime" value of a timer
					m_dragAmount = Distance;
				}
				// Selecting Axis
				else if (m_selectedAxis != Axis::NONE)
				{
					Vector3 Direction = GetGizmoDirection(m_selectedAxis);
					// Make a raycast with the Mouse's ScreenPosition
					Vector3 ScreenAim = mainCamera->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
					Ray		ViewRay(mainCamera->m_transform.getWorldPosition(), ScreenAim);
					Ray		DirectionRay(m_dragStart, Direction);

					// Find shortest distance between Axis and Camera Ray
					ShortestDistance(ViewRay, DirectionRay);
					m_dragAmount = (-DirectionRay.m_t + m_dragPrev);

					uint32_t selectionCount = (uint32_t)_entities.size();
					switch (m_selectedAxis)
					{
					case Axis::X:
						for (uint32_t i = 0; i < selectionCount; i++)
						{
							_entities[i]->m_transform.setScaleX(m_worldScaleStorage[i] * (m_dragAmount / m_constantScale + 1.0f));
						}
						break;
					case Axis::Y:
						for (uint32_t i = 0; i < selectionCount; i++)
						{
							_entities[i]->m_transform.setScaleY(m_worldScaleStorage[i] * (m_dragAmount / m_constantScale + 1.0f));
						}
						break;
					case Axis::Z:
						for (uint32_t i = 0; i < selectionCount; i++)
						{
							_entities[i]->m_transform.setScaleZ(m_worldScaleStorage[i] * (m_dragAmount / m_constantScale + 1.0f));
						}
						break;
					}
				}

			}
			break;

			case Mode::ROTATE:
			{
				// Direction Click
				m_dragEnd = (CameraRayHitPlane(m_targetNormal) - m_transform.WorldPosition).NormalizeAccurate();

				// Only rotate if both vectors are not the same
				if (!m_dragEnd.CompareFuzzy(m_dragStart))
				{
					// Check angle between the two
					float degrees = Vector3::GetAngleDegrees(m_dragStart, m_dragEnd);
					// Check if going in other direction
					Vector3 c = Vector3::Cross(m_dragStart, m_dragEnd);
					float sign = c.Dot(m_targetNormal);
					if (sign > 0)
						degrees = -degrees;

					// Snap rotation
					if (m_rotateSnapping)
					{
						// Fix degrees to multiple of SnapAmount
						if (fabs(degrees) > m_rotateSnappingAmount)
						{
							degrees = round(degrees / m_rotateSnappingAmount) * m_rotateSnappingAmount;
						}
						// Don't rotate
						else
							break;
					}

					// Rotate objects around selected axis
					uint32_t selectionCount = (uint32_t)_entities.size();
					for (uint32_t i = 0; i < selectionCount; i++)
					{
						_entities[i]->m_transform.rotateAroundAxis(m_targetNormal, degrees);
					}

					// Update previous rotation
					m_dragStart = m_dragEnd;

				}

			}
			break;
			}
		}
	}

	// Render
	void Gizmo::RenderOnScreen(MaterialIndex _gizmoMaterial)
	{
		if (!m_show)
			return;

		Material* gizmoMaterial = Assets::getMaterial(_gizmoMaterial);
		if (gizmoMaterial)
		{
			gizmoMaterial->bindProgram();

			Graphics::Uniform uniform_useModel = gizmoMaterial->getUniform("VXL_useModel");
			Graphics::Uniform uniform_model = gizmoMaterial->getUniform("VXL_model");
			Graphics::Uniform uniform_normalMatrix = gizmoMaterial->getUniform("VXL_normalMatrix");
			Graphics::Uniform uniform_color = gizmoMaterial->getUniform("VXL_color");
			Graphics::Uniform uniform_alpha = gizmoMaterial->getUniform("VXL_alpha");

			uniform_useModel.send(true);
			uniform_model.sendMatrix(m_transform.Model, true);
			uniform_normalMatrix.sendMatrix(Matrix3x3(m_transform.Model), true);

			// Movement //
			if (m_mode == Mode::TRANSLATE)
			{
				gizmoMaterial->sendUniform("ignoreLight", false);

				// X Axis //
				if (m_selectedAxis == Axis::X)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::RED);

				Assets::getMesh(Geometry.GetArrowX())->Draw();

				// Y Axis //
				if (m_selectedAxis == Axis::Y)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::GREEN);

				Assets::getMesh(Geometry.GetArrowY())->Draw();

				// Z Axis //
				if (m_selectedAxis == Axis::Z)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::BLUE);

				Assets::getMesh(Geometry.GetArrowZ())->Draw();

				// Cull
				Graphics::SetCullMode(CullMode::NO_CULL);
				// Alpha
				uniform_alpha.send(0.85f);

				// X Plane
				if (m_selectedPlane == Axis::X)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::RED);

				Mesh* mesh_HalfQuadAxisX = Assets::getMesh(m_mesh_HalfQuadAxisX[(m_camAxisSide[1] ? 1 : 0) + (m_camAxisSide[2] ? 2 : 0)]);
				mesh_HalfQuadAxisX->Draw();
				
				// Y Plane
				if (m_selectedPlane == Axis::Y)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::GREEN);

				Mesh* mesh_HalfQuadAxisY = Assets::getMesh(m_mesh_HalfQuadAxisY[(m_camAxisSide[0] ? 1 : 0) + (m_camAxisSide[2] ? 2 : 0)]);
				mesh_HalfQuadAxisY->Draw();

				// Z Plane
				if (m_selectedPlane == Axis::Z)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::BLUE);

				Mesh* mesh_HalfQuadAxisZ = Assets::getMesh(m_mesh_HalfQuadAxisZ[(m_camAxisSide[0] ? 1 : 0) + (m_camAxisSide[1] ? 2 : 0)]);
				mesh_HalfQuadAxisZ->Draw();

				// Cull
				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
				// Alpha
				uniform_alpha.send(1.0f);

				// Small cube in the middle //
				if (m_selectedPlane == Axis::ALL)
					if (m_clicked)
						uniform_color.send(Color3F::GREY);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::WHITE);
				//
				Assets::getMesh(Geometry.GetCubeSmall())->Draw();

			}
			else if (m_mode == Mode::SCALE)
			{
				gizmoMaterial->sendUniform("ignoreLight", false);

				// Gizmo Additional Debug Lines
				float XLineLength = (m_clicked && (m_selectedAxis == Axis::X || m_selectedAxis == Axis::ALL)) ? m_constantScale + m_dragAmount - 0.0f : m_constantScale;
				float YLineLength = (m_clicked && (m_selectedAxis == Axis::Y || m_selectedAxis == Axis::ALL)) ? m_constantScale + m_dragAmount - 0.0f : m_constantScale;
				float ZLineLength = (m_clicked && (m_selectedAxis == Axis::Z || m_selectedAxis == Axis::ALL)) ? m_constantScale + m_dragAmount - 0.0f : m_constantScale;

				Debug.DrawLineNoDepth(m_transform.WorldPosition, m_transform.WorldPosition + m_transform.Right * XLineLength, 5.0f, Color4F::RED, Color4F::RED);
				Debug.DrawLineNoDepth(m_transform.WorldPosition, m_transform.WorldPosition + m_transform.Up * YLineLength, 5.0f, Color4F::GREEN, Color4F::GREEN);
				Debug.DrawLineNoDepth(m_transform.WorldPosition, m_transform.WorldPosition + m_transform.Forward * ZLineLength, 5.0f, Color4F::BLUE, Color4F::BLUE);
				

				// X Cube //
				Matrix4x4 X_Model = m_transform.Model * Matrix4x4::GetTranslate(X_Offset);
				uniform_model.sendMatrix(X_Model, true);
				if (m_selectedAxis == Axis::X)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::RED);

				Assets::getMesh(m_mesh_ScaleCube[0])->Draw();

				// Y Cube //
				Matrix4x4 Y_Model = m_transform.Model * Matrix4x4::GetTranslate(Y_Offset);
				uniform_model.sendMatrix(Y_Model, true);
				if (m_selectedAxis == Axis::Y)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::GREEN);

				Assets::getMesh(m_mesh_ScaleCube[1])->Draw();

				// Z Cube //
				Matrix4x4 Z_Model = m_transform.Model * Matrix4x4::GetTranslate(Z_Offset);
				uniform_model.sendMatrix(Z_Model, true);
				if (m_selectedAxis == Axis::Z)
					if (m_clicked)
						uniform_color.send(Color3F::WHITE);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::BLUE);

				Assets::getMesh(m_mesh_ScaleCube[2])->Draw();

				// Small cube in the middle //
				uniform_model.sendMatrix(m_transform.Model, true);
				uniform_alpha.send(0.85f);

				if (m_selectedAxis == Axis::ALL)
					if (m_clicked)
						uniform_color.send(Color3F::GREY);
					else
						uniform_color.send(Color3F::YELLOW);
				else
					uniform_color.send(Color3F::WHITE);

				Assets::getMesh(Geometry.GetCubeSmall())->Draw();

				// revert
				uniform_alpha.send(1.0f);

			}
			else if (m_mode == Mode::ROTATE)
			{
				gizmoMaterial->sendUniform("ignoreLight", true);

				// Draw Drag Directions
				if (m_clicked)
				{
					Color4F LineColor = Color4F::RED;
					if (m_selectedPlane == Axis::Y)
						LineColor = Color4F::GREEN;
					else if (m_selectedPlane == Axis::Z)
						LineColor = Color4F::BLUE;

					Debug.DrawLineNoDepth(m_transform.WorldPosition, m_transform.WorldPosition + m_rotationDragStart * m_constantScale, 5.0f, LineColor, LineColor);
					Debug.DrawLineNoDepth(m_transform.WorldPosition, m_transform.WorldPosition + m_dragEnd * m_constantScale, 5.0f, LineColor, LineColor);
				}

				//
				Graphics::SetCullMode(CullMode::NO_CULL);

				uniform_model.sendMatrix(m_transform.Model, true);

				// X Circle //
				if (m_showScaleGizmo[0])
				{
					if (m_selectedPlane == Axis::X)
						if (m_clicked)
							uniform_color.send(Color3F::WHITE);
						else
							uniform_color.send(Color3F::YELLOW);
					else
						uniform_color.send(Color3F::RED);

					Assets::getMesh(Geometry.GetDoughtnutX_2D())->Draw();
				}

				// Y Circle //
				if (m_showScaleGizmo[1])
				{
					if (m_selectedPlane == Axis::Y)
						if (m_clicked)
							uniform_color.send(Color3F::WHITE);
						else
							uniform_color.send(Color3F::YELLOW);
					else
						uniform_color.send(Color3F::GREEN);

					Assets::getMesh(Geometry.GetDoughtnutY_2D())->Draw();
				}

				// Z Circle //
				if (m_showScaleGizmo[2])
				{
					if (m_selectedPlane == Axis::Z)
						if (m_clicked)
							uniform_color.send(Color3F::WHITE);
						else
							uniform_color.send(Color3F::YELLOW);
					else
						uniform_color.send(Color3F::BLUE);

					Assets::getMesh(Geometry.GetDoughtnutZ_2D())->Draw();
				}

				// revert
				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
			}
		}
	}
	void Gizmo::RenderIDCapture(MaterialIndex _colorPickerMaterial)
	{
		if (m_clicked || !m_show)
			return;

		// Make sure Cursor is available
		if (!Window.IsCursorOnImguiWindow() && Window.GetCursor() == CursorMode::NORMAL)
		{
			m_fbo->bind();
			m_fbo->clearBuffers();

			Material* material_colorPicker = Assets::getMaterial(_colorPickerMaterial);

			material_colorPicker->bindProgram();
			material_colorPicker->bindStates();

			Graphics::Uniform uniform_useModel = material_colorPicker->getUniform("VXL_useModel");
			Graphics::Uniform uniform_model = material_colorPicker->getUniform("VXL_model");
			Graphics::Uniform uniform_output = material_colorPicker->getUniform("VXL_output");

			uniform_useModel.send(true);
			uniform_model.sendMatrix(m_transform.Model, true);

			if (m_mode == Mode::TRANSLATE)
			{
				Color4F color;

				// X-arrow
				color = Util::Conversion::uint_to_color4(1u);
				uniform_output.send(color);
				Assets::getMesh(Geometry.GetArrowX())->Draw();

				// Y-arrow
				color = Util::Conversion::uint_to_color4(2u);
				uniform_output.send(color);
				Assets::getMesh(Geometry.GetArrowY())->Draw();

				// Z-arrow
				color = Util::Conversion::uint_to_color4(3u);
				uniform_output.send(color);
				Assets::getMesh(Geometry.GetArrowZ())->Draw();

				Graphics::SetCullMode(CullMode::NO_CULL);

				// X Plane
				color = Util::Conversion::uint_to_color4(4u);
				uniform_output.send(color);
				Assets::getMesh(m_mesh_HalfQuadAxisX[(m_camAxisSide[1] ? 1 : 0) + (m_camAxisSide[2] ? 2 : 0)])->Draw();

				// Y Plane
				color = Util::Conversion::uint_to_color4(5u);
				uniform_output.send(color);
				Assets::getMesh(m_mesh_HalfQuadAxisY[(m_camAxisSide[0] ? 1 : 0) + (m_camAxisSide[2] ? 2 : 0)])->Draw();

				// Z Plane
				color = Util::Conversion::uint_to_color4(6u);
				uniform_output.send(color);
				Assets::getMesh(m_mesh_HalfQuadAxisZ[(m_camAxisSide[0] ? 1 : 0) + (m_camAxisSide[1] ? 2 : 0)])->Draw();

				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

				// Cube (ignored)
				color = Util::Conversion::uint_to_color4(7u);
				uniform_output.send(color);
				Assets::getMesh(Geometry.GetCubeSmall())->Draw();

				RawArray<uint8_t> data = m_fbo->readPixelsFromMouse(0, 1, 1);
				if (data.start != nullptr)
				{
					unsigned int EditorIndex = Util::Conversion::uchars_to_uint(data.start);
					data.Deallocate();

					switch (EditorIndex)
					{
					case 1u:
						m_selectedAxis = Axis::X;
						m_selectedPlane = Axis::NONE;
						break;
					case 2u:
						m_selectedAxis = Axis::Y;
						m_selectedPlane = Axis::NONE;
						break;
					case 3u:
						m_selectedAxis = Axis::Z;
						m_selectedPlane = Axis::NONE;

						break;
					case 4u:
						m_selectedAxis = Axis::NONE;
						m_selectedPlane = Axis::X;
						break;
					case 5u:
						m_selectedAxis = Axis::NONE;
						m_selectedPlane = Axis::Y;
						break;
					case 6u:
						m_selectedAxis = Axis::NONE;
						m_selectedPlane = Axis::Z;
						break;
					case 7u:
						m_selectedAxis = Axis::ALL;
						m_selectedPlane = Axis::ALL;
						break;
					default:
						m_selectedAxis = Axis::NONE;
						m_selectedPlane = Axis::NONE;
						break;
					}
				}

				m_selected =
					(m_selectedAxis != Axis::NONE) &&
					(m_selectedPlane != Axis::NONE)
					;
			}
			else if (m_mode == Mode::SCALE)
			{
				Color4F color;

				// X Cube
				color = Util::Conversion::uint_to_color4(1u);
				Matrix4x4 X_Model = m_transform.Model * Matrix4x4::GetTranslate(X_Offset);
				uniform_model.sendMatrix(X_Model, true);
				uniform_output.send(color);
				Assets::getMesh(m_mesh_ScaleCube[0])->Draw();

				// Y Cube
				color = Util::Conversion::uint_to_color4(2u);
				Matrix4x4 Y_Model = m_transform.Model * Matrix4x4::GetTranslate(Y_Offset);
				uniform_model.sendMatrix(Y_Model, true);
				uniform_output.send(color);
				Assets::getMesh(m_mesh_ScaleCube[1])->Draw();

				// Z Cube
				color = Util::Conversion::uint_to_color4(3u);
				Matrix4x4 Z_Model = m_transform.Model * Matrix4x4::GetTranslate(Z_Offset);
				uniform_model.sendMatrix(Z_Model, true);
				uniform_output.send(color);
				Assets::getMesh(m_mesh_ScaleCube[2])->Draw();

				// Center Cube
				color = Util::Conversion::uint_to_color4(4u);
				uniform_model.sendMatrix(m_transform.Model, true);
				uniform_output.send(color);
				Assets::getMesh(Geometry.GetCubeSmall())->Draw();

				RawArray<uint8_t> data = m_fbo->readPixelsFromMouse(0, 1, 1);
				if (data.start != nullptr)
				{
					unsigned int EditorIndex = Util::Conversion::uchars_to_uint(data.start);
					data.Deallocate();

					switch (EditorIndex)
					{
					case 1u:
						m_selectedAxis = Axis::X;
						break;
					case 2u:
						m_selectedAxis = Axis::Y;
						break;
					case 3u:
						m_selectedAxis = Axis::Z;
						break;
					case 4u:
						m_selectedAxis = Axis::ALL;
						break;
					default:
						m_selectedAxis = Axis::NONE;
						break;
					}
				}

				m_selected = (m_selectedAxis != Axis::NONE);
				m_selectedPlane = Axis::NONE;
			}
			else if (m_mode == Mode::ROTATE)
			{
				Color4F color;

				// Draw both sides
				Graphics::SetCullMode(CullMode::NO_CULL);

				uniform_model.sendMatrix(m_transform.Model, true);

				// X Circle
				if (m_showScaleGizmo[0])
				{
					color = Util::Conversion::uint_to_color4(1u);
					uniform_output.send(color);
					Assets::getMesh(Geometry.GetDoughtnutX_2D())->Draw();
				}

				// Y Circle
				if (m_showScaleGizmo[1])
				{
					color = Util::Conversion::uint_to_color4(2u);
					uniform_output.send(color);
					Assets::getMesh(Geometry.GetDoughtnutY_2D())->Draw();
				}

				// Z Circle
				if (m_showScaleGizmo[2])
				{
					color = Util::Conversion::uint_to_color4(3u);
					uniform_output.send(color);
					Assets::getMesh(Geometry.GetDoughtnutZ_2D())->Draw();
				}

				// Revert
				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

				RawArray<uint8_t> data = m_fbo->readPixelsFromMouse(0, 1, 1);
				if (data.start != nullptr)
				{
					unsigned int EditorIndex = Util::Conversion::uchars_to_uint(data.start);
					data.Deallocate();

					switch (EditorIndex)
					{
					case 1u:
						m_selectedPlane = Axis::X;
						break;
					case 2u:
						m_selectedPlane = Axis::Y;
						break;
					case 3u:
						m_selectedPlane = Axis::Z;
						break;
					default:
						m_selectedPlane = Axis::NONE;
						break;
					}
				}
				m_selected = (m_selectedPlane != Axis::NONE);
				m_selectedAxis = Axis::NONE;
			}

		}
		// Force no selection
		else
		{
			m_selectedAxis = Axis::NONE;
			m_selectedPlane = Axis::NONE;
		}
	}
}