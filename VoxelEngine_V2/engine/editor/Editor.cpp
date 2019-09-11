// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "Editor.h"
#include "../modules/Entity.h"
#include "../rendering/RenderManager.h"

#include "../objects/CameraObject.h"

#include "../utilities/Util.h"

namespace Vxl
{
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

		m_SelectionTransform.WorldPosition = Vector3::ZERO;
	}

	void Editor::UpdateSelectionTransformPosition()
	{
		m_SelectionTransform.WorldPosition = Vector3::ZERO;
		float m_selectedEntitiesCount = (float)m_selectedEntities.size();

		for (const auto& Entity : m_selectedEntities)
			m_SelectionTransform.WorldPosition += Entity->m_transform.getWorldPosition() / m_selectedEntitiesCount;

	}

	void Editor::UpdateSelectionTransformModel()
	{
		UpdateSelectionTransformPosition();

		// Only update model if there's an object selected
		if (m_selectedEntities.size() > 0)
		{
			m_SelectionTransform.CameraDistance = Vector3::Distance(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), m_SelectionTransform.WorldPosition);
			float AxisScale = 1.0f;// m_averageSelectionDistanceFromEditorCamera / 3.0f;

			if (m_controlAxisLocal)
			{
				Matrix3x3 RotationMatrix = m_selectedEntities[0]->m_transform.getWorldRotationMatrix();
				m_SelectionTransform.Model = Matrix4x4(RotationMatrix * Matrix3x3::GetScale(AxisScale), m_SelectionTransform.WorldPosition);
				m_SelectionTransform.Forward = m_selectedEntities[0]->m_transform.getForward();
				m_SelectionTransform.Up = m_selectedEntities[0]->m_transform.getUp();
				m_SelectionTransform.Right = m_selectedEntities[0]->m_transform.getRight();
			}
			else
			{
				m_SelectionTransform.Model = Matrix4x4::GetTranslateScale(m_SelectionTransform.WorldPosition, Vector3(AxisScale));
				m_SelectionTransform.Forward = Vector3::FORWARD;
				m_SelectionTransform.Up = Vector3::UP;
				m_SelectionTransform.Right = Vector3::RIGHT;
			}

			// Update Axis Portion for position
			if (m_controlMode == ControlMode::TRANSLATE)
			{
				CameraObject* mainCamera = RenderManager.GetMainCamera();
				Vector3 SelectionTransformToCamera = (mainCamera->m_transform.getWorldPosition() - m_SelectionTransform.WorldPosition);

				bool CamOnXAxis = SelectionTransformToCamera.Dot(m_SelectionTransform.Right) > 0.0f;
				bool CamOnYAxis = SelectionTransformToCamera.Dot(m_SelectionTransform.Up) > 0.0f;
				bool CamOnZAxis = SelectionTransformToCamera.Dot(m_SelectionTransform.Forward) > 0.0f;

				// X-Plane
				Matrix4x4 Offset = Matrix4x4::GetTranslate(Vector3(0, CamOnYAxis ? 0.25f : -0.25f, CamOnZAxis ? 0.25f : -0.25f));
				m_AxisSelectionTransform.X_Model = m_SelectionTransform.Model * Offset;

				// Y-Plane
				Offset = Matrix4x4::GetTranslate(Vector3(CamOnXAxis ? 0.25f : -0.25f, 0, CamOnZAxis ? 0.25f : -0.25f));
				m_AxisSelectionTransform.Y_Model = m_SelectionTransform.Model * Offset;

				// Z-Plane
				Offset = Matrix4x4::GetTranslate(Vector3(CamOnXAxis ? 0.25f : -0.25f, CamOnYAxis ? 0.25f : -0.25f, 0));
				m_AxisSelectionTransform.Z_Model = m_SelectionTransform.Model * Offset;
			}
		}
	}
}