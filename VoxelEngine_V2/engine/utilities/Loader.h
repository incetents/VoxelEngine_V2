// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <string>
#include <vector>

namespace Vxl
{
	enum class Wrap_Mode;
	enum class Filter_Mode_Min;
	enum class Filter_Mode_Mag;

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
		static const std::string TAG_FILTERMODE_NEAREST_NEAREST;
		static const std::string TAG_FILTERMODE_LINEAR_NEAREST;
		static const std::string TAG_FILTERMODE_NEAREST_LINEAR;
		static const std::string TAG_FILTERMODE_LINEAR_LINEAR;

		static Wrap_Mode DecipherWrapMode(const std::string& str);
		static Filter_Mode_Min DecipherFilterModeMin(const std::string& str);
		static Filter_Mode_Mag DecipherFilterModeMag(const std::string& str);
		static ShaderType DeciperShaderType(const std::string& str);
	public:

		static bool LoadScript_ImportFiles(const std::string& filePath);

		static bool Load_Texture	(const std::string& name, const std::string& filePath, Wrap_Mode wrap, Filter_Mode_Min minFilter, Filter_Mode_Mag maxFilter, bool FlipY);
		static bool Load_Shader		(const std::string& name, const std::string& filePath, ShaderType type);
		static bool Load_ShaderProgram	(const std::string& name, std::vector<std::string> shaders);
		static bool Load_Cubemap	(const std::string& name,
			const std::string& filePath1, const std::string& filePath2, const std::string& filePath3, const std::string& filePath4, const std::string& filePath5, const std::string& filePath6,
			Wrap_Mode WrapMode = Wrap_Mode::CLAMP_STRETCH,
			Filter_Mode_Min MinFilter = Filter_Mode_Min::LINEAR,
			Filter_Mode_Mag MagFilter = Filter_Mode_Mag::LINEAR,
			bool InvertY = true);
		static bool Load_Model		(const std::string& name, const std::string& filePath);
	};
}

