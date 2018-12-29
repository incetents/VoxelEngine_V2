// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <string>

namespace Vxl
{
	enum class Wrap_Mode;
	enum class Filter_Mode;

	class Loader
	{
	private:
		enum LoadState
		{
			NONE,
			SHADER,
			SHADER_PROGRAM,
			TEXTURE,
			CUBEMAP
		};

		static const std::string TAG_LOAD_SHADER;
		static const std::string TAG_LOAD_SHADERPROGRAM;
		static const std::string TAG_LOAD_TEXTURE;
		static const std::string TAG_LOAD_CUBEMAP;

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
	public:
		static Wrap_Mode DecipherWrapMode(const std::string& str);
		static Filter_Mode DecipherFilterMode(const std::string& str);

		static bool CheckFile(const std::string& filePath);

		static bool LoadScript_ImportFiles(const std::string& fullpath);
	};
}

