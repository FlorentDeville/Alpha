/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonValue.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"
#include "Core/Memory/MemoryMacro.h"

namespace Core
{
	JsonValue::JsonValue()
		: m_type(JsonType::Null)
		, m_value()
	{}

	JsonValue::JsonValue(int32_t value)
		: m_type(JsonType::Number)
	{
		m_value = static_cast<double>(value);
	}

	JsonValue::JsonValue(uint64_t value)
		: m_type(JsonType::Number)
	{
		m_value = static_cast<double>(value);
	}

	JsonValue::JsonValue(float value)
		: m_type(JsonType::Number)
	{
		m_value = static_cast<double>(value);
	}

	JsonValue::JsonValue(double value)
		: m_type(JsonType::Number)
	{
		m_value = value;
	}

	JsonValue::JsonValue(bool value)
		: m_type(JsonType::Boolean)
	{
		m_value = value;
	}

	JsonValue::JsonValue(const char* pValue)
		: m_type(JsonType::String)
	{
		m_value = MAKENEW(std::string)(pValue);
	}

	JsonValue::JsonValue(const std::string& value)
		: m_type(JsonType::String)
	{
		m_value = MAKENEW(std::string)(value);
	}

	JsonValue::JsonValue(JsonObject* pValue)
		: m_type(JsonType::Object)
	{
		m_value = pValue;
	}

	JsonValue::JsonValue(JsonArray* pValue)
		: m_type(JsonType::Array)
	{
		m_value = pValue;
	}

	void JsonValue::Set(int32_t value)
	{
		m_type = JsonType::Number;
		m_value = static_cast<double>(value);
	}

	void JsonValue::Set(uint64_t value)
	{
		m_type = JsonType::Number;
		m_value = static_cast<double>(value);
	}

	void JsonValue::Set(float value)
	{
		m_type = JsonType::Number;
		m_value = static_cast<double>(value);
	}

	void JsonValue::Set(double value)
	{
		m_type = JsonType::Number;
		m_value = static_cast<double>(value);
	}

	void JsonValue::Set(bool value)
	{
		m_type = JsonType::Boolean;
		m_value = value;
	}

	void JsonValue::Set(const char* value)
	{
		m_type = JsonType::String;
		
		std::string* pString = std::get<std::string*>(m_value);
		if (pString)
			*pString = value;
		else
			m_value = MAKENEW(std::string)(value);
	}

	void JsonValue::Set(const std::string& value)
	{
		m_type = JsonType::String;
		m_value = MAKENEW(std::string)(value);
	}

	void JsonValue::Set(JsonArray* pValue)
	{
		m_type = JsonType::Array;
		m_value = pValue;
	}

	void JsonValue::Set(JsonObject* pValue)
	{
		m_type = JsonType::Object;
		m_value = pValue;
	}

	JsonValue::~JsonValue()
	{
		if (m_type == JsonType::Array)
		{
			JsonArray* pArray = std::get<JsonArray*>(m_value);
			DELETE(pArray);
			m_value = static_cast<JsonArray*>(nullptr);
		}
		else if (m_type == JsonType::Object)
		{
			JsonObject* pObject = std::get<JsonObject*>(m_value);
			DELETE(pObject);
			m_value = static_cast<JsonObject*>(nullptr);
		}
		else if (m_type == JsonType::String)
		{
			std::string* pString = std::get<std::string*>(m_value);
			DELETE(pString);
			m_value = static_cast<std::string*>(nullptr);
		}
	}

	JsonType JsonValue::GetType() const
	{
		return m_type;
	}

	bool JsonValue::GetValueAsBool() const
	{
		return std::get<bool>(m_value);
	}

	double JsonValue::GetValueAsDouble() const
	{
		return std::get<double>(m_value);
	}

	const std::string& JsonValue::GetValueAsString() const
	{
		const std::string* pString = std::get<std::string*>(m_value);
		return *pString;
	}

	JsonArray* JsonValue::GetValueAsArray() const
	{
		return std::get<JsonArray*>(m_value);
	}

	JsonObject* JsonValue::GetValueAsObject() const
	{
		return std::get<JsonObject*>(m_value);
	}
}
