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
		// Selected Entities
		std::vector<Entity*> m_selectedEntities;

		// Transform Selection
		struct SelectionTransform
		{
			Matrix4x4	Model;
			Vector3		WorldPosition;
			Vector3		Forward;
			Vector3		Up;
			Vector3		Right;
			float		CameraDistance;

		} m_SelectionTransform;
		void UpdateSelectionTransformPosition();

		// Axis Selection
		struct AxisSelectionTransform
		{
			Matrix4x4 X_Model;
			Matrix4x4 Y_Model;
			Matrix4x4 Z_Model;

		} m_AxisSelectionTransform;

	public:

		// Selected Entities
		inline const std::vector<Entity*>& GetSelectedEntities(void) const
		{
			return m_selectedEntities;
		}
		
		// [true] if at least one object is selected
		bool HasSelection() const
		{
			return m_selectedEntities.size() > 0;
		}
		void RemoveSelection(Entity* _entity);
		void AddSelection(Entity* _entity);
		void ClearSelection();

		// Transform Selection
		inline const SelectionTransform& GetSelectionTransform(void) const
		{
			return m_SelectionTransform;
		}
		inline const AxisSelectionTransform& GetAxisSelectionTransform(void) const
		{
			return m_AxisSelectionTransform;
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
		Axis m_controlPlane = Axis::NONE;
		bool m_controlAxisClicked = false;
		bool m_controlAxisLocal = true; // Whether Editor axis rotates based on local rotation

		// Update info
		void UpdateSelectionTransformModel();

		// Behaviour
		void Draw() {}


	} SingletonInstance(Editor);
}