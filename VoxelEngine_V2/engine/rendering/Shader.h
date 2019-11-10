// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once
//#include "Enums.h"

#include "../utilities/Types.h"
#include "../utilities/Macros.h"

//#include <GL/gl3w.h>

#include "Uniform.h"

#include <string>
#include <vector>
#include <map>

namespace Vxl
{
	typedef std::map<std::string, Graphics::Attribute> AttributeStorage;
	typedef std::map<std::string, Graphics::Uniform> UniformStorage;
	typedef std::map<std::string, Graphics::UniformBlock> UniformBlockStorage;
	typedef std::map<ShaderType, Graphics::UniformSubroutine> SubroutineStorage;

	class _Shader
	{
		DISALLOW_COPY_AND_ASSIGN(_Shader);
		friend class Assets;
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
		static std::map<ShaderID, _Shader*> m_brokenShaders;

		_Shader(const std::string& name, const std::string& shaderCode, ShaderType type);
	public:
		~_Shader();

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

	class _ShaderProgram
	{
		DISALLOW_COPY_AND_ASSIGN(_ShaderProgram);
		friend class Assets;
		friend class ShaderErrors;
		friend class ShaderCodeViewer;
		friend class _Material;
	private:
		ShaderProgramID				m_id;
		const std::string			m_name;
		bool						m_linked;
		std::vector<_Shader*>		m_shaders;
		AttributeStorage			m_attributes;
		UniformStorage				m_uniforms;
		UniformBlockStorage			m_uniformBlocks;
		SubroutineStorage			m_subroutines;
		std::string					m_errorMessage;
		static std::map<ShaderProgramID, _ShaderProgram*> m_brokenShaderPrograms;

		_ShaderProgram(const std::string& name, const std::vector<_Shader*>& _shaders);
	public:
		~_ShaderProgram();

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
		inline std::vector<_Shader*>	getShaders(void) const
		{
			return m_shaders;
		}
	};

}

