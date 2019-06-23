// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../math/Vector3.h"
#include "../math/MathCore.h"

#include <vector>

namespace Vxl
{
	class Entity;

	static class Editor : public Singleton<class Editor>
	{
	private:
		// Data
		std::vector<Entity*> m_selectedEntities;
		float	m_averageSelectionDistanceFromEditorCamera;

		// For Translate/Rotation/Scale debug object
		Matrix4x4	m_selectionTransform_Model;
		Vector3		m_selectionTransform_WorldPosition;
		Vector3		m_selectionTransform_Forward;
		Vector3		m_selectionTransform_Up;
		Vector3		m_selectionTransform_Right;

		// Update info
		void UpdateSelectionAverage();

	public:

		/* Selection */
		const std::vector<Entity*>& GetSelectedEntities(void) const
		{
			return m_selectedEntities;
		}
		
		float GetAverageSelectionDistanceFromEditorCamera() const
		{
			return m_averageSelectionDistanceFromEditorCamera;
		}
		// [true] if at least one object is selected
		bool HasSelection() const
		{
			return m_selectedEntities.size() > 0;
		}
		void RemoveSelection(Entity* _entity);
		void AddSelection(Entity* _entity);
		void ClearSelection();

		inline const Matrix4x4& GetSelectionTransformModel(void) const
		{
			return m_selectionTransform_Model;
		}
		inline const Vector3& GetSelectionTransformWorldPosition() const
		{
			return m_selectionTransform_WorldPosition;
		}
		inline const Vector3& GetSelectionTransformForward(void) const
		{
			return m_selectionTransform_Forward;
		}
		inline const Vector3& GetSelectionTransformUp(void) const
		{
			return m_selectionTransform_Up;
		}
		inline const Vector3& GetSelectionTransformRight(void) const
		{
			return m_selectionTransform_Right;
		}


		/* Editor Controls */
		enum ControlMode
		{
			TRANSLATE,
			SCALE,
			ROTATE
		};
		ControlMode m_controlMode = ControlMode::TRANSLATE;
		Axis m_controlAxis = Axis::NONE;
		bool m_controlAxisClicked = false;
		bool m_controlAxisLocal = true; // Whether Editor axis rotates based on local rotation

		// Update info
		void UpdateSelectionTransformModel();

		// Behaviour
		void Draw() {}


	} SingletonInstance(Editor);
}