// Copyright (c) 2020 Emmanuel Lajeunesse
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

	//	Graphics::Uniform Material::getUniform(const std::string& name)
	//	{
	//		ShaderMaterial* _shaderMat = Assets.getShaderMaterial(m_shaderMaterial);
	//		if (_shaderMat)
	//		{
	//			ShaderProgram* program = Assets.getShaderProgram(_shaderMat->m_coreProgram);
	//			if (program)
	//			{
	//				// find uniform
	//				auto it = program->m_uniforms.find(name);
	//				if (it != program->m_uniforms.end())
	//				{
	//					return it->second.m_uniform;
	//				}
	//			}
	//		}
	//		
	//		// Empty
	//		Graphics::Uniform empty_uni;
	//		empty_uni.location = -1;
	//		return empty_uni;
	//	}

	//	ShaderProgram* Material::getProgram(ShaderMaterialType type) const
	//	{
	//		ShaderMaterial* _shaderMat = Assets.getShaderMaterial(m_shaderMaterial);
	//		if (_shaderMat)
	//		{
	//			return Assets.getShaderProgram(_shaderMat->m_coreProgram);
	//		}
	//		return nullptr;
	//	}


	ShaderProgram* Material::getProgram(ShaderMaterialType type)
	{
		ShaderMaterial* _shaderMaterial = Assets.getShaderMaterial(m_shaderMaterial);
		if (_shaderMaterial)
			return _shaderMaterial->getProgram(type);

		return nullptr;
	}

	bool Material::bindProgram(ShaderMaterialType type)
	{
		ShaderProgram* _program = getProgram(type);
		if (_program && _program->isLinked())
		{
			_program->bind();
			return true;
		}
		return false;
	}
	bool Material::bindProgramUniforms(ShaderMaterialType type, EntityIndex _entity)
	{
		ShaderProgram* _program = getProgram(type);
		if (_program)
		{
			_program->bindCommonUniforms(_entity);
			_program->bindCustomUniforms();
			return true;
		}
		return false;
	}
	void Material::bindProgramStates(ShaderMaterialType type)
	{
		// Gl States
		Graphics::SetCullMode(m_cullType);
		Graphics::SetDepthPassRule(m_depthFunc);
		Graphics::SetDepthRead(m_depthRead);
		Graphics::SetDepthWrite(m_depthWrite);
		Graphics::SetWireframeState(m_wireframe);

		// Gl States based on type
		if (type == ShaderMaterialType::CORE)
		{
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
		}
		else if (type == ShaderMaterialType::COLORID)
		{
			Graphics::SetBlendState(true);
			Graphics::SetBlendMode(BlendSource::ONE, BlendDestination::ZERO);
			Graphics::SetBlendEquation(BlendEquation::FUNC_ADD);
		}
		
		
	}
	void Material::bindTextures(ShaderMaterialType type, Entity* _entity)
	{
		if (m_wireframe)
			return;

		ShaderMaterial* _shaderMaterial = Assets.getShaderMaterial(m_shaderMaterial);
		if (_shaderMaterial)
		{
			// Acquire Target Levels
			std::vector<std::pair<std::string, TextureLevel>> _targetLevels;
			switch (type)
			{
			case ShaderMaterialType::CORE:
			{
				ShaderProgram* _program = Assets.getShaderProgram(_shaderMaterial->m_coreProgram);
				if (_program)
					_targetLevels = _program->m_targetLevels;
				break;
			}
			//
			case ShaderMaterialType::COLORID:
			{
				ShaderProgram* _program = Assets.getShaderProgram(_shaderMaterial->m_colorIDProgram);
				if (_program)
					_targetLevels = _program->m_targetLevels;
				break;
			}
			}
			
			// Acquire Textures that are stored
			std::map<TextureLevel, TextureIndex> _textures;
			
			if (_entity)
				_textures = _entity->m_textures;
			else
				_textures = m_textures;

			for (const auto& pair : _targetLevels)
			{
				TextureLevel level = pair.second;

				// Check if entity doesn't have the texture location
				if (_textures.find(level) == _textures.end())
				{
					if (level == TextureLevel::LEVEL0)
						GlobalAssets.get_Tex2DNullImageCheckerboard()->bind(level);
					else
						GlobalAssets.get_Tex2DNullImageBlack()->bind(level);
				}
				else
				{
					TextureIndex index = _textures[level];
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
	}

}