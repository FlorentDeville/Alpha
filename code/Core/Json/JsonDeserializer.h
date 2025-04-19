/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Core
{
	class JsonArray;
	class JsonObject;

	class JsonDeserializer
	{
	public:
		JsonDeserializer();
		~JsonDeserializer();

		bool Deserialize(const std::string& input, JsonObject& output);

	private:
		std::string_view m_document;
		std::string m_token;

		bool ReadNextToken();

		void ReadArray(JsonArray& output);
		void ReadObject(JsonObject& output);
	};
}
