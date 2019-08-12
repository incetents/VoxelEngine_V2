// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <unordered_map>
#include <string>
#include <set>
#include <map>

#include "../utilities/logger.h"

// Used as a means of inheriting a unique map of objects //
namespace Vxl
{
	enum class AssetMessage
	{
		NONE,
		CREATED,
		LOADED
	};

	// Storage for objects with names
	template<class Type>
	class Asset
	{
		using UNNAMED_ASSETS = std::set<Type*>;
		using NAMED_ASSETS = std::map<std::string, Type*>;
	protected:
		// Protect Construction
		Asset() {}

		// No copy
		Asset(const Asset&) = delete;
		Asset<Type>& operator=(const Asset&) = delete;
		
		// Database
		static UNNAMED_ASSETS m_assets;
		static NAMED_ASSETS m_named_assets;
		
		// If name already exists, append additional info on it to make it unique
		static std::string FixNameDuplicate(const std::string& name)
		{
			std::string Finalname = name;
			static int DuplicateCounter = 0;
			// Add to name if duplicate found
			while (m_named_assets.find(Finalname) != m_named_assets.end())
				Finalname = name + " (" + std::to_string(DuplicateCounter++) + ')';
			// Return
			return Finalname;
		}
		// Add
		static void AddNamedAsset(const std::string& name, Type* object, AssetMessage message)
		{
			// Delete object with the same name
			if (m_named_assets.find(name) != m_named_assets.end())
			{
				Logger.error("Duplicate [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
				delete m_named_assets[name];
			}

			// Message
			switch (message)
			{
			case AssetMessage::CREATED:
				Logger.log("Created [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
				break;
			case AssetMessage::LOADED:
				Logger.log("Loaded [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
				break;
			}

			// Store
			m_named_assets[name] = object;
		}
		static void AddUnnamedAsset(Type* object, AssetMessage message)
		{
			std::ostringstream address;
			address << (void const *)object;
			std::string name = address.str();

			// Message
			switch (message)
			{
			case AssetMessage::CREATED:
			{
				Logger.log("Created Object, Ptr = " + name + "\t\t object: " + std::string(typeid(Type).name()));
				break;
			}
			case AssetMessage::LOADED:
				Logger.log("Loaded Object, Ptr = " + name + "]\t\t object: " + std::string(typeid(Type).name()));
				break;
			}

			// Check for duplicate
			if(m_assets.find(object) != m_assets.end())
				Logger.error("Duplicate Object, Ptr = " + name + "]\t\t object: " + std::string(typeid(Type).name()));

			// Store
			m_assets.insert(object);
		}
	public:

		// ~ Utility ~ //
		static bool CheckAsset(const std::string& name)
		{
			return (m_named_assets.find(name) != m_named_assets.end());
		}

		// ~ Asset Deletion ~ //
		static void DeleteNamedAsset(const std::string& name)
		{
			// Delete object if exists
			if (m_named_assets.find(name) != m_named_assets.end())
			{
				auto object = m_named_assets[name];
				m_named_assets.erase(name);
				delete object;
			}
		}
		static void DeleteNamedAsset(Type* object)
		{
			if (!object)
				return;

			for (auto it = m_named_assets.begin(); it != m_named_assets.end(); it++)
			{
				if (it->second == object)
				{
					m_named_assets.erase(it->first);
					break;
				}
			}
			delete object;
		}
		static void DeleteUnnamedAsset(Type* object)
		{
			if (!object)
				return;

			if (m_assets.find(object) != m_assets.end())
			{
				m_assets.erase(object);
				delete object;
			}
		}
		static void DeleteAllAssets(void)
		{
			for (auto it = m_named_assets.begin(); it != m_named_assets.end(); it++)
				delete it->second;

			for (auto it = m_assets.begin(); it != m_assets.end(); it++)
				delete *it;

			m_named_assets.clear();
			m_assets.clear();
		}

		// ~ Get Asset Info ~ //
		static Type* GetAsset(const std::string name)
		{
			if (m_named_assets.find(name) != m_named_assets.end())
			{
				return m_named_assets[name];
			}
			return nullptr;
		}
		static NAMED_ASSETS GetAllNamedAssets(void)
		{
			return m_named_assets;
		}
		static size_t GetAssetCount(void)
		{
			return m_named_assets.size();
		}
	};

	template<class Type>
	std::set<Type*> Asset<Type>::m_assets;
	template<class Type>
	std::map<std::string, Type*> Asset<Type>::m_named_assets;
}