// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "stringUtil.h"
#include "../utilities/Logger.h"

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
		// To Lower
		void toLower(std::string& str)
		{
			std::transform(str.begin(), str.end(), str.begin(),
				[](unsigned char c){ return std::tolower(c); });
		}
		std::string toLowerCopy(const std::string& str)
		{
			std::string _str = str;
			toLower(_str);
			return _str;
		}
		// Replace
		bool replace(std::string& str, const std::string& from, const std::string& to) {
			size_t start_pos = str.find(from);
			if (start_pos == std::string::npos)
				return false;
			str.replace(start_pos, from.length(), to);
			return true;
		}
		// Removal
		std::string stripComments(const std::string& str)
		{
			std::string result;
			std::vector<std::string> lines = splitStr(str, '\n');
			for (auto& line : lines)
			{
				std::size_t index = line.find_first_of("//");
				if (index != std::string::npos)
				{
					result += line.substr(0, index - 1) + '\n';
				}
				else
					result += line + '\n';
			}
			return result;
		}
		// Get name from file path
		std::string nameFromFilepath(const std::string& filePath)
		{
			size_t lastDashLoc = filePath.find_last_of("/\\");
			return filePath.substr(lastDashLoc + 1, filePath.length() - lastDashLoc);
		}
		// Count Characters in string
		uint64_t countChar(const std::string& str, char c)
		{
			return std::count(str.begin(), str.end(), c);
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

			// Nothing found
			if (loc1 == std::string::npos || loc2 == std::string::npos)
				return std::string();

			return str.substr(loc1 + 1, loc2 - loc1 - 1);
		}
		std::string extractNameFromPath(const std::string& str)
		{
			// Extract final directory
			size_t location1 = str.find_last_of('\\');

			if (location1 == std::string::npos)
				location1 = str.find_last_of('/');

			if (location1 == std::string::npos)
				return std::string();

			// Extract extension
			size_t location2 = str.find_last_of('.');

			// No extension
			if(location2 == std::string::npos)
				return str.substr(location1 + 1, str.length() - location1 - 1);

			//
			return str.substr(location1 + 1, location2 - location1 - 1);
		}
		std::string extractSection(const std::string& str, char start, char end, std::size_t startIndex)
		{
			std::size_t readStart = str.find_first_of(start, startIndex);
			if (readStart == std::string::npos)
				return std::string();

			size_t length = str.length();
			uint32_t counter = 1;
			size_t readEnd = readStart;
			for (size_t i = readStart + 1; i < length; i++)
			{
				if (str[i] == start)
					counter++;
				else if (str[i] == end)
					counter--;

				if (counter == 0)
				{
					readEnd = i;
					break;
				}
			}

			// Offset to make sure it doesn't extract start and end characters
			readStart++;
			readEnd -= 2;
			if (readEnd < readStart)
				return std::string();

			return str.substr(readStart, readEnd - readStart + 1);
		}
		bool isNumber(const std::string& str)
		{
			std::string::const_iterator it = str.begin();
			while (it != str.end() && std::isdigit(*it)) ++it;
			return !str.empty() && it == str.end();
		}
	}
}