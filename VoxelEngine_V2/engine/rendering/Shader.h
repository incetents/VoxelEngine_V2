// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Types.h"
#include "../utilities/Macros.h"

#include "Uniform.h"

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace Vxl
{
	typedef std::map<std::string, Graphics::Attribute> AttributeStorage;
	typedef std::map<std::string, Graphics::Uniform> UniformStorage;
	typedef std::map<std::string, Graphics::UniformBlock> UniformBlockStorage;
	typedef std::map<ShaderType, Graphics::UniformSubroutine> SubroutineStorage;

	class Entity;

	class Shader
	{
		DISALLOW_COPY_AND_ASSIGN(Shader);
		friend class Assets;
		friend class ShaderMaterial;
		friend class ShaderErrors;
		friend class ShaderCodeViewer;
	private:
		ShaderID			m_id;
		const std::string   m_name;
		bool				m_compiled;
		const ShaderType	m_type;
		std::string			m_source;
		std::string			m_sourceLines;
		std::string			m_errorMessage;
		static std::map<ShaderID, Shader*> m_brokenShaders;

		Shader(const std::string& name, const std::string& shaderCode, ShaderType type);
	public:
		~Shader();

		// Utility
		void setGLName(const std::string& name);

		// Getters
		inline bool				isCompiled(void) const
		{
			return m_compiled;
		}
		inline uint32_t			getID(void) const
		{
			return m_id;
		}
		inline ShaderType		getType(void) const
		{
			return m_type;
		}
		inline std::string_view	getErrorMessage(void) const
		{
			return m_errorMessage;
		}
		inline std::string_view	getSourceCode(void) const
		{
			return m_source;
		}
	};

	class ShaderProgram
	{
		DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
		friend class Assets;
		friend class ShaderErrors;
		friend class ShaderCodeViewer;
		friend class Material;
	private:
		// Core Data
		ShaderProgramID				m_id;
		const std::string			m_name;
		bool						m_linked;
		std::vector<Shader*>		m_shaders;
		AttributeStorage			m_attributes;
		UniformStorage				m_uniforms;
		UniformBlockStorage			m_uniformBlocks;
		SubroutineStorage			m_subroutines;
		std::string					m_errorMessage;
		static std::map<ShaderProgramID, ShaderProgram*> m_brokenShaderPrograms;

		void setupCommonUniform(const std::string& name, std::optional<Graphics::Uniform>& uniform);

		ShaderProgram(const std::string& name, const std::vector<Shader*>& _shaders);
	public:
		~ShaderProgram();

		//
		void bind(void) const;
		static void unbind(void);

		// Utility
		void setGLName(const std::string& name);

		// Uniforms
		template<typename Type>
		void sendUniform(const std::string& name, Type data)
		{
			auto it = m_uniforms.find(name);
			if (it != m_uniforms.end()) {
				it->second.send(data);
			}
		}
		template<typename Type>
		void sendUniformMatrix(const std::string& name, Type data, bool transpose)
		{
			auto it = m_uniforms.find(name);
			if (it != m_uniforms.end()) {
				it->second.sendMatrix(data);
			}
		}

		// Common Uniforms
		std::optional<Graphics::Uniform> m_uniform_useModel = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_model = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_mvp = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_normalMatrix = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_useInstancing = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_useTexture = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_color = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_tint = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_viewport = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_alpha = std::nullopt;
		std::optional<Graphics::Uniform> m_uniform_output = std::nullopt;

		// Binding Common Uniforms
		void bindCommonUniforms(EntityIndex _entity);

		// Getters
		inline bool						isLinked(void) const
		{
			return m_linked;
		}
		inline uint32_t					getID(void) const
		{
			return m_id;
		}
		inline const std::string&		getName(void) const
		{
			return m_name;
		}
		inline std::string_view			getErrorMessage(void) const
		{
			return m_errorMessage;
		}
		inline std::vector<Shader*>		getShaders(void) const
		{
			return m_shaders;
		}
	};

	class ShaderMaterial
	{
		DISALLOW_COPY_AND_ASSIGN(ShaderMaterial);
		friend class Assets;
		friend class RenderManager;
	private:
		ShaderMaterial(const std::string& filePath, bool GlobalAsset);
		void reload(bool GlobalAsset);
	public:
		const std::string			m_filePath;			// File used to load
		std::vector<TextureLevel>	m_targetLevels;		// Textures used in program
		ShaderProgramIndex			m_coreProgram;		// Main Program used for rendering
		ShaderProgramIndex			m_colorIDProgram;	// Alternate program used only for ColorID output
		ShaderProgramIndex			m_depthOnlyProgram;	// Alternate program only outputting depth
	};
}

