// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Macros.h"

#include "singleton.h"

#include <vector>
#include <string>
#include <Windows.h>
#include <iostream>

namespace Vxl
{
	enum class ConsoleColor
	{
		BLACK = 0,
		DARK_BLUE,
		DARK_GREEN,
		DARK_CYAN,
		DARK_RED,
		DARK_PURPLE,
		DARK_YELLOW,
		DARK_WHITE,
		DARK_GRAY,
		BLUE,
		GREEN,
		CYAN,
		RED,
		PURPLE,
		YELLOW,
		WHITE,
		TOTAL
	};
	enum class LogType
	{
		Message,
		Error
	};

	class LoggerMessage
	{
		NO_COPY_CONSTRUCTOR
		//LoggerMessage(const LoggerMessage&) = delete;
		//LoggerMessage(const LoggerMessage&) {}
	private:
		// Data
		std::string  m_string;
		LogType		 m_type;
		ConsoleColor m_color;

	public:
		LoggerMessage(
			HANDLE handle,
			const std::string& msg,
			LogType type,
			ConsoleColor color
		);
	};

	static class Logger : public Singleton<class Logger>
	{
	private:
		// Data
		HANDLE						m_ConsoleHandle;
		std::vector<LoggerMessage*> m_log;

		inline void AddMessage(const std::string& msg, LogType type, ConsoleColor color)
		{
			m_log.push_back(new LoggerMessage(m_ConsoleHandle, msg, type, color));
		}

	public:

		Logger();

		inline const HANDLE getHandle() const { return m_ConsoleHandle;}

		inline void log		(const std::string& msg) { AddMessage(msg, LogType::Message, ConsoleColor::WHITE); }
		inline void error	(const std::string& msg) { AddMessage(msg, LogType::Error, ConsoleColor::DARK_RED); }

	} SingletonInstance(Logger);

	
}

