/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Json/JsonType.h"

#include <string>
#include <variant>

namespace Core
{
	class JsonArray;
	class JsonObject;

	class JsonValue
	{
	public:
		JsonValue();

		explicit JsonValue(int32_t value);
		explicit JsonValue(float value);
		explicit JsonValue(bool value);
		explicit JsonValue(const char* pValue);
		explicit JsonValue(const std::string& value);
		explicit JsonValue(JsonArray* pValue);
		explicit JsonValue(JsonObject* pValue);

		~JsonValue();

	private:
		JsonType m_type;
		std::variant<double, std::string, bool, JsonArray*, JsonObject*> m_value;
	};
}
