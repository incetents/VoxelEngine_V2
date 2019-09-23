// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Gizmo.h"

#include "../objects/CameraObject.h"
#include "../modules/Entity.h"
#include "../modules/Material.h"
#include "../math/Raycast.h"
#include "../rendering/FramebufferObject.h"
#include "../window/window.h"

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

		// Set Local Pivot
		if (!m_pivotAxisAligned || m_mode == Mode::SCALE)
		{
			Matrix3x3 RotationMatrix = _entity.m_transform.getWorldRotation().GetMatrix3x3();

			m_transform.Model = Matrix4x4(RotationMatrix, m_transform.WorldPosition);
			m_transform.Forward = _entity.m_transform.getForward();
			m_transform.Up = _entity.m_transform.getUp();
			m_transform.Right = _entity.m_transform.getRight();
		}
		// Set Axis-Aligned Pivot
		else
		{
			m_transform.Model = Matrix4x4::GetTranslate(m_transform.WorldPosition);
			m_transform.Forward = Vector3::FORWARD;
			m_transform.Up = Vector3::UP;
			m_transform.Right = Vector3::RIGHT;
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
			// Moving Based on Drag
			else
			{
				X_Offset = m_selectedAxis == Axis::X ? Vector3(m_dragAmount + 1.f, 0, 0) : Vector3::RIGHT;
				Y_Offset = m_selectedAxis == Axis::Y ? Vector3(0, m_dragAmount + 1.f, 0) : Vector3::UP;
				Z_Offset = m_selectedAxis == Axis::Z ? Vector3(0, 0, m_dragAmount + 1.f) : Vector3::FORWARD;
			}
		}
	}
	void Gizmo::Update()
	{
		// No Selection
		if (!Input.getMouseButton(MouseButton::LEFT))
		{
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
				if (m_selectedAxis == Axis::ALL)
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
					Vector3 Direction = GetGizmoDirection(m_selectedAxis);
				}
				
			}
			break;

			case Mode::SCALE:
			{


			}
			break;

			case Mode::ROTATE:
			{


			}
			break;
			}

			m_clicked = true;
		}

		// On Update Gizmo based on selection and mouse movements
		if (Input.getMouseButton(MouseButton::LEFT))
		{
			switch (m_mode)
			{
			case Mode::TRANSLATE:
			{


			}
			break;

			case Mode::SCALE:
			{


			}
			break;

			case Mode::ROTATE:
			{


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
				if (m_selectedAxis == Axis::ALL)
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
				if (m_selectedAxis == Axis::X)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::RED);

				Geometry.GetCircleX()->Draw();

				// Y Circle //
				simpleLight->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				if (m_selectedAxis == Axis::Y)
					if (m_clicked)
						simpleLight->m_property_color.SetProperty(Color3F::WHITE);
					else
						simpleLight->m_property_color.SetProperty(Color3F::YELLOW);
				else
					simpleLight->m_property_color.SetProperty(Color3F::GREEN);

				Geometry.GetCircleY()->Draw();

				// Z Circle //
				simpleLight->m_property_model.SetPropertyMatrix(m_transform.Model, true);
				if (m_selectedAxis == Axis::Z)
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
						m_selectedPlane = Axis::NONE;
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
						m_selectedAxis = Axis::X;
						break;
					case 2u:
						m_selectedAxis = Axis::Y;
						break;
					case 3u:
						m_selectedAxis = Axis::Z;
						break;
					default:
						m_selectedAxis = Axis::NONE;
						break;
					}
				}

				m_selected = (m_selectedAxis != Axis::NONE);
				m_selectedPlane = Axis::NONE;
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