// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Component.h"
#include "../math/Transform.h"
#include "../math/Color.h"
#include "../utilities/Database.h"

#define MAX_ENTITY_NAME_LENGTH 256

namespace Vxl
{
	class Mesh;
	class Material;
	class ShaderProgram;

	enum class EntityType
	{
		GAMEOBJECT,
		LIGHT
	};

	class Entity : public ComponentHandler
	{
		friend class RenderManager;
		friend class Transform;
		friend class GameObject;
	protected:
		// Locked Constructor
		Entity(const std::string& name, EntityType type);
		// Database
		static DatabaseSet<Entity> m_database;

		// Data
		Mesh*		m_mesh = nullptr;
		EntityType  m_type;
		std::string	m_name;
		Color3F		m_Color = Color3F(1, 1, 1);
		Color3F		m_Tint	= Color3F(1, 1, 1);
		Vector3		m_OBB[8]; // Object Bounding Box from mesh
		Vector3		m_AABB[2]; // AABB based on OBB

	public:
		// Destructor
		virtual ~Entity() {}
		
		// Data
		Transform			m_transform;
		bool				m_useTransform = true;
		bool				m_isActive = true;
		bool				m_isColoredObject = false;
		bool				m_isClickable = true;

		// Type
		EntityType GetType(void) const
		{
			return m_type;
		}
		
		// Name
		void SetName(const std::string _name);
		inline std::string	GetName(void) const
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
		inline void SetColor(Color3F color, bool enableIsColoredObject = true)
		{
			m_Color = color;
			m_isColoredObject = enableIsColoredObject;
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

		// Mesh
		inline Mesh* GetMesh(void) const
		{
			return m_mesh;
		}

		// Behaviour
		virtual void Update() = 0;
		virtual void Draw() = 0;
	protected:
		virtual void TransformChanged() = 0;
	};
}

