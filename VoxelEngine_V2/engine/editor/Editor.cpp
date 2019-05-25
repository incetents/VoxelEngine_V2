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
		assert(_entity);
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

		m_averageSelectionLocation = Vector3::ZERO;
	}

	void Editor::UpdateSelectionAverage()
	{
		m_averageSelectionLocation = Vector3::ZERO;
		float m_selectedEntitiesCount = (float)m_selectedEntities.size();

		for (const auto& Entity : m_selectedEntities)
			m_averageSelectionLocation += Entity->m_transform.getWorldPosition() / m_selectedEntitiesCount;

	}

	void Editor::UpdateSelectionTransformModel()
	{
		UpdateSelectionAverage();

		// Only update model if there's an object selected
		if (m_selectedEntities.size() > 0)
		{
			float DistanceToEditorCamera = Vector3::Distance(RenderManager.GetMainCamera()->m_transform.getWorldPosition(), m_averageSelectionLocation);

			if (m_controlAxisLocal)
			{
				Matrix3x3 RotationMatrix = m_selectedEntities[0]->m_transform.getWorldRotation().GetMatrix3x3();
				m_selectionTransformModel = Matrix4x4(RotationMatrix.Transpose() * Matrix3x3::GetScale(DistanceToEditorCamera / 4.0f), m_averageSelectionLocation);
			}
			else
			{
				m_selectionTransformModel = Matrix4x4::GetTranslateScale(m_averageSelectionLocation, Vector3(DistanceToEditorCamera / 4.0f));
			}
		}
	}
}