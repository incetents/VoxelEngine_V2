// Copyright (c) 2020 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector.h"
#include "../math/Matrix4x4.h"
#include "../math/MathCore.h"
#include "../utilities/Types.h"

#include <vector>

namespace Vxl
{
	class FramebufferObject;
	class SceneNode;
	class Mesh;

	class Gizmo
	{
		friend class RenderManager;
	private:
		// Global GL Resources
		static void InitGLResources();
		static void DestroyGLResources();

		// Internal Data
		static FramebufferObjectIndex	m_fbo;
		bool							m_show = false;
		float							m_dragPrev = 0.0f;
		float							m_dragAmount = 0.0f;
		Vector3							m_targetNormal;
		Vector3							m_dragStart;
		Vector3							m_dragEnd;
		bool							m_selected = false;
		bool							m_clicked = false;
		bool							m_camAxisSide[3] = { false, false, false };

		// Custom Meshes
		static MeshIndex m_mesh_HalfQuadAxisX[4];
		static MeshIndex m_mesh_HalfQuadAxisY[4];
		static MeshIndex m_mesh_HalfQuadAxisZ[4];
		static MeshIndex m_mesh_ScaleCube[3];// x,y,z

		// Scale By Distance
		float m_distanceToCamera;
		float m_constantScale;
		Matrix4x4 m_constantScaleMatrix;

		// Axis specific information
		std::vector<Vector3>	m_worldPositionStorage;
		std::vector<float>		m_worldScaleStorage;
		Vector3					m_rotationDragStart;
		bool					m_showScaleGizmo[3];

	public:
		// Utility
		Vector3 CameraRayHitPlane(const Vector3& normal);
		Vector3 GetGizmoDirection(Axis axis);

		

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
		
		bool  m_pivotAxisAligned = false; // Force Gizmo to always be aligned to axis [Doesn't work for scale]
		bool  m_translateSnapping = false;
		float m_translateSnapAmount = 1.0f;
		bool  m_rotateSnapping = false;
		float m_rotateSnappingAmount = 15.0f;

		// Utility
		inline bool IsSelected(void) const
		{
			return m_selected;
		}
		inline bool IsClicked(void) const
		{
			return m_clicked;
		}

		// Update
		void Update(const std::vector<SceneNode*> _nodes);

		// Render
		void RenderOnScreen(MaterialIndex _gizmoMaterial); // Display it Visually
		void RenderIDCapture(MaterialIndex _colorPickerMaterial); // Render it in FBO to catch which gizmo part is selected
	};
}