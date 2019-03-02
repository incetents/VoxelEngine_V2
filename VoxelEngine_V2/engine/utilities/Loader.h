// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <string>
#include <vector>

namespace Vxl
{
	enum class Wrap_Mode;
	enum class Filter_Min;
	enum class Filter_Mag;
	enum class ShaderType;

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
		static const std::string TAG_FILTERMODE_NEAREST_NEAREST;
		static const std::string TAG_FILTERMODE_LINEAR_NEAREST;
		static const std::string TAG_FILTERMODE_NEAREST_LINEAR;
		static const std::string TAG_FILTERMODE_LINEAR_LINEAR;

		static const std::string TAG_FLIP_TRUE;
		static const std::string TAG_MIPMAP_TRUE;

		static const std::string TAG_MERGE;
		static const std::string TAG_NORMALIZE;
		static const std::string TAG_NORMALIZE_SCALE;

		static Wrap_Mode DecipherWrapMode(const std::string& str);
		static Filter_Min DecipherFilterModeMin(const std::string& str);
		static Filter_Mag DecipherFilterModeMag(const std::string& str);
		static ShaderType DeciperShaderType(const std::string& str);
		static bool DecipherFlipType(const std::string& str);
		static bool DecipherMipMapType(const std::string& str);
	public:

		static bool LoadScript_ImportFiles(const std::string& filePath);

		static bool Load_Texture(
			const std::string& name,
			const std::string& filePath,
			bool			InvertY = true,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Filter_Min MinFilter = Filter_Min::LINEAR,
			Filter_Mag MagFilter = Filter_Mag::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA,
			Data_Type		DataType = Data_Type::UNSIGNED_BYTE
		);

		static bool Load_Shader(
			const std::string& name,
			const std::string& filePath,
			ShaderType type
		);

		static bool Load_ShaderProgram(
			const std::string& name,
			std::vector<std::string> shaders
		);

		static bool Load_Cubemap(
			const std::string& name,
			const std::string& filePath1, const std::string& filePath2, const std::string& filePath3, const std::string& filePath4, const std::string& filePath5, const std::string& filePath6,
			bool InvertY = false,
			bool UseMipMapping = true,
			Wrap_Mode WrapMode = Wrap_Mode::CLAMP_STRETCH,
			Filter_Min MinFilter = Filter_Min::LINEAR,
			Filter_Mag MagFilter = Filter_Mag::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA,
			Data_Type		DataType = Data_Type::UNSIGNED_BYTE
		);

		static bool Load_Model(
			const std::string& name,
			const std::string& filePath,
			bool mergeMeshes,
			bool normalize,
			float normalizeScale = 1.0f
		);
	};
}

