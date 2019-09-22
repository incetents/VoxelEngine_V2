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
	typedef std::map<std::string, Graphics::Attribute> AttributeStorage;
	typedef std::map<std::string, Graphics::Uniform> UniformStorage;
	typedef std::map<std::string, Graphics::UniformBlock> UniformBlockStorage;
	typedef std::map<ShaderType, Graphics::UniformSubroutine> SubroutineStorage;

	class Shader
	{
		friend class ShaderProgram;
		friend class RenderManager;
	private:
		bool				m_hasCompiled = false;
		ShaderID			m_id = -1;
		const std::string   m_name;
		std::string			m_sourceBackup; // has line numbers appended
		std::string			m_errorMessage;
		const ShaderType	m_type;
		
		bool compile(const std::string& source);
		void updateErrorMessage();

		void load(const std::string& shaderCode);
		void unload();

	public:

		// Constructor
		Shader(const std::string& name, const std::string& shaderCode, ShaderType type)
			: m_name(name + '[' + Graphics::Shader::GetName(type) + ']'), m_type(type)
		{
			load(shaderCode);

			if (!HasCompiled())
			{
				Logger.error("Shader [" + name + "] failed to compile");
				Logger.error(m_errorMessage);
			}
		}

		~Shader()
		{
			unload();
		}
		
		// Current log of all shaders with compilation errors
		static std::unordered_map<std::string, const Shader*> ShaderErrorLog;
		static UINT ShaderErrorLogSize;

		void Reload(const std::string& shaderCode)
		{
			unload();
			load(shaderCode);
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
		DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
		friend class RenderManager;
	private:
		// Program //
		const std::string    m_name;
		ShaderProgramID		 m_id = -1;
		bool				 m_linked = false;
		BYTE				 m_shaderCount : 3; // 3 bits, val is 8 max
		std::vector<Shader*> m_shaders;
		std::vector<std::string> m_filePaths;
		// Attributes //
		AttributeStorage	 m_attributes;
		// Uniforms //
		UniformStorage		 m_uniforms;
		UniformBlockStorage  m_uniformBlocks;
		SubroutineStorage    m_subroutines;
		bool				 m_usingSubroutines;
		// Error //			 
		std::string			 m_errorMessage;

		// Utility
		bool CreateProgram();
		void DestroyProgram();

		void ReloadShaders();
		void LoadShaders();
		void UnloadShaders();

		void Link();

		// Constructor
		ShaderProgram(
			const std::string& name,
			const std::vector<std::string>& filePaths
		);

	public:
		// Load
		static ShaderProgram* Load(
			const std::string& name,
			const std::vector<std::string>& filePaths
		);

		~ShaderProgram();

		static std::set<const ShaderProgram*> ProgramsFailed;
		static UINT ProgramsFailedSize;

		void Bind(void) const;
		static void Unbind(void);

		// Uniform

		// [Faster] Set uniform
		template<typename Type>
		void					SetUniform(const std::string& name, Type data)
		{
			//VXL_RETURN_ON_FAIL(m_uniforms.find(name) != m_uniforms.end(), "Uniform does not exist: " + name);
			m_uniforms[name].Send<Type>(data);
		}
		// [Faster] Set uniform (Custom call for matrix)
		template<typename Type>
		void					SetUniformMatrix(const std::string& name, Type data, bool transpose)
		{
			//VXL_RETURN_ON_FAIL(m_uniforms.find(name) != m_uniforms.end(), "Uniform does not exist: " + name);
			m_uniforms[name].SendMatrix<Type>(data, transpose);
		}
		// [Slower] Set uniform, regardless if shader is bound
		template<typename Type>
		void					SetProgramUniform(const std::string& name, Type data)
		{
			VXL_RETURN_ON_FAIL(m_uniforms.find(name) != m_uniforms.end(), "Uniform does not exist: " + name);
			m_uniforms[name].Send<Type>(m_id, data);
		}
		// [Slower] Set uniform, regardless if shader is bound (Custom call for matrix)
		template<typename Type>
		void					SetProgramUniformMatrix(const std::string& name, Type data, bool transpose)
		{
			VXL_RETURN_ON_FAIL(m_uniforms.find(name) != m_uniforms.end(), "Uniform does not exist: " + name);
			m_uniforms[name].SendMatrix<Type>(m_id, data, transpose);
		}
		
		inline const Graphics::Uniform& GetUniform(const std::string& name) const
		{
			VXL_ASSERT(CheckUniform(name), "Uniform does not exist for this shader: " + name);
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

		// Acquire Shaders
		inline std::vector<Shader*> GetShaders(void) const
		{
			return m_shaders;
		}

	};
}

