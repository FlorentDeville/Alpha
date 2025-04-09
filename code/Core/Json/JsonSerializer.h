/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <sstream>
#include <string>

namespace Core
{
	class JsonObject;

	class JsonSerializer
	{
	public:
		JsonSerializer();
		~JsonSerializer();

		bool Serialize(const JsonObject& input, std::string& output);

	private:
		std::stringstream m_stream;
	};
}