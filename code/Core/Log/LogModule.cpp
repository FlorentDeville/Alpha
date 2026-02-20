/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Log/LogModule.h"

#include <cstdarg>

namespace Core
{
	LogModule::LogModule()
		: Core::Singleton<LogModule>()
		, m_logEntryArray()
	{
		m_logEntryArray.Reserve(100);
	}

	LogModule::~LogModule()
	{ }

	void LogModule::Log(LogLevel level, const std::string& text)
	{
		LogEntry entry;
		entry.m_level = level;
		entry.m_text = text;

		m_logEntryArray.PushBack(entry);
		m_onLogAdded(entry);
	}

	void LogModule::Log(LogLevel level, const char* text, va_list args)
	{
		size_t size = std::vsnprintf(nullptr, 0, text, args);
		std::string str(size, '\0');
		std::vsnprintf(str.data(), size + 1, text, args);

		Log(level, str);
	}

	void LogModule::LogDebug(const std::string& text)
	{
		Log(Debug, text);
	}

	void LogModule::LogDebug(const char* text, ...)
	{
		va_list args;
		va_start(args, text);
		Log(Debug, text, args);
		va_end(args);
	}

	void LogModule::LogInfo(const std::string& text)
	{
		Log(Info, text);
	}

	void LogModule::LogInfo(const char* text, ...)
	{
		va_list args;
		va_start(args, text);
		Log(Info, text, args);
		va_end(args);
	}

	void LogModule::LogWarn(const std::string& text)
	{
		Log(Warn, text);
	}

	void LogModule::LogWarn(const char* text, ...)
	{
		va_list args;
		va_start(args, text);
		Log(Warn, text, args);
		va_end(args);
	}

	void LogModule::LogError(const std::string& text)
	{
		Log(Error, text);
	}

	void LogModule::LogError(const char* text, ...)
	{
		va_list args;
		va_start(args, text);
		Log(Error, text, args);
		va_end(args);
	}

	const Core::Array<LogModule::LogEntry>& LogModule::GetAllLogEntries() const
	{
		return m_logEntryArray;
	}
}
