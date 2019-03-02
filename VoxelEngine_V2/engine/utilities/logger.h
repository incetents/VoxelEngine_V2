// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

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
	private:
		std::string  m_string;
		LogType		 m_type;
		ConsoleColor m_color;

	public:
		LoggerMessage(
			HANDLE handle,
			const std::string& msg,
			LogType type,
			ConsoleColor color
		) : m_string(msg), m_type(type), m_color(color)
		{

			SetConsoleTextAttribute(handle, (int)color);
			printf((msg + '\n').c_str());
		}

		// Remove duplication
		LoggerMessage(LoggerMessage const&) = delete;
		void operator=(LoggerMessage const&) = delete;
	};

	static class Logger : public Singleton<class Logger>
	{
	private:
		HANDLE m_ConsoleHandle;
		std::vector<LoggerMessage*> m_log;

		void AddMessage(
			const std::string& msg,
			LogType type = LogType::Message,
			ConsoleColor color = ConsoleColor::WHITE)
		{
			m_log.push_back(new LoggerMessage(m_ConsoleHandle, msg, type, color));
		}

	public:
		Logger() { m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); }

		inline const HANDLE getHandle() const { return m_ConsoleHandle;}

		void log	(const std::string& msg) { AddMessage(msg); }
		void error	(const std::string& msg) { AddMessage(msg, LogType::Error, ConsoleColor::DARK_RED); }

	} SingletonInstance(Logger);

	
}

