/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <functional>
#include <string>

#include <Windows.h>

class Process
{
public:
	Process(const std::string& commandline);
	~Process();

	void OnStdOut(const std::function<bool(const std::string& msg)>& callback);
	void OnStdErr(const std::function<bool(const std::string& msg)>& callback);

	bool Run();

	void Wait();

private:
	std::function<bool(const std::string& msg)> m_stdOut;
	std::function<bool(const std::string& msg)> m_stdErr;

	std::string m_commandline;

	PROCESS_INFORMATION m_processInfo;
	HANDLE m_standardOutputRead;
	HANDLE m_errorOutputRead;
};
