/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "CommandLine.h"

#include <assert.h>
#include <sstream>

CommandLine::CommandLine()
	: m_arguments()
{}

CommandLine::~CommandLine()
{}

void CommandLine::AddArgument(const std::string& flag, const std::string& defaultValue, ARG_TYPE type, ARG_ACTION action, void* pDestination, const std::string& description)
{
	m_arguments.push_back(Argument());
	Argument& arg = m_arguments.back();
	arg.m_flags.push_back(flag);
	arg.m_defaultValue = defaultValue;
	arg.m_type = type;
	arg.m_action = action;
	arg.m_pDestination = pDestination;
	arg.m_description = description;

	if (action == ARG_ACTION::STORE_TRUE)
	{
		bool* pDest = static_cast<bool*>(pDestination);
		if (defaultValue == "0" || defaultValue == "false" || defaultValue == "FALSE")
			*pDest = false;
		else
			*pDest = true;
	}
}

bool CommandLine::Parse(const char* pCommandLine)
{
	std::stringstream stream;
	stream.str(pCommandLine);

	while (!stream.eof())
	{
		std::string argument;
		stream >> argument;

		std::vector<Argument>::const_iterator it = std::find_if(m_arguments.cbegin(), m_arguments.cend(), [&argument](const Argument& arg) {
			if (std::find(arg.m_flags.cbegin(), arg.m_flags.cend(), argument) == arg.m_flags.cend())
				return false;

			return true;
			});

		if (it == m_arguments.cend())
		{
			assert(false);
		}

		switch (it->m_action)
		{
		case ARG_ACTION::STORE:
		{
			switch (it->m_type)
			{
			case ARG_TYPE::STRING:
			{
				std::string value;
				stream >> value;

				std::string* pDest = static_cast<std::string*>(it->m_pDestination);
				*pDest = value;
			}
				break;

			default:
				assert(false);
				break;
			}
		}
			break;

		case ARG_ACTION::STORE_TRUE:
		{
			bool* pDest = static_cast<bool*>(it->m_pDestination);
			*pDest = true;
		}
			break;

		default:
			assert(false);
			break;
		}
	}

	return true;
}

void CommandLine::PrintHelp()
{

}
