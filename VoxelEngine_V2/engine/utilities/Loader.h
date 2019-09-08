// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <string>
#include <vector>
#include <map>

namespace Vxl
{
	enum class TextureWrapping;
	enum class TextureFilter;
	enum class ShaderType;
	enum class AnisotropicMode;

	class Loader
	{
	private:
		enum LoadState
		{
			NONE,
			SHADER,
			SHADER_PROGRAM,
			TEXTURE,
			CUBEMAP,
			MODEL
		};
		
		static std::map<std::string, std::string> ShaderPaths;

		static const std::string TAG_DEFAULT;

		static const std::string TAG_LOAD_SHADER;
		static const std::string TAG_LOAD_SHADERPROGRAM;
		static const std::string TAG_LOAD_TEXTURE;
		static const std::string TAG_LOAD_CUBEMAP;
		static const std::string TAG_LOAD_MODEL;

		static const std::string TAG_VERT;
		static const std::string TAG_GEOM;
		static const std::string TAG_FRAG;
		static const std::string TAG_TESS_CTRL;
		static const std::string TAG_TESS_EVAL;
		static const std::string TAG_COMP;

		static const std::string TAG_WRAPMODE_REPEAT;
		static const std::string TAG_WRAPMODE_CLAMP_BORDER;
		static const std::string TAG_WRAPMODE_CLAMP_STRETCH;
		static const std::string TAG_WRAPMODE_MIRROR_REPEAT;
		static const std::string TAG_WRAPMODE_MIRROR_CLAMP;

		static const std::string TAG_FILTERMODE_NEAREST;
		static const std::string TAG_FILTERMODE_LINEAR;

		static const std::string TAG_FLIP_TRUE;
		static const std::string TAG_MIPMAP_TRUE;

		static const std::string TAG_MERGE;
		static const std::string TAG_NORMALIZE;
		static const std::string TAG_NORMALIZE_SCALE;

		static const std::string TAG_LOW;
		static const std::string TAG_MEDIUM;
		static const std::string TAG_HIGH;

		static TextureWrapping DecipherWrapMode(const std::string& str);
		static TextureFilter DecipherFilterMode(const std::string& str);
		static ShaderType DeciperShaderType(const std::string& str);
		static bool DecipherFlipType(const std::string& str);
		static bool DecipherMipMapType(const std::string& str);
		static AnisotropicMode DecipherAnisotropicMode(const std::string& str);
	
	public:

		static bool LoadScript_ImportFiles(const std::string& filePath);
	};
}

