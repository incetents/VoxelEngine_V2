// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Component.h"

#include "../rendering/Graphics.h"

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
		friend class Assets;
	private:
		// Data
		std::string					m_name;
		ShaderProgramIndex			m_shader = -1;
		uint32_t					m_sequenceNumber = -1;
		static std::set<uint32_t>	m_allSequenceNumbers;
		std::map<TextureLevel, TextureIndex> m_textures;
		std::vector<TextureLevel> m_targetLevels; // What the shader is expecting for textures

		// Common Uniforms
		void setupCommonUniform(const ShaderProgram& program, const std::string& name, std::optional<Graphics::Uniform>& uniform);

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
		Material(const std::string& name) 
			: m_name(name)
		{}

	public:
		~Material()
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

		// Set Shader
		void setShader(ShaderProgramIndex index);

		// Set Uniform
		template<typename Type>
		void sendUniform(const std::string& name, Type data) const
		{
			ShaderProgram* program = Assets::getShaderProgram(m_shader);
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
			ShaderProgram* program = Assets::getShaderProgram(m_shader);
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
		bool bindProgram();
		void bindStates();
		void bindTextures(); // Bind material textures
		void bindTextures(Entity* _entity); // Bind entity textures
		void bindCommonUniforms(Entity* _entity);
	};

}