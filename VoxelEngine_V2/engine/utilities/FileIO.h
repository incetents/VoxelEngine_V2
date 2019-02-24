// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "stringUtil.h"

namespace Vxl
{
	namespace IO
	{
		// Check if file exists
		bool fileExists(const std::string& filePath);
		// Interpret file as string
		std::string readFile(const std::string& filePath);
		// Add include directory to txt
		std::string addInclude(std::string data, const std::string& originalDirectory);
	}
}