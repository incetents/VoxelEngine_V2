// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "Editor.h"
#include "../modules/Entity.h"
#include "../utilities/Util.h"

#include "../modules/RenderManager.h"
#include "../objects/CameraObject.h"

namespace Vxl
{
	void Editor::RemoveSelection(Entity* _entity)
	{
		Util::RemoveFromVector(m_selectedEntities, _entity);
		_entity->m_isSelected = false;

		UpdateSelectionAverage();
	}
	void Editor::AddSelection(Entity* _entity)
	{
		VXL_ASSERT(_entity, "Adding nullptr in Editor::AddSelection");
		if (_entity->m_isSelectable == false)
			return;

		for (auto Entity : m_selectedEntities)
			if (Entity == _entity)
				return;

		m_selectedEntities.push_back(_entity);
		_entity->m_isSelected = true;

		UpdateSelectionAverage();
	}
	void Editor::ClearSelection()
	{
		for (auto Entity : m_selectedEntities)
			Entity->m_isSelected = false;
		m_selectedEntities.clear();

		m_selectionTransform_WorldPosition = Vector3::ZERO;
	}

	void Editor::UpdateSelectionAverage()
	{
		m_selectionTransform_WorldPosition = Vector3::ZERO;
		float m_selectedEntitiesCount = (float)m_selectedEntities.size();

		for (const auto& Entity : m_selectedEntities)
			m_selectionTransform_WorldPosition += Entity->m_transform.getWorldPosition() / m_selectedEntitiesCount;

	}

	void Editor::UpdateSelectionTransformModel()
	{
		UpdateSelectionAverage();

		// Only update model if there's an object selected
		if (m_selectedEntities.size() > 0)
		{
			m_averageSelectionDistanceFromEditorCamera = Vector3::Distance(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), m_selectionTransform_WorldPosition);
			float AxisScale = 1.0f;// m_averageSelectionDistanceFromEditorCamera / 3.0f;

			if (m_controlAxisLocal)
			{
				Matrix3x3 RotationMatrix = m_selectedEntities[0]->m_transform.getWorldRotation().GetMatrix3x3();
				m_selectionTransform_Model = Matrix4x4(RotationMatrix * Matrix3x3::GetScale(AxisScale), m_selectionTransform_WorldPosition);
				m_selectionTransform_Forward = m_selectedEntities[0]->m_transform.getForward();
				m_selectionTransform_Up = m_selectedEntities[0]->m_transform.getUp();
				m_selectionTransform_Right = m_selectedEntities[0]->m_transform.getRight();
			}
			else
			{
				m_selectionTransform_Model = Matrix4x4::GetTranslateScale(m_selectionTransform_WorldPosition, Vector3(AxisScale));
				m_selectionTransform_Forward = Vector3::FORWARD;
				m_selectionTransform_Up = Vector3::UP;
				m_selectionTransform_Right = Vector3::RIGHT;
			}
		}
	}
}