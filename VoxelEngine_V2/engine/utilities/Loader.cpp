// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Loader.h"

#include "../opengl/Shader.h"
#include "../opengl/Texture.h"
#include "../opengl/Enums.h"

#include "logger.h"

#include "stringUtil.h"

namespace Vxl
{
	const std::string Loader::TAG_LOAD_SHADER = "#SHADER";
	const std::string Loader::TAG_LOAD_SHADERPROGRAM = "#SHADERPROGRAM";
	const std::string Loader::TAG_LOAD_TEXTURE = "#TEXTURE";
	const std::string Loader::TAG_LOAD_CUBEMAP = "#CUBEMAP";

	const std::string Loader::TAG_VERT = "#VERT";
	const std::string Loader::TAG_GEOM = "#GEOM";
	const std::string Loader::TAG_FRAG = "#FRAG";
	const std::string Loader::TAG_TESS_CTRL = "#TESS_CTRL";
	const std::string Loader::TAG_TESS_EVAL = "#TESS_EVAL";
	const std::string Loader::TAG_COMP = "#COMP";

	const std::string Loader::TAG_WRAPMODE_REPEAT = "REPEAT";
	const std::string Loader::TAG_WRAPMODE_CLAMP_BORDER = "CLAMP_BORDER";
	const std::string Loader::TAG_WRAPMODE_CLAMP_STRETCH = "CLAMP_STRETCH";
	const std::string Loader::TAG_WRAPMODE_MIRROR_REPEAT = "MIRROR_REPEAT";
	const std::string Loader::TAG_WRAPMODE_MIRROR_CLAMP = "MIRROR_CLAMP";

	const std::string Loader::TAG_FILTERMODE_NEAREST = "NEAREST";
	const std::string Loader::TAG_FILTERMODE_LINEAR = "LINEAR";

	Wrap_Mode Loader::DecipherWrapMode(const std::string& str)
	{
		if (str.compare(TAG_WRAPMODE_REPEAT) == 0)
			return Wrap_Mode::REPEAT;
		else if (str.compare(TAG_WRAPMODE_CLAMP_BORDER) == 0)
			return Wrap_Mode::CLAMP_BORDER;
		else if (str.compare(TAG_WRAPMODE_CLAMP_STRETCH) == 0)
			return Wrap_Mode::CLAMP_STRETCH;
		else if (str.compare(TAG_WRAPMODE_MIRROR_REPEAT) == 0)
			return Wrap_Mode::MIRROR_REPEAT;
		else if (str.compare(TAG_WRAPMODE_MIRROR_CLAMP) == 0)
			return Wrap_Mode::MIRROR_CLAMP;
		else
			return Wrap_Mode::NONE;
	}
	Filter_Mode Loader::DecipherFilterMode(const std::string& str)
	{
		if (str.compare(TAG_FILTERMODE_NEAREST) == 0)
			return Filter_Mode::NEAREST;
		else if (str.compare(TAG_FILTERMODE_LINEAR) == 0)
			return Filter_Mode::LINEAR;
		else
			return Filter_Mode::NONE;
	}

	bool Loader::CheckFile(const std::string& filePath)
	{
		struct stat buffer;
		return (stat(filePath.c_str(), &buffer) == 0);
	}

	bool Loader::LoadScript_ImportFiles(const std::string& fullpath)
	{
		std::string File = stringUtil::readFile(fullpath);
		if (File.empty())
			return false;
		
		Logger.log("Loading Files: " + fullpath);
		Logger.log("~~~~~~~~~~~~~~~~~~~~~~~~~~");
		
		LoadState _state = LoadState::NONE; // STATE

		auto Lines = stringUtil::splitStr(File, '\n');
		for (auto Line : Lines)
		{
			// Ignore comments
			std::size_t found = Line.find("//");
			if (found != std::string::npos)
				Line = Line.substr(0, found);

			// Trim edges
			stringUtil::trim(Line);

			// Ignore empty line
			if (Line.empty())
				continue;

			// Seperate Lines by spaces/tabs
			auto Segments = stringUtil::splitStr(Line, " \t");

			unsigned int SegmentCount = (unsigned int)Segments.size();

			if (SegmentCount == 1)
			{
				/* STATE */
				auto Tag = Segments[0];
				if (Tag.compare(TAG_LOAD_SHADER) == 0)
					_state = LoadState::SHADER;
				else if (Tag.compare(TAG_LOAD_SHADERPROGRAM) == 0)
					_state = LoadState::SHADER_PROGRAM;
				else if (Tag.compare(TAG_LOAD_TEXTURE) == 0)
					_state = LoadState::TEXTURE;
				else if (Tag.compare(TAG_LOAD_CUBEMAP) == 0)
					_state = LoadState::CUBEMAP;
				else
					_state = LoadState::NONE;
			}
			else
			{
				/* SHADER */
				if (_state == LoadState::SHADER && SegmentCount == 3)
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

					Shader::Create(Name, Path, Type);
				}
				/* SHADER PROGRAM */
				else if (_state == LoadState::SHADER_PROGRAM && SegmentCount >= 3)
				{
					auto Name = Segments[0];

					ShaderProgram* Sp = ShaderProgram::Create(Name);
					if (Sp == nullptr)
						continue;

					int SegmentCount = (int)Segments.size();
					for (int i = 1; i < SegmentCount; i++)
					{
						Shader* S = Shader::Get(Segments[i]);
						if (S != nullptr)
							Sp->AddShader(S);
					}
					Sp->Link();

					Logger.log("Created Shader Program: " + Name);
				}
				/* TEXTURE */
				else if (_state == LoadState::TEXTURE && SegmentCount >= 2)
				{
					auto Name = Segments[0];
					Texture* tex;
					if (SegmentCount == 2)
						tex = new Texture(Segments[1]);
					else if (SegmentCount == 3)
					{
						Wrap_Mode WM = DecipherWrapMode(Segments[2]);
						tex = new Texture(Segments[1], WM);
					}
					else if (SegmentCount == 4)
					{
						Wrap_Mode WM = DecipherWrapMode(Segments[2]);
						Filter_Mode FM = DecipherFilterMode(Segments[3]);
						tex = new Texture(Segments[1], WM, FM, FM);
					}
					else
						continue;

					if (tex->IsLoaded())
					{
						if (Texture::m_database.Check(Name))
						{
							Logger.error("Duplicate Texture: " + Name);
							delete tex;
						}
						else
						{
							Logger.log("Loaded Texture: " + Name);
							Texture::m_database.Set(Name, tex);
						}
					}
					else
						delete tex;
				}
				/* CUBEMAP */
				else if (_state == LoadState::CUBEMAP && SegmentCount == 7)
				{
					auto Name = Segments[0];
					Cubemap* cubemap = new Cubemap(
						Segments[1],
						Segments[2],
						Segments[3],
						Segments[4],
						Segments[5],
						Segments[6],
						Wrap_Mode::CLAMP_STRETCH
					);
					if (cubemap->IsLoaded())
					{
						if (Cubemap::m_database.Check(Name))
						{
							Logger.error("Duplicate Cubemap: " + Name);
							delete cubemap;
						}
						else
						{
							Logger.log("Loaded Cubemap: " + Name);
							Cubemap::m_database.Set(Name, cubemap);
						}
					}
					else
						delete cubemap;
				}

			}
		}
		return true;
	}
}