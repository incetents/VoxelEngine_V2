// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FileSystem.h"

#include <iostream>

namespace fs = std::experimental::filesystem;

namespace Vxl
{
	namespace File
	{
		// //	Legacy
		//	fs::create_directory(_path);
		//
		//	fs::create_directory("screenshots");
		//	
		//	if(fs::is_directory("screenshots"))
		//		fs::create_directory("screenshots\\scr");
		//
		//	std::cout << "exists() = " << fs::exists(pathToShow) << "\n"
		//		<< "root_name() = " << pathToShow.root_name() << "\n"
		//		<< "root_path() = " << pathToShow.root_path() << "\n"
		//		<< "relative_path() = " << pathToShow.relative_path() << "\n"
		//		<< "parent_path() = " << pathToShow.parent_path() << "\n"
		//		<< "filename() = " << pathToShow.filename() << "\n"
		//		<< "stem() = " << pathToShow.stem() << "\n"
		//		<< "extension() = " << pathToShow.extension() << "\n";

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
				if(!fs::is_directory(_parentPaths[i]))
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