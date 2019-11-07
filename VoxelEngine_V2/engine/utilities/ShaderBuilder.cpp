// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ShaderBuilder.h"

#include "stringUtil.h"
#include "FileIO.h"
#include "../rendering/Shader.h"

namespace Vxl
{
	const std::string SECTION_NAME = "#Name";
	const std::string SECTION_INCLUDE = "#Include";
	const std::string SECTION_PROPERTIES = "#Properties";
	const std::string SECTION_ATTRIBUTE = "#Attributes";
	const std::string SECTION_VERTEX = "#Vertex";
	const std::string SECTION_GEOMETRY = "#Geometry";
	const std::string SECTION_FRAGMENT = "#Fragment";

	ShaderBuilder::ShaderBuilder(const std::string& filepath)
	{
		std::string file = FileIO::readFile(filepath);
		if (file.empty())
			return;

		struct OUTPUT
		{
			bool active;
			std::string version = "#version 420 core";
			std::string include;
			std::string input;
			std::string output;
			std::string behaviour;
		};

		OUTPUT output_vertex;
		OUTPUT output_geometry;
		OUTPUT output_fragment;

		struct SECTION
		{
			std::size_t name;
			std::size_t include;
			std::size_t properties;
			std::size_t attributes;
			std::size_t vertex;
			std::size_t geometry;
			std::size_t fragment;
		} locations;

		locations.name = file.find(SECTION_NAME);
		locations.include = file.find(SECTION_INCLUDE);
		locations.properties = file.find(SECTION_PROPERTIES);
		locations.attributes = file.find(SECTION_ATTRIBUTE);
		locations.vertex = file.find(SECTION_VERTEX);
		locations.geometry = file.find(SECTION_GEOMETRY);
		locations.fragment = file.find(SECTION_FRAGMENT);

		// Quick active state check
		output_vertex.active = (locations.vertex != std::string::npos);
		output_geometry.active = (locations.geometry != std::string::npos);
		output_fragment.active = (locations.fragment != std::string::npos);

		// Name
		if (locations.name != std::string::npos)
		{
			Name = stringUtil::extractSection(file, '{', '}', locations.name);
			stringUtil::trim(Name);
		}

		// Include
		if (locations.include != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.include);
			std::vector<std::string> includes = stringUtil::splitStr(section, ',');
			for (auto& include : includes)
			{
				stringUtil::trim(include);
				File* _fileStorage = SceneAssets.getFile(include);
				if (_fileStorage)
				{
					std::string file = _fileStorage->file + '\n';

					if (output_vertex.active)
						output_vertex.include += file;

					if (output_geometry.active)
						output_geometry.include += file;

					if (output_fragment.active)
						output_fragment.include += file;
				}
			}
		}

		// Properties
		if (locations.properties != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.properties);

			if(output_vertex.active)
				output_vertex.include += section;

			if(output_geometry.active)
				output_geometry.include += section;

			if(output_fragment.active)
				output_fragment.include += section;
		}

		// Attributes
		if (locations.attributes != std::string::npos)
		{
			output_vertex.input = "// Attributes\n";

			// Acquire all
			std::string section = stringUtil::extractSection(file, '{', '}', locations.attributes);
			std::vector<std::string> lines = stringUtil::splitStr(section, '\n');
			for (auto& line : lines)
			{
				line = stringUtil::stripComments(line);
				if (line.empty())
					continue;
				
				// [ First Section : Second Section ]
				std::vector<std::string> property = stringUtil::splitStr(line, ':');
				if (property.size() != 2)
					continue;

				stringUtil::trim(property[0]);
				stringUtil::trim(property[1]);

				// Add attribute information
				output_vertex.input += std::string("layout (location = " + property[1] + ") in " + property[0] + ";\n");
			}
		}
		// Vertex
		if (output_vertex.active)
		{
			output_vertex.behaviour = stringUtil::extractSection(file, '{', '}', locations.vertex);

			VertexShader =
				output_vertex.version + '\n' +
				output_vertex.include + '\n' +
				output_vertex.input + '\n' +
				output_vertex.output + '\n' +
				output_vertex.behaviour;
		}
		// Geometry
		if (output_geometry.active)
		{
			output_geometry.behaviour = stringUtil::extractSection(file, '{', '}', locations.geometry);

			GeomShader =
				output_geometry.version + '\n' +
				output_geometry.include + '\n' +
				output_geometry.input + '\n' +
				output_geometry.output + '\n' +
				output_geometry.behaviour;
		}
		// Fragment
		if (output_fragment.active)
		{
			output_fragment.behaviour = stringUtil::extractSection(file, '{', '}', locations.fragment);

			FragShader =
				output_fragment.version + '\n' +
				output_fragment.include + '\n' +
				output_fragment.input + '\n' +
				output_fragment.output + '\n' +
				output_fragment.behaviour;
		}

	}
}