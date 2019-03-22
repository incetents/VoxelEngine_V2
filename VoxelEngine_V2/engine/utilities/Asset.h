// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <unordered_map>
#include <string>
#include <set>

#include "../utilities/logger.h"

namespace Vxl
{
	template<class Type>
	class Asset
	{
	protected:
		// Protect Construction
		Asset() {}
		Asset(const Asset&) = delete;
		// Database
		static std::unordered_map<std::string, Type*> m_database;
		// If name already exists, append additional info on it to make it unique
		static std::string FixNameDuplicate(const std::string& name)
		{
			std::string Finalname = name;
			static int DuplicateCounter = 0;
			// Add to name if duplicate found
			while (m_database.find(Finalname) != m_database.end())
				Finalname = name + " (" + std::to_string(DuplicateCounter++) + ')';
			// Return
			return Finalname;
		}
		// Add
		static void AddToDatabase(const std::string& name, Type* object)
		{
			// Delete object with the same name
			if (m_database.find(name) != m_database.end())
			{
				Logger.error("Duplicate [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
				delete m_database[name];
			}

			// Store
			m_database[name] = object;
		}
		// Message
		static void Message_Created(const std::string& name, Type* object)
		{
			Logger.log("Created [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
		}
		static void Message_Loaded(const std::string& name, Type* object)
		{
			Logger.log("Loaded [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
		}
	public:

		// Check Duplication
		static bool CheckIfExists(const std::string& name)
		{
			return (m_database.find(name) != m_database.end());
		}
		// Deletion
		static void Delete(const std::string& name)
		{
			// Delete object if exists
			if (m_database.find(name) != m_database.end())
			{
				delete m_database[name];
				m_database.erase(name);
			}
		}
		static void Delete(Type* value)
		{
			if (!value)
				return;

			for (auto it = m_database.begin(); it != m_database.end(); it++)
			{
				if (it->second == value)
				{
					m_database.erase(it->first);
					break;
				}
			}
			delete value;
		}
		// Delete All
		static void DeleteAndClearAll(void)
		{
			for (auto it = m_database.begin(); it != m_database.end(); it++)
				delete it->second;

			m_database.clear();
		}

		// Get
		static Type* Get(const std::string name)
		{
			if (m_database.find(name) != m_database.end())
			{
				return m_database[name];
			}
			return nullptr;
		}
		// Get All Assets
		static std::unordered_map<std::string, Type*> GetDatabase(void)
		{
			return m_database;
		}
		// Size
		static size_t GetTotalCount(void)
		{
			return m_database.size();
		}
	};

	template<class Type>
	std::unordered_map<std::string, Type*> Asset<Type>::m_database;
}