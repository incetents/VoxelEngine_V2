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
			m_rotation = Quaternion::GetEuler(Degrees(m_euler_rotation.x), Degrees(m_euler_rotation.y), Degrees(m_euler_rotation.z));
			Matrix3x3 RotationScale = m_rotation.GetMatrix3x3() * Matrix3x3::GetScale(m_scale);
			// Result Model
			m_ModelMatrix = Matrix4x4(RotationScale, m_position);

			// Model affected by Parent
			Matrix4x4 ParentModel = m_ModelMatrix;
			if (m_parent != nullptr)
				ParentModel = m_ModelMatrix * m_parent->getModel();

			// Update World position
			m_worldPosition = Vector3(ParentModel[12], ParentModel[13], ParentModel[14]);
			// Update World Scale
			m_lossyScale.x = Vector3::Length(ParentModel[0], ParentModel[1], ParentModel[2]);
			m_lossyScale.y = Vector3::Length(ParentModel[4], ParentModel[5], ParentModel[6]);
			m_lossyScale.z = Vector3::Length(ParentModel[8], ParentModel[9], ParentModel[10]);

			// Update directions
			m_forward = Vector3::Normalize(m_rotation * Vector3::FORWARD);

			// If forward is pointing directly up, calculate right the expensive way
			if (fabs((m_forward - Vector3::UP).Length()) < 0.005f)
				m_right = Vector3::Normalize(m_rotation * Vector3::RIGHT);
			// If not, use cheaper method of calculating right
			else
				m_right = -Vector3::Normalize(Vector3::Cross(m_forward, Vector3::UP));

			// Calculate Up
			m_up = Vector3::Cross(m_forward, m_right);

			// Set Flag
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
		{
			return m_ModelMatrix * m_parent->getModel();
		}

		// Orphan
		return m_ModelMatrix;
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
		}
		// If it has a parent, break connection
		if (m_parent)
		{
			m_parent->SimpleRemoveChild(this);
			m_parent->m_owner->TransformChanged();
		}
	}
}