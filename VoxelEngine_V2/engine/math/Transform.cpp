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
			m_local_ModelMatrix = Matrix4x4(RotationMatrix * Matrix3x3::GetScale(m_scale), m_position);
			m_world_ModelMatrix = m_local_ModelMatrix;

			// Add Rotation / Model Matrix from parent
			Transform* parent = m_parent;
			while (parent != nullptr)
			{
				//m_worldrotation *= parent->getLocalRotation();
				m_worldrotation = parent->getLocalRotation() * m_worldrotation;
				m_world_ModelMatrix = parent->getLocalModel() * m_world_ModelMatrix;
				parent = parent->getParent();
			}

			// Calculate Axis Directions
			Matrix3x3 FinalRotationMatrix = m_worldrotation.GetMatrix3x3();
			m_right		= FinalRotationMatrix.GetColumn(0).Normalize();
			m_up		= FinalRotationMatrix.GetColumn(1).Normalize();
			m_forward	= FinalRotationMatrix.GetColumn(2).Normalize();

			// Update World position
			m_worldPosition = Vector3(m_world_ModelMatrix.GetColumn(3));
			// Update World Scale
			m_lossyScale.x = Vector3::Length(m_world_ModelMatrix[0], m_world_ModelMatrix[1], m_world_ModelMatrix[2]);
			m_lossyScale.y = Vector3::Length(m_world_ModelMatrix[4], m_world_ModelMatrix[5], m_world_ModelMatrix[6]);
			m_lossyScale.z = Vector3::Length(m_world_ModelMatrix[8], m_world_ModelMatrix[9], m_world_ModelMatrix[10]);

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

		// Make sure parent matrices are correct
		updateValues();

		// Get parent matrix
		Matrix4x4 Model = m_parent->getLocalModel();
		Transform* parent = m_parent->getParent();

		// Additional parents need to stack on current parent matrix
		while (parent != nullptr)
		{
			Model = parent->getLocalModel() * Model;
			parent = parent->getParent();
		}

		// Apply inverse/transpose on model matrix to figure out correct local position
		m_position = Model.Inverse() * position;

		SetDirty();
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
		Degrees pitch, yaw, roll;
		Quaternion::ToEuler(quat, roll, pitch, yaw);

		m_euler_rotation = Vector3(pitch.Get(), yaw.Get(), roll.Get());

		SetDirty();
		return *this;
	}
}