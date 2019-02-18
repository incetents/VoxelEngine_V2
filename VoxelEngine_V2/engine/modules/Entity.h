// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Component.h"
#include "../math/Transform.h"
#include "../math/Color.h"
#include "../opengl/MeshRenderer.h"
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
		Entity();
		// Database
		static DatabaseSet<Entity> m_database;

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
		static Entity* Create();
		
		// Data
		std::string			m_name;
		Transform			m_transform;
		bool				m_useTransform = true;
		MeshRenderer		m_meshRenderer;
		MaterialData		m_material;
		
		// Mesh Rendering
		bool m_isActive = true;
		bool m_isColoredObject = false;
		Mesh* m_mesh;

		// Utility
		inline void			SetName(const std::string _name)
		{
			m_name = _name;
		}
		inline std::string	GetName(void) const
		{
			return m_name;
		}

		void			SetMaterial(Material* _base);
		Material*		GetMaterial(void) const;
		UINT			GetMaterialOrder(void) const;

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
		void SetColor(Color3F color, bool enableIsColoredObject = true)
		{
			m_Color = color;
			m_isColoredObject = enableIsColoredObject;
		}
		Color3F& GetColor(void)
		{
			return m_Color;
		}

		void SetTint(Color3F tint)
		{
			m_Tint = tint;
		}
		Color3F GetTint(void)
		{
			return m_Tint;
		}

		// Behaviour
		virtual void Update();
		virtual void Draw();

	};
}

