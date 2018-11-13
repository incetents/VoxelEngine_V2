// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Loader.h"

#include "../opengl/Shader.h"

#include "logger.h"

#include "stringUtil.h"

namespace Vxl
{
	const std::string Loader::TAG_LOAD_SHADER = "#SHADER";
	const std::string Loader::TAG_LOAD_SHADERPROGRAM = "#SHADERPROGRAM";

	const std::string Loader::TAG_VERT = "#VERT";
	const std::string Loader::TAG_GEOM = "#GEOM";
	const std::string Loader::TAG_FRAG = "#FRAG";
	const std::string Loader::TAG_TESS_CTRL = "#TESS_CTRL";
	const std::string Loader::TAG_TESS_EVAL = "#TESS_EVAL";
	const std::string Loader::TAG_COMP = "#COMP";

	bool Loader::LoadScript_Shaders(const std::string& fullpath)
	{
		std::string File = stringUtil::readFile(fullpath);
		if (File.empty())
			return false;

		LoadState _state = LoadState::NONE; // STATE

		auto Lines = stringUtil::splitStr(File, '\n');
		for (auto Line : Lines)
		{
			if (!Line.empty())
			{
				auto Segments = stringUtil::splitStr(Line, ' ');
				if (Segments.size() == 1)
				{
					auto Tag = Segments[0];
					if (Tag.compare(TAG_LOAD_SHADER) == 0)
						_state = LoadState::SHADER;
					else if (Tag.compare(TAG_LOAD_SHADERPROGRAM) == 0)
						_state = LoadState::SHADER_PROGRAM;
				}
				else if (Segments.size() >= 3)
				{
					if (_state == LoadState::SHADER)
					{
						auto Tag = Segments[0];
						auto Name = Segments[1];
						auto Path = Segments[2];
						ShaderType Type;

						if (Tag.compare(TAG_VERT) == 0)
							Type = ShaderType::VERTEX;
						else if (Tag.compare(TAG_GEOM) == 0)
							Type = ShaderType::GEOMETRY;
						else if (Tag.compare(TAG_FRAG) == 0)
							Type = ShaderType::FRAGMENT;
						else if (Tag.compare(TAG_TESS_CTRL) == 0)
							Type = ShaderType::TESSELATION_CONTROL;
						else if (Tag.compare(TAG_TESS_EVAL) == 0)
							Type = ShaderType::TESSELATION_EVALUATION;
						else if (Tag.compare(TAG_COMP) == 0)
							Type = ShaderType::COMPUTE;
						else
							continue;

						
						std::string File = stringUtil::readFile(Path);
						Shader* S = new Shader(Name, File, Type);
						if (!S->HasFailed())
						{
							// Duplicate
							if (Shader::m_database.Check(Name))
							{
								Logger.error("Duplicate Shader: " + Name);
							}
							else
							{
								Logger.log("Loaded Shader: " + Name);
								Shader::m_database.Set(Name, S);
							}
						}
					}
					else if (_state == LoadState::SHADER_PROGRAM)
					{
						auto Name = Segments[0];
						ShaderProgram* Sp = new ShaderProgram(Name);
						int SegmentCount = (int)Segments.size();
						for (int i = 1; i < SegmentCount; i++)
						{
							Shader* S = Shader::m_database.Get(Segments[i]);
							if(S != nullptr)
								Sp->AddShader(S);
						}
						Sp->Link();
						if (Sp->IsLinked())
						{
							// Duplicate
							if (Shader::m_database.Check(Name))
							{
								Logger.error("Duplicate Shader Program: " + Name);
							}
							else
							{
								Logger.log("Created Shader Program: " + Name);
								ShaderProgram::m_database.Set(Name, Sp);
							}
						}
					}
				}
			}
		}
		return true;
	}
}