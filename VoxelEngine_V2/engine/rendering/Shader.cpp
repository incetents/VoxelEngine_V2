// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Shader.h"

#include "../utilities/Logger.h"
#include "../utilities/FileIO.h"
#include "../utilities/Asset.h"

namespace Vxl
{
	std::map<ShaderID, Shader*> Shader::m_brokenShaders;
	std::map<ShaderProgramID, ShaderProgram*> ShaderProgram::m_brokenShaderPrograms;

	Shader::Shader(const std::string& name, const std::string& shaderCode, ShaderType type)
		: m_name(name), m_source(shaderCode), m_type(type)
	{
		// Create
		m_id = Graphics::Shader::Create(m_type);
		if (m_id == -1)
		{
			m_compiled = false;
			return;
		}

		// Fix includes
		m_source = FileIO::addInclude(m_source);

		// Lines for source
		uint32_t lineCount = (uint32_t)stringUtil::countChar(m_source, '\n') + 1;
		for (uint32_t i = 1; i <= lineCount; i++)
			m_sourceLines += std::to_string(i) + '\n';

		// Convert file to const char
		const char* Source = static_cast<const char *>(m_source.c_str());

		// Attach Shader and Compile
		m_compiled = Graphics::Shader::Compile(m_id, m_type, Source);

		// Error
		if (!m_compiled)
		{
			m_errorMessage = "Failed to Compile Shader\n";
			m_errorMessage += "Name: " + m_name + "\n";

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
			m_errorMessage += "Error:\n";
			m_errorMessage += Graphics::Shader::GetError(m_id);
			m_errorMessage += "~~~~~~~~~~\n";
			m_errorMessage.shrink_to_fit();

			m_brokenShaders[m_id] = this;

		}
	}
	Shader::~Shader()
	{
		m_brokenShaders.erase(m_id);

		if (m_id != -1)
			Graphics::Shader::Delete(m_id);
	}
	void Shader::setGLName(const std::string& name)
	{
		Graphics::SetGLName(ObjectType::SHADER, m_id, "Shader_" + name);
	}

	ShaderProgram::ShaderProgram(const std::string& name, const std::vector<Shader*>& _shaders)
		: m_name(name), m_shaders(_shaders)
	{
		m_id = Graphics::ShaderProgram::Create();
		if (m_id == -1)
		{
			m_linked = false;
			return;
		}

		uint32_t shaderCount = (uint32_t)m_shaders.size();

		// Auto fail if any shaders aren't compiled
		for (unsigned int i = 0; i < shaderCount; i++)
		{
			if (!m_shaders[i]->isCompiled())
			{
				m_linked = false;
				return;
			}
		}

		// Attach Shaders to Shader Program
		for (unsigned int i = 0; i < shaderCount; i++)
			Graphics::ShaderProgram::AttachShader(m_id, m_shaders[i]->getID());

		// Link
		m_linked = Graphics::ShaderProgram::Link(m_id);

#if _DEBUG
		// Validation
		m_linked &= Graphics::ShaderProgram::Validate(m_id);
#endif

		if (m_linked)
		{
			// attributes //
			m_attributes = Graphics::ShaderProgram::AcquireAttributes(m_id);
			// uniforms //
			m_uniforms = Graphics::ShaderProgram::AcquireUniforms(m_id);
			m_uniformBlocks = Graphics::ShaderProgram::AcquireUniformBlocks(m_id);

			std::vector<ShaderType> types;
			types.reserve(shaderCount);
			for (const auto& shader : m_shaders)
				types.push_back(shader->getType());

			m_subroutines = Graphics::ShaderProgram::AcquireUniformSubroutines(m_id, types);
		}
		else
		{
			m_errorMessage = Graphics::ShaderProgram::GetError(m_id);

			Logger.error(m_errorMessage);

			m_brokenShaderPrograms[m_id] = this;
		}

		// Detach Shaders from Shader Program
		for (unsigned int i = 0; i < shaderCount; i++)
			Graphics::ShaderProgram::DetachShader(m_id, m_shaders[i]->getID());

	}
	ShaderProgram::~ShaderProgram()
	{
		m_brokenShaderPrograms.erase(m_id);

		Graphics::ShaderProgram::Delete(m_id);
	}
	void ShaderProgram::bind() const
	{
		if (m_linked)
		{
			Graphics::ShaderProgram::Enable(m_id);

			// Bind subroutines
			if (!m_subroutines.empty())
			{
				for (const auto& s : m_subroutines)
					s.second.Bind();
			}
		}
	}
	void ShaderProgram::unbind()
	{
		Graphics::ShaderProgram::Disable();
	}
	void ShaderProgram::setGLName(const std::string& name)
	{
		Graphics::SetGLName(ObjectType::PROGRAM, m_id, "Program_" + name);
	}

}