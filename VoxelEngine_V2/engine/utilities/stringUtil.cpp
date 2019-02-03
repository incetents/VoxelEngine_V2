// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "stringUtil.h"
#include "../utilities/logger.h"

namespace Vxl
{
	std::string stringUtil::readFile(const std::string& filePath)
	{
		std::ifstream t(filePath);
		if (t.is_open())
		{
			std::string str((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
			t.close();
			return str;
		}
		// error, return empty string
		Logger.error("Unable to read file: " + filePath);
		return std::string();
	}
	void stringUtil::ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}
	void stringUtil::rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}
	void stringUtil::trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}
	std::string stringUtil::ltrim_copy(std::string s) {
		ltrim(s);
		return s;
	}
	std::string stringUtil::rtrim_copy(std::string s) {
		rtrim(s);
		return s;
	}
	std::string stringUtil::trim_copy(std::string s) {
		trim(s);
		return s;
	}
	std::vector<std::string> stringUtil::splitStr(const std::string& str, char delimiter)
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
	std::vector<std::string> stringUtil::splitStr(const std::string& str, std::string delimiters)
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
}