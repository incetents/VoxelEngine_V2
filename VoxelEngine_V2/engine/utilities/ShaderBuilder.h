// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <string>

namespace Vxl
{
	class ShaderBuilder
	{
	public:
		ShaderBuilder(const std::string& filepath);

		std::string Name;
		std::string VertexShader;
		std::string GeomShader;
		std::string FragShader;
	};
}