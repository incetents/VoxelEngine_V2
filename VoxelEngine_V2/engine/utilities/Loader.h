// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <string>

namespace Vxl
{
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
	public:
		static bool CheckFile(const std::string& filePath);

		static bool LoadScript_ImportFiles(const std::string& fullpath);
	};
}

