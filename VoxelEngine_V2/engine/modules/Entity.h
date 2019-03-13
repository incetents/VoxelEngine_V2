// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Component.h"
#include "../math/Transform.h"
#include "../math/Color.h"
#include "../modules/Material.h"
#include "../utilities/Database.h"

namespace Vxl
{
	class Mesh;
	class Material;
	class Material;
	class ShaderProgram;

	class Entity : public ComponentHandler
	{
		friend class RenderManager;
		friend class Transform;
	private:
		// Locked Constructor
		Entity(const std::string& displayName);
		// Database
		static DatabaseSet<Entity> m_database;

		// Mesh
		Mesh* m_mesh = nullptr;

		// Colors
		Color3F m_Color = Color3F(1, 1, 1);
		Color3F m_Tint	= Color3F(1, 1, 1);

		// Bounding Box Data
		Vector3 m_OBB[8]; // Object Bounding Box from mesh
		Vector3 m_AABB[2]; // AABB based on OBB

		void TransformChanged();

	public:
		virtual ~Entity();

		// Database Creation
		static Entity* Create(const std::string& displayName = "");
		
		// Data
		std::string			m_name;
		Transform			m_transform;
		bool				m_useTransform = true;
		MaterialData		m_material;
		bool				m_isActive = true;
		bool				m_isColoredObject = false;

		// Utility
		inline void			SetName(const std::string _name)
		{
			m_name = _name;
		}
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

		void			SetMaterial(Material* _base);
		Material*		GetMaterial(void) const;
		UINT			GetMaterialOrder(void) const;

		std::vector<Vector3> GetOBB(void) const
		{
			return std::vector<Vector3>(m_OBB, m_OBB + 8);
		}
		Vector3				 GetLocalAABBMin(void) const
		{
			return m_AABB[0];
		}
		Vector3				 GetLocalAABBMax(void) const
		{
			return m_AABB[1];
		}

		// Mesh
		inline void SetMesh(Mesh* _mesh)
		{
			m_mesh = _mesh;
			// call callbacks since transform might change
			TransformChanged();
		}
		inline Mesh* GetMesh(void) const
		{
			return m_mesh;
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

		// Behaviour
		virtual void Update();
		virtual void Draw();

	};
}

