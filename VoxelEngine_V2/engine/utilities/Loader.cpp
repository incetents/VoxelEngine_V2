// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Loader.h"

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../math/Vector.h"
#include "../math/Model.h"

#include "../textures/Texture2D.h"
#include "../textures/Cubemap.h"

#include "../rendering/Shader.h"
#include "../rendering/Graphics.h"

#include "Logger.h"
#include "FileIO.h"

namespace Vxl
{
	const std::string Loader::TAG_DEFAULT = "DEFAULT";

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

	const std::string Loader::TAG_FLIP_TRUE = "YES_FLIP";
	const std::string Loader::TAG_MIPMAP_TRUE = "YES_MIPMAP";

	const std::string Loader::TAG_MERGE = "MERGE";
	const std::string Loader::TAG_NORMALIZE = "NORMALIZE";
	const std::string Loader::TAG_NORMALIZE_SCALE = "SCALE";

	const std::string Loader::TAG_LOW = "LOW";
	const std::string Loader::TAG_MEDIUM = "MEDIUM";
	const std::string Loader::TAG_HIGH = "HIGH";

	TextureWrapping Loader::DecipherWrapMode(const std::string& str)
	{
		if (str.compare(TAG_DEFAULT) == 0 || str.compare(TAG_WRAPMODE_REPEAT) == 0)
			return TextureWrapping::REPEAT;
		else if (str.compare(TAG_WRAPMODE_CLAMP_BORDER) == 0)
			return TextureWrapping::CLAMP_BORDER;
		else if (str.compare(TAG_WRAPMODE_CLAMP_STRETCH) == 0)
			return TextureWrapping::CLAMP_STRETCH;
		else if (str.compare(TAG_WRAPMODE_MIRROR_REPEAT) == 0)
			return TextureWrapping::MIRROR_REPEAT;
		else if (str.compare(TAG_WRAPMODE_MIRROR_CLAMP) == 0)
			return TextureWrapping::MIRROR_CLAMP;
		else
			return TextureWrapping::NONE;
	}
	TextureFilter Loader::DecipherFilterMode(const std::string& str)
	{
		if (str.compare(TAG_DEFAULT) == 0 || str.compare(TAG_FILTERMODE_LINEAR) == 0)
			return TextureFilter::LINEAR;
		else if (str.compare(TAG_FILTERMODE_NEAREST) == 0)
			return TextureFilter::NEAREST;
		else
			return TextureFilter::NONE;
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
		return (str.compare(TAG_DEFAULT) == 0 || str.compare(TAG_FLIP_TRUE) == 0);
	}
	bool Loader::DecipherMipMapType(const std::string& str)
	{
		return (str.compare(TAG_DEFAULT) == 0 || str.compare(TAG_MIPMAP_TRUE) == 0);
	}
	AnisotropicMode Loader::DecipherAnisotropicMode(const std::string& str)
	{
		if(str.compare(TAG_DEFAULT) == 0 || str.compare(TAG_HIGH) == 0)
			return AnisotropicMode::HIGH;
		else if (str.compare(TAG_MEDIUM) == 0)
			return AnisotropicMode::MEDIUM;
		else if (str.compare(TAG_LOW) == 0)
			return AnisotropicMode::LOW;
		else
			return AnisotropicMode::NONE;
	}

	bool Loader::LoadScript_ImportFiles(const std::string& filePath)
	{
		std::string File = IO::readFile(filePath);
		if (File.empty())
			return false;
		
		Logger.log("~~~~~~~~~~~~~~~~~~~~~~~~~~");
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
					std::vector<Shader*> Shaders;
					
					for (unsigned int i = 1; i < SegmentCount; i++)
					{
						if (Shader::CheckAsset(Segments[i]))
							Shaders.push_back(Shader::GetAsset(Segments[i]));
						else
							Logger.error("Attempting to load non-existing Shader: " + Segments[i]);
					}
					
					ShaderProgram::Load(Name, Shaders);
				}
				/* TEXTURE */
				else if (_state == LoadState::TEXTURE && SegmentCount >= 2)
				{
					auto Name = Segments[0];
					auto filePath = Segments[1];
					TextureWrapping WM = TextureWrapping::REPEAT;
					TextureFilter Filter = TextureFilter::LINEAR;
					AnisotropicMode ANSO = AnisotropicMode::HIGH;
					bool FlipY = true;
					bool MipMap = true;

					switch (SegmentCount)
					{
					case 7:
						ANSO = DecipherAnisotropicMode(Segments[6]);
					case 6:
						MipMap = DecipherMipMapType(Segments[5]);
					case 5:
						FlipY = DecipherFlipType(Segments[4]);
					case 4:
						Filter = DecipherFilterMode(Segments[3]);
					case 3:
						WM = DecipherWrapMode(Segments[2]);
						break;
					}

					Texture2D::Load(Name, filePath, FlipY, MipMap, WM, Filter, TextureFormat::RGBA8, TexturePixelType::UNSIGNED_BYTE, ANSO);
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