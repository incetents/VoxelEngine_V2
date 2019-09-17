// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../math/Vector.h"
#include "../math/Matrix4x4.h"
#include "../math/MathCore.h"

#include <vector>

namespace Vxl
{
	class FramebufferObject;
	class Entity;

	static class Editor : public Singleton<class Editor>
	{
	private:
		std::vector<Entity*>	m_selectedEntities;
		FramebufferObject*		m_fbo_colorPicker;

		Vector4 m_ScreenSpace_SelectionCenter;
		Vector4 m_ScreenSpace_Selection1;
		Vector4 m_ScreenSpace_Selection2;

		Vector2 m_Axis1_Direction;
		Vector2 m_Axis2_Direction;
		bool	m_useAxis1 = false;
		bool	m_useAxis2 = false;
		float	m_totalDrag1 = 0.0f;
		float	m_totalDrag2 = 0.0f;

		std::vector<float> m_scaleBackups;

	public:

		float m_GizmoDragSpeed;
		Axis  m_GizmoSelectedAxis = Axis::NONE;
		Axis  m_GizmoSelectedPlane = Axis::NONE;
		bool  m_GizmoSelected = false;
		bool  m_GizmoClicked = false;

		// Transform Selection
		struct GizmoTransform
		{
			// Multi-purpose offsets
			Matrix4x4	X_Model;
			Matrix4x4	Y_Model;
			Matrix4x4	Z_Model;

			float		CameraDistance;
			Matrix4x4	Model;
			Vector3		WorldPosition;
			Vector3		Forward;
			Vector3		Up;
			Vector3		Right;

		} m_GizmoTransform;

		// Global GL Resources
		void InitGLResources();
		void DestroyGLResources();

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

		/* Editor Controls */
		enum GizmoMode
		{
			TRANSLATE,
			SCALE,
			ROTATE
		};
		GizmoMode m_controlMode = GizmoMode::TRANSLATE;
		
		bool m_controlAxisLocal = true; // Whether Editor axis rotates based on local rotation

		// Update info
		void UpdateSelectionTransformPosition();
		void UpdateSelectionTransformModel();

		// Behaviour
		void UpdateSelectionInfo();


	} SingletonInstance(Editor);
}