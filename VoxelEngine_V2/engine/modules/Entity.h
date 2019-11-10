// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Component.h"
#include "../math/Transform.h"
#include "../math/Color.h"
#include "../utilities/Types.h"
#include "../utilities/Util.h"
#include "../modules/Material.h"

#include <map>
#include <stack>
#include <typeinfo>

#define MAX_ENTITY_NAME_LENGTH 256

namespace Vxl
{
	class Mesh;

	class Entity
	{
		DISALLOW_COPY_AND_ASSIGN(Entity);
		friend class Assets;
		friend class RenderManager;
		friend class Transform;
		friend class Editor;
		friend class Material;
	protected:
		// Locked Constructor
		Entity(const std::string& name);

		// Hidden Data
		uint32_t		m_uniqueID;
		Color4F			m_colorID;
		MeshIndex		m_mesh = -1;
		MaterialIndex	m_material = -1;
		std::map<TextureLevel, TextureIndex> m_textures;
		
		// Editor Information
		bool		m_isSelected = false;
		
		// Bounding Boxes
		Vector3		m_OBB[8]; // Object Bounding Box from mesh
		Vector3		m_AABB[2]; // AABB based on OBB

		// Component
		std::map<const std::type_info*, Component*> m_components;

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

		// Textures
		void setTexture(TextureIndex tex, TextureLevel level)
		{
			m_textures[level] = tex;
		}
		TextureIndex eraseTexture(TextureLevel level)
		{
			auto it = m_textures.find(level);
			if (it == m_textures.end())
				return -1;

			TextureIndex index = m_textures[level];
			m_textures.erase(level);
			return index;
		}
		void eraseAllTextures(void)
		{
			m_textures.clear();
		}

		// Components
		template <typename Type = Component * >
		bool hasComponent()
		{
			return m_components.count(&typeid(Type)) != 0;
		}
		
		template <typename Type = Component * >
		Type* addComponent(Type* a_component)
		{
			if (a_component != nullptr && m_components.count(&typeid(*a_component)) == 0)
			{
				m_components[&typeid(*a_component)] = (a_component);
				m_components[&typeid(*a_component)]->m_owner = m_uniqueID;

				ComponentMaster<Type>::allComponents.push_back(a_component);

				return a_component;
			}
			return nullptr;
		}
		
		template <typename Type = Component * >
		Type* getComponent()
		{
			if (m_components.count(&typeid(Type)) != 0)
			{
				return static_cast<Type*>(m_components[&typeid(Type)]);
			}
			return nullptr;
		}

		template <typename Type = Component * >
		Type* eraseComponent()
		{
			if (m_components.count(&typeid(Type)) > 0)
			{
				Type* data = static_cast<Type*>(m_components[&typeid(Type)]);

				m_components.erase(&typeid(Type));

				// Normal Erase
				//	ComponentMaster<Type>::allComponents.erase(
				//		std::remove(
				//			ComponentMaster<Type>::allComponents.begin(), ComponentMaster<Type>::allComponents.end(), data
				//		)
				//	);

				// Swap element with last element + erase last element
				auto it = std::find(ComponentMaster<Type>::allComponents.begin(), ComponentMaster<Type>::allComponents.end(), &m_transform);

				if (it != ComponentMaster<Type>::allComponents.end())
				{
					std::iter_swap(it, ComponentMaster<Type>::allComponents.end() - 1);

					ComponentMaster<Type>::allComponents.pop_back();
				}

				return data;
			}
			return nullptr;
		}

		//
		inline uint32_t getUniqueID(void) const
		{
			return m_uniqueID;
		}

		// Check if Editor can see this
		inline bool IsSelected(void) const
		{
			return m_isSelected;
		}

		// check if all parents are active
		bool IsFamilyActive();

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

