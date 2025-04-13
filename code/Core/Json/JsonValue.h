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
		explicit JsonValue(uint64_t value);
		explicit JsonValue(float value);
		explicit JsonValue(double value);
		explicit JsonValue(bool value);
		explicit JsonValue(const char* pValue);
		explicit JsonValue(const std::string& value);
		explicit JsonValue(JsonArray* pValue);
		explicit JsonValue(JsonObject* pValue);

		void Set(int32_t value);
		void Set(uint64_t value);
		void Set(float value);
		void Set(double value);
		void Set(bool value);
		void Set(const char* value);
		void Set(const std::string& value);
		void Set(JsonArray* pValue);
		void Set(JsonObject* pValue);

		~JsonValue();

		JsonType GetType() const;

		bool GetValueAsBool() const;
		double GetValueAsDouble() const;
		std::string GetValueAsString() const;
		JsonArray* GetValueAsArray() const;
		JsonObject* GetValueAsObject() const;

	private:
		JsonType m_type;
		std::variant<double, std::string, bool, JsonArray*, JsonObject*> m_value;
	};
}
