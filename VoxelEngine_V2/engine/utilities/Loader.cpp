// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Loader.h"

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Model.h"

#include "../opengl/Shader.h"
#include "../opengl/Texture.h"
#include "../opengl/Enums.h"

#include "logger.h"
#include "FileIO.h"

namespace Vxl
{
	const std::string Loader::TAG_LOAD_SHADER = "#SHADER";
	const std::string Loader::TAG_LOAD_SHADERPROGRAM = "#SHADERPROGRAM";
	const std::string Loader::TAG_LOAD_TEXTURE = "#TEXTURE";
	const std::string Loader::TAG_LOAD_CUBEMAP = "#CUBEMAP";
	const std::string Loader::TAG_LOAD_MODEL = "#MODEL";

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
	const std::string Loader::TAG_FILTERMODE_NEAREST_NEAREST = "NEAREST_NEAREST";
	const std::string Loader::TAG_FILTERMODE_LINEAR_NEAREST = "LINEAR_NEAREST";
	const std::string Loader::TAG_FILTERMODE_NEAREST_LINEAR = "NEAREST_LINEAR";
	const std::string Loader::TAG_FILTERMODE_LINEAR_LINEAR = "LINEAR_LINEAR";

	const std::string Loader::TAG_FLIP_TRUE = "YES_FLIP";
	const std::string Loader::TAG_MIPMAP_TRUE = "YES_MIPMAP";

	const std::string Loader::TAG_MERGE = "MERGE";
	const std::string Loader::TAG_NORMALIZE = "NORMALIZE";
	const std::string Loader::TAG_NORMALIZE_SCALE = "SCALE";

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
	Filter_Mag Loader::DecipherFilterModeMag(const std::string& str)
	{
		if (str.compare(TAG_FILTERMODE_NEAREST) == 0)
			return Filter_Mag::NEAREST;
		else if (str.compare(TAG_FILTERMODE_LINEAR) == 0)
			return Filter_Mag::LINEAR;
		else
			return Filter_Mag::NONE;
	}
	Filter_Min Loader::DecipherFilterModeMin(const std::string& str)
	{
		if (str.compare(TAG_FILTERMODE_NEAREST) == 0)
			return Filter_Min::NEAREST;
		else if (str.compare(TAG_FILTERMODE_LINEAR) == 0)
			return Filter_Min::LINEAR;
		else if (str.compare(TAG_FILTERMODE_NEAREST_NEAREST) == 0)
			return Filter_Min::NEAREST_MIPMAP_NEAREST;
		else if (str.compare(TAG_FILTERMODE_LINEAR_NEAREST) == 0)
			return Filter_Min::LINEAR_MIPMAP_NEAREST;
		else if (str.compare(TAG_FILTERMODE_NEAREST_LINEAR) == 0)
			return Filter_Min::NEAREST_MIPMAP_LINEAR;
		else if (str.compare(TAG_FILTERMODE_LINEAR_LINEAR) == 0)
			return Filter_Min::LINEAR_MIPMAP_LINEAR;
		else
			return Filter_Min::NONE;
	}
	ShaderType Loader::DeciperShaderType(const std::string& str)
	{
		if (str.compare(TAG_VERT) == 0)
			return ShaderType::VERTEX;
		else if (str.compare(TAG_GEOM) == 0)
			return ShaderType::GEOMETRY;
		else if (str.compare(TAG_FRAG) == 0)
			return ShaderType::FRAGMENT;
		else if (str.compare(TAG_TESS_CTRL) == 0)
			return ShaderType::TESSELATION_CONTROL;
		else if (str.compare(TAG_TESS_EVAL) == 0)
			return ShaderType::TESSELATION_EVALUATION;
		else if (str.compare(TAG_COMP) == 0)
			return ShaderType::COMPUTE;
		else
			return ShaderType::NONE;
	}
	bool Loader::DecipherFlipType(const std::string& str)
	{
		return (str.compare(TAG_FLIP_TRUE) == 0);
	}
	bool Loader::DecipherMipMapType(const std::string& str)
	{
		return (str.compare(TAG_MIPMAP_TRUE) == 0);
	}

	bool Loader::LoadScript_ImportFiles(const std::string& filePath)
	{
		std::string File = IO::readFile(filePath);
		if (File.empty())
			return false;
		
		Logger.log("Loading Files: " + filePath);
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
				else if (Tag.compare(TAG_LOAD_MODEL) == 0)
					_state = LoadState::MODEL;
				else
					_state = LoadState::NONE;
			}
			else
			{
				/* SHADER */
				if (_state == LoadState::SHADER && SegmentCount == 3)
				{
					auto Tag  = Segments[0];
					auto Name = Segments[1];
					auto Path = Segments[2];

					ShaderType Type = DeciperShaderType(Tag);

					if (Type == ShaderType::NONE)
						continue;

					Shader::Load(Name, Path, Type);
				}
				/* SHADER PROGRAM */
				else if (_state == LoadState::SHADER_PROGRAM && SegmentCount >= 3)
				{
					auto Name = Segments[0];
					std::vector<std::string> Shaders;
					
					for (unsigned int i = 1; i < SegmentCount; i++)
						Shaders.push_back(Segments[i]);
					
					ShaderProgram::Load(Name, Shaders);
				}
				/* TEXTURE */
				else if (_state == LoadState::TEXTURE && SegmentCount >= 2)
				{
					auto Name = Segments[0];
					auto filePath = Segments[1];
					Wrap_Mode WM = Wrap_Mode::REPEAT;
					Filter_Min FMIN = Filter_Min::LINEAR;
					Filter_Mag FMAG = Filter_Mag::LINEAR;
					bool FlipY = true;
					bool MipMap = true;

					switch (SegmentCount)
					{
					case 7:
						MipMap = DecipherMipMapType(Segments[6]); // 7
					case 6:
						FlipY = DecipherFlipType(Segments[5]); // 6
					case 5:
						FMAG = DecipherFilterModeMag(Segments[4]); // 5
					case 4:
						FMIN = DecipherFilterModeMin(Segments[3]); // 4
					case 3:
						WM = DecipherWrapMode(Segments[2]); // 3
						break;
					}

					Texture::Load(Name, filePath, FlipY, MipMap, WM, FMIN, FMAG, Format_Type::RGBA, Data_Type::UNSIGNED_BYTE);
				}
				/* CUBEMAP */
				else if (_state == LoadState::CUBEMAP && SegmentCount == 7)
				{
					Cubemap::Load(
						// Name
						Segments[0],
						// Filepaths 1-6
						Segments[1], Segments[2], Segments[3],
						Segments[4], Segments[5], Segments[6]
					);	
				}
				/* MODEL */
				else if (_state == LoadState::MODEL && SegmentCount >= 2)
				{
					auto Name = Segments[0];
					auto FilePath = Segments[1];
					bool Merge = false;
					bool Normalize = false;
					float scale = 1.0f;

					switch (SegmentCount)
					{
					case 5:
						scale = std::stof(stringUtil::extractFirst(Segments[4], '[', ']'));
					case 4:
						Normalize = Segments[3].compare(TAG_NORMALIZE) == 0;
					case 3:
						Merge = Segments[2].compare(TAG_MERGE) == 0;
						break;
					}

					Model::Load(Name, FilePath, Merge, Normalize, scale);
				}
			}
		}
		return true;
	}
}