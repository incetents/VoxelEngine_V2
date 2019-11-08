// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Component.h"
#include "../math/Transform.h"
#include "../math/Color.h"
#include "../utilities/Asset.h"
#include "../utilities/Util.h"

#include <map>
#include <stack>

#define MAX_ENTITY_NAME_LENGTH 256

namespace Vxl
{
	class Mesh;

	class Entity //: public ComponentHandler
	{
		DISALLOW_COPY_AND_ASSIGN(Entity);
		friend class Assets;
		friend class RenderManager;
		friend class GameObject;
		friend class Transform;
		friend class Editor;
	protected:
		// Locked Constructor
		Entity(const std::string& name);

		// Hidden Data
		uint32_t		m_uniqueID;
		Color4F			m_colorID;
		MeshIndex		m_mesh = -1;
		MaterialIndex	m_material = -1;
		
		Vector3		m_OBB[8]; // Object Bounding Box from mesh
		Vector3		m_AABB[2]; // AABB based on OBB

		// Editor Information
		bool		m_isSelected = false; //

		// Utility
		void UpdateBoundingBoxCheap();
		void TransformChanged()
		{
			UpdateBoundingBoxCheap();
		}

	public:
		// Destructor
		virtual ~Entity();

		// Data
		std::string	m_name;
		Transform	m_transform;
		Color3F		m_labelColor	= Color3F(1, 1, 1); // Inspector
		Color3F		m_Color			= Color3F(0, 0, 0);
		Color3F		m_Tint			= Color3F(1, 1, 1);
		float		m_alpha			= 1.0f;
		bool		m_useTransform	= true;
		bool		m_isActive		= true;
		bool		m_isSelectable	= true; // for editor
		//bool		m_isColoredObject = false;
		
		// Mesh
		void setMesh(MeshIndex index);
		inline MeshIndex getMesh(void) const
		{
			return m_mesh;
		}
		// Material
		void setMaterial(MaterialIndex index);
		inline MaterialIndex getMaterial(void) const
		{
			return m_material;
		}

		// Editor Information
		inline bool IsSelected(void) const
		{
			return m_isSelected;
		}

		inline uint32_t getUniqueID(void) const
		{
			return m_uniqueID;
		}

		// check if all parents are active
		bool IsFamilyActive()
		{
			// if no parent, check normal active state
			Transform* parent = m_transform.getParent();
			if (parent == nullptr)
				return m_isActive;

			// auto fail if off
			if (!m_isActive)
				return false;

			// iterate through all parents
			while (parent != nullptr)
			{
				Entity* owner = Assets::getEntity(parent->m_owner);
				VXL_ASSERT(owner, "Component not attached to entity");
				if (!owner->m_isActive)
					return false;
				// Acquire new parent
				else
					parent = owner->m_transform.getParent();
			}
			// If no parents failed, that means it's all good
			return true;
		}

		// Bounding Box
		std::vector<Vector3> GetOBB(void) const
		{
			return std::vector<Vector3>(m_OBB, m_OBB + 8);
		}
		Vector3				 GetAABBMin(void) const
		{
			return m_AABB[0];
		}
		Vector3				 GetAABBMax(void) const
		{
			return m_AABB[1];
		}
	};
}

