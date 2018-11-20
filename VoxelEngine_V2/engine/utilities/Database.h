// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <string>

template<class Type>
class Database
{
private:
	std::unordered_map<std::string, Type*> m_database;
	
public:
	inline bool Check(const std::string& name)
	{
		return (m_database.find(name) != m_database.end());
	}
	inline void Delete(const std::string& name)
	{
		if (m_database.find(name) != m_database.end())
		{
			delete m_database[name];
			m_database.erase(name);
		}
	}
	inline void DeleteAndClear()
	{
		for (auto it = m_database.begin(); it != m_database.end(); it++)
		{
			delete it->second;
		}
		m_database.clear();
	}
	inline void Clear()
	{
		m_database.clear();
	}
	inline void Set(const std::string& name, Type* data)
	{
		if (m_database.find(name) != m_database.end())
		{
			delete m_database[name];
		}
		m_database[name] = data;
	}
	inline Type* Get(const std::string& name)
	{
		if (m_database.find(name) != m_database.end())
		{
			return m_database[name];
		}
		return nullptr;
	}
};
