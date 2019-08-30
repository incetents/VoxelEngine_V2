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
	class ShaderProgram;

	enum class EntityType
	{
		GAMEOBJECT,
		LIGHT,
		CAMERA
		//SKYBOX
	};

	class Entity : public ComponentHandler
	{
		friend class RenderManager;
		friend class GameObject;
		friend class Transform;
		friend class Editor;
	protected:
		// Locked Constructor
		Entity(const std::string& name, EntityType type);

		// Core Data
		uint32_t	m_uniqueID;
		Color4F		m_colorID;
		EntityType  m_type;
		std::string	m_name;			// Used for storage with Asset class [Never modified]
		std::string m_displayName;	// Used with editor when displaying name [can be modified]
		Color3F		m_Color = Color3F(1, 1, 1);
		Color3F		m_Tint	= Color3F(1, 1, 1);
		float		m_alpha = 1.0f;
		Vector3		m_OBB[8]; // Object Bounding Box from mesh
		Vector3		m_AABB[2]; // AABB based on OBB

		// Editor Information
		Color3F		m_labelColor = Color3F(1, 1, 1); // Inspector
		bool		m_isSelectable = true; //
		bool		m_isSelected = false; // 

		// Unique IDs
		static uint32_t m_maxUniqueID;
		static std::stack<uint32_t> m_discardedUniqueIDs;
		static std::map<uint32_t, Entity*> m_EntitiesByID;

	public:
		// Destructor
		virtual ~Entity();

		// Data
		Transform			m_transform;
		bool				m_useTransform = true;
		bool				m_isActive = true;
		bool				m_isColoredObject = false;
		
		// ID
		inline uint64_t GetUniqueID(void) const
		{
			return m_uniqueID;
		}
		static Entity* GetEntityByID(uint32_t id)
		{
			if (m_EntitiesByID.find(id) == m_EntitiesByID.end())
				return nullptr;

			return m_EntitiesByID[id];
		}

		// Editor Information
		inline void SetSelectable(bool state)
		{
			m_isSelectable = state;
		}
		inline bool IsSelectable(void) const
		{
			return m_isSelectable;
		}
		inline bool IsSelected(void) const
		{
			return m_isSelected;
		}

		// Type
		EntityType GetType(void) const
		{
			return m_type;
		}
		
		// Name
		void SetDisplayName(const std::string _name);
		inline std::string GetDisplayName(void) const
		{
			return m_displayName;
		}
		inline std::string GetName(void) const
		{
			return m_name;
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
				if (!parent->GetOwner()->m_isActive)
					return false;
				// Acquire new parent
				else
					parent = parent->GetOwner()->m_transform.getParent();
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

		// Color
		inline void SetLabelColor(Color3F color)
		{
			m_labelColor = color;
		}
		inline Color3F& GetLabelColor(void)
		{
			return m_labelColor;
		}

		inline void SetColor(Color3F color, bool colorOverTexture = true)
		{
			m_Color = color;
			m_isColoredObject = colorOverTexture;
		}
		inline Color3F& GetColor(void)
		{
			return m_Color;
		}

		inline void SetTint(Color3F tint)
		{
			m_Tint = tint;
		}
		inline Color3F GetTint(void)
		{
			return m_Tint;
		}

		// Warning, alpha only does anything if material is transparent
		inline void SetAlpha(float a)
		{
			m_alpha = a;
		}
		inline float GetAlpha(void) const
		{
			return m_alpha;
		}

		// Update Bounding Box from Mesh
		void UpdateBoundingBoxCheap(Mesh* _mesh);

		// Behaviour
		virtual void Update(void) = 0;
		virtual void Draw(void) = 0;
	protected:
		virtual void TransformChanged(void) = 0;
	};
}

