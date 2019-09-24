// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Gizmo.h"

#include "../modules/Entity.h"
#include "../modules/Material.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/Geometry.h"
#include "../rendering/Mesh.h"
#include "../objects/CameraObject.h"
#include "../math/Raycast.h"
#include "../window/window.h"
#include "../input/Input.h"
#include "RenderManager.h"

namespace Vxl
{
	// Static
	FramebufferObject*	Gizmo::m_fbo;

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
		FramebufferObject::DeleteNamedAsset(m_fbo);
		m_fbo = nullptr;
	}

	// Update
	void Gizmo::UpdateModel(Entity& _entity)
	{
		// Store World Position
		m_transform.WorldPosition = _entity.m_transform.getWorldPosition();

		// Set Axis-Aligned Pivot
		if((m_pivotAxisAligned && m_mode != Mode::SCALE) || (m_mode == Mode::TRANSLATE && m_translateSnapping))
		{
			m_transform.Model = Matrix4x4::GetTranslate(m_transform.WorldPosition);
			m_transform.Forward = Vector3::FORWARD;
			m_transform.Up = Vector3::UP;
			m_transform.Right = Vector3::RIGHT;
		}

		// Set Local Pivot
		else
		{
			Matrix3x3 RotationMatrix = _entity.m_transform.getWorldRotation().GetMatrix3x3();

			m_transform.Model = Matrix4x4(RotationMatrix, m_transform.WorldPosition);
			m_transform.Forward = _entity.m_transform.getForward();
			m_transform.Up = _entity.m_transform.getUp();
			m_transform.Right = _entity.m_transform.getRight();
		}
		

		// ~~ Visual Displacements based on mode ~~ //


		// Translate XYZ Planes to be in correct position
		if (m_mode == Mode::TRANSLATE)
		{
			CameraObject* mainCamera = RenderManager.GetMainCamera();
			Vector3 SelectionTransformToCamera = (mainCamera->m_transform.getWorldPosition() - m_transform.WorldPosition);

			// Which side of the axis is the camera on
			bool CamOnXAxis = SelectionTransformToCamera.Dot(m_transform.Right) > 0.0f;
			bool CamOnYAxis = SelectionTransformToCamera.Dot(m_transform.Up) > 0.0f;
			bool CamOnZAxis = SelectionTransformToCamera.Dot(m_transform.Forward) > 0.0f;

			X_Offset = Vector3(0, CamOnYAxis ? 0.25f : -0.25f, CamOnZAxis ? 0.25f : -0.25f);
			Y_Offset = Vector3(CamOnXAxis ? 0.25f : -0.25f, 0, CamOnZAxis ? 0.25f : -0.25f);
			Z_Offset = Vector3(CamOnXAxis ? 0.25f : -0.25f, CamOnYAxis ? 0.25f : -0.25f, 0);
		}
		// Scale Boxes move along with the cursor when dragging
		else if (m_mode == Mode::SCALE)
		{
			// Default position
			if (!m_clicked)
			{
				X_Offset = Vector3::RIGHT;
				Y_Offset = Vector3::UP;
				Z_Offset = Vector3::FORWARD;
			}
			// Moving Based on Drag Amount
			else
			{
				X_Offset = m_selectedAxis == Axis::X ? Vector3(m_dragAmount, 0, 0) : Vector3::RIGHT;
				Y_Offset = m_selectedAxis == Axis::Y ? Vector3(0, m_dragAmount, 0) : Vector3::UP;
				Z_Offset = m_selectedAxis == Axis::Z ? Vector3(0, 0, m_dragAmount) : Vector3::FORWARD;
			}
		}
	}
	void Gizmo::Update(const std::vector<Entity*> _entities)
	{
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

					// Clip Space position for Gizmo
					Matrix4x4 MVP = RenderManager.GetMainCamera()->getViewProjection() * m_transform.Model;

					// Center of Gizmo
					Vector4 SelectionCenter = MVP.GetColumn(3); // Same as MVP * Vector4(0, 0, 0, 1);
					m_dragStart = (SelectionCenter / SelectionCenter.w).GetVector3(); // [-1 to +1] Range for XYZ
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

					std::cout << Distance << std::endl;

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
	void Gizmo::RenderOnScreen()
	{
		auto simpleLight = Material::GetAsset("simpleLight");
		if (simpleLight->IsValid())
		{
			simpleLight->BindProgram();

			simpleLight->m_property_useModel.SetProperty(true);
			simpleLight->m_property_model.SetPropertyMatrix(m_transform.Model, true);
			simpleLight->m_property_normalMatrix.SetPropertyMatrix(Matrix3x3(m_transform.Model), true);

			// Movement //
			if (m_mode == Mode::TRANSLATE)
			{
				// X Axis //
				if (m_selectedAxis == Axis::X)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::RED);

				Geometry.GetArrowX()->Draw();

				// Y Axis //
				if (m_selectedAxis == Axis::Y)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::GREEN);

				Geometry.GetArrowY()->Draw();

				// Z Axis //
				if (m_selectedAxis == Axis::Z)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::BLUE);

				Geometry.GetArrowZ()->Draw();

				// Cull
				Graphics::SetCullMode(CullMode::NO_CULL);
				// Alpha
				simpleLight->m_property_alpha.SetProperty(0.85f);

				// X Plane
				Matrix4x4 X_Model = m_transform.Model * Matrix4x4::GetTranslate(X_Offset);
				simpleLight->m_property_model.SetPropertyMatrix(X_Model, true);
				if (m_selectedPlane == Axis::X)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::RED);
				Geometry.GetHalfQuadX()->Draw();

				// Y Plane
				Matrix4x4 Y_Model = m_transform.Model * Matrix4x4::GetTranslate(Y_Offset);
				simpleLight->m_property_model.SetPropertyMatrix(Y_Model, true);
				if (m_selectedPlane == Axis::Y)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::GREEN);
				Geometry.GetHalfQuadY()->Draw();

				// Z Plane
				Matrix4x4 Z_Model = m_transform.Model * Matrix4x4::GetTranslate(Z_Offset);
				simpleLight->m_property_model.SetPropertyMatrix(Z_Model, true);
				if (m_selectedPlane == Axis::Z)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::BLUE);
				Geometry.GetHalfQuadZ()->Draw();

				// Cull
				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
				// Alpha
				simpleLight->m_property_alpha.SetProperty(1.0f);

				// Small cube in the middle //
				simpleLight->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				if (m_selectedPlane == Axis::ALL)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::GREY);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::WHITE);
				//
				Geometry.GetCubeSmall()->Draw();

			}
			else if (m_mode == Mode::SCALE)
			{
				// X Cube //
				Matrix4x4 X_Model = m_transform.Model * Matrix4x4::GetTranslate(X_Offset);
				simpleLight->m_property_model.SetPropertyMatrix(X_Model, true);
				if (m_selectedAxis == Axis::X)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::RED);

				Geometry.GetCubeSmall()->Draw();

				// Y Cube //
				Matrix4x4 Y_Model = m_transform.Model * Matrix4x4::GetTranslate(Y_Offset);
				simpleLight->m_property_model.SetPropertyMatrix(Y_Model, true);
				if (m_selectedAxis == Axis::Y)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::GREEN);

				Geometry.GetCubeSmall()->Draw();

				// Z Cube //
				Matrix4x4 Z_Model = m_transform.Model * Matrix4x4::GetTranslate(Z_Offset);
				simpleLight->m_property_model.SetPropertyMatrix(Z_Model, true);
				if (m_selectedAxis == Axis::Z)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::BLUE);

				Geometry.GetCubeSmall()->Draw();

				// Small cube in the middle //
				simpleLight->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				simpleLight->m_property_alpha.SetProperty(0.85f);

				if (m_selectedAxis == Axis::ALL)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::GREY);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::WHITE);

				Geometry.GetCubeSmall()->Draw();

				// revert
				simpleLight->m_property_alpha.SetProperty(1.0f);

			}
			else if (m_mode == Mode::ROTATE)
			{
				Graphics::SetCullMode(CullMode::NO_CULL);

				// X Circle //
				simpleLight->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				if (m_selectedPlane == Axis::X)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::RED);

				Geometry.GetCircleX()->Draw();

				// Y Circle //
				simpleLight->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				if (m_selectedPlane == Axis::Y)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::GREEN);

				Geometry.GetCircleY()->Draw();

				// Z Circle //
				simpleLight->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				if (m_selectedPlane == Axis::Z)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::BLUE);

				Geometry.GetCircleZ()->Draw();

				// revert
				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);
			}
		}
	}
	void Gizmo::RenderIDCapture()
	{
		if (m_clicked)
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
			material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model, true);

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
				Matrix4x4 X_Model = m_transform.Model * Matrix4x4::GetTranslate(X_Offset);
				material_colorPicker->m_property_model.SetPropertyMatrix(X_Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetHalfQuadX()->Draw();

				// Y Plane
				color = Util::Conversion::uint_to_color4(5u);
				Matrix4x4 Y_Model = m_transform.Model * Matrix4x4::GetTranslate(Y_Offset);
				material_colorPicker->m_property_model.SetPropertyMatrix(Y_Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetHalfQuadY()->Draw();

				// Z Plane
				color = Util::Conversion::uint_to_color4(6u);
				Matrix4x4 Z_Model = m_transform.Model * Matrix4x4::GetTranslate(Z_Offset);
				material_colorPicker->m_property_model.SetPropertyMatrix(Z_Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetHalfQuadZ()->Draw();

				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

				// Cube (ignored)
				color = Util::Conversion::uint_to_color4(7u);
				material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model, true);
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
				material_colorPicker->m_property_model.SetPropertyMatrix(X_Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetCubeSmall()->Draw();

				// Y Cube
				color = Util::Conversion::uint_to_color4(2u);
				Matrix4x4 Y_Model = m_transform.Model * Matrix4x4::GetTranslate(Y_Offset);
				material_colorPicker->m_property_model.SetPropertyMatrix(Y_Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetCubeSmall()->Draw();

				// Z Cube
				color = Util::Conversion::uint_to_color4(3u);
				Matrix4x4 Z_Model = m_transform.Model * Matrix4x4::GetTranslate(Z_Offset);
				material_colorPicker->m_property_model.SetPropertyMatrix(Z_Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetCubeSmall()->Draw();

				// Center Cube
				color = Util::Conversion::uint_to_color4(4u);
				material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model, true);
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

				// X Circle
				color = Util::Conversion::uint_to_color4(1u);
				material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetCircleX()->Draw();

				// Y Circle
				color = Util::Conversion::uint_to_color4(2u);
				material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetCircleY()->Draw();

				// Z Circle
				color = Util::Conversion::uint_to_color4(3u);
				material_colorPicker->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetCircleZ()->Draw();

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