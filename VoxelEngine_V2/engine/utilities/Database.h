// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <string>
#include <set>

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
			m_database.erase(name);
			delete m_database[name];
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
		// Delete Existing Item if duplicate is found
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
	inline const std::unordered_map<std::string, Type*> Get() const
	{
		return m_database;
	}
	inline UINT Size(void) const
	{
		return (UINT)m_database.size();
	}
};

template<class Type>
class DatabaseSet
{
private:
	std::set<Type*> m_database;

public:
	inline bool Check(Type* data)
	{
		return (m_database.find(data) != m_database.end());
	}
	inline void Delete(Type* data)
	{
		m_database.erase(data);
		delete data;
	}
	inline void DeleteAndClear()
	{
		for (auto it = m_database.begin(); it != m_database.end(); it++)
		{
			Type* test = *it;
			delete *it;
		}
		m_database.clear();
	}
	inline void Clear()
	{
		m_database.clear();
	}
	inline void Set(Type* data)
	{
		m_database.insert(data);
	}
};