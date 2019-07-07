// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once
//#include "Enums.h"

#include "../utilities/Asset.h"
#include "../utilities/Macros.h"

//#include <GL/gl3w.h>

#include "Uniform.h"

#include <string>
#include <vector>
#include <map>

namespace Vxl
{
	typedef std::map<std::string, Graphics::Uniform> UniformStorage;
	typedef std::map<std::string, Graphics::UniformBlock> UniformBlockStorage;
	typedef std::map<ShaderType, Graphics::UniformSubroutine> SubroutineStorage;

	class Shader : public Asset<Shader>
	{
		friend class ShaderProgram;
		friend class RenderManager;
	private:
		bool				m_hasLoaded = false;
		bool				m_hasCompiled = false;
		ShaderID			m_id = -1;
		const std::string   m_name;
		const std::string	m_filePath;
		std::string			m_sourceBackup; // has line numbers appended
		std::string			m_errorMessage;
		const ShaderType	m_type;
		
		bool compile(const std::string& source);
		void updateErrorMessage();

		bool load();
		void unload();

		// Constructor
		Shader(const std::string& name, const std::string& filePath, ShaderType type)
			: m_name(name + '[' + Graphics::Shader::GetName(type) + ']'), m_filePath(filePath), m_type(type)
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
		inline uint32_t				GetID(void) const
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
		friend class RenderManager;
	private:
		// Program //
		const std::string    m_name;
		ShaderProgramID		 m_id = -1;
		bool				 m_linked = false;
		BYTE				 m_shaderCount : 3; // 3 bits, val is 8 max
		std::vector<Shader*> m_shaders;
		// Uniforms //
		UniformStorage		 m_uniforms;
		UniformBlockStorage  m_uniformBlocks;
		SubroutineStorage    m_subroutines;
		bool				 m_usingSubroutines;
		// Error //			 
		std::string			 m_errorMessage;
		// Tracker //		 
		static uint32_t		 m_boundID;

		// Utility
		bool createProgram();
		void destroyProgram();

		void attachShaders();
		void detachShaders();

		void Link();

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
			const std::vector<Shader*>& shaders
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
		static UINT GetBoundProgram(void);

		// Uniform

		// [Faster] Set uniform
		template<typename Type>
		void					SetUniform(const std::string& name, Type data)
		{
			VXL_RETURN_ON_FAIL(m_uniforms.find(name) != m_uniforms.end());
			m_uniforms[name].Send<Type>(data);
		}
		// [Faster] Set uniform (Custom call for matrix)
		template<typename Type>
		void					SetUniformMatrix(const std::string& name, Type data, bool transpose)
		{
			VXL_RETURN_ON_FAIL(m_uniforms.find(name) != m_uniforms.end());
			m_uniforms[name].SendMatrix<Type>(data, transpose);
		}
		// [Slower] Set uniform, regardless if shader is bound
		template<typename Type>
		void					SetProgramUniform(const std::string& name, Type data)
		{
			VXL_RETURN_ON_FAIL(m_uniforms.find(name) != m_uniforms.end());
			m_uniforms[name].Send<Type>(m_id, data);
		}
		// [Slower] Set uniform, regardless if shader is bound (Custom call for matrix)
		template<typename Type>
		void					SetProgramUniformMatrix(const std::string& name, Type data, bool transpose)
		{
			VXL_RETURN_ON_FAIL(m_uniforms.find(name) != m_uniforms.end());
			m_uniforms[name].SendMatrix<Type>(m_id, data, transpose);
		}
		
		inline const Graphics::Uniform& GetUniform(const std::string& name) const
		{
			VXL_ASSERT(CheckUniform(name), "Uniform does not exist for this shader");
			return m_uniforms.at(name);
		}
		inline bool				CheckUniform(const std::string& name) const
		{
			return (m_uniforms.find(name) != m_uniforms.end());
		}
		inline uint32_t			GetUniformCount(void) const
		{
			return (uint32_t)m_uniforms.size();
		}
		const UniformStorage	GetAllUniforms(void) const
		{
			return m_uniforms;
		}


		// Uniform Blocks
		inline const Graphics::UniformBlock& GetUniformBlock(const std::string& name) const
		{
			VXL_ASSERT(m_uniformBlocks.find(name) != m_uniformBlocks.end(), "Uniform block does not exist");
			return m_uniformBlocks.at(name);
		}
		inline uint32_t			GetUniformBlockCount(void) const
		{
			return (uint32_t)m_uniformBlocks.size();
		}

		// Subroutines
		inline Graphics::UniformSubroutine&	GetSubroutine(ShaderType type)
		{
			VXL_ASSERT(m_subroutines.find(type) != m_subroutines.end(), "Uniform subroutine does not exist");
			return m_subroutines[type];
		}
		inline uint32_t			GetSubroutineCount(void) const
		{
			return (uint32_t)m_subroutines.size();
		}

		// Misc
		inline bool		IsLinked(void) const
		{
			return m_linked;
		}
		inline uint32_t	GetID(void) const
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

