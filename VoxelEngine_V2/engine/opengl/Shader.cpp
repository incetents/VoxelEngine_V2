// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Shader.h"

#include <GL/gl3w.h>

#include "../utilities/logger.h"
#include "../utilities/FileIO.h"

namespace Vxl
{
	// SHADER //
	std::unordered_map<std::string, const Shader*> Shader::ShaderErrorLog;
	UINT Shader::ShaderErrorLogSize = 0;

	std::set<const ShaderProgram*> ShaderProgram::ProgramsFailed;
	UINT ShaderProgram::ProgramsFailedSize = 0;

	bool Shader::compile(const std::string& source)
	{
		// Convert file to const char
		const GLchar* Source = static_cast<const GLchar *>(source.c_str());

		// Keep backup with line numbers
		m_sourceBackup.clear();
		auto lines = stringUtil::splitStr(source, '\n');
		//m_sourceBackup = source;
		int lineNumber = 0;
		for (auto line : lines)
		{
			m_sourceBackup += std::to_string(++lineNumber) + '\t' + line + '\n';
		}

		// Attach Shader and Compile
		glShaderSource(m_id, 1, &Source, NULL);
		glCompileShader(m_id);

		// Error Check
		m_hasCompiled = !checkError();

		if (!m_hasCompiled)
		{
			// update error message
			readError();

			// for logging system
			Logger.error(m_errorMessage);

			// store in map of shaders with errors
			ShaderErrorLog[m_name] = this;
			ShaderErrorLogSize++;
		}

		return m_hasCompiled;
	}

	bool Shader::checkError() const
	{
		GLint result;
		glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);
		return result == GL_FALSE;
	}
	void Shader::readError()
	{
		m_errorMessage.clear();
		m_errorMessage = "Failed to Compile Shader\n\n";
		m_errorMessage += "Name: " + m_name + "\n";
		m_errorMessage += "Path: " + m_filePath + "\n";

		switch (m_type)
		{
		case ShaderType::VERTEX:
			m_errorMessage += ("Type: VERTEX\n");
			break;
		case ShaderType::FRAGMENT:
			m_errorMessage += ("Type: FRAGMENT\n");
			break;
		case ShaderType::GEOMETRY:
			m_errorMessage += ("Type: GEOMETRY\n");
			break;
		case ShaderType::TESSELATION_CONTROL:
			m_errorMessage += ("Type: TESSELATION CONTROL\n");
			break;
		case ShaderType::TESSELATION_EVALUATION:
			m_errorMessage += ("Type: TESSELATION EVALUATION\n");
			break;
		case ShaderType::COMPUTE:
			m_errorMessage += ("Type: COMPUTE SHADER\n");
			break;
		default:
			m_errorMessage += ("Type: UNKNOWN SHADER\n");
			break;
		}
		m_errorMessage += ("\nERROR:\n");

		// Output Error Message
		GLint length;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> error(length);
		glGetShaderInfoLog(m_id, length, &length, &error[0]);
		m_errorMessage.insert(m_errorMessage.end(), error.begin(), error.end());
	}

	bool Shader::load()
	{
		// Create Shader ID
		m_id = glCreateShader((GLuint)m_type);
		if (m_id == -1)
		{
			Logger.error("Unable to create shader: " + m_name);
			return false;
		}

		// Read file
		std::string source = IO::readFile(m_filePath);
		source = IO::addInclude(source, m_filePath);

		// Compile Source
		if (!compile(source))
		{
			Logger.error("Unable to compile shader: " + m_name);
			return false;
		}

		// set gl name
		glUtil::setGLName(glNameType::SHADER, m_id, "Shader_" + m_name);

		return true;
	}

	void Shader::unload()
	{
		glDeleteShader(m_id);
		m_id = -1;
		m_hasCompiled = false;
	}

	Shader* Shader::Load(const std::string& name, const std::string& filePath, ShaderType type)
	{
		Shader* _shader = new Shader(name, filePath, type);

		AddToDatabase(name, _shader);

		if(_shader == nullptr)
			return false;
		else if (!_shader->HasLoaded() || !_shader->HasCompiled())
			return false;

		Message_Created(name, _shader);

		return _shader;
	}

	// SHADER PROGRAM //

	GLuint ShaderProgram::m_boundID = 0;

	bool ShaderProgram::createProgram()
	{
		m_id = glCreateProgram();
		if (m_id == -1)
		{
			Logger.error("Unable to create shader program " + m_name);
			return false;
		}
		return true;
	}

	void ShaderProgram::destroyProgram()
	{
		glDeleteProgram(m_id);
		m_id = -1;
		m_linked = false;
	}

	void ShaderProgram::attachShaders()
	{
		for (unsigned int i = 0; i < m_shaderCount; i++)
			glAttachShader(m_id, m_shaders[i]->GetID());
	}

	void ShaderProgram::detachShaders()
	{
		for (unsigned int i = 0; i < m_shaderCount; i++)
			glDetachShader(m_id, m_shaders[i]->GetID());
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
		{
			Logger.error("Could not retrieve program compilation log");
		}
		else
		{
			//The maxLength includes the NULL character
			std::vector<GLchar> ErrorMessage(maxLength);
			glGetProgramInfoLog(m_id, maxLength, &maxLength, &m_errorMessage[0]);

			m_errorMessage = std::string(ErrorMessage.begin(), ErrorMessage.end());

			Logger.error(&m_errorMessage[0]);
		}
		return true;
	}

	ShaderProgram* ShaderProgram::Load(const std::string& name, std::vector<std::string> shaders)
	{
		ShaderProgram* _program = new ShaderProgram(name);

		AddToDatabase(name, _program);

		int shaderCount = (int)shaders.size();
		for (int i = 0; i < shaderCount; i++)
		{
			Shader* _shader = Shader::Get(shaders[i]);
			if (_shader != nullptr)
				_program->AddShader(_shader);
		}
		_program->attachShaders();
		_program->Link();

		if (_program == nullptr)
			return false;
		else if (!_program->IsLinked())
			return false;

		Message_Loaded(name, _program);

		return _program;
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
			glUniform U(glGetUniformLocation(m_id, name), type);
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
	
			// Get BlockIndex
			GLuint blockIndex = glGetUniformBlockIndex(m_id, name_str.c_str());

			if (blockIndex == -1)
			{
				VXL_ASSERT(false, "Unable to get blockIndex from name");
			}

			// Get Binding point from name (ex: "ColorBlock_17" => Binding Point is 17)
			GLuint bindingPoint = -1;
			std::string bindingPointStr = "";
			size_t loc = name_str.find_last_of('_');
			if (loc != -1)
				bindingPointStr = name_str.substr(loc + 1, nameLen);
			if (!bindingPointStr.empty())
				bindingPoint = std::stoi(bindingPointStr);
			
			if (bindingPoint == -1)
			{
				VXL_ASSERT(false, "Could not figure out binding point from name, ex: MyBuffer_1 == bindingPoint of 1");
				continue;
			}

			// Lock Block Index to Binding Point in shader
			glUniformBlockBinding(m_id, blockIndex, bindingPoint);

			// Assign
			m_uniformBlocks[name_str] = glUniformBlock(bindingPoint, blockIndex);
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

		glLinkProgram(m_id);
		glValidateProgram(m_id);

		m_linked = !checkError();

		if (m_linked)
		{
			acquireUniforms();
			acquireUniformBlocks();
			acquireSubroutines();

			// set gl name
			glUtil::setGLName(glNameType::PROGRAM, m_id, "Program_" + m_name);
		}
		else
		{
			ProgramsFailed.insert(this);
			ProgramsFailedSize = (UINT)ProgramsFailed.size();
		}
	}

	void ShaderProgram::Bind() const
	{
		// Don't bind program if it hasn't changed
		if (m_boundID != m_id)
			glUseProgram(m_id);

		// update bound program
		ShaderProgram::m_boundID = m_id;
	}

	void ShaderProgram::Unbind()
	{
		// Don't bind program if it hasn't changed
		if (m_boundID != 0)
			glUseProgram(0);

		// update bound program
		ShaderProgram::m_boundID = 0;
	}

	UINT ShaderProgram::GetBoundProgram(void)
	{
		return m_boundID;
	}
}