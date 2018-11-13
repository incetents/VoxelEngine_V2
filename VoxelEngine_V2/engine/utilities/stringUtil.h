// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include <algorithm> 
#include <cctype>
#include <locale>

namespace Vxl
{
	namespace stringUtil
	{
		// File
		std::string readFile(const std::string& filePath);
		// Trim
		void ltrim(std::string &s);
		void rtrim(std::string &s);
		void trim(std::string &s);
		std::string ltrim_copy(std::string s);
		std::string rtrim_copy(std::string s);
		std::string trim_copy(std::string s);
		// Split
		std::vector<std::string> splitStr(const std::string& str, char delimiter);
	}
}