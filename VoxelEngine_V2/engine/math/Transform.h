// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../modules/Component.h"

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Rotation.h"

#include <assert.h>
#include <vector>

#include "../utilities/Macros.h"

namespace Vxl
{
	class Transform : public Component
	{
		friend class CameraObject;
	protected:
		// Model for transformations
		Matrix4x4	m_local_ModelMatrix; // Row Major
		Matrix4x4	m_world_ModelMatrix; // Row Major

		// Raw Values
		Vector3		m_worldPosition;	// World Position
		Vector3		m_position; // Local Position
		Vector3		m_euler_rotation; // Local Euler
		Quaternion	m_localrotation; // Local Rotation
		Quaternion	m_worldrotation; // World Rotation
		Vector3		m_scale;		// Local Scale
		Vector3		m_lossyScale;   // World Scale (Not supper accurate)

		// Direction Space [ World Direction ]
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
		~Transform();

		// Dirty Setter
		void SetDirty()
		{
			isDirty = true;
			for (Transform* child : m_children)
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
			VXL_ASSERT(index < m_totalChildren, "Transform index out of range");
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
			if(m_parent)
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
			return setWorldPosition(Vector3(x, y, z));
		}
		inline Transform& setWorldPosition(const Vector2& position)
		{
			return setWorldPosition(Vector3(position));
		}
		Transform& setWorldPosition(const Vector3& position);
		inline Transform& setWorldPosition(const Vector4& position)
		{
			return setWorldPosition(Vector3(position));
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
			m_position = Vector3(position);
			SetDirty();
			return *this;
		}
		inline Transform& setRotation(float euler_x, float euler_y, float euler_z)
		{
			m_euler_rotation = Vector3(euler_x, euler_y, euler_z);
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
		inline Transform& setCameraForward(const Vector3& forward)
		{
			return setForward(-forward);
		}
		Transform& setForward(const Vector3& forward);

		Transform& setRotation(const Quaternion& quat);

		// Increasers
		inline Transform& increaseWorldPosition(float x, float y, float z)
		{
			setWorldPosition(getWorldPosition() + Vector3(x, y, z));
			return *this;
		}
		inline Transform& increaseWorldPosition(const Vector2& translate)
		{
			setWorldPosition(getWorldPosition() + Vector3(translate));
			return *this;
		}
		inline Transform& increaseWorldPosition(const Vector3& translate)
		{
			setWorldPosition(getWorldPosition() + translate);
			return *this;
		}
		inline Transform& increaseWorldPosition(const Vector4& translate)
		{
			setWorldPosition(getWorldPosition() + Vector3(translate));
			return *this;
		}

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
			m_position += Vector3(translate);
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
		inline const Matrix4x4& getLocalModel(void)
		{
			updateValues();
			return m_local_ModelMatrix;
		}
		inline const Matrix4x4& getWorldModel(void)
		{
			updateValues();
			return m_world_ModelMatrix;
		}

		// Getters
		inline Vector3		getWorldPosition(void)
		{
			updateValues();
			return m_worldPosition;
		}
		inline Vector3		getPosition(void) const
		{
			return m_position;
		}
		inline Vector3		getRotationEuler(void) const
		{
			return m_euler_rotation;
		}
		inline Vector3		getWorldScale(void)
		{
			updateValues();
			return m_lossyScale;
		}
		inline Vector3		getScale(void) const
		{
			return m_scale;
		}
		inline Quaternion	getLocalRotation(void)
		{
			updateValues();
			return m_localrotation;
		}
		inline Quaternion	getWorldRotation(void)
		{
			updateValues();
			return m_worldrotation;
		}
		const Vector3& getForward(void)
		{
			updateValues();
			return m_forward;
		}
		const Vector3& getUp(void)
		{
			updateValues();
			return m_up;
		}
		const Vector3& getRight(void)
		{
			updateValues();
			return m_right;
		}
		Vector3 getBackwards(void)
		{
			updateValues();
			return -m_forward;
		}
		Vector3 getDown(void)
		{
			updateValues();
			return -m_up;
		}
		Vector3 getLeft(void)
		{
			updateValues();
			return -m_right;
		}

		// Since Camera Forward is flipped, this helps with readability
		Vector3 getCameraForward(void)
		{
			updateValues();
			return -m_forward;
		}
		const Vector3& getCameraBackwards(void)
		{
			updateValues();
			return m_forward;
		}

	};
}