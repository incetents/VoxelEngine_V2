// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector.h"
#include "../math/Matrix4x4.h"
#include "../math/MathCore.h"

#include <vector>

namespace Vxl
{
	class FramebufferObject;
	class Entity;

	class Gizmo
	{
	private:
		// Internal Data
		static FramebufferObject*	m_fbo;
		float						m_dragAmount = 0.0f;
		Vector3						m_targetNormal;
		Vector3						m_dragStart;
		Vector3						m_dragEnd;

		// Utility
		Vector3 CameraRayHitPlane(const Vector3& normal);
		Vector3 GetGizmoDirection(Axis axis);

	public:
		// Global GL Resources
		static void InitGLResources();
		static void DestroyGLResources();

		enum class Mode
		{
			TRANSLATE,
			SCALE,
			ROTATE
		} m_mode = Mode::TRANSLATE;

		struct Transform
		{
			Matrix4x4	Model;
			Vector3		WorldPosition;
			Vector3		Forward;
			Vector3		Up;
			Vector3		Right;

		} m_transform;

		// Visual Displacements
		Vector3 X_Offset;
		Vector3 Y_Offset;
		Vector3 Z_Offset;

		// Selection Info
		Axis  m_selectedAxis = Axis::NONE;
		Axis  m_selectedPlane = Axis::NONE;
		bool  m_selected = false;
		bool  m_clicked = false;
		bool  m_pivotAxisAligned = false; // Force Gizmo to always be aligned to axis [Doesn't work for scale]
	
		// Update
		void UpdateModel(Entity& _entity);
		void Update();

		// Render
		void RenderOnScreen(); // Display it Visually
		void RenderIDCapture(); // Render it in FBO to catch which gizmo part is selected
	};
}