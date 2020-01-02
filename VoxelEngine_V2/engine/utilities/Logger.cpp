// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Logger.h"

#include "Macros.h"

namespace Vxl
{
	LoggerMessage::LoggerMessage(
			HANDLE handle,
			const std::string& msg,
			LogType type,
			ConsoleColor color
		) : m_string(msg), m_type(type), m_color(color)
		{

#ifdef GLOBAL_OUTPUT_CONSOLE
			SetConsoleTextAttribute(handle, (int)color);
			printf((msg + '\n').c_str());
#endif
		}

	Logger::Logger()
	{
#ifdef GLOBAL_OUTPUT_CONSOLE
		m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	}
}