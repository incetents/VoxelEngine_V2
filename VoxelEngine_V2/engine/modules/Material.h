// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Component.h"

#include "../rendering/Graphics.h"
#include "../rendering/Shader.h"
#include "../utilities/Types.h"
#include "../utilities/Asset.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <map>
#include <string>
//

#define MAX_MATERIAL_TEXTURES 8

namespace Vxl
{
	class ShaderProgram;
	class BaseTexture;
	class Entity;

	enum class MaterialRenderMode
	{
		Opaque,
		Transparent
	};

	class Material
	{
		DISALLOW_COPY_AND_ASSIGN(Material);
		friend class _Assets;
		friend class ShaderProgram;
	private:
		// Data
		std::string					m_name;
		ShaderMaterialIndex			m_shaderMaterial = -1;
		uint32_t					m_sequenceNumber = -1;
		static std::set<uint32_t>	m_allSequenceNumbers;
		std::map<TextureLevel, TextureIndex> m_textures;

		// Protected
		Material(const std::string& name) 
			: m_name(name)
		{}

	public:
		~Material();

		bool setSequenceID(uint32_t sequence);
		void eraseSequenceID(void);
		uint32_t getSequenceID(void) const
		{
			return m_sequenceNumber;
		}

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

		// Set ShaderMaterial
		void setShaderMaterial(ShaderMaterialIndex index);
		ShaderMaterialIndex getShaderMaterial(void) const { return m_shaderMaterial; }

		// Set Uniform
		template<typename Type>
		void sendUniform(const std::string& name, Type data) const
		{
			ShaderMaterial* _shaderMat = Assets.getShaderMaterial(m_shaderMaterial);
			if (_shaderMat)
			{
				ShaderProgram* program = Assets.getShaderProgram(_shaderMat->m_coreProgram);
				if (program)
				{
					program->sendUniform(name, data, true);
				}
			}
		}
		
		// Set Uniform Matrix
		template<typename Type>
		void sendUniformMatrix(const std::string& name, Type data, bool transpose) const
		{
			ShaderMaterial* _shaderMat = Assets.getShaderMaterial(m_shaderMaterial);
			if (_shaderMat)
			{
				ShaderProgram* program = Assets.getShaderProgram(_shaderMat->m_coreProgram);
				if (program)
				{
					program->sendUniformMatrix(name, data, transpose, true);
				}
			}
		}

		// Get Core Program
		ShaderProgram* getCoreProgram(void) const;

		// Behaviour
		bool bindCoreProgram();
		void bindCoreProgramUniforms(EntityIndex _entity);
		void bindStates();
		void bindTextures(); // bind material textures
		void bindTextures(Entity* _entity); // bind entity textures
	};

}