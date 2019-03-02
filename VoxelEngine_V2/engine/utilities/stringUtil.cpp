// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "stringUtil.h"
#include "../utilities/logger.h"

namespace Vxl
{
	namespace stringUtil
	{
		// Trim
		void ltrim(std::string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
				return !std::isspace(ch);
			}));
		}
		void rtrim(std::string &s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
				return !std::isspace(ch);
			}).base(), s.end());
		}
		void trim(std::string &s) {
			ltrim(s);
			rtrim(s);
		}
		std::string ltrim_copy(std::string s) {
			ltrim(s);
			return s;
		}
		std::string rtrim_copy(std::string s) {
			rtrim(s);
			return s;
		}
		std::string trim_copy(std::string s) {
			trim(s);
			return s;
		}
		// Get name from file path
		std::string nameFromFilepath(const std::string& filePath)
		{
			size_t lastDashLoc = filePath.find_last_of("/\\");
			return filePath.substr(lastDashLoc + 1, filePath.length() - lastDashLoc);
		}
		// Remove characters from string
		void removeChar(std::string& str, char c)
		{
			str.erase(std::remove(str.begin(), str.end(), c), str.end());
		}
		void removeChar(std::string& str, std::string chars)
		{
			size_t charcount = chars.length();
			for(size_t i = 0; i < charcount; i++)
				removeChar(str, chars[i]);
		}
		// Split
		std::vector<std::string> splitStr(const std::string& str, char delimiter)
		{
			std::vector<std::string> tokens;
			std::string token;
			std::istringstream tokenStream(str);
			while (std::getline(tokenStream, token, delimiter))
			{
				tokens.push_back(token);
			}
			return tokens;
		}
		std::vector<std::string> splitStr(const std::string& str, std::string delimiters)
		{
			std::vector<std::string> tokens;
			std::stringstream stringStream(str);
			std::string line;
			while (std::getline(stringStream, line))
			{
				std::size_t prev = 0, pos;
				while ((pos = line.find_first_of(delimiters, prev)) != std::string::npos)
				{
					if (pos > prev)
						tokens.push_back(line.substr(prev, pos - prev));
					prev = pos + 1;
				}
				if (prev < line.length())
					tokens.push_back(line.substr(prev, std::string::npos));
			}
			return tokens;
		}
		// Extract Data Simply
		std::string extractFirst(const std::string& str, char start, char end)
		{
			size_t loc1 = str.find_first_of(start);
			size_t loc2 = str.find_first_of(end);
			if (loc1 == -1 || loc2 == -1)
				return std::string();

			return str.substr(loc1 + 1, loc2 - loc1 - 1);
		}
	}
}