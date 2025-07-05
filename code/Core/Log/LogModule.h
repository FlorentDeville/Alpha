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

		void LogDebug(const std::string& text);
		void LogInfo(const std::string& text);
		void LogWarn(const std::string& text);
		void LogError(const std::string& text);

		EVENT_DECL(LogAdded, void(const LogEntry& entry))

	private:
		Core::Array<LogEntry> m_logEntryArray;
	};
}
