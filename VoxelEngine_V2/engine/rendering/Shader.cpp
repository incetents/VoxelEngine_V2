// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Shader.h"

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
		const char* Source = static_cast<const char *>(source.c_str());

		// Keep backup with line numbers
		m_sourceBackup.clear();
		auto lines = stringUtil::splitStr(source, '\n');
		int lineNumber = 0;
		for (auto line : lines)
		{
			m_sourceBackup += std::to_string(++lineNumber) + '\t' + line + '\n';
		}

		// Attach Shader and Compile
		m_hasCompiled = Graphics::Shader::Compile(m_id, m_type, Source);

		// Compilation Failure
		if (!m_hasCompiled)
		{
			updateErrorMessage();

			// store in map of shaders with errors
			ShaderErrorLog[m_name] = this;
			ShaderErrorLogSize++;
		}

		return m_hasCompiled;
	}

	void Shader::updateErrorMessage()
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
		m_errorMessage += Graphics::Shader::GetError(m_id);
	}

	bool Shader::load()
	{
		// Create Shader ID
		m_id = Graphics::Shader::Create(m_type);
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
			return false;

		// GLName
		Graphics::SetGLName(ObjectType::SHADER, m_id, "Shader_" + m_name);

		return true;
	}

	void Shader::unload()
	{
		Graphics::Shader::Delete(m_id);
		m_id = -1;
		m_hasCompiled = false;
	}

	Shader* Shader::Load(const std::string& name, const std::string& filePath, ShaderType type)
	{
		Shader* _shader = new Shader(name, filePath, type);
		AddToDatabase(name, _shader);

		if (!_shader->HasLoaded())
		{
			Logger.error("Shader [" + name + "] failed to load");
			Logger.error(_shader->m_errorMessage);
		}
		else if (!_shader->HasCompiled())
		{
			Logger.error("Shader [" + name + "] failed to compile");
			Logger.error(_shader->m_errorMessage);
		}
		else
		{
			Message_Created(name, _shader);
		}


		return _shader;
	}

	// SHADER PROGRAM //

	//uint32_t ShaderProgram::m_boundID = 0;

	bool ShaderProgram::createProgram()
	{
		m_id = Graphics::ShaderProgram::Create();
		if (m_id == -1)
		{
			Logger.error("Unable to create shader program " + m_name);
			return false;
		}
		return true;
	}

	void ShaderProgram::destroyProgram()
	{
		Graphics::ShaderProgram::Delete(m_id);
		m_id = -1;
		m_linked = false;
	}

	void ShaderProgram::attachShaders()
	{
		for (unsigned int i = 0; i < m_shaderCount; i++)
			Graphics::ShaderProgram::AttachShader(m_id, m_shaders[i]->GetID());
	}

	void ShaderProgram::detachShaders()
	{
		for (unsigned int i = 0; i < m_shaderCount; i++)
			Graphics::ShaderProgram::DetachShader(m_id, m_shaders[i]->GetID());
	}

	ShaderProgram* ShaderProgram::Load(
		const std::string& name,
		const std::vector<Shader*>& shaders
	)
	{
		ShaderProgram* _program = new ShaderProgram(name);
		AddToDatabase(name, _program);

		_program->m_shaders = shaders;
		_program->m_shaderCount = _program->m_shaders.size();
		_program->attachShaders();
		_program->Link();

		if (!_program->IsLinked())
		{
			Logger.error("Shader Program [" + name + "] failed to link");
		}
		else
		{
			Message_Loaded(name, _program);
		}

		return _program;
	}

	void ShaderProgram::Link()
	{
		if (m_linked)
			return;

		m_linked = Graphics::ShaderProgram::Link(m_id);

		// Also check Validation in debug mode [extra safety measure]
#if _DEBUG
		m_linked &= Graphics::ShaderProgram::Validate(m_id);
#endif

		if (m_linked)
		{
			// attributes //
			m_attributes = Graphics::ShaderProgram::AcquireAttributes(m_id);
			// uniforms //
			m_uniforms = Graphics::ShaderProgram::AcquireUniforms(m_id);
			m_uniformBlocks = Graphics::ShaderProgram::AcquireUniformBlocks(m_id);
			m_subroutines = Graphics::ShaderProgram::AcquireUniformSubroutines(m_id, m_shaders);
			m_usingSubroutines = m_subroutines.size() > 0;

			// set gl name
			Graphics::SetGLName(ObjectType::PROGRAM, m_id, "Program_" + m_name);
		}
		else
		{
			m_errorMessage = Graphics::ShaderProgram::GetError(m_id);

			Logger.error(m_errorMessage);

			ProgramsFailed.insert(this);
			ProgramsFailedSize = (UINT)ProgramsFailed.size();
		}
	}

	void ShaderProgram::Bind() const
	{
		// Don't bind program if it hasn't changed
		//if (m_boundID != m_id)
			Graphics::ShaderProgram::Enable(m_id);

		// Bind subroutines
		if (m_usingSubroutines)
		{
			for (const auto& s : m_subroutines)
				s.second.Bind();
		}

		// update bound program
		//ShaderProgram::m_boundID = m_id;
	}

	void ShaderProgram::Unbind()
	{
		// Don't bind program if it hasn't changed
		//if (m_boundID != 0)
			Graphics::ShaderProgram::Disable();

		// update bound program
		//ShaderProgram::m_boundID = 0;
	}

}