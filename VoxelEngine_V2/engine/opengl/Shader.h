// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once
#include "Enums.h"

#include "../utilities/Database.h"

#include "Uniform.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Vxl
{
	typedef std::unordered_map<std::string, glUniform> UniformStorage;
	typedef std::unordered_map<std::string, glUniformBlock> UniformBlockStorage;
	typedef std::unordered_map<ShaderType, glSubroutine> SubroutineStorage;

	class Shader
	{
		friend class Loader;
	private:
		bool				m_fail = true;
		GLuint				m_id = -1;
		const std::string   m_name;
		const ShaderType	m_type;
		
		bool compile(const std::string& source);
		bool checkError() const;

	public:
		Shader(const std::string& name, const std::string& source, ShaderType type);
		~Shader();
		// Database
		static Database<Shader> m_database;

		inline bool					HasFailed(void) const
		{
			return m_fail;
		}
		inline GLuint				GetID(void) const
		{
			return m_id;
		}
		inline ShaderType			GetType(void) const
		{
			return m_type;
		}
		inline const std::string&	GetName(void) const
		{
			return m_name;
		}

		// Remove default
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
	};

	class ShaderProgram
	{
		friend class Loader;
	private:
		// Program //
		bool				m_linked = false;
		GLuint				m_id = -1;
		const std::string   m_name;
		BYTE				m_shaderCount : 3; // 3 bits, val is 8 max
		std::vector<Shader*> m_shaders;
		// Uniform //
		UniformStorage		m_uniforms;
		GLint				m_uniformCount = 0;
		UniformBlockStorage m_uniformBlocks;
		GLint				m_uniformBlockCount = 0;
		SubroutineStorage   m_subroutines;
		GLint				m_subroutineCount = 0;
		
		void attachShaders();
		void detachShaders();

		void acquireUniforms();
		void acquireUniformBlocks();
		void acquireSubroutines();

		bool checkError();

	public:
		ShaderProgram(const std::string& name);
		~ShaderProgram();
		// Database
		static Database<ShaderProgram> m_database;

		void AddShader(Shader* _shader);
		void Link();

		void Bind();
		static void Unbind();

		// Uniform
		template<typename Type>
		void					SetUniform(const std::string& name, Type data)
		{
			m_uniforms[name].set<Type>(data);
		}
		inline const glUniform& GetUniform(const std::string& name)
		{
#if _DEBUG
			assert(m_uniforms.find(name) != m_uniforms.end());
#endif
			return m_uniforms[name];
		}
		inline GLuint			GetUniformCount(void) const
		{
			return m_uniformCount;
		}
		// Uniform Blocks
		inline glUniformBlock*  GetUniformBlock(const std::string& name)
		{
#if _DEBUG
			assert(m_uniformBlocks.find(name) != m_uniformBlocks.end());
#endif
			return &m_uniformBlocks[name];
		}
		inline GLuint			GetUniformBlockCount(void) const
		{
			return m_uniformBlockCount;
		}
		// Subroutines
		inline glSubroutine*	GetSubroutine(ShaderType type)
		{
#if _DEBUG
			assert(m_subroutines.find(type) != m_subroutines.end());
#endif
			return &m_subroutines[type];
		}
		inline GLuint			GetSubroutineCount(void) const
		{
			return m_subroutineCount;
		}

		inline bool		IsLinked(void) const
		{
			return m_linked;
		}
		inline GLuint	GetID(void) const
		{
			return m_id;
		}

		// Remove default
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
	};
}

