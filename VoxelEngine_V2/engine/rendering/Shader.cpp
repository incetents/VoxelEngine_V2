﻿// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Shader.h"

#include "../utilities/Logger.h"
#include "../utilities/FileIO.h"
#include "../utilities/ShaderBuilder.h"

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
#ifdef GLOBAL_SHADER_CODE_BACKUP
		m_sourceBackup.clear();
		auto lines = stringUtil::splitStr(source, '\n');
		int lineNumber = 0;
		for (auto line : lines)
		{
			m_sourceBackup += std::to_string(++lineNumber) + '\t' + line + '\n';
		}
#endif

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

	void Shader::load(const std::string& shaderCode)
	{
		// Create Shader ID
		m_id = Graphics::Shader::Create(m_type);
		if (m_id == -1)
		{
			Logger.error("Unable to create shader: " + m_name);
			return;
		}

		// Compile Source
		if (!compile(shaderCode))
			return;

		// GLName
		Graphics::SetGLName(ObjectType::SHADER, m_id, "Shader_" + m_name);
	}

	void Shader::unload()
	{
		Graphics::Shader::Delete(m_id);
		m_id = -1;
		m_hasCompiled = false;
	}

	// SHADER PROGRAM //

	bool ShaderProgram::CreateProgram()
	{
		m_id = Graphics::ShaderProgram::Create();
		if (m_id == -1)
		{
			Logger.error("Unable to create shader program " + m_name);
			return false;
		}
		return true;
	}

	void ShaderProgram::DestroyProgram()
	{
		Graphics::ShaderProgram::Delete(m_id);
		m_id = -1;
		m_linked = false;
	}

	void ShaderProgram::AttachShaders()
	{
		for (unsigned int i = 0; i < m_shaderCount; i++)
			Graphics::ShaderProgram::AttachShader(m_id, m_shaders[i]->GetID());
	}

	void ShaderProgram::DetachShaders()
	{
		for (unsigned int i = 0; i < m_shaderCount; i++)
			Graphics::ShaderProgram::DetachShader(m_id, m_shaders[i]->GetID());
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

	void ShaderProgram::ReloadShaders()
	{
		UnloadShaders();
		LoadShaders();
	}
	void ShaderProgram::LoadShaders()
	{
		VXL_ASSERT(!m_linked, "Cannot Load ShaderProgram if already linked, " + m_name);

		// Load Shaders
		for (const auto& filePath : m_filePaths)
		{
			if (FileIO::fileExists(filePath))
			{
				std::string extension = FileIO::getExtension(filePath);
				if (extension.compare("vert") == 0)
				{
					auto name = FileIO::getName(filePath) + "_vert";
					auto file = FileIO::readFile(filePath);
					file = FileIO::addInclude(file);
					m_shaders.push_back(new Shader(name, file, ShaderType::VERTEX));
				}
				else if (extension.compare("geom") == 0)
				{
					auto name = FileIO::getName(filePath) + "_geom";
					auto file = FileIO::readFile(filePath);
					file = FileIO::addInclude(file);
					m_shaders.push_back(new Shader(name, file, ShaderType::GEOMETRY));
				}
				else if (extension.compare("frag") == 0)
				{
					auto name = FileIO::getName(filePath) + "_frag";
					auto file = FileIO::readFile(filePath);
					file = FileIO::addInclude(file);
					m_shaders.push_back(new Shader(name, file, ShaderType::FRAGMENT));
				}
				// Material
				else if (extension.compare("material") == 0)
				{
					ShaderBuilder builder(filePath);
					if ( builder.Name.empty())
						 builder.Name = "ShaderProgram(" + std::to_string(m_id) + ')';

					if (!builder.VertexShader.empty())
					{
						m_shaders.push_back(new Shader( m_name + "_Vert", builder.VertexShader, ShaderType::VERTEX));
					}

					if (!builder.GeomShader.empty())
					{
						m_shaders.push_back(new Shader( m_name + "_Geom", builder.GeomShader, ShaderType::GEOMETRY));
					}

					if (!builder.FragShader.empty())
					{
						m_shaders.push_back(new Shader( m_name + "_Frag", builder.FragShader, ShaderType::FRAGMENT));
					}
					
				}
			}
		}

		m_shaderCount = m_shaders.size();
		AttachShaders();
		Link();
		DetachShaders();

		if (!IsLinked())
		{
			Logger.error("Shader Program [" + m_name + "] failed to link");
		}
	}
	void ShaderProgram::UnloadShaders()
	{
		for (auto& shader : m_shaders)
		{
			delete shader;
		}
		m_shaders.clear();
		m_shaderCount = 0;
		m_linked = false;
	}

	ShaderProgram::ShaderProgram(const std::string& name, const std::vector<std::string>& filePaths)
		: m_name(name), m_filePaths(filePaths)
	{
		CreateProgram();
		LoadShaders();
	}

	ShaderProgram::~ShaderProgram()
	{
		UnloadShaders();
		DestroyProgram();
	}

	ShaderProgram* ShaderProgram::Load(
		const std::string& name,
		const std::vector<std::string>& filePaths
	)
	{
		ShaderProgram* _program = new ShaderProgram(name, filePaths);

		AddNamedAsset(name, _program, AssetMessage::LOADED);

		return _program;
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