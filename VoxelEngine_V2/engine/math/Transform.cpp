// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "Transform.h"
#include "../math/Matrix3x3.h"
#include "../Math/Matrix4x4.h"
#include "../math/MathCore.h"

#include "../modules/Entity.h"

namespace Vxl
{
	void Transform::updateValues()
	{
		// If dirty is true, update values
		if (isDirty)
		{
			// Acquire Rotation
			m_localrotation = Quaternion::ToQuaternion_YXZ(Degrees(m_euler_rotation.x), Degrees(m_euler_rotation.y), Degrees(m_euler_rotation.z));
			m_worldrotation = m_localrotation;
			Matrix3x3 RotationMatrix = m_localrotation.GetMatrix3x3();
			
			// Base Model Matrix
			m_ModelMatrix = Matrix4x4(RotationMatrix.Transpose() * Matrix3x3::GetScale(m_scale), m_position);
			
			// Add Rotation / Model Matrix from parent
			Transform* parent = m_parent;
			while (parent != nullptr)
			{
				m_worldrotation = m_worldrotation * parent->getWorldRotation();
				m_ModelMatrix = m_ModelMatrix * parent->getModel();
				parent = parent->getParent();
			}

			// Calculate Axis Directions
			Matrix3x3 FinalRotationMatrix = m_worldrotation.GetMatrix3x3();
			m_right		= FinalRotationMatrix.GetColumn(0);
			m_up		= FinalRotationMatrix.GetColumn(1);
			m_forward	= FinalRotationMatrix.GetColumn(2);

			// Update World position
			m_worldPosition = m_ModelMatrix.GetRow(3);
			// Update World Scale
			m_lossyScale.x = Vector3::Length(m_ModelMatrix[0], m_ModelMatrix[1], m_ModelMatrix[2]);
			m_lossyScale.y = Vector3::Length(m_ModelMatrix[4], m_ModelMatrix[5], m_ModelMatrix[6]);
			m_lossyScale.z = Vector3::Length(m_ModelMatrix[8], m_ModelMatrix[9], m_ModelMatrix[10]);


			isDirty = false;

			// Call entity about change in transform class
			if (m_owner != nullptr)
				m_owner->TransformChanged();
		}
	}

	Transform::Transform()
	{
		m_position = Vector3(0, 0, 0);
		m_euler_rotation = Vector3(0, 0, 0);
		m_scale = Vector3(1, 1, 1);
	}
	Transform::Transform(const Vector3& position, const Vector3& euler_rotation, const Vector3& scale)
	{
		m_position = position;
		m_euler_rotation = euler_rotation;
		m_scale = scale;
	}
	Transform::~Transform()
	{
		// Make all children become orphans
		for (auto child : m_children)
		{
			child->simpleRemoveParent();
			child->m_owner->TransformChanged();
			// Check for new parent for current children/soon-to-be orphans
			if (m_parent)
			{
				// Preserve World Position
				Vector3 WorldPosition = child->getWorldPosition();
				// Current Parent = Grandparent
				child->setParent(m_parent);
				// World Position fix
				child->setWorldPosition(WorldPosition);
			}
		}
		// If it has a parent, break connection
		if (m_parent)
		{
			m_parent->SimpleRemoveChild(this);
			m_parent->m_owner->TransformChanged();
		}
	}
}