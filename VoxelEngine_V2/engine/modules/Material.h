// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../rendering/Graphics.h"
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
	class Texture2D;
	class GameObject;

	template<typename Type>
	class MaterialProperty
	{
		friend class Material;
	private:
		const ShaderProgram*	m_program = nullptr;
		const std::string		m_propertyName;
		Graphics::Uniform		m_uniform;
		bool					m_isUsed = true;

		void AcquireUniform(const ShaderProgram& program)
		{
			m_program = &program;

			if (program.CheckUniform(m_propertyName))
			{
				m_uniform = program.GetUniform(m_propertyName);

				uint32_t UniformType = m_uniform.type;
				std::string TypeName = typeid(Type).name();
		
				// Check if Uniform data type from shader matches selected data type from material template
				VXL_ASSERT(Graphics::VeryifyDataType(TypeName, UniformType), "Material Type does not Match Uniform Type, Property Name = " + m_propertyName);

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
			VXL_ASSERT(Graphics::ShaderProgram::GetCurrentlyActive() == m_program->GetID(), "Cannot change property without binding associated program first");
			VXL_ASSERT(m_uniform.location != -1, "Property has no attached Uniform");
			m_uniform.Send<Type>(_data);
		}
		// Only works for matrices
		void SetPropertyMatrix(Type _data, bool _transpose)
		{
			VXL_ASSERT(Graphics::ShaderProgram::GetCurrentlyActive() == m_program->GetID(), "Cannot change property without binding associated program first");
			VXL_ASSERT(m_uniform.location != -1, "Property has no attached Uniform");
			m_uniform.SendMatrix<Type>(_data, _transpose);
		}

		inline bool IsUsed(void) const
		{
			return m_isUsed;
		}
	};

	enum class MaterialRenderMode
	{
		Opaque,
		Transparent
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
		MaterialRenderMode		m_renderMode = MaterialRenderMode::Opaque;

		// Locked Constructor
		Material(const std::string& _name, UINT _order)
			: m_name(_name)
		{
			SetOrder(_order);
		}

		// Blend Attachment Overrides [for specific textures]
		std::map<uint32_t, BlendFunction> m_BlendFuncAttachments;

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
					VXL_RETURN_ON_FAIL(NewOrder < (0xffffffff), "Order for Material has reached largest possible UINT number");
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
		inline void SetRenderMode(MaterialRenderMode mode)
		{
			m_renderMode = mode;
			m_masterOrderDirty = true;
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
		inline MaterialRenderMode	GetRenderMode(void) const
		{
			return m_renderMode;
		}

		// GL States
		CullMode			m_CullType		= CullMode::COUNTER_CLOCKWISE;
		bool				m_BlendState	= true;
		BlendFunction		m_BlendFunc;
		BlendEquation		m_BlendEq		= BlendEquation::FUNC_ADD;
		DepthPassRule		m_DepthFunc		= DepthPassRule::LESS_OR_EQUAL;
		bool				m_DepthRead		= true;
		bool				m_DepthWrite	= true;
		bool				m_Wireframe		= false;
		void SetBlendFuncAttachment(uint32_t attachmentID, BlendSource src, BlendDestination dest)
		{
			m_BlendFuncAttachments[attachmentID].source = src;
			m_BlendFuncAttachments[attachmentID].destination = dest;
		}
		void RemoveBlendFuncAttachment(uint32_t attachmentID)
		{
			if (m_BlendFuncAttachments.find(attachmentID) != m_BlendFuncAttachments.end())
				m_BlendFuncAttachments.erase(attachmentID);
		}
		void RemoveAllBlendFuncAttachments()
		{
			m_BlendFuncAttachments.clear();
		}

		// Hardcoded Uniforms
		MaterialProperty<bool>			m_property_useModel		 = MaterialProperty<bool>("VXL_useModel");
		MaterialProperty<Matrix4x4>		m_property_model		 = MaterialProperty<Matrix4x4>("VXL_model");
		MaterialProperty<bool>			m_property_useInstancing = MaterialProperty<bool>("VXL_useInstancing");
		MaterialProperty<bool>			m_property_useTexture	 = MaterialProperty<bool>("VXL_useTexture");
		MaterialProperty<Color3F>		m_property_color		 = MaterialProperty<Color3F>("VXL_color");
		MaterialProperty<Color3F>		m_property_tint			 = MaterialProperty<Color3F>("VXL_tint");
		MaterialProperty<Vector4>		m_property_viewport		 = MaterialProperty<Vector4>("VXL_viewport");
		MaterialProperty<float>			m_property_alpha		 = MaterialProperty<float>("VXL_alpha");
		MaterialProperty<Color4F>		m_property_output		 = MaterialProperty<Color4F>("VXL_output");

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