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
					float x = m_GizmoSelectedAxis == Axis::X ? m_totalDrag1 : 1.0f;
					float y = m_GizmoSelectedAxis == Axis::Y ? m_totalDrag1 : 1.0f;
					float z = m_GizmoSelectedAxis == Axis::Z ? m_totalDrag1 : 1.0f;

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
				m_useAxis1 = false;
				m_useAxis2 = false;
				m_totalDrag1 = 0.0f;
				m_totalDrag2 = 0.0f;

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

					// Cube (ignored)
					material_colorPicker->m_property_output.SetProperty(Color4F(0, 0, 0, 0));
					Geometry.GetCubeSmall()->Draw();

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
						default:
							m_GizmoSelectedAxis = Axis::NONE;
							m_GizmoSelectedPlane = Axis::NONE;
							break;
						}
					}
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
				}

				m_GizmoSelected =
					(m_GizmoSelectedAxis != Axis::NONE) &&
					(m_GizmoSelectedPlane != Axis::NONE)
					;
			}
			// Store internal information before movement occurs
			else if (Input.getMouseButtonDown(MouseButton::LEFT) && (m_GizmoSelectedAxis != Axis::NONE || m_GizmoSelectedPlane != Axis::NONE))
			{
				// Init speed
				m_GizmoDragSpeed = 0.01f * m_GizmoTransform.CameraDistance / 3.0f;

				// Clip Space position for Gizmo
				Matrix4x4 MVP = RenderManager.GetMainCamera()->getViewProjection() * m_GizmoTransform.Model;

				// Center of Gizmo
				Vector4 SelectionCenter = MVP.GetColumn(3); // Same as MVP * Vector4(0, 0, 0, 1);
				m_ScreenSpace_SelectionCenter = (SelectionCenter / SelectionCenter.w).GetVector2(); // [-1 to +1] Range for XY

				if (m_controlMode == GizmoMode::TRANSLATE)
				{
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

					// Get Screenspace direction of axis
					m_ScreenSpace_Selection1 = (Selection1 / Selection1.w).GetVector2(); // [-1 to +1] Range for XY
					m_Axis1_Direction = (Vector2(m_ScreenSpace_Selection1) - Vector2(m_ScreenSpace_SelectionCenter)).Normalize();
					m_useAxis1 = true;
					m_useAxis2 = false;

					if (m_GizmoSelectedPlane != Axis::NONE)
					{
						// Get Screenspace direction of secondary axis
						m_ScreenSpace_Selection2 = (Selection2 / Selection2.w).GetVector2(); // [-1 to +1] Range for XY
						m_Axis2_Direction = (Vector2(m_ScreenSpace_Selection2) - Vector2(m_ScreenSpace_SelectionCenter)).Normalize();
						m_useAxis2 = true;
					}
				}
				else if (m_controlMode == GizmoMode::SCALE)
				{
					uint32_t selectionCount = (uint32_t)m_selectedEntities.size();
					m_scaleBackups.clear();

					Vector4 Selection;

					// Check Selections of Gizmo
					switch (m_GizmoSelectedAxis)
					{
					case Axis::X:
						Selection = MVP * Vector4(1, 0, 0, 1);

						for (uint32_t i = 0; i < selectionCount; i++)
							m_scaleBackups.push_back(m_selectedEntities[i]->m_transform.getScale().x);
						break;
					case Axis::Y:
						Selection = MVP * Vector4(0, 1, 0, 1);

						for (uint32_t i = 0; i < selectionCount; i++)
							m_scaleBackups.push_back(m_selectedEntities[i]->m_transform.getScale().y);
						break;
					case Axis::Z:
						Selection = MVP * Vector4(0, 0, 1, 1);

						for (uint32_t i = 0; i < selectionCount; i++)
							m_scaleBackups.push_back(m_selectedEntities[i]->m_transform.getScale().z);
						break;
					case Axis::ALL:
						Selection = SelectionCenter;
						break;
					case Axis::NONE:
						break;
					}

					// Get Screenspace direction of axis
					m_ScreenSpace_Selection1 = (Selection / Selection.w).GetVector2(); // [-1 to +1] Range for XY
					m_Axis1_Direction = (Vector2(m_ScreenSpace_Selection1) - Vector2(m_ScreenSpace_SelectionCenter)).Normalize();
					m_useAxis1 = true;

					// Special State for Scale
					m_totalDrag1 = 1.0f;
				}
				else if (m_controlMode == GizmoMode::ROTATE)
				{
					// Get Mouse position in -1 to +1 range of screen
					Vector2 MouseScreenSpace = Input.getMousePosScreenspace(true);
						
					//	m_Axis1_Direction = (MouseViewport - Vector2(m_ScreenSpace_SelectionCenter)).Normalize();
					m_Axis1_Direction = MouseScreenSpace;
					m_useAxis1 = true;
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
					if (m_controlMode == GizmoMode::TRANSLATE)
					{
						float Drag1 = MouseChange.ProjectLength(m_Axis1_Direction);
						float Drag2 = MouseChange.ProjectLength(m_Axis2_Direction);
						m_totalDrag1 += Drag1;
						m_totalDrag2 += Drag2;

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
									MoveDirection1 * (Drag1)* m_GizmoDragSpeed +
									MoveDirection2 * (Drag2)* m_GizmoDragSpeed
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
					else if (m_controlMode == GizmoMode::SCALE)
					{
						// Special case for Global Scale
						if (m_GizmoSelectedAxis == Axis::ALL)
						{
							// Get Mouse position in -1 to +1 range of screen
							Vector2 MouseScreenSpace = Input.getMousePosScreenspace(true);

							std::cout << MouseScreenSpace << std::endl;

							// Distance from mouse to center point of Gizmo
							Vector2 MouseDelta = MouseScreenSpace - Vector2(m_ScreenSpace_SelectionCenter);
							float Distance = MouseDelta.x + MouseDelta.y + 1.0f;

							// Scale everything based on distance
							uint32_t selectionCount = (uint32_t)m_selectedEntities.size();
							for (uint32_t i = 0; i < selectionCount; i++)
							{
								m_selectedEntities[i]->m_transform.increaseScale(Distance - m_totalDrag1);
							}
							// Use "m_totalDrag1" similar to a "lastTime" value of a timer
							m_totalDrag1 = Distance;
						}
						else
						{
							uint32_t selectionCount = (uint32_t)m_selectedEntities.size();

							// Scale objects based on how much dragging the mouse does
							float Drag = MouseChange.ProjectLength(m_Axis1_Direction) * 0.02f;

							m_totalDrag1 += Drag;

							if (m_totalDrag1 != 0.0f)
							{
								Vector3 MoveDirection;

								switch (m_GizmoSelectedAxis)
								{
								case Axis::X:
									MoveDirection = m_GizmoTransform.Right;

									for (uint32_t i = 0; i < selectionCount; i++)
									{
										m_selectedEntities[i]->m_transform.setScaleX(m_scaleBackups[i] * m_totalDrag1);
									}
									break;
								case Axis::Y:
									MoveDirection = m_GizmoTransform.Up;

									for (uint32_t i = 0; i < selectionCount; i++)
									{
										m_selectedEntities[i]->m_transform.setScaleY(m_scaleBackups[i] * m_totalDrag1);
									}
									break;
								case Axis::Z:
									MoveDirection = m_GizmoTransform.Forward;

									for (uint32_t i = 0; i < selectionCount; i++)
									{
										m_selectedEntities[i]->m_transform.setScaleZ(m_scaleBackups[i] * m_totalDrag1);
									}
									break;
								case Axis::NONE:
									break;
								}
							}
						}
					}
					else if (m_controlMode == GizmoMode::ROTATE)
					{
						// Get Mouse position in -1 to +1 range of screen
						Vector2 MouseViewport = Vector2(Input.getMousePosViewportX(), Input.getMousePosViewportY(true));
						MouseViewport = MouseViewport * 2.0f - 1.0f;// go from [0->1] range to [-1->+1] range

						// Distance from mouse to center point of Gizmo
						Vector2 MouseDelta = MouseViewport - m_Axis1_Direction;
						float Distance = MouseDelta.x + MouseDelta.y;

						// Rotate everything based on distance
						float rotationStrength = 35.0f;
						uint32_t selectionCount = (uint32_t)m_selectedEntities.size();
						for (uint32_t i = 0; i < selectionCount; i++)
						{
							switch (m_GizmoSelectedAxis)
							{
							case Axis::X:
								m_selectedEntities[i]->m_transform.increaseRotationX(rotationStrength * (Distance - m_totalDrag1));
								break;
							case Axis::Y:
								m_selectedEntities[i]->m_transform.increaseRotationY(rotationStrength * (Distance - m_totalDrag1));
								break;
							case Axis::Z:
								m_selectedEntities[i]->m_transform.increaseRotationZ(rotationStrength * (Distance - m_totalDrag1));
								break;
							}
							
						}
						// Use "m_totalDrag1" similar to a "lastTime" value of a timer
						m_totalDrag1 = Distance;
					}
				}
			}

		}

	}
}