// Copyright (c) 2020 Emmanuel Lajeunesse
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
		friend class Inspector;
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

		//
		void setShaderMaterial(ShaderMaterialIndex index);
		ShaderMaterialIndex getShaderMaterial(void) const { return m_shaderMaterial; }

		//
		template<typename Type>
		void sendUniform(const std::string& name, Type data) const
		{
			ShaderMaterial* _shaderMat = Assets.getShaderMaterial(m_shaderMaterial);
			if (_shaderMat)
			{
				// Send uniform to all shaderMaterial programs
				ShaderProgram* program;
				program = Assets.getShaderProgram(_shaderMat->m_coreProgram);
				if (program)
					program->sendUniform(name, data, true);

				program = Assets.getShaderProgram(_shaderMat->m_colorIDProgram);
				if (program)
					program->sendUniform(name, data, true);
			}
		}
		
		//
		template<typename Type>
		void sendUniformMatrix(const std::string& name, Type data, bool transpose) const
		{
			ShaderMaterial* _shaderMat = Assets.getShaderMaterial(m_shaderMaterial);
			if (_shaderMat)
			{
				// Send uniform to all shaderMaterial programs
				ShaderProgram* program;
				program = Assets.getShaderProgram(_shaderMat->m_coreProgram);
				if (program)
					program->sendUniformMatrix(name, data, transpose, true);

				program = Assets.getShaderProgram(_shaderMat->m_colorIDProgram);
				if (program)
					program->sendUniformMatrix(name, data, transpose, true);
			}
		}

		//
		ShaderProgram* getProgram(ShaderMaterialType type);

		// Shader Binding
		bool bindProgram(ShaderMaterialType type);

		// Shader Uniform Binding
		bool bindProgramUniforms(ShaderMaterialType type, EntityIndex _entity);

		// GL States
		void bindProgramStates(ShaderMaterialType type);

		// Texture Binding
		void bindTextures(ShaderMaterialType type, Entity* _entity); // Entity nullptr = Material textures instead
	};

}