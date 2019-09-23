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
		m_fbo_colorPicker = FramebufferObject::Create("_ColorPicker");
		m_fbo_colorPicker->SetSizeToWindowSize();
		m_fbo_colorPicker->Bind();
		//
		m_fbo_colorPicker->NewAttachment(0, RenderTarget::Type::TEXTURE, "color", TextureFormat::RGBA8);
		m_fbo_colorPicker->SetDepth(TextureDepthFormat::DEPTH16, RenderTarget::Type::BUFFER);
		//
		m_fbo_colorPicker->checkFBOStatus();
		m_fbo_colorPicker->Unbind();
	}
	void Editor::DestroyGLResources()
	{
		FramebufferObject::DeleteNamedAsset(m_fbo_colorPicker);
		m_fbo_colorPicker = nullptr;
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
			//float AxisScale = 1.0f;// m_averageSelectionDistanceFromEditorCamera / 3.0f;

			// Local/World Selection + Scaling can only occur locally
			if (m_controlAxisLocal || m_controlMode == GizmoMode::SCALE)
			{
				Matrix3x3 RotationMatrix = m_selectedEntities[0]->m_transform.getWorldRotation().GetMatrix3x3();
				m_GizmoTransform.Model = Matrix4x4(RotationMatrix, m_GizmoTransform.WorldPosition);
				m_GizmoTransform.Forward = m_selectedEntities[0]->m_transform.getForward();
				m_GizmoTransform.Up = m_selectedEntities[0]->m_transform.getUp();
				m_GizmoTransform.Right = m_selectedEntities[0]->m_transform.getRight();
			}
			else
			{
				m_GizmoTransform.Model = Matrix4x4::GetTranslate(m_GizmoTransform.WorldPosition);
				m_GizmoTransform.Forward = Vector3::FORWARD;
				m_GizmoTransform.Up = Vector3::UP;
				m_GizmoTransform.Right = Vector3::RIGHT;
			}

			// Update Axis Portion for position
			if (m_controlMode == GizmoMode::TRANSLATE)
			{
				CameraObject* mainCamera = RenderManager.GetMainCamera();
				Vector3 SelectionTransformToCamera = (mainCamera->m_transform.getWorldPosition() - m_GizmoTransform.WorldPosition);

				bool CamOnXAxis = SelectionTransformToCamera.Dot(m_GizmoTransform.Right) > 0.0f;
				bool CamOnYAxis = SelectionTransformToCamera.Dot(m_GizmoTransform.Up) > 0.0f;
				bool CamOnZAxis = SelectionTransformToCamera.Dot(m_GizmoTransform.Forward) > 0.0f;

				// X-Plane
				Matrix4x4 Offset = Matrix4x4::GetTranslate(Vector3(0, CamOnYAxis ? 0.25f : -0.25f, CamOnZAxis ? 0.25f : -0.25f));
				m_GizmoTransform.X_Model = m_GizmoTransform.Model * Offset;

				// Y-Plane
				Offset = Matrix4x4::GetTranslate(Vector3(CamOnXAxis ? 0.25f : -0.25f, 0, CamOnZAxis ? 0.25f : -0.25f));
				m_GizmoTransform.Y_Model = m_GizmoTransform.Model * Offset;

				// Z-Plane
				Offset = Matrix4x4::GetTranslate(Vector3(CamOnXAxis ? 0.25f : -0.25f, CamOnYAxis ? 0.25f : -0.25f, 0));
				m_GizmoTransform.Z_Model = m_GizmoTransform.Model * Offset;
			}
			else if (m_controlMode == GizmoMode::SCALE)
			{
				if (!m_GizmoClicked)
				{
					Matrix4x4 Offset = Matrix4x4::GetTranslate(Vector3(1, 0, 0));
					m_GizmoTransform.X_Model = m_GizmoTransform.Model * Offset;

					Offset = Matrix4x4::GetTranslate(Vector3(0, 1, 0));
					m_GizmoTransform.Y_Model = m_GizmoTransform.Model * Offset;

					Offset = Matrix4x4::GetTranslate(Vector3(0, 0, 1));
					m_GizmoTransform.Z_Model = m_GizmoTransform.Model * Offset;
				}
				else
				{
					float x = m_GizmoSelectedAxis == Axis::X ? m_totalDrag : 1.0f;
					float y = m_GizmoSelectedAxis == Axis::Y ? m_totalDrag : 1.0f;
					float z = m_GizmoSelectedAxis == Axis::Z ? m_totalDrag : 1.0f;

					Matrix4x4 Offset = Matrix4x4::GetTranslate(Vector3(x, 0, 0));
					m_GizmoTransform.X_Model = m_GizmoTransform.Model * Offset;

					Offset = Matrix4x4::GetTranslate(Vector3(0, y, 0));
					m_GizmoTransform.Y_Model = m_GizmoTransform.Model * Offset;

					Offset = Matrix4x4::GetTranslate(Vector3(0, 0, z));
					m_GizmoTransform.Z_Model = m_GizmoTransform.Model * Offset;
				}
			}
		}
	}

	void Editor::UpdateSelectionInfo()
	{
		if (!Window.IsCursorOnImguiWindow() && Window.GetCursor() == CursorMode::NORMAL && HasSelection())
		{
			// Highlighting over an Editor Axis not selected
			if (!Input.getMouseButton(MouseButton::LEFT))
			{
				// Setup flags
				m_GizmoClicked = false;
				m_totalDrag = 0.0f;

				m_fbo_colorPicker->Bind();
				m_fbo_colorPicker->ClearBuffers();

				Material* material_colorPicker = Material::GetAsset("colorPicker");

				material_colorPicker->BindProgram();
				material_colorPicker->BindStates();

				material_colorPicker->m_property_useModel.SetProperty(true);
				material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Model, true);

				if (m_controlMode == GizmoMode::TRANSLATE)
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
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.X_Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetHalfQuadX()->Draw();

					// Y Plane
					color = Util::Conversion::uint_to_color4(5u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Y_Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetHalfQuadY()->Draw();

					// Z Plane
					color = Util::Conversion::uint_to_color4(6u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Z_Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetHalfQuadZ()->Draw();

					Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

					// Cube (ignored)
					color = Util::Conversion::uint_to_color4(7u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetCubeSmall()->Draw();

					RawArray<uint8_t> data = m_fbo_colorPicker->readPixelsFromMouse(0, 1, 1);
					if (data.start != nullptr)
					{
						unsigned int EditorIndex = Util::Conversion::uchars_to_uint(data.start);
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
						case 7u:
							m_GizmoSelectedAxis = Axis::ALL;
							m_GizmoSelectedPlane = Axis::NONE;
							break;
						default:
							m_GizmoSelectedAxis = Axis::NONE;
							m_GizmoSelectedPlane = Axis::NONE;
							break;
						}
					}

					m_GizmoSelected =
						(m_GizmoSelectedAxis != Axis::NONE) &&
						(m_GizmoSelectedPlane != Axis::NONE)
						;
				}
				else if (m_controlMode == GizmoMode::SCALE)
				{
					Color4F color;

					// X Cube
					color = Util::Conversion::uint_to_color4(1u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.X_Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetCubeSmall()->Draw();

					// Y Cube
					color = Util::Conversion::uint_to_color4(2u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Y_Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetCubeSmall()->Draw();

					// Z Cube
					color = Util::Conversion::uint_to_color4(3u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Z_Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetCubeSmall()->Draw();

					// Center Cube
					color = Util::Conversion::uint_to_color4(4u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetCubeSmall()->Draw();

					RawArray<uint8_t> data = m_fbo_colorPicker->readPixelsFromMouse(0, 1, 1);
					if (data.start != nullptr)
					{
						unsigned int EditorIndex = Util::Conversion::uchars_to_uint(data.start);
						data.Deallocate();

						switch (EditorIndex)
						{
						case 1u:
							m_GizmoSelectedAxis = Axis::X;
							break;
						case 2u:
							m_GizmoSelectedAxis = Axis::Y;
							break;
						case 3u:
							m_GizmoSelectedAxis = Axis::Z;
							break;
						case 4u:
							m_GizmoSelectedAxis = Axis::ALL;
							break;
						default:
							m_GizmoSelectedAxis = Axis::NONE;
							break;
						}
					}

					m_GizmoSelected = (m_GizmoSelectedAxis != Axis::NONE);
					m_GizmoSelectedPlane = Axis::NONE;
				}
				else if (m_controlMode == GizmoMode::ROTATE)
				{
					Color4F color;

					// Draw both sides
					Graphics::SetCullMode(CullMode::NO_CULL);

					// X Circle
					color = Util::Conversion::uint_to_color4(1u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetCircleX()->Draw();

					// Y Circle
					color = Util::Conversion::uint_to_color4(2u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetCircleY()->Draw();

					// Z Circle
					color = Util::Conversion::uint_to_color4(3u);
					material_colorPicker->m_property_model.SetPropertyMatrix(m_GizmoTransform.Model, true);
					material_colorPicker->m_property_output.SetProperty(color);
					Geometry.GetCircleZ()->Draw();

					// Revert
					Graphics::SetCullMode(CullMode::COUNTER_CLOCKWISE);

					RawArray<uint8_t> data = m_fbo_colorPicker->readPixelsFromMouse(0, 1, 1);
					if (data.start != nullptr)
					{
						unsigned int EditorIndex = Util::Conversion::uchars_to_uint(data.start);
						data.Deallocate();

						switch (EditorIndex)
						{
						case 1u:
							m_GizmoSelectedAxis = Axis::X;
							break;
						case 2u:
							m_GizmoSelectedAxis = Axis::Y;
							break;
						case 3u:
							m_GizmoSelectedAxis = Axis::Z;
							break;
						default:
							m_GizmoSelectedAxis = Axis::NONE;
							break;
						}
					}

					m_GizmoSelected = (m_GizmoSelectedAxis != Axis::NONE);
					m_GizmoSelectedPlane = Axis::NONE;
				}

			}
			// Store internal information before movement occurs
			else if (Input.getMouseButtonDown(MouseButton::LEFT) && (m_GizmoSelectedAxis != Axis::NONE || m_GizmoSelectedPlane != Axis::NONE))
			{
				// Clip Space position for Gizmo
				Matrix4x4 MVP = RenderManager.GetMainCamera()->getViewProjection() * m_GizmoTransform.Model;

				// Center of Gizmo
				Vector4 SelectionCenter = MVP.GetColumn(3); // Same as MVP * Vector4(0, 0, 0, 1);
				m_ScreenSpace_SelectionCenter = (SelectionCenter / SelectionCenter.w).GetVector2(); // [-1 to +1] Range for XY

				if (m_controlMode == GizmoMode::TRANSLATE)
				{
					// Selecting all axis
					if (m_GizmoSelectedAxis == Axis::ALL)
					{
						// Make a raycast with the Mouse's ScreenPosition
						Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));

						// Plane Normal
						m_SelectedPlane = RenderManager.GetMainCamera()->m_transform.getCameraBackwards();

						// Calculate where it hits
						Plane p(m_SelectedPlane, m_GizmoTransform.WorldPosition);
						Ray r(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
						RayHit hit = Intersection(r, p); // Guaranteed intersection

						m_dragStart = hit.m_location;
						m_GizmoClicked = true;
					}

					// Selecing a plane
					else if (m_GizmoSelectedPlane != Axis::NONE)
					{
						// Make a raycast with the Mouse's ScreenPosition
						Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));

						// Plane Normal
						switch (m_GizmoSelectedPlane)
						{
						case Axis::X:
							m_SelectedPlane = m_GizmoTransform.Right;
							break;
						case Axis::Y:
							m_SelectedPlane = m_GizmoTransform.Up;
							break;
						case Axis::Z:
							m_SelectedPlane = m_GizmoTransform.Forward;
							break;
						default:
							VXL_ERROR("Plane Selection Error");
							break;
						}

						// Calculate where it hits
						Plane p(m_SelectedPlane, m_GizmoTransform.WorldPosition);
						Ray r(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
						RayHit hit = Intersection(r, p); // Guaranteed intersection

						m_dragStart = hit.m_location;
						m_GizmoClicked = true;
					}

					// Selecting just 1 axis
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

						m_dragStart = m_GizmoTransform.WorldPosition;

						// Make a raycast with the Mouse's ScreenPosition
						Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
						Ray		ViewRay(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
						Ray		DirectionRay(m_dragStart, MoveDirection);

						ShortestDistance(ViewRay, DirectionRay);

						m_previousDrag = DirectionRay.m_t;
						
						m_GizmoClicked = true;
					}

				}
				else if (m_controlMode == GizmoMode::SCALE)
				{
					// Selecting Scale Axis
					if (m_GizmoSelectedAxis != Axis::ALL)
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
						}

						m_dragStart = m_GizmoTransform.WorldPosition;

						// Make a raycast with the Mouse's ScreenPosition
						Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
						Ray		ViewRay(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
						Ray		DirectionRay(m_dragStart, MoveDirection);

						ShortestDistance(ViewRay, DirectionRay);

						m_previousDrag = DirectionRay.m_t;

						// Start Scale at 1.0f
						m_totalDrag = 1.0f;

						// Store all original scales to avoid precision loss
						uint32_t selectionCount = (uint32_t)m_selectedEntities.size();
						m_scaleBackups.clear();

						switch (m_GizmoSelectedAxis)
						{
						case Axis::X:
							//Selection = MVP * Vector4(1, 0, 0, 1);

							for (uint32_t i = 0; i < selectionCount; i++)
								m_scaleBackups.push_back(m_selectedEntities[i]->m_transform.getScale().x);
							break;
						case Axis::Y:
							//Selection = MVP * Vector4(0, 1, 0, 1);

							for (uint32_t i = 0; i < selectionCount; i++)
								m_scaleBackups.push_back(m_selectedEntities[i]->m_transform.getScale().y);
							break;
						case Axis::Z:
							//Selection = MVP * Vector4(0, 0, 1, 1);

							for (uint32_t i = 0; i < selectionCount; i++)
								m_scaleBackups.push_back(m_selectedEntities[i]->m_transform.getScale().z);
							break;
						}

					}

					
					m_GizmoClicked = true;

					
					//	
					//	Vector4 Selection;
					//	
					// Check Selections of Gizmo
					
					//	
					//	// Get Screenspace direction of axis
					//	m_ScreenSpace_Selection = (Selection / Selection.w).GetVector2(); // [-1 to +1] Range for XY
					//	m_Axis_Direction = (Vector2(m_ScreenSpace_Selection) - Vector2(m_ScreenSpace_SelectionCenter)).Normalize();
					//	m_GizmoClicked = true;
					//	
					//	
					
				}
				else if (m_controlMode == GizmoMode::ROTATE)
				{
					switch (m_GizmoSelectedAxis)
					{
					case Axis::X:
						m_SelectedPlane = m_GizmoTransform.Right;
						break;
					case Axis::Y:
						m_SelectedPlane = m_GizmoTransform.Up;
						break;
					case Axis::Z:
						m_SelectedPlane = m_GizmoTransform.Forward;
						break;
					}

					// Make a raycast with the Mouse's ScreenPosition
					Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));

					// Calculate where it hits
					Plane p(m_SelectedPlane, m_GizmoTransform.WorldPosition);
					Ray r(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
					RayHit hit = Intersection(r, p); // Guaranteed intersection

					m_dragStart = (hit.m_location - m_GizmoTransform.WorldPosition).Normalize();
					m_GizmoClicked = true;
				}
			}

			// Movement
			if (Input.getMouseButton(MouseButton::LEFT) && m_GizmoClicked)
			{
				// Flags
				m_GizmoClicked = true;

				Vector2 MouseChange = Vector2(
					(float)+Input.getMouseDeltaX() * Window.GetAspectRatio(),
					(float)-Input.getMouseDeltaY()
				);

				if (MouseChange.IsNotEmpty())
				{
					if (m_controlMode == GizmoMode::TRANSLATE)
					{
						// Dragging all Axis
						if (m_GizmoSelectedAxis == Axis::ALL)
						{
							// Make a raycast with the Mouse's ScreenPosition
							Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));

							// Calculate where it hits
							Plane p(m_SelectedPlane, m_GizmoTransform.WorldPosition);
							Ray r(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
							RayHit hit = Intersection(r, p); // Guaranteed intersection

							m_dragEnd = hit.m_location;

							for (auto& Entity : m_selectedEntities)
							{
								Entity->m_transform.increaseWorldPosition((m_dragEnd - m_dragStart) * 1);
							}

							m_dragStart = m_dragEnd;
						}
						// Dragging a plane
						else if (m_GizmoSelectedPlane != Axis::NONE)
						{
							// Make a raycast with the Mouse's ScreenPosition
							Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));

							// Calculate where it hits
							Plane p(m_SelectedPlane, m_GizmoTransform.WorldPosition);
							Ray r(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
							RayHit hit = Intersection(r, p); // Guaranteed intersection

							m_dragEnd = hit.m_location;

							for (auto& Entity : m_selectedEntities)
							{
								Entity->m_transform.increaseWorldPosition((m_dragEnd - m_dragStart) * 1);
							}

							m_dragStart = m_dragEnd;
						}
						// Dragging 1 axis
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

							// Make a raycast with the Mouse's ScreenPosition
							Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
							Ray		ViewRay(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
							Ray		DirectionRay(m_dragStart, MoveDirection);

							ShortestDistance(ViewRay, DirectionRay);

							for (auto& Entity : m_selectedEntities)
							{
								Entity->m_transform.increaseWorldPosition(
									-MoveDirection * (DirectionRay.m_t - m_previousDrag)
								);
							}

							m_previousDrag = DirectionRay.m_t;
						}
					}
					else if (m_controlMode == GizmoMode::SCALE)
					{
						// Special case for Global Scale
						if (m_GizmoSelectedAxis == Axis::ALL)
						{
							// Get Mouse position in -1 to +1 range of screen
							Vector2 MouseScreenSpace = Input.getMousePosScreenspace(true);

							// Distance from mouse to center point of Gizmo
							Vector2 MouseDelta = MouseScreenSpace - Vector2(m_ScreenSpace_SelectionCenter);
							float Distance = MouseDelta.x + MouseDelta.y + 1.0f;

							std::cout << Distance << std::endl;

							// Scale everything based on distance
							uint32_t selectionCount = (uint32_t)m_selectedEntities.size();
							for (uint32_t i = 0; i < selectionCount; i++)
							{
								m_selectedEntities[i]->m_transform.increaseScale(Distance - m_totalDrag);
							}
							// Use "m_totalDrag1" similar to a "lastTime" value of a timer
							m_totalDrag = Distance;
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

							// Make a raycast with the Mouse's ScreenPosition
							Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));
							Ray		ViewRay(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
							Ray		DirectionRay(m_dragStart, MoveDirection);

							ShortestDistance(ViewRay, DirectionRay);

							m_totalDrag = -DirectionRay.m_t;
							
							uint32_t selectionCount = (uint32_t)m_selectedEntities.size();
							switch (m_GizmoSelectedAxis)
							{
							case Axis::X:
								for (uint32_t i = 0; i < selectionCount; i++)
								{
									m_selectedEntities[i]->m_transform.setScaleX(m_scaleBackups[i] * m_totalDrag);
								}
								break;
							case Axis::Y:
								for (uint32_t i = 0; i < selectionCount; i++)
								{
									m_selectedEntities[i]->m_transform.setScaleY(m_scaleBackups[i] * m_totalDrag);
								}
								break;
							case Axis::Z:
								for (uint32_t i = 0; i < selectionCount; i++)
								{
									m_selectedEntities[i]->m_transform.setScaleZ(m_scaleBackups[i] * m_totalDrag);
								}
								break;
							case Axis::NONE:
								break;
							}

							//	uint32_t selectionCount = (uint32_t)m_selectedEntities.size();
							//	
							//	// Scale objects based on how much dragging the mouse does
							//	float Drag = MouseChange.ProjectLength(m_Axis_Direction) * 0.02f;
							//	m_totalDrag += Drag;
							//	
							//	if (m_totalDrag != 0.0f)
							//	{
							//		Vector3 MoveDirection;
							//	
							//		switch (m_GizmoSelectedAxis)
							//		{
							//		case Axis::X:
							//			MoveDirection = m_GizmoTransform.Right;
							//	
							//			for (uint32_t i = 0; i < selectionCount; i++)
							//			{
							//				m_selectedEntities[i]->m_transform.setScaleX(m_scaleBackups[i] * m_totalDrag);
							//			}
							//			break;
							//		case Axis::Y:
							//			MoveDirection = m_GizmoTransform.Up;
							//	
							//			for (uint32_t i = 0; i < selectionCount; i++)
							//			{
							//				m_selectedEntities[i]->m_transform.setScaleY(m_scaleBackups[i] * m_totalDrag);
							//			}
							//			break;
							//		case Axis::Z:
							//			MoveDirection = m_GizmoTransform.Forward;
							//	
							//			for (uint32_t i = 0; i < selectionCount; i++)
							//			{
							//				m_selectedEntities[i]->m_transform.setScaleZ(m_scaleBackups[i] * m_totalDrag);
							//			}
							//			break;
							//		case Axis::NONE:
							//			break;
							//		}
							//	}
						}
					}
					else if (m_controlMode == GizmoMode::ROTATE)
					{
						// Make a raycast with the Mouse's ScreenPosition
						Vector3 ScreenAim = RenderManager.GetMainCamera()->ScreenSpaceToDirection(Input.getMousePosScreenspace(true));

						// Calculate where it hits
						Plane p(m_SelectedPlane, m_GizmoTransform.WorldPosition);
						Ray r(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), ScreenAim);
						RayHit hit = Intersection(r, p); // Guaranteed intersection

						m_dragEnd = (hit.m_location - m_GizmoTransform.WorldPosition).Normalize();

						// Only rotate if both vectors are not the same
						if (!m_dragEnd.CompareFuzzy(m_dragStart))
						{
							// Check angle between the two
							float degrees = Vector3::GetAngleDegrees(m_dragStart, m_dragEnd);

							// Check if going in other direction
							Vector3 c = Vector3::Cross(m_dragStart, m_dragEnd);
							float sign = c.Dot(m_SelectedPlane);
							if (sign > 0)
								degrees = -degrees;

							uint32_t selectionCount = (uint32_t)m_selectedEntities.size();
							for (uint32_t i = 0; i < selectionCount; i++)
							{
								if (m_controlAxisLocal)
								{
									m_selectedEntities[i]->m_transform.rotateAroundAxis(m_SelectedPlane, degrees);
								}
								else
									switch (m_GizmoSelectedAxis)
									{
									case Axis::X:
										m_selectedEntities[i]->m_transform.increaseRotationX(degrees);
										break;
									case Axis::Y:
										m_selectedEntities[i]->m_transform.increaseRotationY(degrees);
										break;
									case Axis::Z:
										m_selectedEntities[i]->m_transform.increaseRotationZ(degrees);
										break;
									}
							}

							// Update previous rotation
							m_dragStart = m_dragEnd;
						}
					}
				}
			}

		}

	}
}