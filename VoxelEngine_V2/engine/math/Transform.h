// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../modules/Component.h"

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Rotation.h"

#include <assert.h>
#include <vector>

namespace Vxl
{
	class Transform : public Component
	{
	protected:
		// Model for transformations
		Matrix4x4	m_ModelMatrix;

		// Raw Values
		Vector3		m_world;	// World Position
		Vector3		m_position; // Local Position
		Vector3		m_euler_rotation;
		Quaternion	m_rotation;
		Vector3		m_scale;

		// Direction Space
		Vector3		m_forward	= Vector3::FORWARD;
		Vector3     m_up		= Vector3::UP;
		Vector3     m_right		= Vector3::RIGHT;

		// Parent/Child relationships
		Transform*				m_parent = nullptr;
		std::vector<Transform*> m_children;
		u_int					m_totalChildren = 0;

		// Parent / Child Functionality
		void simpleRemoveParent()
		{
			m_parent = nullptr;
		}
		void SimpleRemoveChild(int index)
		{
			m_children.erase(m_children.begin() + index);
			m_totalChildren--;
		}
		void SimpleRemoveChild(Transform* child)
		{
			int child_index = getChildIndex(child);
			if (child_index != -1)
			{
				SimpleRemoveChild(child_index);
			}
		}
		void SimpleAddChild(Transform* child)
		{
			m_children.push_back(child);
			m_totalChildren++;
		}
		void InheritChildren()
		{
			// Parent takes this node's children, this node then becomes an orphan
			if (m_parent == nullptr)
				return;

			for (u_int i = 0; i < m_totalChildren; i++)
			{
				m_parent->SimpleAddChild(m_children[i]);
			}
			m_children.clear();
			m_totalChildren = 0;
			m_parent = nullptr;
		}

		// Update Flag
		bool isDirty = true;
		void updateValues();

	public:
		Transform(void);
		Transform(const Vector3& position, const Vector3& euler_rotation = Vector3(0,0,0), const Vector3& scale = Vector3(1,1,1));

		// Dirty Setter
		void SetDirty()
		{
			isDirty = true;
			for (auto child : m_children)
				child->SetDirty();
		}

		// Returns index of child in list of children (-1 = child does not exist)
		bool checkChildDuplicate(Transform* c)
		{
			for (u_int i = 0; i < m_totalChildren; i++)
			{
				if (c == m_children[i])
					return true;
			}
			return false;
		}
		bool checkParentExists(void)
		{
			return m_parent != nullptr;
		}

		// Get Children Data
		u_int					getChildCount(void)
		{
			return m_totalChildren;
		}
		std::vector<Transform*> getChildren(void)
		{
			return m_children;
		}
		Transform*				getChild(u_int index)
		{
			assert(index >= 0 && index < m_totalChildren);

			return m_children[index];
		}
		int						getChildIndex(Transform* c)
		{
			for (u_int i = 0; i < m_totalChildren; i++)
			{
				if (c == m_children[i])
					return i;
			}
			// Nothing found
			return -1;
		}
		// Get Parent
		Transform*				getParent()
		{
			return m_parent;
		}

		// Parent / Child - Changes
		void setParent(Transform* parent)
		{
			// cannot parent self
			if (parent == this)
				return;

			// Check if new parent is found somewhere in the chain upwards
			Transform* current = parent;
			while (current != nullptr)
			{
				if (current->m_parent == this)
				{
					// New parent must have his parent disconnect from it
					parent->m_parent->SimpleRemoveChild(parent);
					// New parent must disconnect from his parent
					parent->simpleRemoveParent();
					break;
				}
				else
				{
					current = current->m_parent;
				}
			}

			// Make sure previous parent removes this
			if (m_parent != nullptr)
				m_parent->removeChild(this);

			// Set Parent
			m_parent = parent;

			// Make sure parent has child
			m_parent->addChild(this);

			// Flag
			SetDirty();
		}
		void addChild(Transform* child)
		{
			// cannot child self
			if (child == this)
				return;

			// Ignore if child is already present
			if (checkChildDuplicate(child))
				return;

			// Check if new child is found somewhere in the chain upwards
			Transform* current = this;
			while (current != nullptr)
			{
				if (current->m_parent == child)
				{
					// Chain Link discovered, disconnect from current
					current->m_parent->SimpleRemoveChild(current);
					// Current removes parent
					current->simpleRemoveParent();
					break;
				}
				else
				{
					current = current->m_parent;
				}
			}

			// Child removes its current parent
			child->removeParent();

			// Child now treats this class as its parent
			child->m_parent = this;

			// Add child
			SimpleAddChild(child);

			// Flag
			child->SetDirty();
		}
		void removeParent(void)
		{
			// Ignore if no parent present
			if (m_parent == nullptr)
				return;

			// Parent removes this child
			m_parent->SimpleRemoveChild(this);

			// Child removes parent
			m_parent = nullptr;

			// Flag
			SetDirty();
		}
		void removeChild(Transform* child)
		{
			int child_index = getChildIndex(child);
			if (child_index == -1)
				return;

			// Child removes parent
			m_children[child_index]->simpleRemoveParent();

			// This loses child
			SimpleRemoveChild(child_index);

			// Flag
			child->SetDirty();
		}

		// Setters
		inline Transform& setWorldPosition(float x, float y, float z)
		{
			Vector3 Move = Vector3(x, y, z) - getWorldPosition();
			m_position += Move;
			SetDirty();
			return *this;
		}
		inline Transform& setWorldPosition(const Vector2& position)
		{
			Vector3 Move = Vector3(position) - getWorldPosition();
			m_position += Move;
			SetDirty();
			return *this;
		}
		inline Transform& setWorldPosition(const Vector3& position)
		{
			Vector3 Move = position - getWorldPosition();
			m_position += Move;
			SetDirty();
			return *this;
		}
		inline Transform& setWorldPosition(const Vector4& position)
		{
			Vector3 Move = Vector3(position) - getWorldPosition();
			m_position += Move;
			SetDirty();
			return *this;
		}
		inline Transform& setPosition(float x, float y, float z)
		{
			m_position = Vector3(x, y, z);
			SetDirty();
			return *this;
		}
		inline Transform& setPosition(const Vector2& position)
		{
			m_position = position;
			SetDirty();
			return *this;
		}
		inline Transform& setPosition(const Vector3& position)
		{
			m_position = position;
			SetDirty();
			return *this;
		}
		inline Transform& setPosition(const Vector4& position)
		{
			m_position = position;
			SetDirty();
			return *this;
		}
		inline Transform& setRotation(float x, float y, float z)
		{
			m_euler_rotation = Vector3(x, y, z);
			SetDirty();
			return *this;
		}
		inline Transform& setRotation(const Vector3& euler_rotation)
		{
			m_euler_rotation = euler_rotation;
			SetDirty();
			return *this;
		}
		inline Transform& setScale(float scaleAll)
		{
			m_scale = Vector3(scaleAll);
			SetDirty();
			return *this;
		}
		inline Transform& setScale(float x, float y, float z)
		{
			m_scale = Vector3(x, y, z);
			SetDirty();
			return *this;
		}
		inline Transform& setScale(const Vector3 scale)
		{
			m_scale = scale;
			SetDirty();
			return *this;
		}

		// Specific Setters
		inline Transform& setPositionX(float x)
		{
			m_position.x = x;
			SetDirty();
			return *this;
		}
		inline Transform& setPositionY(float y)
		{
			m_position.y = y;
			SetDirty();
			return *this;
		}
		inline Transform& setPositionZ(float z)
		{
			m_position.z = z;
			SetDirty();
			return *this;
		}

		inline Transform& setRotationX(float x)
		{
			m_euler_rotation.x = x;
			SetDirty();
			return *this;
		}
		inline Transform& setRotationY(float y)
		{
			m_euler_rotation.y = y;
			SetDirty();
			return *this;
		}
		inline Transform& setRotationZ(float z)
		{
			m_euler_rotation.z = z;
			SetDirty();
			return *this;
		}

		inline Transform& setScaleX(float x)
		{
			m_scale.x = x;
			SetDirty();
			return *this;
		}
		inline Transform& setScaleY(float y)
		{
			m_scale.y = y;
			SetDirty();
			return *this;
		}
		inline Transform& setScaleZ(float z)
		{
			m_scale.z = z;
			SetDirty();
			return *this;
		}

		// Special
		virtual Transform& setForward(const Vector3& forward)
		{
			Vector3 Nforward = forward.Normalize();

			if (Nforward == Vector3::UP)
			{
				m_euler_rotation.x = 90;
				m_euler_rotation.y = 0;
				m_euler_rotation.z = 0; // This should never change
				return *this;
			}
			else if (Nforward == Vector3::DOWN)
			{
				m_euler_rotation.x = -90;
				m_euler_rotation.y = 0;
				m_euler_rotation.z = 0; // This should never change
				return *this;
			}

			//Quaternion::ToEuler(Quaternion(forward.x, forward.y, forward.z, 0), roll, pitch, yaw);
			Degrees yaw = Vector3::GetAngleDegrees(Vector3::FORWARD, Nforward.NormalizeXZ());
			if (Nforward.z < 0)
				yaw = -yaw;
			Degrees pitch = Vector3::GetAngleDegrees(Vector3::UP, Nforward);

			m_euler_rotation = Vector3(pitch.Get() - 90.0f, yaw.Get(), 0);

			SetDirty();

			auto test = Transform::getForward();

			return *this;
		}

		// Increasers
		inline Transform& increasePosition(float x, float y, float z)
		{
			m_position += Vector3(x, y, z);
			SetDirty();
			return *this;
		}
		inline Transform& increasePosition(const Vector2& translate)
		{
			m_position += Vector3(translate);
			SetDirty();
			return *this;
		}
		inline Transform& increasePosition(const Vector3& translate)
		{
			m_position += translate;
			SetDirty();
			return *this;
		}
		inline Transform& increasePosition(const Vector4& translate)
		{
			m_position += translate;
			SetDirty();
			return *this;
		}

		inline Transform& increaseRotation(float x, float y, float z)
		{
			m_euler_rotation += Vector3(x, y, z);
			SetDirty();
			return *this;
		}
		inline Transform& increaseRotation(const Vector3& euler_increase)
		{
			m_euler_rotation += euler_increase;
			SetDirty();
			return *this;
		}

		inline Transform& increaseScale(float x, float y, float z)
		{
			m_scale += Vector3(x, y, z);
			SetDirty();
			return *this;
		}
		inline Transform& increaseScale(const Vector3& scaler)
		{
			m_scale += scaler;
			SetDirty();
			return *this;
		}
		inline Transform& increaseScale(float scaleAll)
		{
			m_scale += scaleAll;
			SetDirty();
			return *this;
		}

		// Specific Increasers
		inline Transform& increasePositionX(float x)
		{
			m_position.x += x;
			SetDirty();
			return *this;
		}
		inline Transform& increasePositionY(float y)
		{
			m_position.y += y;
			SetDirty();
			return *this;
		}
		inline Transform& increasePositionZ(float z)
		{
			m_position.z += z;
			SetDirty();
			return *this;
		}

		inline Transform& increaseRotationX(float x)
		{
			m_euler_rotation.x += x;
			SetDirty();
			return *this;
		}
		inline Transform& increaseRotationY(float y)
		{
			m_euler_rotation.y += y;
			SetDirty();
			return *this;
		}
		inline Transform& increaseRotationZ(float z)
		{
			m_euler_rotation.z += z;
			SetDirty();
			return *this;
		}

		inline Transform& increaseScaleX(float x)
		{
			m_scale.x += x;
			SetDirty();
			return *this;
		}
		inline Transform& increaseScaleY(float y)
		{
			m_scale.y += y;
			SetDirty();
			return *this;
		}
		inline Transform& increaseScaleZ(float z)
		{
			m_scale.z += z;
			SetDirty();
			return *this;
		}

		// Alt name
		inline Transform& translate(const Vector3& translate)
		{
			return increasePosition(translate);
		}
		inline Transform& rotate(const Vector3& euler_increase)
		{
			return increaseRotation(euler_increase);
		}

		// Turn object into a 4x4 matrix for math
		Matrix4x4 getModel(void);

		// Getters
		inline Vector3		getWorldPosition(void)
		{
			updateValues();
			return m_world;
		}
		inline Vector3		getPosition(void) const
		{
			return m_position;
		}
		inline Vector3		getRotationEuler(void) const
		{
			return m_euler_rotation;
		}
		inline Vector3		getScale(void) const
		{
			return m_scale;
		}
		inline Quaternion	getRotation(void)
		{
			updateValues();
			return m_rotation;
		}
		virtual Vector3		getForward(void)
		{
			updateValues();
			return m_forward;
		}
		virtual Vector3		getBackwards(void)
		{
			updateValues();
			return -m_forward;
		}
		virtual Vector3		getUp(void)
		{
			updateValues();
			return m_up;
		}
		virtual Vector3		getDown(void)
		{
			updateValues();
			return -m_up;
		}
		virtual Vector3		getLeft(void)
		{
			updateValues();
			return -m_right;
		}
		virtual Vector3		getRight(void)
		{
			updateValues();
			return m_right;
		}

	};
}