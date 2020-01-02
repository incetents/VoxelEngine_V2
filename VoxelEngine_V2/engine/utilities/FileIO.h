// Copyright (c) 2020 Emmanuel Lajeunesse
#pragma once

#include "stringUtil.h"
#include "singleton.h"
#include <map>

namespace Vxl
{
	struct File
	{
		const std::string filepath;
		std::string file;

		File(const std::string& _filepath);
	};

	namespace FileIO
	{
		// Check if file exists
		bool fileExists(const std::string& filePath);
		// Interpret file as string
		std::string readFile(const std::string& filePath);
		// Add include directory to txt
		std::string addInclude(std::string data);
		// Get Extension
		std::string getExtension(const std::string& filePath);
		// Get Name
		std::string getName(const std::string& filePath);

		// Creates necessary folders for filepath to be true
		void EnsureDirectory(const std::string& filePath);
		// If filepath is already taken, attempt to fix name
		void DuplicateFixer(std::string& filePath);
	}
}