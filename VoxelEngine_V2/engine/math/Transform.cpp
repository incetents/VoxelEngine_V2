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
			// Make sure Euler Rotations stay in range of [-360, +360]
			m_euler_rotation.x = (m_euler_rotation.x > 360.0f) ? std::fmod(m_euler_rotation.x, 360.0f) : m_euler_rotation.x;
			m_euler_rotation.y = (m_euler_rotation.y > 360.0f) ? std::fmod(m_euler_rotation.y, 360.0f) : m_euler_rotation.y;
			m_euler_rotation.z = (m_euler_rotation.z > 360.0f) ? std::fmod(m_euler_rotation.z, 360.0f) : m_euler_rotation.z;

			m_euler_rotation.x = (m_euler_rotation.x < -360.0f) ? -std::fmod(-m_euler_rotation.x, 360.0f) : m_euler_rotation.x;
			m_euler_rotation.y = (m_euler_rotation.y < -360.0f) ? -std::fmod(-m_euler_rotation.y, 360.0f) : m_euler_rotation.y;
			m_euler_rotation.z = (m_euler_rotation.z < -360.0f) ? -std::fmod(-m_euler_rotation.z, 360.0f) : m_euler_rotation.z;

			// Acquire Rotation
			if(m_rotationOrder == EulerRotationOrder::ZYX)
				m_worldRotation = Quaternion::ToQuaternion_ZYX(ToRadians(m_euler_rotation.x), ToRadians(m_euler_rotation.y), ToRadians(m_euler_rotation.z));
			else
				m_worldRotation = Quaternion::ToQuaternion_YXZ(ToRadians(m_euler_rotation.x), ToRadians(m_euler_rotation.y), ToRadians(m_euler_rotation.z));

			// Base Model Matrix
			m_modelMatrix = Matrix4x4(m_worldRotation.GetMatrix3x3() * Matrix3x3::GetScale(m_scale), m_position);

			// Add Rotation / Model Matrix from parent
			Transform* parent = m_parent;
			if (parent != nullptr)
			{
				m_worldRotation = parent->getWorldRotation() * m_worldRotation;
				m_modelMatrix = parent->getModel() * m_modelMatrix;
			}

			// Calculate Normal Matrix
			m_normalMatrix = Matrix3x3(m_modelMatrix).Inverse();

			// Calculate Axis Directions
			Matrix3x3 m_rotation_matrix = m_worldRotation.GetMatrix3x3();
			m_right		= m_rotation_matrix.GetColumn(0).Normalize();
			m_up		= m_rotation_matrix.GetColumn(1).Normalize();
			m_forward	= m_rotation_matrix.GetColumn(2).Normalize();

			// Update World position
			m_worldPosition = Vector3(m_modelMatrix.GetColumn(3));

			// Update World Scale
			m_lossyScale.x = Vector3::Length(m_modelMatrix[0], m_modelMatrix[4], m_modelMatrix[8]);
			m_lossyScale.y = Vector3::Length(m_modelMatrix[1], m_modelMatrix[5], m_modelMatrix[9]);
			m_lossyScale.z = Vector3::Length(m_modelMatrix[2], m_modelMatrix[6], m_modelMatrix[10]);

			// Clean
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

	Transform& Transform::setWorldPosition(const Vector3& position)
	{
		// No parent = change local instead
		if (m_parent == nullptr)
		{
			setPosition(position);
			return *this;
		}

		// Change local position to zero
		m_position = Vector3::ZERO;
		// Make sure it updates model matrix correctly
		isDirty = true;

		// Apply inverse/transpose on model matrix to figure out correct local position
		m_position = getModel().Inverse() * position;

		SetDirty();
		return *this;
	}
	Transform& Transform::increaseWorldPosition(const Vector3& translate)
	{
		// No parent = increase local instead
		if (m_parent == nullptr)
		{
			increasePosition(translate);
			return *this;
		}

		setWorldPosition(getWorldPosition() + translate);

		return *this;
	}

	Transform& Transform::setForward(const Vector3& forward)
	{
		float lengthSqr = forward.LengthSqr();
		if (lengthSqr < FLT_EPSILON)
			return *this;

		Vector3 Nforward = forward.Normalize();
		Vector3 NforwardXZ = forward.NormalizeXZ();

		if (Nforward.CompareFuzzy(Vector3::UP))
		{
			m_euler_rotation.x = 90;
			m_euler_rotation.y = 0;
			m_euler_rotation.z = 0; // This should never change
			return *this;
		}
		else if (Nforward.CompareFuzzy(Vector3::DOWN))
		{
			m_euler_rotation.x = -90;
			m_euler_rotation.y = 0;
			m_euler_rotation.z = 0; // This should never change
			return *this;
		}

		float yaw = Vector3::GetAngleDegrees(Vector3::FORWARD, NforwardXZ);
		if (Nforward.x > 0)
			yaw = -yaw;
		float pitch = Vector3::GetAngleDegrees(Vector3::UP, Nforward);

		m_euler_rotation = Vector3(-pitch + 90.0f, yaw, 0);

		SetDirty();
		return *this;
	}

	Transform& Transform::setRotation(const Quaternion& quat)
	{
		float pitch, yaw, roll;
		VXL_ASSERT(m_rotationOrder == EulerRotationOrder::ZYX, "ToEuler is using incorrect Rotation Order");
		Quaternion::ToEuler_ZYX(quat, roll, pitch, yaw);

		m_euler_rotation = Vector3(ToDegrees(pitch), ToDegrees(yaw), ToDegrees(roll));

		SetDirty();
		return *this;
	}

	Transform& Transform::rotateAroundAxis(const Vector3& axis, float degrees)
	{
		// Make sure Rotation is up to date
		updateValues();

		// Rotation Quaternion too apply
		Quaternion Rotation(ToRadians(degrees), axis.Normalize());

		// Apply NewRotation on current rotation
		m_worldRotation = Rotation * m_worldRotation;
		if (m_parent != nullptr)
			// Apply inverse of parent to cancel out additional rotation
			m_worldRotation = m_parent->getWorldRotation().Inverse() * m_worldRotation;

		setRotation(m_worldRotation);
		return *this;
	}
}