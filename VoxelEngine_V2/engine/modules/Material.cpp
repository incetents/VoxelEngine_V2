// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../textures/Texture2D.h"

#include "../rendering/Shader.h"
#include "../rendering/Debug.h"
#include "../rendering/Mesh.h"

#include "../modules/Entity.h"
#include "../objects/GameObject.h"

#include "../math/Transform.h"

#include "../utilities/Logger.h"
#include "../utilities/Macros.h"
#include "../utilities/Asset.h"
#include "../modules/GlobalData.h"

namespace Vxl
{
	std::set<uint32_t> _Material::m_allSequenceNumbers;

	void _Material::setupCommonUniform(const _ShaderProgram& program, const std::string& name, std::optional<Graphics::Uniform>& uniform)
	{
		auto useModel = program.m_uniforms.find(name);
		if (useModel != program.m_uniforms.end())
			uniform = useModel->second;
		else
			uniform = std::nullopt;
	}

	void _Material::setSceneShader(ShaderProgramIndex index)
	{
		_ShaderProgram* program = SceneAssets.getShaderProgram(index);
		// Check if valid
		if (program)
		{
			m_shader = index;

			setupCommonUniform(*program, "VXL_useModel", m_uniform_useModel);
			setupCommonUniform(*program, "VXL_model", m_uniform_model);
			setupCommonUniform(*program, "VXL_normalMatrix", m_uniform_normalMatrix);
			setupCommonUniform(*program, "VXL_useInstancing", m_uniform_useInstancing);
			setupCommonUniform(*program, "VXL_useTexture", m_uniform_useTexture);
			setupCommonUniform(*program, "VXL_color", m_uniform_color);
			setupCommonUniform(*program, "VXL_tint", m_uniform_tint);
			setupCommonUniform(*program, "VXL_viewport", m_uniform_viewport);
			setupCommonUniform(*program, "VXL_alpha", m_uniform_alpha);
			setupCommonUniform(*program, "VXL_output", m_uniform_output);
		}
		else
			m_shader = -1;
	}

	Graphics::Uniform _Material::getUniform(const std::string& name)
	{
		_ShaderProgram* program = SceneAssets.getShaderProgram(m_shader);
		// Check if valid
		if (program)
		{
			// find uniform
			auto it = program->m_uniforms.find(name);
			if (it != program->m_uniforms.end())
			{
				return it->second;
			}
		}
		
		// Empty
		Graphics::Uniform empty_uni;
		empty_uni.location = -1;
		return empty_uni;
	}

	void _Material::bindProgram()
	{
		_ShaderProgram* program = SceneAssets.getShaderProgram(m_shader);
		if (program)
			program->bind();
	}
	void _Material::bindStates()
	{
		// Bind Modes
		Graphics::SetCullMode(m_cullType);
		Graphics::SetBlendState(m_blendState);
		if (m_blendState)
		{
			// Normal Blend modes
			Graphics::SetBlendMode(m_blendFunc.source, m_blendFunc.destination);
			// Special Blend modes
			for (auto& blendAttach : m_blendFuncAttachments)
				Graphics::SetBlendMode((int)blendAttach.first - 1, blendAttach.second.source, blendAttach.second.destination);

			Graphics::SetBlendEquation(m_blendEq);
		}
		Graphics::SetDepthPassRule(m_depthFunc);
		Graphics::SetDepthRead(m_depthRead);
		Graphics::SetDepthWrite(m_depthWrite);
		Graphics::SetWireframeState(m_wireframe);
	}
	void _Material::bindTextures()
	{
		if (!m_wireframe)
			return;

		// Null Texture if no textures are used
		if (m_textures.empty())
		{
			// Bind null texture on first active layer
			GlobalData.tex_nullImage->Bind(TextureLevel::LEVEL0);
		}
		else
		{
			for (auto it = m_textures.begin(); it != m_textures.end(); it++)
			{
				TextureLevel _level = it->first;
				BaseTexture* _tex = m_textures[it->first];

				// Bind Null texture if texture isn't loaded
				if (_tex == nullptr || !_tex->IsLoaded())
				{
					GlobalData.tex_nullImage->Bind(_level);
				}
				// Bind texture normally
				else
				{
					_tex->Bind(it->first);
				}
			}
		}
	}
	void _Material::bindCommonUniforms(Entity* _entity)
	{
		// ~ Model ~ //
		if (m_uniform_useModel.has_value())
		{
			m_uniform_useModel.value().send(_entity->m_useTransform);
		
			if (_entity->m_useTransform)
			{
				if (m_uniform_model.has_value())
					m_uniform_model.value().sendMatrix(_entity->m_transform.getModel(), true);
		
				if(m_uniform_normalMatrix.has_value())
					m_uniform_normalMatrix.value().sendMatrix(_entity->m_transform.getNormalMatrix(), true);
			}
		}
		
		// ~ Instancing ~ //
		if (m_uniform_useInstancing.has_value())
		{
			Mesh* _mesh = Assets::getMesh(_entity->getMesh());

			m_uniform_useInstancing.value().send(_mesh && _mesh->m_instances.GetDrawCount() > 0);
		}
		
		// ~ Texture ~ //
		if (m_uniform_useTexture.has_value())
		{
			m_uniform_useTexture.value().send(m_textures.find(TextureLevel::LEVEL0) != m_textures.end());
		}
		
		// ~ Colors ~ //
		if (m_uniform_color.has_value())
		{
			m_uniform_color.value().send(_entity->m_Color);
		}
		// ~ Tint ~ //
		if (m_uniform_tint.has_value())
		{
			m_uniform_tint.value().send(_entity->m_Tint);
		}
		
		// ~ Alpha ~ //
		if (m_uniform_alpha.has_value())
		{
			if (m_renderMode == MaterialRenderMode::Transparent)
				m_uniform_alpha.value().send(_entity->m_alpha);
			else
				m_uniform_alpha.value().send(0.0f);
		}
		
		// ~ ColorID ~ //
		//	if (m_uniform_output.has_value())
		//	{
		//		m_uniform_output.value
		//	
		//		m_material->m_property_output.SetProperty(m_colorID);
		//	}
		
	}







	// Static
	//std::map<UINT, Material*> Material::m_masterOrder;
	//bool Material::m_masterOrderDirty = false;

	// Database Creation
	//	Material* Material::Create(const std::string& _name, UINT _order)
	//	{
	//		Material* _material = new Material(_name, _order);
	//	
	//		AddNamedAsset(_name, _material, AssetMessage::CREATED);
	//	
	//		return _material;
	//	}

	//void Material::UpdateProperties()
	//{
	//	VXL_ASSERT(m_shaderProgram, "Material has no Program");

	//	m_property_useModel.AcquireUniform(*m_shaderProgram);
	//	m_property_model.AcquireUniform(*m_shaderProgram);
	//	m_property_normalMatrix.AcquireUniform(*m_shaderProgram);
	//	m_property_useInstancing.AcquireUniform(*m_shaderProgram);
	//	m_property_useTexture.AcquireUniform(*m_shaderProgram);
	//	m_property_color.AcquireUniform(*m_shaderProgram);
	//	m_property_tint.AcquireUniform(*m_shaderProgram);
	//	m_property_viewport.AcquireUniform(*m_shaderProgram);
	//	m_property_alpha.AcquireUniform(*m_shaderProgram);
	//	m_property_output.AcquireUniform(*m_shaderProgram);
	//}

	//// Setters
	//void Material::SetProgram(const ShaderProgram& _program)
	//{
	//	m_shaderProgram = &_program;

	//	UpdateProperties();
	//}

	//// Bind
	//void Material::BindProgram()
	//{
	//	VXL_ASSERT(m_shaderProgram, "Material missing Shader Program");

	//	// Bind Shader
	//	m_shaderProgram->Bind();
	//	
	//}
	//void Material::BindStates()
	//{
	//	// Bind Modes
	//	Graphics::SetCullMode(m_CullType);
	//	Graphics::SetBlendState(m_BlendState);
	//	if (m_BlendState)
	//	{
	//		// Normal Blend modes
	//		Graphics::SetBlendMode(m_BlendFunc.source, m_BlendFunc.destination);
	//		// Special Blend modes
	//		for (auto& blendAttach : m_BlendFuncAttachments)
	//			Graphics::SetBlendMode(blendAttach.first, blendAttach.second.source, blendAttach.second.destination);

	//		Graphics::SetBlendEquation(m_BlendEq);
	//	}
	//	Graphics::SetDepthPassRule(m_DepthFunc);
	//	Graphics::SetDepthRead(m_DepthRead);
	//	Graphics::SetDepthWrite(m_DepthWrite);
	//	Graphics::SetWireframeState(m_Wireframe);
	//}
	//void Material::BindTextures()
	//{
	//	// Bind Textures (ignore if wireframe mode is ON)
	//	if (m_Wireframe == false && m_sharedTextures)
	//	{
	//		TextureBinder::BindTextures();
	//	}
	//}

}