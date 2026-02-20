/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"
#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

#include <string>

namespace Core
{
	class LogModule : public Core::Singleton<LogModule>
	{
	public:

		enum LogLevel
		{
			Debug,
			Info,
			Warn,
			Error,
		};

		struct LogEntry
		{
			LogLevel m_level;
			std::string m_text;
		};

		LogModule();
		~LogModule();

		void Log(LogLevel level, const std::string& text);
		void Log(LogLevel level, const char* text, va_list args);

		void LogDebug(const std::string& text);
		void LogDebug(const char* text, ...);

		void LogInfo(const std::string& text);
		void LogInfo(const char* text, ...);
		
		void LogWarn(const std::string& text);
		void LogWarn(const char* text, ...);
		
		void LogError(const std::string& text);
		void LogError(const char* text, ...);

		const Core::Array<LogEntry>& GetAllLogEntries() const;

		EVENT_DECL(LogAdded, void(const LogEntry& entry))

	private:
		Core::Array<LogEntry> m_logEntryArray;
	};
}
