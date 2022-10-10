/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Process.h"

#include <assert.h>

Process::Process(const std::string& commandline)
	: m_commandline(commandline)
	, m_stdOut(nullptr)
	, m_stdErr(nullptr)
	, m_processInfo()
{}

Process::~Process()
{}

void Process::OnStdOut(const std::function<bool(const std::string& msg)>& callback)
{
	m_stdOut = callback;
}

void Process::OnStdErr(const std::function<bool(const std::string& msg)>& callback)
{
	m_stdErr = callback;
}

bool Process::Run()
{
	// Set the bInheritHandle flag so pipe handles are inherited. 
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	//create standard output pipe
	HANDLE standardOutputWrite;
	bool standardOutputPipe = CreatePipe(&m_standardOutputRead, &standardOutputWrite, &saAttr, 0);
	assert(standardOutputPipe);
	SetHandleInformation(m_standardOutputRead, HANDLE_FLAG_INHERIT, 0);

	//create error output pipe
	HANDLE errorOutputWrite;
	bool errorOutputPipe = CreatePipe(&m_errorOutputRead, &errorOutputWrite, &saAttr, 0);
	assert(errorOutputPipe);
	SetHandleInformation(m_errorOutputRead, HANDLE_FLAG_INHERIT, 0);

	//start process
	STARTUPINFO info = { sizeof(info) };
	info.hStdError = errorOutputWrite;
	info.hStdOutput = standardOutputWrite;
	info.dwFlags = STARTF_USESTDHANDLES;

	bool processCreated = CreateProcess(NULL, m_commandline.data(), NULL, NULL, TRUE, 0, NULL, NULL, &info, &m_processInfo);

	//always close the write pipe handles. I don't use them.
	CloseHandle(standardOutputWrite);
	CloseHandle(errorOutputWrite);

	if (!processCreated)
	{
		OutputDebugString("Failed to start process\n");

		CloseHandle(m_processInfo.hProcess);
		CloseHandle(m_processInfo.hThread);
		CloseHandle(m_standardOutputRead);		
		CloseHandle(m_errorOutputRead);
		

		return false;
	}

	return true;
}

void Process::Wait()
{
	HANDLE WaitHandles[] = { m_processInfo.hProcess, m_standardOutputRead, m_errorOutputRead };

	const DWORD BUFSIZE = 4096;
	BYTE buff[BUFSIZE];

	while (1)
	{
		DWORD dwBytesRead, dwBytesAvailable;

		DWORD dwWaitResult = WaitForMultipleObjects(3, WaitHandles, FALSE, 60000L);

		// Read from the pipes...
		while (PeekNamedPipe(m_standardOutputRead, NULL, 0, NULL, &dwBytesAvailable, NULL) && dwBytesAvailable)
		{
			bool res = ReadFile(m_standardOutputRead, buff, BUFSIZE - 1, &dwBytesRead, 0);
			assert(res);
			std::string output((char*)buff, (size_t)dwBytesRead);

			if (m_stdOut)
				m_stdOut(output);

			//OutputDebugString(output.c_str());
		}

		while (PeekNamedPipe(m_errorOutputRead, NULL, 0, NULL, &dwBytesAvailable, NULL) && dwBytesAvailable)
		{
			bool res = ReadFile(m_errorOutputRead, buff, BUFSIZE - 1, &dwBytesRead, 0);
			assert(res);
			std::string outputErr((char*)buff, (size_t)dwBytesRead);

			if (m_stdErr)
				m_stdErr(outputErr);
		
			//OutputDebugString(outputErr.c_str());
		}

		// Process is done, or we timed out:
		if (dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_TIMEOUT)
			break;
	}

	OutputDebugString("Process over\n");
	CloseHandle(m_processInfo.hProcess);
	CloseHandle(m_processInfo.hThread);
	CloseHandle(m_standardOutputRead);
	CloseHandle(m_errorOutputRead);
}