// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../rendering/glUtil.h"
#include "../rendering/Uniform.h"
#include "../rendering/Shader.h"

#include "../utilities/Asset.h"

#include "../textures/TextureBinder.h"

#include "Component.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <map>
#include <string>
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
#ifdef _DEBUG
		const ShaderProgram* m_program = nullptr;
#endif
		const std::string	m_propertyName;
		Graphics::Uniform	m_uniform;
		bool				m_isUsed = true;

		void AcquireUniform(const ShaderProgram& program)
		{
#ifdef _DEBUG
			m_program = &program;
#endif

			if (program.CheckUniform(m_propertyName))
			{
				m_uniform = program.GetUniform(m_propertyName);

				uint32_t UniformType = m_uniform.type;
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
			VXL_ASSERT(ShaderProgram::GetBoundProgram() == m_program->GetID(), "Cannot change property without binding associated program first");
			VXL_ASSERT(m_uniform.location != -1, "Property has no attached Uniform");
			m_uniform.Send<Type>(_data);
		}
		// Only works for matrices
		void SetPropertyMatrix(Type _data, bool _transpose)
		{
			VXL_ASSERT(ShaderProgram::GetBoundProgram() == m_program->GetID(), "Cannot change property without binding associated program first");
			VXL_ASSERT(m_uniform.location != -1, "Property has no attached Uniform");
			m_uniform.SendMatrix<Type>(_data, _transpose);
		}

		inline bool IsUsed(void) const
		{
			return m_isUsed;
		}
	};

	class Material : public Asset<Material>, public TextureBinder
	{
		friend class RenderManager;
	private:
		// Data
		const std::string		m_name; // Name
		const ShaderProgram*	m_shaderProgram = nullptr; // Shader
		static std::map<UINT, Material*> m_masterOrder; // List of all used Orders and whether they are dirty
		static bool				m_masterOrderDirty; // Whether or not the master order has changed
		UINT					m_order = -1; // Render Order
		bool					m_sharedTextures = false; // [true] = binds materials textures

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
		void		SetProgram(const ShaderProgram& _program);
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
		inline void	SetSharedTextures(bool state)
		{
			m_sharedTextures = state;
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
		inline bool					HasSharedTextures(void) const
		{
			return m_sharedTextures;
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
		MaterialProperty<bool>			m_property_useTexture	 = MaterialProperty<bool>("VXL_useTexture");
		MaterialProperty<Color3F>		m_property_color		 = MaterialProperty<Color3F>("VXL_color");
		MaterialProperty<Color3F>		m_property_tint			 = MaterialProperty<Color3F>("VXL_tint");
		MaterialProperty<Vector4>		m_property_viewport		 = MaterialProperty<Vector4>("VXL_viewport");
		MaterialProperty<float>			m_property_alpha		 = MaterialProperty<float>("VXL_alpha");
		MaterialProperty<Vector4>		m_property_output		 = MaterialProperty<Vector4>("VXL_output");

		template<typename Type>
		void SetProperty(const std::string& _uniformName, Type _data)
		{
			VXL_ASSERT(m_shaderProgram, "Missing ShaderProgram for Material");
			VXL_ASSERT(m_shaderProgram->CheckUniform(_uniformName), "Shader Program doesn't contain Uniform: " + _uniformName);
			Graphics::Uniform uniform = m_shaderProgram->GetUniform(_uniformName);
			uniform.Send<Type>(_data);
		}

		// Bind Program, GL States, and Textures
		void BindProgram();
		void BindStates();
		void BindTextures();
	};

}