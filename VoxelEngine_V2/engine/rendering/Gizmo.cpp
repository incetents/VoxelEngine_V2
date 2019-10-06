// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Gizmo.h"

#include "../modules/Entity.h"
#include "../modules/Material.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/Geometry.h"
#include "../rendering/Mesh.h"
#include "../rendering/Debug.h"
#include "../objects/CameraObject.h"
#include "../math/Raycast.h"
#include "../window/window.h"
#include "../input/Input.h"
#include "RenderManager.h"

namespace Vxl
{
	// Static
	FramebufferObject*	Gizmo::m_fbo;
	Mesh*				Gizmo::m_mesh_HalfQuadAxisX[4];
	Mesh*				Gizmo::m_mesh_HalfQuadAxisY[4];
	Mesh*				Gizmo::m_mesh_HalfQuadAxisZ[4];
	Mesh*				Gizmo::m_mesh_ScaleCube[3];

	// Utility
	Vector3 Gizmo::CameraRayHitPlane(const Vector3& normal)
	{
		// Make a raycast with the Mouse's ScreenPosition
		Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));

		// Calculate where it hits
		Plane p(normal, m_transform.WorldPosition);
		Ray r(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
		
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
		m_fbo = FramebufferObject::Create("GizmoColorPicker");
		m_fbo->SetSizeToWindowSize();
		m_fbo->Bind();
		//
		m_fbo->NewAttachment(0, RenderTarget::Type::BUFFER, "color", TextureFormat::RGBA8);
		m_fbo->SetDepth(TextureDepthFormat::DEPTH16, RenderTarget::Type::BUFFER);
		//
		m_fbo->checkFBOStatus();
		m_fbo->Unbind();
	}
	void Gizmo::DestroyGLResources()
	{
		for (int i = 0; i < 4; i++)
		{
			m_mesh_HalfQuadAxisX[i] = nullptr;
			m_mesh_HalfQuadAxisY[i] = nullptr;
			m_mesh_HalfQuadAxisZ[i] = nullptr;
		}

		FramebufferObject::DeleteNamedAsset(m_fbo);
		m_fbo = nullptr;
	}

	// Update
	void Gizmo::Update(const std::vector<Entity*> _entities)
	{
		// If only 1 Entity and its the EditorCamera, ignore it
		if (_entities.size() == 0 || _entities.size() == 1 && _entities[0] == (Entity*)RenderManager.GetMainCamera())
		{
			m_show = false;
			return;
		}

		// Show
		m_show = true;

		// Update Distance from Camera as a matrix4x4
		m_distanceToCamera = (m_transform.WorldPosition - RenderManager.GetMainCamera()->m_transform.getWorldPosition()).Length();
		m_constantScale = m_distanceToCamera * 0.5f;
		m_constantScaleMatrix = Matrix4x4::GetScale(Vector3(m_constantScale));

		// Don't move Gizmo while clicking [unless it's translation]
		if (!m_clicked || m_mode == Mode::TRANSLATE)
		{
			// Update Average Selection Position
			Vector3 AverageSelection = Vector3::ZERO;
			uint32_t count = (uint32_t)_entities.size();
			for (const auto& _Entity : _entities)
			{
				// Don't Account for Editor Camera
				if (_Entity == (Entity*)RenderManager.GetMainCamera())
					count--;
				else
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
		if ((m_pivotAxisAligned && m_mode != Mode::SCALE) || (m_mode == Mode::TRANSLATE && m_translateSnapping))
		{
			m_transform.Model = Matrix4x4::GetTranslate(m_transform.WorldPosition);
			m_transform.Forward = Vector3::FORWARD;
			m_transform.Up = Vector3::UP;
			m_transform.Right = Vector3::RIGHT;
		}

		// Set Local Pivot
		else
		{
			Matrix3x3 RotationMatrix = _entities[0]->m_transform.getWorldRotation().GetMatrix3x3();

			m_transform.Model = Matrix4x4(RotationMatrix, m_transform.WorldPosition);
			m_transform.Forward = _entities[0]->m_transform.getForward();
			m_transform.Up = _entities[0]->m_transform.getUp();
			m_transform.Right = _entities[0]->m_transform.getRight();
		}

		// Translate XYZ Planes to be in correct position
		if (m_mode == Mode::TRANSLATE)
		{
			CameraObject* mainCamera = RenderManager.GetMainCamera();
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
				X_Offset = m_selectedAxis == Axis::X ? Vector3(m_dragAmount - 1.0f, 0, 0) : Vector3::ZERO;
				Y_Offset = m_selectedAxis == Axis::Y ? Vector3(0, m_dragAmount - 1.0f, 0) : Vector3::ZERO;
				Z_Offset = m_selectedAxis == Axis::Z ? Vector3(0, 0, m_dragAmount - 1.0f) : Vector3::ZERO;
			}
		}
		// Rotation, figure out if axis can be seen
		else
		{
			// If Camera can see Axis
			Vector3 ModelToCamera = (RenderManager.GetMainCamera()->m_transform.getWorldPosition() - m_transform.WorldPosition).Normalize();

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
					m_targetNormal = RenderManager.GetMainCamera()->m_transform.getCameraBackwards();
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
					Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
					Ray		ViewRay(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
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
					m_dragAmount = 1.0f;

					// Get Mouse position in -1 to +1 range of screen
					m_dragStart = Input.getMousePosScreenspace(true);
				}
				// Selecting Axis
				else if (m_selectedAxis != Axis::NONE)
				{
					m_dragStart = m_transform.WorldPosition;

					Vector3 Direction = GetGizmoDirection(m_selectedAxis);
					// Make a raycast with the Mouse's ScreenPosition
					Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
					Ray		ViewRay(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
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
						// Don't Translate Editor Camera
						if (_entities[i] == (Entity*)RenderManager.GetMainCamera())
							continue;

						Vector3 Target = m_worldPositionStorage[i] + (m_dragEnd - m_dragStart);
						_entities[i]->m_transform.setWorldPosition(Target);
					}
				}
				// Translate based on Axis
				else if (m_selectedAxis != Axis::NONE)
				{
					Vector3 Direction = GetGizmoDirection(m_selectedAxis);
					// Make a raycast with the Mouse's ScreenPosition
					Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
					Ray		ViewRay(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
					Ray		DirectionRay(m_dragStart, Direction);

					// Find shortest distance between Axis and Camera Ray
					ShortestDistance(ViewRay, DirectionRay);

					// Check if snapping should happen
					float totalDrag = DirectionRay.m_t - m_dragAmount;

					uint32_t _entityCount = (uint32_t)_entities.size();
					for (uint32_t i = 0; i < _entityCount; i++)
					{
						// Don't Translate Editor Camera
						if (_entities[i] == (Entity*)RenderManager.GetMainCamera())
							continue;

						// Target Position
						Vector3 Target = m_worldPositionStorage[i] -Direction.NormalizeAccurate() * (totalDrag);
						// Snap to World
						if (m_translateSnapping)
							Target = (Target / m_translateSnapAmount).Round() * m_translateSnapAmount;

						_entities[i]->m_transform.setWorldPosition(Target);
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
					float Distance = MouseDelta.x + MouseDelta.y + 1.0f;

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
					Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
					Ray		ViewRay(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
					Ray		DirectionRay(m_dragStart, Direction);

					// Find shortest distance between Axis and Camera Ray
					ShortestDistance(ViewRay, DirectionRay);
					m_dragAmount = -DirectionRay.m_t + m_dragPrev + 1.0f;

					uint32_t selectionCount = (uint32_t)_entities.size();
					switch (m_selectedAxis)
					{
					case Axis::X:
						for (uint32_t i = 0; i < selectionCount; i++)
						{
							_entities[i]->m_transform.setScaleX(m_worldScaleStorage[i] * m_dragAmount);
						}
						break;
					case Axis::Y:
						for (uint32_t i = 0; i < selectionCount; i++)
						{
							_entities[i]->m_transform.setScaleY(m_worldScaleStorage[i] * m_dragAmount);
						}
						break;
					case Axis::Z:
						for (uint32_t i = 0; i < selectionCount; i++)
						{
							_entities[i]->m_transform.setScaleZ(m_worldScaleStorage[i] * m_dragAmount);
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
					if (m_pivotAxisAligned)
					{
						uint32_t selectionCount = (uint32_t)_entities.size();
						for (uint32_t i = 0; i < selectionCount; i++)
						{
							// Don't Rotate Editor Camera
							if (_entities[i] == (Entity*)RenderManager.GetMainCamera())
								continue;

							_entities[i]->m_transform.rotateAroundAxis(m_targetNormal, degrees);
						}
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
	void Gizmo::RenderOnScreen()
	{
		if (!m_show)
			return;

		auto gizmoMaterial = Material::GetAsset("gizmo");
		if (gizmoMaterial->IsValid())
		{
			gizmoMaterial->BindProgram();

			gizmoMaterial->m_property_model.SetPropertyMatrix(m_transform.Model * m_constantScaleMatrix, true); // 
			gizmoMaterial->m_property_normalMatrix.SetPropertyMatrix(Matrix3x3(m_transform.Model), true);

			// Movement //
			if (m_mode == Mode::TRANSLATE)
			{
				gizmoMaterial->SetProperty("ignoreLight", false);

				// X Axis //
				if (m_selectedAxis == Axis::X)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::RED);

				Geometry.GetArrowX()->Draw();

				// Y Axis //
				if (m_selectedAxis == Axis::Y)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::GREEN);

				Geometry.GetArrowY()->Draw();

				// Z Axis //
				if (m_selectedAxis == Axis::Z)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::BLUE);

				Geometry.GetArrowZ()->Draw();

				// Cull
				Graphics::SetCullMode(CullMode::NO_CULL);
				// Alpha
				gizmoMaterial->m_property_alpha.SetProperty(0.85f);

				// X Plane
				if (m_selectedPlane == Axis::X)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::RED);

				m_mesh_HalfQuadAxisX[(m_camAxisSide[1] ? 1 : 0) + (m_camAxisSide[2] ? 2 : 0)]->Draw();
				
				// Y Plane
				if (m_selectedPlane == Axis::Y)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::GREEN);

				m_mesh_HalfQuadAxisY[(m_camAxisSide[0] ? 1 : 0) + (m_camAxisSide[2] ? 2 : 0)]->Draw();

				// Z Plane
				if (m_selectedPlane == Axis::Z)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::BLUE);

				m_mesh_HalfQuadAxisZ[(m_camAxisSide[0] ? 1 : 0) + (m_camAxisSide[1] ? 2 : 0)]->Draw();

				// Cull
				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
				// Alpha
				gizmoMaterial->m_property_alpha.SetProperty(1.0f);

				// Small cube in the middle //
				if (m_selectedPlane == Axis::ALL)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::GREY);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
				//
				Geometry.GetCubeSmall()->Draw();

			}
			else if (m_mode == Mode::SCALE)
			{
				gizmoMaterial->SetProperty("ignoreLight", false);

				// Gizmo Additional Debug Lines
				float XLineLength = (m_clicked && m_selectedAxis == Axis::X) ? m_constantScale + m_dragAmount - 1.0f : m_constantScale;
				float YLineLength = (m_clicked && m_selectedAxis == Axis::Y) ? m_constantScale + m_dragAmount - 1.0f : m_constantScale;
				float ZLineLength = (m_clicked && m_selectedAxis == Axis::Z) ? m_constantScale + m_dragAmount - 1.0f : m_constantScale;

				Debug.DrawLineNoDepth(m_transform.WorldPosition, m_transform.WorldPosition + m_transform.Right * XLineLength, 5.0f, Color4F::RED, Color4F::RED);
				Debug.DrawLineNoDepth(m_transform.WorldPosition, m_transform.WorldPosition + m_transform.Up * YLineLength, 5.0f, Color4F::GREEN, Color4F::GREEN);
				Debug.DrawLineNoDepth(m_transform.WorldPosition, m_transform.WorldPosition + m_transform.Forward * ZLineLength, 5.0f, Color4F::BLUE, Color4F::BLUE);
				

				// X Cube //
				Matrix4x4 X_Model = m_transform.Model * Matrix4x4::GetTranslate(X_Offset);
				gizmoMaterial->m_property_model.SetPropertyMatrix(X_Model * m_constantScaleMatrix, true);
				if (m_selectedAxis == Axis::X)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::RED);

				m_mesh_ScaleCube[0]->Draw();

				// Y Cube //
				Matrix4x4 Y_Model = m_transform.Model * Matrix4x4::GetTranslate(Y_Offset);
				gizmoMaterial->m_property_model.SetPropertyMatrix(Y_Model * m_constantScaleMatrix, true);
				if (m_selectedAxis == Axis::Y)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::GREEN);

				m_mesh_ScaleCube[1]->Draw();

				// Z Cube //
				Matrix4x4 Z_Model = m_transform.Model * Matrix4x4::GetTranslate(Z_Offset);
				gizmoMaterial->m_property_model.SetPropertyMatrix(Z_Model * m_constantScaleMatrix, true);
				if (m_selectedAxis == Axis::Z)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::BLUE);

				m_mesh_ScaleCube[2]->Draw();

				// Small cube in the middle //
				gizmoMaterial->m_property_model.SetPropertyMatrix(m_transform.Model * m_constantScaleMatrix, true);
				gizmoMaterial->m_property_alpha.SetProperty(0.85f);

				if (m_selectedAxis == Axis::ALL)
					if (m_clicked)
						gizmoMaterial->m_property_color.SetProperty(Color3F::GREY);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
				else
					gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);

				Geometry.GetCubeSmall()->Draw();

				// revert
				gizmoMaterial->m_property_alpha.SetProperty(1.0f);

			}
			else if (m_mode == Mode::ROTATE)
			{
				gizmoMaterial->SetProperty("ignoreLight", true);

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

				gizmoMaterial->m_property_model.SetPropertyMatrix(m_transform.Model * m_constantScaleMatrix, true);

				// X Circle //
				if (m_showScaleGizmo[0])
				{
					if (m_selectedPlane == Axis::X)
						if (m_clicked)
							gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
						else
							gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::RED);

					Geometry.GetDoughtnutX_2D()->Draw();
				}

				// Y Circle //
				if (m_showScaleGizmo[1])
				{
					if (m_selectedPlane == Axis::Y)
						if (m_clicked)
							gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
						else
							gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::GREEN);

					Geometry.GetDoughtnutY_2D()->Draw();
				}

				// Z Circle //
				if (m_showScaleGizmo[2])
				{
					if (m_selectedPlane == Axis::Z)
						if (m_clicked)
							gizmoMaterial->m_property_color.SetProperty(Color3F::WHITE);
						else
							gizmoMaterial->m_property_color.SetProperty(Color3F::YELLOW);
					else
						gizmoMaterial->m_property_color.SetProperty(Color3F::BLUE);

					Geometry.GetDoughtnutZ_2D()->Draw();
				}

				// revert
				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
			}
		}
	}
	void Gizmo::RenderIDCapture()
	{
		if (m_clicked || !m_show)
			return;

		// Make sure Cursor is available
		if (!Window.IsCursorOnImguiWindow() && Window.GetCursor() == CursorMode::NORMAL)
		{
			m_fbo->Bind();
			m_fbo->ClearBuffers();

			Material* material_colorPicker = Material::GetAsset("colorPicker");

			material_colorPicker->BindProgram();
			material_colorPicker->BindStates();

			material_colorPicker->m_property_useModel.SetProperty(true);
			material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model * m_constantScaleMatrix, true);

			if (m_mode == Mode::TRANSLATE)
			{
				Color4F color;

				// X-arrow
				color = Util::Conversion::uint_to_color4(1u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetArrowX()->Draw();

				// Y-arrow
				color = Util::Conversion::uint_to_color4(2u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetArrowY()->Draw();

				// Z-arrow
				color = Util::Conversion::uint_to_color4(3u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetArrowZ()->Draw();

				Graphics::SetCullMode(CullMode::NO_CULL);

				// X Plane
				color = Util::Conversion::uint_to_color4(4u);
				material_colorPicker->m_property_output.SetProperty(color);
				m_mesh_HalfQuadAxisX[(m_camAxisSide[1] ? 1 : 0) + (m_camAxisSide[2] ? 2 : 0)]->Draw();

				// Y Plane
				color = Util::Conversion::uint_to_color4(5u);
				material_colorPicker->m_property_output.SetProperty(color);
				m_mesh_HalfQuadAxisY[(m_camAxisSide[0] ? 1 : 0) + (m_camAxisSide[2] ? 2 : 0)]->Draw();

				// Z Plane
				color = Util::Conversion::uint_to_color4(6u);
				material_colorPicker->m_property_output.SetProperty(color);
				m_mesh_HalfQuadAxisZ[(m_camAxisSide[0] ? 1 : 0) + (m_camAxisSide[1] ? 2 : 0)]->Draw();

				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

				// Cube (ignored)
				color = Util::Conversion::uint_to_color4(7u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetCubeSmall()->Draw();

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
				material_colorPicker->m_property_model.SetPropertyMatrix(X_Model * m_constantScaleMatrix, true);
				material_colorPicker->m_property_output.SetProperty(color);
				m_mesh_ScaleCube[0]->Draw();

				// Y Cube
				color = Util::Conversion::uint_to_color4(2u);
				Matrix4x4 Y_Model = m_transform.Model * Matrix4x4::GetTranslate(Y_Offset);
				material_colorPicker->m_property_model.SetPropertyMatrix(Y_Model * m_constantScaleMatrix, true);
				material_colorPicker->m_property_output.SetProperty(color);
				m_mesh_ScaleCube[1]->Draw();

				// Z Cube
				color = Util::Conversion::uint_to_color4(3u);
				Matrix4x4 Z_Model = m_transform.Model * Matrix4x4::GetTranslate(Z_Offset);
				material_colorPicker->m_property_model.SetPropertyMatrix(Z_Model * m_constantScaleMatrix, true);
				material_colorPicker->m_property_output.SetProperty(color);
				m_mesh_ScaleCube[2]->Draw();

				// Center Cube
				color = Util::Conversion::uint_to_color4(4u);
				material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model * m_constantScaleMatrix, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetCubeSmall()->Draw();

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

				material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model * m_constantScaleMatrix, true);

				// X Circle
				if (m_showScaleGizmo[0])
				{
					color = Util::Conversion::uint_to_color4(1u);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetDoughtnutX_2D()->Draw();
				}

				// Y Circle
				if (m_showScaleGizmo[1])
				{
					color = Util::Conversion::uint_to_color4(2u);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetDoughtnutY_2D()->Draw();
				}

				// Z Circle
				if (m_showScaleGizmo[2])
				{
					color = Util::Conversion::uint_to_color4(3u);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetDoughtnutZ_2D()->Draw();
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