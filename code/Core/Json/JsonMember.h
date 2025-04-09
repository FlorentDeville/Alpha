/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Json/JsonType.h"
#include "Core/Json/JsonValue.h"

#include <string>

namespace Core
{
	class JsonArray;
	class JsonObject;

	class JsonMember
	{
	public:
		JsonMember();
		explicit JsonMember(const std::string& name); //create a mamber set to null
		explicit JsonMember(const std::string& name, int32_t value);
		explicit JsonMember(const std::string& name, float value);
		explicit JsonMember(const std::string& name, double value);
		explicit JsonMember(const std::string& name, const char* pValue);
		explicit JsonMember(const std::string& name, const std::string& value);
		explicit JsonMember(const std::string& name, bool value);
		explicit JsonMember(const std::string& name, JsonObject* pValue);
		explicit JsonMember(const std::string& name, JsonArray* pValue);

		~JsonMember();

		const std::string& GetName() const;
		const JsonValue& GetValue() const;

	private:
		std::string m_name;
		JsonValue m_value;
	};
}
