// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <algorithm> 
#include <cctype>
#include <fstream>
#include <locale>
#include <iostream>
#include <iterator>
#include <streambuf>
#include <sstream>
#include <string>
#include <vector>

namespace Vxl
{
	namespace stringUtil
	{
		// Trim
		void ltrim(std::string &s);
		void rtrim(std::string &s);
		void trim(std::string &s);
		std::string ltrim_copy(std::string s);
		std::string rtrim_copy(std::string s);
		std::string trim_copy(std::string s);

		// Removal
		std::string stripComments(const std::string& str);
		
		// Get name from file path
		std::string nameFromFilepath(const std::string& filePath);

		// Count Characters in string
		uint64_t countChar(const std::string& str, char c);
		
		// Remove characters from string
		void removeChar(std::string& str, char c);
		void removeChar(std::string& str, std::string chars);
		
		// Split
		std::vector<std::string> splitStr(const std::string& str, char delimiter);
		std::vector<std::string> splitStr(const std::string& str, std::string delimiters);
		
		// Extract Data Simply
		std::string extractFirst(const std::string& str, char start, char end);
		std::string extractNameFromPath(const std::string& str);
		std::string extractSection(const std::string& str, char start, char end, std::size_t startIndex = 0);
	}
}