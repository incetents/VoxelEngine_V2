// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once
#include "Enums.h"

#include "../utilities/Asset.h"
#include "../utilities/Macros.h"

#include "Uniform.h"

#include <string>
#include <vector>
#include <map>

namespace Vxl
{
	typedef std::map<std::string, glUniform> UniformStorage;
	typedef std::map<std::string, glUniformBlock> UniformBlockStorage;
	typedef std::map<ShaderType, glSubroutine> SubroutineStorage;

	class Shader : public Asset<Shader>
	{
		friend class Loader;
		friend class ShaderProgram;
		friend class RenderManager;
	private:
		bool				m_hasLoaded = false;
		bool				m_hasCompiled = false;
		GLuint				m_id = -1;
		const std::string   m_name;
		const std::string	m_filePath;
		std::string			m_sourceBackup; // has line numbers appended
		std::string			m_errorMessage;
		const ShaderType	m_type;
		
		bool compile(const std::string& source);
		bool checkError() const;
		void readError();

		bool load();
		void unload();

		// Constructor
		Shader(const std::string& name, const std::string& filePath, ShaderType type)
			: m_name(name + '[' + glUtil::shaderTypeToString(type) + ']'), m_filePath(filePath), m_type(type)
		{
			m_hasLoaded = load();
		}

	public:
		// Load
		static Shader* Load(
			const std::string& name,
			const std::string& filePath,
			ShaderType type
		);

		~Shader()
		{
			unload();
		}
		
		// Current log of all shaders with compilation errors
		static std::unordered_map<std::string, const Shader*> ShaderErrorLog;
		static UINT ShaderErrorLogSize;

		void reload()
		{
			unload();
			m_hasLoaded = load();
		}

		inline bool					HasLoaded(void) const
		{
			return m_hasLoaded;
		}
		inline bool					HasCompiled(void) const
		{
			return m_hasCompiled;
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
		inline const std::string&	GetErrorMessage(void) const
		{
			return m_errorMessage;
		}
		inline const std::string&	GetCompiledCode(void) const
		{
			return m_sourceBackup;
		}

		// Remove default
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
	};

	class ShaderProgram : public Asset<ShaderProgram>
	{
		friend class Loader;
		friend class RenderManager;
	private:
		// Program //
		const std::string   m_name;
		GLuint				m_id = -1;
		bool				m_linked = false;
		BYTE				m_shaderCount : 3; // 3 bits, val is 8 max
		std::vector<Shader*> m_shaders;
		// Uniform //
		UniformStorage		m_uniforms;
		GLint				m_uniformCount = 0;
		UniformBlockStorage m_uniformBlocks;
		GLint				m_uniformBlockCount = 0;
		SubroutineStorage   m_subroutines;
		GLint				m_subroutineCount = 0;
		// Error //
		std::string			m_errorMessage;
		// Tracker //
		static GLuint		m_boundID;

		// Utility
		bool createProgram();
		void destroyProgram();

		void attachShaders();
		void detachShaders();

		void AddShader(Shader* _shader);
		void Link();

		void acquireUniforms();
		void acquireUniformBlocks();
		void acquireSubroutines();

		bool checkError();

		// Constructor
		ShaderProgram(const std::string& name)
			: m_name(name)
		{
			createProgram();
		}

	public:
		// Load
		static ShaderProgram* Load(
			const std::string& name,
			std::vector<std::string> shaders
		);

		~ShaderProgram()
		{
			detachShaders();
			destroyProgram();
		}

		static std::set<const ShaderProgram*> ProgramsFailed;
		static UINT ProgramsFailedSize;

		void Bind(void) const;
		static void Unbind(void);

		// Uniform

		// [Faster] Set uniform
		template<typename Type>
		void					SetUniform(const std::string& name, Type data)
		{
#if _DEBUG
			// Check if uniform is missing
			if (m_uniforms.find(name) == m_uniforms.end())
				return;
#endif
			m_uniforms[name].Set<Type>(data);
		}
		// [Faster] Set uniform (Custom call for matrix)
		template<typename Type>
		void					SetUniformMatrix(const std::string& name, Type data, bool transpose)
		{
#if _DEBUG
			// Check if uniform is missing
			if (m_uniforms.find(name) == m_uniforms.end())
				return;
#endif
			m_uniforms[name].SetMatrix<Type>(data, transpose);
		}
		// [Slower] Set uniform, regardless if shader is bound
		template<typename Type>
		void					SetProgramUniform(const std::string& name, Type data)
		{
#if _DEBUG
			// Check if uniform is missing
			if (m_uniforms.find(name) == m_uniforms.end())
				return;
#endif

			m_uniforms[name].Set<Type>(m_id, data);
		}
		// [Slower] Set uniform, regardless if shader is bound (Custom call for matrix)
		template<typename Type>
		void					SetProgramUniformMatrix(const std::string& name, Type data, bool transpose)
		{
#if _DEBUG
			// Check if uniform is missing
			if (m_uniforms.find(name) == m_uniforms.end())
				return;
#endif
			m_uniforms[name].SetMatrix<Type>(m_id, data, transpose);
		}
		
		inline const glUniform& GetUniform(const std::string& name)
		{
#if _DEBUG
			VXL_ASSERT(CheckUniform(name), "Uniform does not exist for this shader");
#endif
			return m_uniforms[name];
		}
		inline bool				CheckUniform(const std::string& name)
		{
			return (m_uniforms.find(name) != m_uniforms.end());
		}
		inline GLuint			GetUniformCount(void) const
		{
			return m_uniformCount;
		}
		const UniformStorage	GetAllUniforms(void) const
		{
			return m_uniforms;
		}


		// Uniform Blocks
		inline glUniformBlock   GetUniformBlock(const std::string& name)
		{
#if _DEBUG
			VXL_ASSERT(m_uniformBlocks.find(name) != m_uniformBlocks.end(), "Uniform block does not exist");
#endif
			return m_uniformBlocks[name];
		}
		inline GLuint			GetUniformBlockCount(void) const
		{
			return m_uniformBlockCount;
		}

		// Subroutines
		inline glSubroutine*	GetSubroutine(ShaderType type)
		{
#if _DEBUG
			VXL_ASSERT(m_subroutines.find(type) != m_subroutines.end(), "Uniform subroutine does not exist");
#endif
			return &m_subroutines[type];
		}
		inline GLuint			GetSubroutineCount(void) const
		{
			return m_subroutineCount;
		}

		// Misc
		inline bool		IsLinked(void) const
		{
			return m_linked;
		}
		inline GLuint	GetID(void) const
		{
			return m_id;
		}
		inline const std::string& GetName(void) const
		{
			return m_name;
		}
		inline std::string GetErrorMessage(void) const
		{
			return m_errorMessage;
		}

		// Remove default
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
	};
}

