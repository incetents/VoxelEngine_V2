// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <filesystem>
#include <string>

namespace Vxl
{
	namespace File
	{
		// Creates necessary folders for filepath to be true
		void EnsureDirectory(const std::string& filePath);
		// If filepath is already taken, attempt to fix name
		void DuplicateFixer(std::string& filePath);
	}
}
