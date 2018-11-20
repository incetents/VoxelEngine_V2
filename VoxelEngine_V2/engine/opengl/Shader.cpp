// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Shader.h"

#include <GL/gl3w.h>

#include "../utilities/logger.h"
#include "../utilities/stringUtil.h"

namespace Vxl
{
	// STATIC //
	Database<Shader> Shader::m_database;
	Database<ShaderProgram> ShaderProgram::m_database;

	// SHADER //

	Shader::Shader(const std::string& name, const std::string& source, ShaderType type)
		: m_name(name), m_type(type)
	{
		m_id = glCreateShader((GLuint)type);
		if (m_id == -1)
		{
			Logger.error("Unable to create shader: " + name);
			return;
		}

		if (!compile(source))
			Logger.error("Unable to compile shader: " + name);
	}

	Shader::~Shader()
	{
		glDeleteShader(m_id);
	}

	bool Shader::compile(const std::string& source)
	{
		// Convert file to const char
		const GLchar* Source = static_cast<const GLchar *>(source.c_str());

		// Attach Shader and Compile
		glShaderSource(m_id, 1, &Source, NULL);
		glCompileShader(m_id);

		// Error Check
		m_fail = checkError();

		return !m_fail;
	}

	bool Shader::checkError() const
	{
		GLint result;
		glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);

		// No errors = exit
		if (result != GL_FALSE)
			return false;

		Logger.error("Failed to Compile Shader: " + m_name);

		// Output Error Type
		switch (m_type)
		{
		case ShaderType::VERTEX:
			Logger.error("(VERTEX SHADER) : ");
			break;
		case ShaderType::FRAGMENT:
			Logger.error("(FRAGMENT SHADER) : ");
			break;
		case ShaderType::GEOMETRY:
			Logger.error("(GEOMETRY SHADER) : ");
			break;
		case ShaderType::TESSELATION_CONTROL:
			Logger.error("(TESSELATION CONTROL SHADER) : ");
			break;
		case ShaderType::TESSELATION_EVALUATION:
			Logger.error("(TESSELATION EVALUATION SHADER) : ");
			break;
		case ShaderType::COMPUTE:
			Logger.error("(COMPUTE SHADER) : ");
			break;
		default:
			Logger.error("(UNKNOWN SHADER) : ");
			break;
		}

		// Output Error Message
		GLint length;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> error(length);
		glGetShaderInfoLog(m_id, length, &length, &error[0]);

		Logger.error(&error[0]);

		return true;
	}

	// SHADER PROGRAM //

	ShaderProgram::ShaderProgram(const std::string& name)
		: m_shaderCount(0), m_name(name)
	{
		m_id = glCreateProgram();
		if (m_id == -1)
		{
			Logger.error("Unable to create shader program " + name);
			return;
		}

		attachShaders();
	}

	ShaderProgram::~ShaderProgram()
	{
		detachShaders();
		glDeleteProgram(m_id);
		m_id = -1;

		// cleanup uniform blocks
		for (auto it = m_uniformBlocks.begin(); it != m_uniformBlocks.end(); it++)
			delete it->second;

	}

	void ShaderProgram::attachShaders()
	{
		for (unsigned int i = 0; i < m_shaderCount; i++)
		{
			glAttachShader(m_id, m_shaders[i]->GetID());
		}
	}

	void ShaderProgram::detachShaders()
	{
		for (unsigned int i = 0; i < m_shaderCount; i++)
		{
			glDetachShader(m_id, m_shaders[i]->GetID());
		}
	}

	bool ShaderProgram::checkError()
	{
		GLint isLinked = 0;
		glGetProgramiv(m_id, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_TRUE)
			return false;

		GLint maxLength = 0;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength <= 0)
			Logger.error("Could not retrieve program compilation log");
		else
		{
			//The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);

			Logger.error(&infoLog[0]);
		}
		return true;
	}

	void ShaderProgram::AddShader(Shader* _shader)
	{
		m_shaders.push_back(_shader);
		m_shaderCount++;
	}

	void ShaderProgram::acquireUniforms()
	{
		m_uniforms.clear();

		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)

		const GLsizei bufSize = 128; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length

		glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &m_uniformCount);
		//printf("Active Uniforms: %d\n", m_uniformCount);

		for (GLint i = 0; i < m_uniformCount; i++)
		{
			glGetActiveUniform(m_id, (GLuint)i, bufSize, &length, &size, &type, name);
			//printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
			glUniform U(glGetUniformLocation(m_id, name));
			m_uniforms[std::string(name)] = U;
		}
	}

	void ShaderProgram::acquireUniformBlocks()
	{
		GLint m_uniformBlockCount;
		glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_BLOCKS, &m_uniformBlockCount);
		
		const GLsizei bufSize = 128; // maximum name length

		for (int blockIx = 0; blockIx < m_uniformBlockCount; ++blockIx)
		{
			GLint nameLen;
			glGetActiveUniformBlockiv(m_id, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
			
			GLchar name[bufSize];
			glGetActiveUniformBlockName(m_id, blockIx, nameLen, NULL, &name[0]);
			std::string name_str(name);
		
			GLint size;
			glGetActiveUniformBlockiv(m_id, blockIx, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

			// Get Binding point from name (ex: "ColorBlock_17" => Binding Point is 17)
			GLuint bindingPoint = 0;
			std::string bindingPointStr = "";
			size_t loc = name_str.find_last_of('_');
			if (loc != -1)
				bindingPointStr = name_str.substr(loc + 1, nameLen);
			if (!bindingPointStr.empty())
				bindingPoint = std::stoi(bindingPointStr);
		
			m_uniformBlocks[name_str] = new glUniformBlock(*this, name_str, bindingPoint, (GLuint)size);
		}
	}

	void ShaderProgram::acquireSubroutines()
	{
		m_subroutines.clear();
		m_subroutineCount = 0;

		char name[256]; int len, numCompS;

		for (int s = 0; s < m_shaderCount; s++)
		{
			GLint subroutineCount;
			GLenum type = (GLenum)m_shaders[s]->GetType();

			glGetProgramStageiv(m_id, type, GL_ACTIVE_SUBROUTINE_UNIFORMS, &subroutineCount);

			if (subroutineCount > 0)
			{
				auto& Subroutine = m_subroutines[m_shaders[s]->GetType()];
				Subroutine = glSubroutine(m_shaders[s], subroutineCount);

				m_subroutineCount++;

				for (int i = 0; i < subroutineCount; ++i)
				{
					glGetActiveSubroutineUniformName(m_id, type, i, 256, &len, name);

					//printf("Suroutine Uniform: %d name: %s\n", i, name);
					glGetActiveSubroutineUniformiv(m_id, type, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);

					Subroutine.m_uniforms[name] = i;

					int *s = (int *)malloc(sizeof(int) * numCompS);
					glGetActiveSubroutineUniformiv(m_id, type, i, GL_COMPATIBLE_SUBROUTINES, s);
					//printf("Compatible Subroutines:\n");
					for (int j = 0; j < numCompS; ++j) {

						glGetActiveSubroutineName(m_id, type, s[j], 256, &len, name);
						//printf("\t%d - %s\n", s[j], name);

						Subroutine.m_functions[name] = glGetSubroutineIndex(m_id, type, name);
					}
					//printf("\n");
					free(s);
				}
			}
		}
	}

	void ShaderProgram::Link()
	{
		if (m_linked)
			return;

		attachShaders();

		glLinkProgram(m_id);
		glValidateProgram(m_id);

		m_linked = !checkError();

		if (m_linked)
		{
			acquireUniforms();
			acquireUniformBlocks();
			acquireSubroutines();
		}
	}

	void ShaderProgram::Bind()
	{
		glUseProgram(m_id);
	}

	void ShaderProgram::Unbind()
	{
		glUseProgram(0);
	}
}