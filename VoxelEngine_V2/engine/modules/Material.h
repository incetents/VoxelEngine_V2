// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <map>
#include <string>
#include "Component.h"
#include "../opengl/glUtil.h"
#include "../opengl/Uniform.h"
#include "../utilities/Asset.h"

#include "../opengl/Shader.h"
//

#define MAX_MATERIAL_TEXTURES 8

namespace Vxl
{
	class BaseTexture;
	class Transform;
	class Texture;
	class GameObject;

	template<typename Type>
	class MaterialProperty
	{
		friend class Material;
	private:
		const std::string	m_propertyName;
		glUniform			m_uniform;
		bool				m_isUsed = true;

		void AcquireUniform(const ShaderProgram& program)
		{
			if (program.CheckUniform(m_propertyName))
			{
				m_uniform = program.GetUniform(m_propertyName);

				GLenum UniformType = m_uniform.GetType();
				std::string TypeName = typeid(Type).name();
				
				glUtil::VeryifyDataType(TypeName, UniformType);

				m_isUsed = true;
			}
			else
				m_isUsed = false;
		}
	public:
		MaterialProperty(const std::string& propertyName)
			: m_propertyName(propertyName)
		{}
		void SetProperty(Type _data)
		{
			VXL_ASSERT(m_uniform.GetLocation() != -1, "Property has no attached Uniform");
			m_uniform.Set<Type>(_data);
		}
		inline bool IsUsed(void) const
		{
			return m_isUsed;
		}
	};

	class Material : public Asset<Material>
	{
		friend class RenderManager;
	private:
		// Data
		const std::string		m_name; // Name
		const ShaderProgram*	m_shaderProgram = nullptr; // Shader
		static std::map<UINT, Material*> m_masterOrder; // List of all used Orders and whether they are dirty
		static bool				m_masterOrderDirty; // Whether or not the master order has changed
		UINT					m_order = -1; // Render Order

		std::map<Active_Texture, BaseTexture*> m_textures;
		UINT					m_textureCount = 0;

		// Locked Constructor
		Material(const std::string& _name, UINT _order)
			: m_name(_name)
		{
			SetOrder(_order);
		}

		// Utility
		void UpdateProperties();

	public:
		// Database Creation
		static Material* Create(const std::string& _name, UINT _order);

		// Setters
		void SetProgram(const ShaderProgram& _program);
		inline void SetOrder(UINT _order)
		{
			VXL_ASSERT(m_order != _order, "Cannot set order as its own value");
			VXL_ASSERT(_order != -1, "Cannot set order as -1 or max UINT value");

			// If previously had order, remove it
			if (m_order != -1)
				m_masterOrder.erase(m_order);

			// Check if material spot is free
			if (m_masterOrder.find(_order) == m_masterOrder.end())
			{
				m_masterOrder[_order] = this;
				m_order = _order;
			}
			// If not, keep moving that Material until it finds a free spot
			else
			{
				UINT NewOrder = _order + 1;
				while (m_masterOrder.find(NewOrder) != m_masterOrder.end())
				{
					NewOrder++;
					VXL_ASSERT(NewOrder < (0xffffffff), "Order for Material has reached largest possible UINT number");
					VXL_RETURN_ON_FAIL(NewOrder < (0xffffffff));
				}
				Material* MovingMaterial = m_masterOrder[_order];
				m_masterOrder[_order] = this;
				m_masterOrder[NewOrder] = MovingMaterial;
			}

			m_masterOrderDirty = true;
		}
		inline void SetTexture(BaseTexture* texture, Active_Texture layer)
		{
			m_textures[layer] = texture;
			m_textureCount = (UINT)m_textures.size();
		}
		inline void RemoveTexture(Active_Texture layer)
		{
			m_textures.erase(layer);
			m_textureCount = (UINT)m_textures.size();
		}
		inline void ClearTextures(void)
		{
			m_textures.clear();
			m_textureCount = 0;
		}

		// Getters
		inline const ShaderProgram* GetShader(void) const
		{
			return m_shaderProgram;
		}
		inline UINT					GetOrder(void) const
		{
			return m_order;
		}

		// GL States
		Cull_Type			m_CullType		= Cull_Type::COUNTER_CLOCKWISE;
		bool				m_BlendState	= true;
		Blend_Source		m_BlendSource	= Blend_Source::SRC_ALPHA;
		Blend_Destination	m_BlendDest		= Blend_Destination::ONE_MINUS_SRC_ALPHA;
		Blend_Equation		m_BlendEq		= Blend_Equation::FUNC_ADD;
		Depth_Pass_Rule		m_DepthFunc		= Depth_Pass_Rule::LESS_OR_EQUAL;
		bool				m_DepthRead		= true;
		bool				m_DepthWrite	= true;
		bool				m_Wireframe		= false;

		// Hardcoded Uniforms
		MaterialProperty<bool>			m_property_useModel		 = MaterialProperty<bool>("VXL_useModel");
		MaterialProperty<Matrix4x4>		m_property_model		 = MaterialProperty<Matrix4x4>("VXL_model");
		MaterialProperty<bool>			m_property_useInstancing = MaterialProperty<bool>("VXL_useInstancing");
		MaterialProperty<bool>			m_property_useColor		 = MaterialProperty<bool>("VXL_useColor");
		MaterialProperty<Color3F>		m_property_color		 = MaterialProperty<Color3F>("VXL_color");
		MaterialProperty<Color3F>		m_property_tint			 = MaterialProperty<Color3F>("VXL_tint");

		// Bind Program, GL States, and Textures
		void Bind();
	};

}