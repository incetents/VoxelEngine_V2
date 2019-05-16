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
			// Update model matrix
			m_rotation = Quaternion::ToQuaternion_ZXY(Degrees(m_euler_rotation.x), Degrees(m_euler_rotation.y), Degrees(m_euler_rotation.z));
			Matrix3x3 RotationScale = m_rotation.GetMatrix3x3() * Matrix3x3::GetScale(m_scale);
			// Result Model
			m_ModelMatrix = Matrix4x4(RotationScale, m_position);

			// Model affected by Parent
			Quaternion ParentRotation = m_rotation;
			Matrix4x4 ParentModel = m_ModelMatrix;
			if (m_parent != nullptr)
			{
				ParentRotation = m_rotation * m_parent->getWorldRotation();
				ParentModel = m_ModelMatrix * m_parent->getModel();
			}

			// Update World position
			m_worldPosition = Vector3(ParentModel[12], ParentModel[13], ParentModel[14]);
			// Update World Scale
			m_lossyScale.x = Vector3::Length(ParentModel[0], ParentModel[1], ParentModel[2]);
			m_lossyScale.y = Vector3::Length(ParentModel[4], ParentModel[5], ParentModel[6]);
			m_lossyScale.z = Vector3::Length(ParentModel[8], ParentModel[9], ParentModel[10]);

			// Calculate Axis Directions
			m_forward	= Vector3::Normalize(ParentRotation * Vector3::FORWARD);
			m_right		= Vector3::Normalize(ParentRotation * Vector3::RIGHT); // Cross of forward and up is not accurate when Roll is taken into account
			m_up		= Vector3::Cross(m_forward, m_right);
			//
			isDirty = false;

			// Call entity about change in transform class
			if (m_owner != nullptr)
				m_owner->TransformChanged();
		}
	}

	Matrix4x4 Transform::getModel()
	{
		updateValues();

		// Affected by parent transformations
		if (m_parent != nullptr)
			return m_ModelMatrix * m_parent->getModel();

		// Orphan
		return m_ModelMatrix;
	}

	inline Quaternion Transform::getWorldRotation(void)
	{
		updateValues();

		// Affected by parent transformations
		if (m_parent != nullptr)
			return m_rotation * m_parent->getWorldRotation();

		// Orphan
		return m_rotation;
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