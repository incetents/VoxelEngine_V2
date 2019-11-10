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

namespace Vxl
{
	std::set<uint32_t> Material::m_allSequenceNumbers;

	void Material::setupCommonUniform(const ShaderProgram& program, const std::string& name, std::optional<Graphics::Uniform>& uniform)
	{
		auto useModel = program.m_uniforms.find(name);
		if (useModel != program.m_uniforms.end())
			uniform = useModel->second;
		else
			uniform = std::nullopt;
	}

	void Material::setShader(ShaderProgramIndex index)
	{
		ShaderProgram* program = Assets::getShaderProgram(index);
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

	Graphics::Uniform Material::getUniform(const std::string& name)
	{
		ShaderProgram* program = Assets::getShaderProgram(m_shader);
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

	bool Material::bindProgram()
	{
		ShaderProgram* program = Assets::getShaderProgram(m_shader);
		if (program && program->isLinked())
		{
			program->bind();
			return true;
		}
		return false;
	}
	void Material::bindStates()
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
	void Material::bindTextures()
	{
		if (m_wireframe)
			return;

		for (const auto& level : m_targetLevels)
		{
			TextureIndex index = m_textures[level];
			BaseTexture* _tex = Assets::getBaseTexture(index);

			// Bind error texture
			if (_tex == nullptr || !_tex->IsLoaded())
			{
				if(level == TextureLevel::LEVEL0)
					GlobalAssets.getTex2DNullImageCheckerboard()->Bind(level);
				else
					GlobalAssets.getTex2DNullImageBlack()->Bind(level);
			}
			// Bind texture normally
			else
			{
				_tex->Bind(level);
			}
		}
	}
	void Material::bindTextures(Entity* _entity)
	{
		if (m_wireframe)
			return;

		for (const auto& level : m_targetLevels)
		{
			TextureIndex index = _entity->m_textures[level];
			BaseTexture* _tex = Assets::getBaseTexture(index);

			// Bind error texture
			if (_tex == nullptr || !_tex->IsLoaded())
			{
				if (level == TextureLevel::LEVEL0)
					GlobalAssets.getTex2DNullImageCheckerboard()->Bind(level);
				else
					GlobalAssets.getTex2DNullImageBlack()->Bind(level);
			}
			// Bind texture normally
			else
			{
				_tex->Bind(level);
			}
		}
	}
	void Material::bindCommonUniforms(Entity* _entity)
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
			if(m_sharedTextures)
				m_uniform_useTexture.value().send(m_textures.find(TextureLevel::LEVEL0) != m_textures.end());
			else
				m_uniform_useTexture.value().send(_entity->m_textures.find(TextureLevel::LEVEL0) != _entity->m_textures.end());
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
				m_uniform_alpha.value().send(1.0f);
		}
		
		// ~ ColorID ~ //
		//	if (m_uniform_output.has_value())
		//	{
		//		m_uniform_output.value
		//	
		//		m_material->m_property_output.SetProperty(m_colorID);
		//	}
		
	}
}