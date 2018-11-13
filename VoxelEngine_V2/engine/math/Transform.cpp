// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "Transform.h"
#include "../math/Matrix3x3.h"
#include "../Math/Matrix4x4.h"

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

			m_ModelMatrix = Matrix4x4(RotationScale, m_position);

			// Update directions
			m_forward = m_rotation * Vector3::FORWARD;
			m_right = m_rotation * Vector3::RIGHT; // (Vector3::UP.Cross(m_forward)).Normalize();
			m_up = m_forward.Cross(m_right);

			// Set Flag
			isDirty = false;
		}
	}

	const Matrix4x4 Transform::getModel()
	{
		updateValues();

		// Affected by parent
		if (m_parent != nullptr)
		{
			Matrix4x4 Parent = m_parent->getModel();
			Matrix4x4 Self = m_ModelMatrix;
			Matrix4x4 Final = Parent * Self;

			return m_parent->getModel() * m_ModelMatrix;
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

	std::vector<Vector3> Transform::getDirections(const Matrix4x4& model)
	{
		Vector3 P = model * Vec3::ZERO;
		Vector3 P_F = model * (Vec3::FORWARD);
		Vector3 P_U = model * (Vec3::UP);
		Vector3 P_R = model * (Vec3::RIGHT);

		Vector3 F = (P_F - P).Normalize();
		Vector3 U = (P_U - P).Normalize();
		Vector3 R = (P_R - P).Normalize();
		//
		Vector3 v[] = { P, F, U, R };
		std::vector<Vector3> vectors(v, v + sizeof(v) / sizeof(Vector3));
		return vectors;
	}
}