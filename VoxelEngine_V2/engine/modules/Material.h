// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Component.h"

#include "../rendering/Graphics.h"

#include "../textures/TextureBinder.h"

#include "../utilities/Types.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <optional>
//

#define MAX_MATERIAL_TEXTURES 8

namespace Vxl
{
	class _ShaderProgram;
	class BaseTexture;
	class Entity;

	enum class MaterialRenderMode
	{
		Opaque,
		Transparent
	};

	class _Material
	{
		DISALLOW_COPY_AND_ASSIGN(_Material);
		friend class Assets;
	private:
		// Data
		std::string					m_name;
		ShaderProgramIndex			m_shader = -1;
		uint32_t					m_sequenceNumber = -1;
		static std::set<uint32_t>	m_allSequenceNumbers;
		std::map<TextureLevel, BaseTexture*> m_textures;
		std::vector<TextureLevel> m_targetLevels; // What the shader is expecting for textures

		// Common Uniforms
		void setupCommonUniform(const _ShaderProgram& program, const std::string& name, std::optional<Graphics::Uniform>& uniform);

		std::optional<Graphics::Uniform> m_uniform_useModel		 = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_model		 = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_normalMatrix	 = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_useInstancing = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_useTexture	 = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_color		 = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_tint			 = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_viewport		 = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_alpha		 = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_output		 = std::nullopt;

		// Protected
		_Material(const std::string& name) 
			: m_name(name)
		{}

	public:
		~_Material()
		{
			if(m_sequenceNumber != -1)
				m_allSequenceNumbers.erase(m_sequenceNumber);
		}

		bool setSequenceID(uint32_t sequence)
		{
			if (sequence == -1)
				return false;

			// check if number is free
			if (m_allSequenceNumbers.find(sequence) == m_allSequenceNumbers.end())
			{
				m_sequenceNumber = sequence;
				m_allSequenceNumbers.insert(sequence);
				return true;
			}
			return false;
		}
		void eraseSequenceID(void)
		{
			m_allSequenceNumbers.erase(m_sequenceNumber);
			m_sequenceNumber = -1;
		}
		uint32_t getSequenceID(void) const
		{
			return m_sequenceNumber;
		}

		void setTexture(BaseTexture* tex, TextureLevel level)
		{
			m_textures[level] = tex;
		}
		void eraseTexture(TextureLevel level)
		{
			m_textures.erase(level);
		}
		void eraseAllTextures(void)
		{
			m_textures.clear();
		}

		// Which pass for rendering
		MaterialRenderMode m_renderMode = MaterialRenderMode::Opaque;
		// SharedTextures uses material textures, else it uses entity textures
		bool m_sharedTextures = true;

		// GL States
		CullMode			m_cullType = CullMode::COUNTER_CLOCKWISE;
		bool				m_blendState = true;
		BlendFunction		m_blendFunc;
		std::map<TextureLevel, BlendFunction> m_blendFuncAttachments;
		BlendEquation		m_blendEq = BlendEquation::FUNC_ADD;
		DepthPassRule		m_depthFunc = DepthPassRule::LESS_OR_EQUAL;
		bool				m_depthRead = true;
		bool				m_depthWrite = true;
		bool				m_wireframe = false;

		// Set TextureLevelTarget
		void setTextureLevelTargets(const std::initializer_list<TextureLevel>& levels)
		{
			m_targetLevels = levels;
		}

		// Set Shader
		void setShader(ShaderProgramIndex index);

		// Set Uniform
		template<typename Type>
		void sendUniform(const std::string& name, Type data) const
		{
			_ShaderProgram* program = Assets::getShaderProgram(m_shader);
			if (program)
			{
				auto it = program->m_uniforms.find(name);
				if (it != program->m_uniforms.end())
					it->second.send(data);
			}
		}
		
		// Set Uniform Matrix
		template<typename Type>
		void sendUniformMatrix(const std::string& name, Type data, bool transpose) const
		{
			_ShaderProgram* program = Assets::getShaderProgram(m_shader);
			if (program)
			{
				auto it = program->m_uniforms.find(name);
				if (it != program->m_uniforms.end())
					it->second.sendMatrix(data, transpose);
			}
		}

		// Get Uniform
		Graphics::Uniform getUniform(const std::string& name);

		// Behaviour
		void bindProgram();
		void bindStates();
		void bindTextures();
		void bindCommonUniforms(Entity* _entity);
	};

	//template<typename Type>
	//class MaterialProperty
	//{
	//	friend class Material;
	//private:
	//	const ShaderProgram*	m_program = nullptr;
	//	const std::string		m_propertyName;
	//	Graphics::Uniform		m_uniform;
	//	bool					m_isUsed = true;

	//	void AcquireUniform(const ShaderProgram& program)
	//	{
	//		m_program = &program;

	//		if (program.CheckUniform(m_propertyName))
	//		{
	//			m_uniform = program.GetUniform(m_propertyName);

	//			uint32_t UniformType = m_uniform.type;
	//			std::string TypeName = typeid(Type).name();
	//	
	//			// Check if Uniform data type from shader matches selected data type from material template
	//			VXL_ASSERT(Graphics::VeryifyDataType(TypeName, UniformType), "Material Type does not Match Uniform Type, Property Name = " + m_propertyName);

	//			m_isUsed = true;
	//		}
	//		else
	//			m_isUsed = false;
	//	}
	//public:
	//	MaterialProperty(const std::string& propertyName)
	//		: m_propertyName(propertyName)
	//	{}
	//	void SetProperty(Type _data)
	//	{
	//		VXL_ASSERT(Graphics::ShaderProgram::GetCurrentlyActive() == m_program->GetID(), "Cannot change property without binding associated program first");
	//		VXL_ASSERT(m_uniform.location != -1, "Property has no attached Uniform");
	//		m_uniform.Send<Type>(_data);
	//	}
	//	// Only works for matrices
	//	void SetPropertyMatrix(Type _data, bool _transpose)
	//	{
	//		VXL_ASSERT(Graphics::ShaderProgram::GetCurrentlyActive() == m_program->GetID(), "Cannot change property without binding associated program first");
	//		VXL_ASSERT(m_uniform.location != -1, "Property has no attached Uniform");
	//		m_uniform.SendMatrix<Type>(_data, _transpose);
	//	}

	//	inline bool IsUsed(void) const
	//	{
	//		return m_isUsed;
	//	}
	//};

	//

	//class Material : public Asset<Material>, public TextureBinder
	//{
	//	friend class RenderManager;
	//private:
	//	// Data
	//	const std::string		m_name; // Name
	//	//const ShaderProgram*	m_shaderProgram = nullptr; // Shader
	//	static std::map<UINT, Material*> m_masterOrder; // List of all used Orders and whether they are dirty
	//	static bool				m_masterOrderDirty; // Whether or not the master order has changed
	//	UINT					m_order = -1; // Render Order
	//	bool					m_sharedTextures = false; // [true] = binds materials textures
	//	MaterialRenderMode		m_renderMode = MaterialRenderMode::Opaque;

	//	// Locked Constructor
	//	Material(const std::string& _name, UINT _order)
	//		: m_name(_name)
	//	{
	//		SetOrder(_order);
	//	}

	//	// Blend Attachment Overrides [for specific textures]
	//	std::map<uint32_t, BlendFunction> m_BlendFuncAttachments;

	//	// Utility
	//	void UpdateProperties();

	//public:
	//	// Database Creation
	//	//static Material* Create(const std::string& _name, UINT _order);

	//	// Setters
	//	void		SetProgram(const ShaderProgram& _program);
	//	inline void SetOrder(UINT _order)
	//	{
	//		VXL_ASSERT(m_order != _order, "Cannot set order as its own value");
	//		VXL_ASSERT(_order != -1, "Cannot set order as -1 or max UINT value");

	//		// If previously had order, remove it
	//		if (m_order != -1)
	//			m_masterOrder.erase(m_order);

	//		// Check if material spot is free
	//		if (m_masterOrder.find(_order) == m_masterOrder.end())
	//		{
	//			m_masterOrder[_order] = this;
	//			m_order = _order;
	//		}
	//		// If not, keep moving that Material until it finds a free spot
	//		else
	//		{
	//			UINT NewOrder = _order + 1;
	//			while (m_masterOrder.find(NewOrder) != m_masterOrder.end())
	//			{
	//				NewOrder++;
	//				VXL_RETURN_ON_FAIL(NewOrder < (0xffffffff), "Order for Material has reached largest possible UINT number");
	//			}
	//			Material* MovingMaterial = m_masterOrder[_order];
	//			m_masterOrder[_order] = this;
	//			m_masterOrder[NewOrder] = MovingMaterial;
	//		}

	//		m_masterOrderDirty = true;
	//	}
	//	inline void	SetSharedTextures(bool state)
	//	{
	//		m_sharedTextures = state;
	//	}
	//	inline void SetRenderMode(MaterialRenderMode mode)
	//	{
	//		m_renderMode = mode;
	//		m_masterOrderDirty = true;
	//	}

	//	// Getters
	//	inline bool					IsValid(void) const
	//	{
	//		return m_shaderProgram->IsLinked();
	//	}
	//	inline const ShaderProgram* GetShader(void) const
	//	{
	//		return m_shaderProgram;
	//	}
	//	inline UINT					GetOrder(void) const
	//	{
	//		return m_order;
	//	}
	//	inline bool					HasSharedTextures(void) const
	//	{
	//		return m_sharedTextures;
	//	}
	//	inline MaterialRenderMode	GetRenderMode(void) const
	//	{
	//		return m_renderMode;
	//	}

	//	// GL States
	//	CullMode			m_CullType		= CullMode::COUNTER_CLOCKWISE;
	//	bool				m_BlendState	= true;
	//	BlendFunction		m_BlendFunc;
	//	BlendEquation		m_BlendEq		= BlendEquation::FUNC_ADD;
	//	DepthPassRule		m_DepthFunc		= DepthPassRule::LESS_OR_EQUAL;
	//	bool				m_DepthRead		= true;
	//	bool				m_DepthWrite	= true;
	//	bool				m_Wireframe		= false;
	//	void SetBlendFuncAttachment(uint32_t attachmentID, BlendSource src, BlendDestination dest)
	//	{
	//		m_BlendFuncAttachments[attachmentID].source = src;
	//		m_BlendFuncAttachments[attachmentID].destination = dest;
	//	}
	//	void RemoveBlendFuncAttachment(uint32_t attachmentID)
	//	{
	//		if (m_BlendFuncAttachments.find(attachmentID) != m_BlendFuncAttachments.end())
	//			m_BlendFuncAttachments.erase(attachmentID);
	//	}
	//	void RemoveAllBlendFuncAttachments()
	//	{
	//		m_BlendFuncAttachments.clear();
	//	}

	//	// Hardcoded Uniforms
	//	MaterialProperty<bool>			m_property_useModel			= MaterialProperty<bool>("VXL_useModel");
	//	MaterialProperty<Matrix4x4>		m_property_model			= MaterialProperty<Matrix4x4>("VXL_model");
	//	MaterialProperty<Matrix3x3>		m_property_normalMatrix		= MaterialProperty<Matrix3x3>("VXL_normalMatrix");
	//	MaterialProperty<bool>			m_property_useInstancing	= MaterialProperty<bool>("VXL_useInstancing");
	//	MaterialProperty<bool>			m_property_useTexture		= MaterialProperty<bool>("VXL_useTexture");
	//	MaterialProperty<Color3F>		m_property_color			= MaterialProperty<Color3F>("VXL_color");
	//	MaterialProperty<Color3F>		m_property_tint				= MaterialProperty<Color3F>("VXL_tint");
	//	MaterialProperty<Vector4>		m_property_viewport			= MaterialProperty<Vector4>("VXL_viewport");
	//	MaterialProperty<float>			m_property_alpha			= MaterialProperty<float>("VXL_alpha");
	//	MaterialProperty<Color4F>		m_property_output			= MaterialProperty<Color4F>("VXL_output");

	//	template<typename Type>
	//	void SetProperty(const std::string& _uniformName, Type _data)
	//	{
	//		VXL_ASSERT(m_shaderProgram, "Missing ShaderProgram for Material");
	//		VXL_ASSERT(m_shaderProgram->CheckUniform(_uniformName), "Shader Program doesn't contain Uniform: " + _uniformName);
	//		Graphics::Uniform uniform = m_shaderProgram->GetUniform(_uniformName);
	//		uniform.Send<Type>(_data);
	//	}

	//	// Bind Program, GL States, and Textures
	//	void BindProgram();
	//	void BindStates();
	//	void BindTextures();
	//};

}