// Copyright (c) 2020 Emmanuel Lajeunesse
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
	// String Hashing
	// https://gist.github.com/underscorediscovery/81308642d0325fd386237cfa3b44785c
	// fnv1a 32 and 64 bit hash functions
	// key is the data to hash, len is the size of the data (or how much of it to hash against)
	// code license: public domain or equivalent
	// post: https://notes.underscorediscovery.com/constexpr-fnv1a/

	//
	constexpr uint32_t val_32_const = 0x811c9dc5;
	constexpr uint32_t prime_32_const = 0x1000193;
	constexpr uint64_t val_64_const = 0xcbf29ce484222325;
	constexpr uint64_t prime_64_const = 0x100000001b3;
	//
	inline const uint32_t hash_32_fnv1a(const void* key, const uint32_t len)
	{
		const char* data = (char*)key;
		uint32_t hash = val_32_const;
		uint32_t prime = prime_32_const;

		for (uint32_t i = 0; i < len; ++i) {
			uint8_t value = data[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}
	inline const uint64_t hash_64_fnv1a(const void* key, const uint64_t len)
	{
		const char* data = (char*)key;
		uint64_t hash = val_64_const;
		uint64_t prime = prime_64_const;

		for (uint64_t i = 0; i < len; ++i) {
			uint8_t value = data[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}
	//
	inline constexpr uint32_t StringHash32(const char* const str, const uint32_t value = val_32_const) noexcept
	{
		return (str[0] == '\0') ? value : StringHash32(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
	}
	inline constexpr uint64_t StringHash64(const char* const str, const uint64_t value = val_64_const) noexcept
	{
		return (str[0] == '\0') ? value : StringHash64(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
	}

	// Util
	namespace stringUtil
	{
		// Trim
		void ltrim(std::string &s);
		void rtrim(std::string &s);
		void trim(std::string &s);
		std::string ltrim_copy(std::string s);
		std::string rtrim_copy(std::string s);
		std::string trim_copy(std::string s);

		// To Lower
		void toLower(std::string& str);
		std::string toLowerCopy(const std::string& str);

		// Replace
		bool replace(std::string& str, const std::string& from, const std::string& to);

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

		// Check if string is number
		bool isNumber(const std::string& str);
	}
}