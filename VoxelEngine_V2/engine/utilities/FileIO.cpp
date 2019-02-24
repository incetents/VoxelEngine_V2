// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FileIO.h"

#include "logger.h"

namespace Vxl
{
	namespace IO
	{
		// Check if file exists
		bool fileExists(const std::string& filePath)
		{
			struct stat buffer;
			return (stat(filePath.c_str(), &buffer) == 0);
			//std::ifstream infile(filePath);
			//return infile.good();
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
		std::string addInclude(std::string data, const std::string& originalDirectory)
		{
			// Do this for every include statement in the file
			size_t includeLoc;
			while ((includeLoc = data.find("#include")) != std::string::npos)
			{
				size_t endlineLoc = data.find('\n', includeLoc);
				std::string includePath = data.substr(includeLoc + 8, endlineLoc - includeLoc - 8);
				// Find end of line,
				stringUtil::removeChar(includePath, " \t\"");

				size_t lastDashLoc = originalDirectory.find_last_of("/\\");
				std::string directory = originalDirectory.substr(0, lastDashLoc + 1) + includePath;

				if (fileExists(directory))
				{
					// Leave include signature
					std::string include = readFile(directory);
					include = "// [INCLUDE \"" + includePath + "\"]\n" + include + "\n// [END INCLUDE]\n";
					// replace #include line with the file loaded from the include
					data.replace(includeLoc, endlineLoc - includeLoc, include);
				}
				else
				{
					Logger.error("addInclude() for: " + directory + " does not exist!");
					data.erase(includeLoc, endlineLoc - includeLoc);
				}

			}
			return data;
		}
	}
}