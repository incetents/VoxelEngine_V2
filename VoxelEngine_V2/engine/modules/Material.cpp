// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../math/Transform.h"

#include "../modules/Entity.h"

#include "../rendering/Shader.h"
#include "../rendering/Debug.h"
#include "../rendering/Mesh.h"
#include "../rendering/RenderManager.h"

#include "../textures/Texture2D.h"

#include "../objects/GameObject.h"

#include "../utilities/Logger.h"
#include "../utilities/Macros.h"
#include "../utilities/Asset.h"

namespace Vxl
{
	std::set<uint32_t> Material::m_allSequenceNumbers;

	Material::~Material()
	{
		if (m_sequenceNumber != -1)
		{
			m_allSequenceNumbers.erase(m_sequenceNumber);
			// Rendermanager must re-sort materials
			RenderManager.dirtyMaterialSequence();
		}
	}

	bool Material::setSequenceID(uint32_t sequence)
	{
		// check if number is free
		if (m_allSequenceNumbers.find(sequence) == m_allSequenceNumbers.end())
		{
			m_sequenceNumber = sequence;

			if(sequence != -1)
				m_allSequenceNumbers.insert(sequence);

			// Rendermanager must re-sort materials
			RenderManager.dirtyMaterialSequence();
			return true;
		}
		return false;
	}
	void Material::eraseSequenceID(void)
	{
		if (m_sequenceNumber == -1)
			return;

		m_allSequenceNumbers.erase(m_sequenceNumber);
		m_sequenceNumber = -1;
		// Rendermanager must re-sort materials
		RenderManager.dirtyMaterialSequence();
	}

	void Material::setShaderMaterial(ShaderMaterialIndex index)
	{
		ShaderMaterial* _shaderMat = Assets.getShaderMaterial(index);
		if (_shaderMat)
		{
			m_shaderMaterial = index;
		}
	}

	Graphics::Uniform Material::getUniform(const std::string& name)
	{
		ShaderMaterial* _shaderMat = Assets.getShaderMaterial(m_shaderMaterial);
		if (_shaderMat)
		{
			ShaderProgram* program = Assets.getShaderProgram(_shaderMat->m_coreProgram);
			if (program)
			{
				// find uniform
				auto it = program->m_uniforms.find(name);
				if (it != program->m_uniforms.end())
				{
					return it->second;
				}
			}
		}
		
		// Empty
		Graphics::Uniform empty_uni;
		empty_uni.location = -1;
		return empty_uni;
	}

	ShaderProgram* Material::getCoreProgram(void) const
	{
		ShaderMaterial* _shaderMat = Assets.getShaderMaterial(m_shaderMaterial);
		if (_shaderMat)
		{
			return Assets.getShaderProgram(_shaderMat->m_coreProgram);
		}
		return nullptr;
	}

	bool Material::bindCoreProgram()
	{
		ShaderProgram* corePogram = getCoreProgram();
		if (corePogram && corePogram->isLinked())
		{
			corePogram->bind();
			return true;
		}
		return false;
	}
	void Material::bindCoreProgramCommonUniforms(EntityIndex _entity)
	{
		ShaderMaterial* _shaderMaterial = Assets.getShaderMaterial(m_shaderMaterial);
		if (_shaderMaterial)
		{
			ShaderProgram* _shaderProgram = Assets.getShaderProgram(_shaderMaterial->m_coreProgram);
			if (_shaderProgram)
				_shaderProgram->bindCommonUniforms(_entity);
		}
	}
	void Material::bindStates()
	{
		// bind Modes
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

		const std::vector<TextureLevel>& targetLevels = Assets.getShaderMaterial(m_shaderMaterial)->m_targetLevels;

		for (const auto& level : targetLevels)
		{
			// Check if material doesn't have the texture location
			if (m_textures.find(level) == m_textures.end())
			{
				if (level == TextureLevel::LEVEL0)
					GlobalAssets.get_Tex2DNullImageCheckerboard()->bind(level);
				else
					GlobalAssets.get_Tex2DNullImageBlack()->bind(level);
			}

			TextureIndex index = m_textures[level];
			BaseTexture* _tex = Assets.getBaseTexture(index);

			// bind error texture
			if (_tex == nullptr || !_tex->isLoaded())
			{
				if(level == TextureLevel::LEVEL0)
					GlobalAssets.get_Tex2DNullImageCheckerboard()->bind(level);
				else
					GlobalAssets.get_Tex2DNullImageBlack()->bind(level);
			}
			// bind texture normally
			else
			{
				_tex->bind(level);
			}
		}
	}
	void Material::bindTextures(Entity* _entity)
	{
		if (m_wireframe)
			return;

		const std::vector<TextureLevel>& targetLevels = Assets.getShaderMaterial(m_shaderMaterial)->m_targetLevels;

		for (const auto& level : targetLevels)
		{
			// Check if entity doesn't have the texture location
			if (_entity->m_textures.find(level) == _entity->m_textures.end())
			{
				if (level == TextureLevel::LEVEL0)
					GlobalAssets.get_Tex2DNullImageCheckerboard()->bind(level);
				else
					GlobalAssets.get_Tex2DNullImageBlack()->bind(level);
			}

			TextureIndex index = _entity->m_textures[level];
			BaseTexture* _tex = Assets.getBaseTexture(index);

			// bind error texture
			if (_tex == nullptr || !_tex->isLoaded())
			{
				if (level == TextureLevel::LEVEL0)
					GlobalAssets.get_Tex2DNullImageCheckerboard()->bind(level);
				else
					GlobalAssets.get_Tex2DNullImageBlack()->bind(level);
			}
			// bind texture normally
			else
			{
				_tex->bind(level);
			}
		}
	}

}