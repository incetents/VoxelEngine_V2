// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FileIO.h"

#include "Macros.h"
#include "Logger.h"

#include <filesystem>

namespace fs = std::experimental::filesystem;

namespace Vxl
{
	File::File(const std::string& _filepath)
		: filepath(_filepath)
	{
		if (FileIO::fileExists(filepath))
			file = FileIO::readFile(filepath);
		else
			VXL_ERROR("Filepath does not exist:" + _filepath);
	}

	namespace FileIO
	{
		// Check if file exists
		bool fileExists(const std::string& filePath)
		{
			struct stat buffer;
			return (stat(filePath.c_str(), &buffer) == 0);
		}
		// Interpret file as string
		std::string readFile(const std::string& filePath)
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
		// Add include directory to txt
		std::string addInclude(std::string data)
		{
			// Do this for every include statement in the file
			size_t includeLoc;
			while ((includeLoc = data.find("#include")) != std::string::npos)
			{
				size_t endlineLoc = data.find('\n', includeLoc);
				std::string includePath = data.substr(includeLoc + 8, endlineLoc - includeLoc - 8);
				// Find end of line,
				stringUtil::removeChar(includePath, " \t\"");
				stringUtil::trim(includePath);

				//size_t lastDashLoc = originalDirectory.find_last_of("/\\");
				//std::string directory = originalDirectory.substr(0, lastDashLoc + 1) + includePath;

				if (fileExists(includePath))
				{
					// Leave include signature
					std::string include = readFile(includePath);
					include = "// [INCLUDE \"" + includePath + "\"]\n" + include + "\n// [END INCLUDE]\n";
					// replace #include line with the file loaded from the include
					data.replace(includeLoc, endlineLoc - includeLoc, include);
				}
				else
				{
					Logger.error("addInclude() for: " + includePath + " does not exist!");
					data.erase(includeLoc, endlineLoc - includeLoc);
				}

			}
			return data;
		}
		std::string getExtension(const std::string& filePath)
		{
			std::size_t pos = filePath.find_last_of('.');
			if (pos == std::string::npos)
				return "";

			return filePath.substr(pos + 1);
		}
		std::string getName(const std::string& filePath)
		{
			std::size_t dir = filePath.find_last_of('/');
			if (dir == std::string::npos)
				return "";

			std::size_t dot = filePath.find_last_of('.');
			if (dot == std::string::npos)
				return filePath.substr(dir + 1);

			return filePath.substr(dir + 1, dot - dir - 1);
		}

		// Creates necessary folders for filepath to be true
		void EnsureDirectory(const std::string& filePath)
		{
			fs::path _path(filePath);
			std::vector<fs::path> _parentPaths;

			while (_path.has_parent_path())
			{
				_path = _path.parent_path();
				_parentPaths.push_back(_path);
			}

			int totalParentPathsCount = (int)_parentPaths.size() - 1;
			for (int i = totalParentPathsCount; i >= 0; i--)
			{
				if (!fs::is_directory(_parentPaths[i]))
					fs::create_directory(_parentPaths[i]);
			}
		}
		// If filepath is already taken, attempt to fix name
		void DuplicateFixer(std::string& filePath)
		{
			fs::path _path(filePath);

			// no duplicates
			if (!fs::exists(_path))
				return;

			uint32_t suffix = 0;
			std::string stem = _path.stem().string();
			while (fs::exists(_path))
			{
				_path = _path.parent_path().string() + '\\' + stem + '(' + std::to_string(suffix) + ')' + _path.extension().string();
				suffix++;
			}
			filePath = _path.string();
		}
	}
}