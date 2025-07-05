/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Log/LogModule.h"

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

	void LogModule::LogDebug(const std::string& text)
	{
		Log(Debug, text);
	}

	void LogModule::LogInfo(const std::string& text)
	{
		Log(Info, text);
	}

	void LogModule::LogWarn(const std::string& text)
	{
		Log(Warn, text);
	}

	void LogModule::LogError(const std::string& text)
	{
		Log(Error, text);
	}
}
