// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <string>

namespace Vxl
{
	class ShaderBuilder
	{
	private:
		std::string SECTION_NAME		= "#Name";
		std::string SECTION_INCLUDE		= "#Include";
		std::string SECTION_PROPERTIES	= "#Properties";
		std::string SECTION_ATTRIBUTE	= "#Attributes";
		std::string SECTION_VERTEX		= "#Vertex";
		std::string SECTION_GEOMETRY	= "#Geometry";
		std::string SECTION_FRAGMENT	= "#Fragment";

	public:
		ShaderBuilder(const std::string& filepath);

		std::string Name;
		std::string VertexShader;
		std::string GeomShader;
		std::string FragShader;
	};
}