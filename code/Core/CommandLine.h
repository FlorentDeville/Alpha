/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <vector>

class CommandLine
{
public:
	enum class ARG_TYPE
	{
		STRING,
		BOOL
	};

	enum class ARG_ACTION
	{
		STORE,		//store the given value
		STORE_TRUE	//Flag set to true
	};

	CommandLine();
	~CommandLine();

	void AddArgument(const std::string& flag, const std::string& defaultValue, ARG_TYPE type, ARG_ACTION action, void* destination, const std::string& description);

	bool Parse(const char* pCommandLine);

	void PrintHelp();

private:
	struct Argument
	{
		std::vector<std::string>	m_flags;
		std::string					m_defaultValue;
		ARG_TYPE					m_type;
		ARG_ACTION					m_action;
		void*						m_pDestination;
		std::string					m_description;
	};

	std::vector<Argument> m_arguments;
};
