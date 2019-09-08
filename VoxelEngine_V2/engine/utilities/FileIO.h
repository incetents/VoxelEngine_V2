// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "stringUtil.h"
#include "singleton.h"
#include <map>

namespace Vxl
{
	static class GlobalFiles : public Singleton<class GlobalFiles>
	{
	private:
		struct FileData
		{
			std::string filepath;
			std::string file;

			FileData() {}
			FileData(const std::string& _filepath, const std::string& _file)
			{
				filepath = _filepath;
				file = _file;
			}
		};

		static std::map<std::string, FileData> m_files;
	public:
		void LoadFile(const std::string& name, const std::string& filepath);
		void UnloadFile(const std::string& name);
		void ReloadFiles();
		bool HasFile(const std::string& name)
		{
			return m_files.find(name) != m_files.end();
		}
		const std::string& GetFile(const std::string& name)
		{
			if (HasFile(name))
				return m_files[name].file;
			else
			{
				static std::string empty;
				return empty;
			}
		}


	} SingletonInstance(GlobalFiles);


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
	}
}