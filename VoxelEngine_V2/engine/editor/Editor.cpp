// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "Editor.h"
#include "../modules/Entity.h"
#include "../modules/Material.h"
#include "../rendering/RenderManager.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/Geometry.h"
#include "../rendering/Mesh.h"

#include "../objects/CameraObject.h"

#include "../window/window.h"
#include "../input/Input.h"
#include "../utilities/Util.h"
#include "../utilities/Time.h"

namespace Vxl
{
	void Editor::InitGLResources()
	{
		m_colorPickerFBO = FramebufferObject::Create("_ColorPicker");
		m_colorPickerFBO->SetSizeToWindowSize();
		m_colorPickerFBO->Bind();
		//
		m_colorPickerFBO->SetAttachment(0, m_colorPickerFBO->CreateRenderTexture("color"));
		m_colorPickerFBO->SetDepth(TextureDepthFormat::DEPTH16, FBOAttachment::Type::BUFFER);
		//
		m_colorPickerFBO->checkFBOStatus();
		m_colorPickerFBO->Unbind();
	}
	void Editor::DestroyGLResources()
	{
		FramebufferObject::DeleteNamedAsset(m_colorPickerFBO);
		m_colorPickerFBO = nullptr;
	}

	void Editor::RemoveSelection(Entity* _entity)
	{
		Util::RemoveFromVector(m_selectedEntities, _entity);
		_entity->m_isSelected = false;

		UpdateSelectionTransformPosition();
	}
	void Editor::AddSelection(Entity* _entity)
	{
		VXL_ASSERT(_entity, "Adding nullptr in Editor::AddSelection");

		for (auto Entity : m_selectedEntities)
			if (Entity == _entity)
				return;

		m_selectedEntities.push_back(_entity);
		_entity->m_isSelected = true;

		UpdateSelectionTransformPosition();
	}
	void Editor::ClearSelection()
	{
		for (auto Entity : m_selectedEntities)
			Entity->m_isSelected = false;
		m_selectedEntities.clear();

		m_GizmoTransform.WorldPosition = Vector3::ZERO;
	}

	void Editor::UpdateSelectionTransformPosition()
	{
		m_GizmoTransform.WorldPosition = Vector3::ZERO;
		float m_selectedEntitiesCount = (float)m_selectedEntities.size();

		for (const auto& Entity : m_selectedEntities)
			m_GizmoTransform.WorldPosition += Entity->m_transform.getWorldPosition();

		if (m_selectedEntitiesCount > 0)
		{
			float inva = 1.0f / (float)m_selectedEntitiesCount;
			m_GizmoTransform.WorldPosition *= inva;
		}
	}

	void Editor::UpdateSelectionTransformModel()
	{
		UpdateSelectionTransformPosition();

		// Only update model if there's an object selected
		if (m_selectedEntities.size() > 0)
		{
			m_GizmoTransform.CameraDistance = Vector3::Distance(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), m_GizmoTransform.WorldPosition);
			float AxisScale = 1.0f;// m_averageSelectionDistanceFromEditorCamera / 3.0f;

			if (m_controlAxisLocal)
			{
				Matrix3x3 RotationMatrix = m_selectedEntities[0]->m_transform.getWorldRotation().GetMatrix3x3();
				m_GizmoTransform.Model = Matrix4x4(RotationMatrix * Matrix3x3::GetScale(AxisScale), m_GizmoTransform.WorldPosition);
				m_GizmoTransform.Forward = m_selectedEntities[0]->m_transform.getForward();
				m_GizmoTransform.Up = m_selectedEntities[0]->m_transform.getUp();
				m_GizmoTransform.Right = m_selectedEntities[0]->m_transform.getRight();
			}
			else
			{
				m_GizmoTransform.Model = Matrix4x4::GetTranslateScale(m_GizmoTransform.WorldPosition, Vector3(AxisScale));
				m_GizmoTransform.Forward = Vector3::FORWARD;
				m_GizmoTransform.Up = Vector3::UP;
				m_GizmoTransform.Right = Vector3::RIGHT;
			}

			// Update Axis Portion for position
			if (m_controlMode == ControlMode::TRANSLATE)
			{
				CameraObject* mainCamera = RenderManager.GetMainCamera();
				Vector3 SelectionTransformToCamera = (mainCamera->m_transform.getWorldPosition() - m_GizmoTransform.WorldPosition);

				bool CamOnXAxis = SelectionTransformToCamera.Dot(m_GizmoTransform.Right) > 0.0f;
				bool CamOnYAxis = SelectionTransformToCamera.Dot(m_GizmoTransform.Up) > 0.0f;
				bool CamOnZAxis = SelectionTransformToCamera.Dot(m_GizmoTransform.Forward) > 0.0f;

				// X-Plane
				Matrix4x4 Offset = Matrix4x4::GetTranslate(Vector3(0, CamOnYAxis ? 0.25f : -0.25f, CamOnZAxis ? 0.25f : -0.25f));
				m_GizmoTransform.Xquad_Model = m_GizmoTransform.Model * Offset;

				// Y-Plane
				Offset = Matrix4x4::GetTranslate(Vector3(CamOnXAxis ? 0.25f : -0.25f, 0, CamOnZAxis ? 0.25f : -0.25f));
				m_GizmoTransform.Yquad_Model = m_GizmoTransform.Model * Offset;

				// Z-Plane
				Offset = Matrix4x4::GetTranslate(Vector3(CamOnXAxis ? 0.25f : -0.25f, CamOnYAxis ? 0.25f : -0.25f, 0));
				m_GizmoTransform.Zquad_Model = m_GizmoTransform.Model * Offset;
			}
		}
	}

	void Editor::UpdateSelectionInfo()
	{
		//static float Proj1_Prev = 0.0f;
		//static float Proj2_Prev = 0.0f;

		if (!Window.IsCursorOnImguiWindow() && Window.GetCursor() == CursorMode::NORMAL && HasSelection())
		{
			// Highlighting over an Editor Axis not selected
			if (!Input.getMouseButton(MouseButton::LEFT))
			{
				// Setup flags
				m_GizmoClicked = false;
				m_useAxis1 = false;
				m_useAxis2 = false;

				m_colorPickerFBO->Bind();
				m_colorPickerFBO->ClearBuffers();

				Material* material_colorPicker = Material::GetAsset("colorPicker");

				material_colorPicker->BindProgram();
				material_colorPicker->BindStates();

				material_colorPicker->m_property_useModel.SetProperty(true);
				material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Model, true);

				Color4F color;

				// Cube (ignored)
				material_colorPicker->m_property_output.SetProperty(Color4F(0, 0, 0, 0));
				Geometry.GetCubeSmall()->Draw();

				// X-arrow
				color = Util::DataConversion::uint_to_color4(1u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetArrowX()->Draw();

				// Y-arrow
				color = Util::DataConversion::uint_to_color4(2u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetArrowY()->Draw();

				// Z-arrow
				color = Util::DataConversion::uint_to_color4(3u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetArrowZ()->Draw();

				Graphics::SetCullMode(CullMode::NO_CULL);

				// X Plane
				material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Xquad_Model, true);
				color = Util::DataConversion::uint_to_color4(4u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetHalfQuadX()->Draw();

				// Y Plane
				material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Yquad_Model, true);
				color = Util::DataConversion::uint_to_color4(5u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetHalfQuadY()->Draw();

				// Z Plane
				material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Zquad_Model, true);
				color = Util::DataConversion::uint_to_color4(6u);
				material_colorPicker->m_property_output.SetProperty(color);
				Geometry.GetHalfQuadZ()->Draw();

				Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);


				RawArray<uint8_t> data = m_colorPickerFBO->readPixelsFromMouse(0, 1, 1);
				if (data.start != nullptr)
				{
					unsigned int EditorIndex;
					Util::DataConversion::uchars_to_uint(data.start, EditorIndex);
					data.Deallocate();

					switch (EditorIndex)
					{
					case 1u:
						m_GizmoSelectedAxis = Axis::X;
						m_GizmoSelectedPlane = Axis::NONE;
						break;
					case 2u:
						m_GizmoSelectedAxis = Axis::Y;
						m_GizmoSelectedPlane = Axis::NONE;
						break;
					case 3u:
						m_GizmoSelectedAxis = Axis::Z;
						m_GizmoSelectedPlane = Axis::NONE;

						break;
					case 4u:
						m_GizmoSelectedAxis = Axis::NONE;
						m_GizmoSelectedPlane = Axis::X;
						break;
					case 5u:
						m_GizmoSelectedAxis = Axis::NONE;
						m_GizmoSelectedPlane = Axis::Y;
						break;
					case 6u:
						m_GizmoSelectedAxis = Axis::NONE;
						m_GizmoSelectedPlane = Axis::Z;

						break;
					default:
						m_GizmoSelectedAxis = Axis::NONE;
						m_GizmoSelectedPlane = Axis::NONE;
						break;
					}

					m_GizmoSelected =
						(m_GizmoSelectedAxis != Axis::NONE) &&
						(m_GizmoSelectedPlane != Axis::NONE)
						;
				}
			}
			// Store internal information before movement occurs
			else if (Input.getMouseButtonDown(MouseButton::LEFT) && m_GizmoSelectedAxis != Axis::NONE || m_GizmoSelectedPlane != Axis::NONE)
			{
				//Proj1_Prev = 0.0f;
				//Proj2_Prev = 0.0f;
				//std::cout << "RESET" << std::endl;

				// Init speed
				m_GizmoDragSpeed = 0.01f * m_GizmoTransform.CameraDistance / 3.0f;

				// Clip Space position for Gizmo
				Matrix4x4 MVP = RenderManager.GetMainCamera()->getViewProjection() * m_GizmoTransform.Model;

				// Center of Gizmo
				Vector4 SelectionCenter = MVP * Vector4(0, 0, 0, 1); // Get getting the last last column of the MVP instead
				m_ScreenSpace_SelectionCenter = (SelectionCenter / SelectionCenter.w).GetVector2(); // [-1 to +1] Range for XY

				Vector4 Selection1;
				Vector4 Selection2;

				// Check Selections of Gizmo
				switch (m_GizmoSelectedPlane)
				{
				case Axis::X:
					Selection1 = MVP * Vector4(0, 1, 0, 1);
					Selection2 = MVP * Vector4(0, 0, 1, 1);
					break;
				case Axis::Y:
					Selection1 = MVP * Vector4(1, 0, 0, 1);
					Selection2 = MVP * Vector4(0, 0, 1, 1);
					break;
				case Axis::Z:
					Selection1 = MVP * Vector4(1, 0, 0, 1);
					Selection2 = MVP * Vector4(0, 1, 0, 1);
					break;
				case Axis::NONE:
					switch (m_GizmoSelectedAxis)
					{
					case Axis::X:
						Selection1 = MVP * Vector4(1, 0, 0, 1);
						break;
					case Axis::Y:
						Selection1 = MVP * Vector4(0, 1, 0, 1);
						break;
					case Axis::Z:
						Selection1 = MVP * Vector4(0, 0, 1, 1);
						break;
					}
					break;
				}

				m_ScreenSpace_Selection1 = (Selection1 / Selection1.w).GetVector2(); // [-1 to +1] Range for XY
				m_Axis1_Direction = (Vector2(m_ScreenSpace_Selection1) - Vector2(m_ScreenSpace_SelectionCenter)).Normalize();
				m_useAxis1 = true;
				m_useAxis2 = false;

				if (m_GizmoSelectedPlane != Axis::NONE)
				{
					m_ScreenSpace_Selection2 = (Selection2 / Selection2.w).GetVector2(); // [-1 to +1] Range for XY
					m_Axis2_Direction = (Vector2(m_ScreenSpace_Selection2) - Vector2(m_ScreenSpace_SelectionCenter)).Normalize();
					m_useAxis2 = true;
				}

			}

			// Movement
			if (Input.getMouseButton(MouseButton::LEFT) && (m_useAxis1 || m_useAxis2))
			{
				// Flags
				m_GizmoClicked = true;

				Vector2 MouseChange = Vector2(
					(float)+Input.getMouseDeltaX() * Window.GetAspectRatio(),
					(float)-Input.getMouseDeltaY()
				);

				if (MouseChange.IsNotEmpty())
				{
					float Drag1 = MouseChange.ProjectLength(m_Axis1_Direction);
					float Drag2 = MouseChange.ProjectLength(m_Axis2_Direction);

					if (m_useAxis2)
					{
						Vector3 MoveDirection1;
						Vector3 MoveDirection2;

						switch (m_GizmoSelectedPlane)
						{
						case Axis::X:
							MoveDirection1 = m_GizmoTransform.Up;
							MoveDirection2 = m_GizmoTransform.Forward;
							break;
						case Axis::Y:
							MoveDirection1 = m_GizmoTransform.Right;
							MoveDirection2 = m_GizmoTransform.Forward;
							break;
						case Axis::Z:
							MoveDirection1 = m_GizmoTransform.Right;
							MoveDirection2 = m_GizmoTransform.Up;
							break;
						case Axis::NONE:
							break;
						}

						for (auto& Entity : m_selectedEntities)
						{
							Entity->m_transform.increaseWorldPosition(
								MoveDirection1 * (Drag1) * m_GizmoDragSpeed +
								MoveDirection2 * (Drag2) * m_GizmoDragSpeed
							);
						}
					}
					else
					{
						Vector3 MoveDirection;

						switch (m_GizmoSelectedAxis)
						{
						case Axis::X:
							MoveDirection = m_GizmoTransform.Right;
							break;
						case Axis::Y:
							MoveDirection = m_GizmoTransform.Up;
							break;
						case Axis::Z:
							MoveDirection = m_GizmoTransform.Forward;
							break;
						case Axis::NONE:
							break;
						}

						for (auto& Entity : m_selectedEntities)
						{
							Entity->m_transform.increaseWorldPosition(
								MoveDirection * (Drag1)* m_GizmoDragSpeed
							);
						}

					}
				}
			}


			//	// Drag Editor Axis around
			//	if (Input.getMouseButton(MouseButton::LEFT) && (m_controlAxis != Axis::NONE || m_controlPlane != Axis::NONE) && HasSelection())
			//	{
			//		Vector2 MouseChange = Vector2(
			//			(float)+Input.getMouseDragDeltaX(),
			//			(float)-Input.getMouseDragDeltaY()
			//		);
			//	
			//		// Clip space axis should only be calculated on the initial click
			//		static Vector2 Axis_ScreenDirection;
			//		static Vector2 CenterAxis_ScreenPos;
			//		static Vector2 Axis_ScreenPos;
			//		static float DragSpeed;
			//	
			//		if (m_controlAxisClicked == false)
			//		{
			//			// Object MVP
			//			Matrix4x4 MVP = RenderManager.GetMainCamera()->getViewProjection() * GetSelectionTransform().Model;
			//	
			//			// Calculate Center Axis in ClipSpace
			//			Vector4 ClipSpaceAxis = MVP * Vector4(0, 0, 0, 1);
			//			ClipSpaceAxis /= ClipSpaceAxis.w; // [-1 to +1] Range
			//			CenterAxis_ScreenPos = ClipSpaceAxis.GetVector2(0, 1);
			//	
			//			// Calculate Selected Axis in ClipSpace
			//			switch (m_controlAxis)
			//			{
			//			case Axis::X:
			//				ClipSpaceAxis = MVP * Vector4(1, 0, 0, 1);
			//				break;
			//			case Axis::Y:
			//				ClipSpaceAxis = MVP * Vector4(0, 1, 0, 1);
			//				break;
			//			case Axis::Z:
			//				ClipSpaceAxis = MVP * Vector4(0, 0, 1, 1);
			//				break;
			//			case Axis::NONE:
			//			{
			//				switch (m_controlPlane)
			//				{
			//				case Axis::X:
			//					ClipSpaceAxis = MVP * Vector4(0, 0.577f, 0.577f, 1);
			//					break;
			//				case Axis::Y:
			//					ClipSpaceAxis = MVP * Vector4(0.577f, 0, 0.577f, 1);
			//					break;
			//				case Axis::Z:
			//					ClipSpaceAxis = MVP * Vector4(0.577f, 0.577f, 0, 1);
			//					break;
			//				}
			//			}
			//			}
			//	
			//			ClipSpaceAxis /= ClipSpaceAxis.w; // [-1 to +1] Range
			//			Axis_ScreenPos = ClipSpaceAxis.GetVector2(0, 1);
			//	
			//			//std::cout << Axis_ScreenPos << std::endl;
			//			//std::cout << CenterAxis_ScreenPos << std::endl;
			//			//std::cout << "~~~" << std::endl;
			//	
			//			Axis_ScreenDirection = (Axis_ScreenPos - CenterAxis_ScreenPos).Normalize();
			//	
			//			// Update Drag Speed
			//			DragSpeed = GetSelectionTransform().CameraDistance / 3.0f;
			//		}
			//	
			//		//std::cout << DragSpeed << std::endl;
			//	
			//		VXL_ASSERT(Axis_ScreenDirection.LengthSqr() != 0.0f, "Axis_ScreenDirection is zero");
			//	
			//		// Project Mouse Change onto Xaxis in screenspace, now we'll see how far the mouse drags across the axis
			//		static float PreviousDragAmount = 0.0f;
			//		float DragAmount = MouseChange.ProjectLength(Axis_ScreenDirection) * DragSpeed;
			//		// Drag Multiply
			//	
			//		if (m_controlAxisClicked == false)
			//			PreviousDragAmount = DragAmount;
			//	
			//		// No movement occurs if mouse change is empty
			//		if (!MouseChange.Is_Empty())
			//		{
			//			Vector3 MoveDirection;
			//	
			//			switch (m_controlAxis)
			//			{
			//			case Axis::X:
			//				MoveDirection = GetSelectionTransform().Right;
			//				break;
			//			case Axis::Y:
			//				MoveDirection = GetSelectionTransform().Up;
			//				break;
			//			case Axis::Z:
			//				MoveDirection = GetSelectionTransform().Forward;
			//				break;
			//			case Axis::NONE:
			//			{
			//				switch (m_controlPlane)
			//				{
			//				case Axis::X:
			//					MoveDirection = GetSelectionTransform().Up + GetSelectionTransform().Forward;
			//					break;
			//				case Axis::Y:
			//					MoveDirection = GetSelectionTransform().Right + GetSelectionTransform().Forward;
			//					break;
			//				case Axis::Z:
			//					MoveDirection = GetSelectionTransform().Right + GetSelectionTransform().Up;
			//					break;
			//				}
			//			}
			//			}
			//	
			//			for (auto& Entity : m_selectedEntities)
			//			{
			//				Entity->m_transform.increaseWorldPosition(MoveDirection * 0.01f * (DragAmount - PreviousDragAmount));
			//			}
			//		}
			//	
			//		
			//		//std::cout << (MouseChange) << std::endl;
			//		//std::cout << (Axis_ScreenDirection) << std::endl;
			//		//std::cout << (DragAmount) << std::endl;
			//		//std::cout << (PreviousDragAmount) << std::endl;
			//		//std::cout << "~~~" << std::endl;
			//	
			//		PreviousDragAmount = DragAmount;
			//	
			//		m_controlAxisClicked = true;
			//	}
			//	else
			//		m_controlAxisClicked = false;
		}

	}
}