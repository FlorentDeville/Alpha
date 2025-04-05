/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonArray.h"

#include "Core/Json/JsonValue.h"

namespace Core
{
	JsonArray::JsonArray()
		: m_elements()
	{}

	JsonArray::~JsonArray()
	{}

	void JsonArray::AddElement(int32_t value)
	{
		m_elements.push_back(JsonValue(value));
	}

	void JsonArray::AddElement(float value)
	{
		m_elements.push_back(JsonValue(value));
	}

	void JsonArray::AddElement(bool value)
	{
		m_elements.push_back(JsonValue(value));
	}

	void JsonArray::AddElement(const std::string& value)
	{
		m_elements.push_back(JsonValue(value));
	}

	void JsonArray::AddElement(JsonObject* pValue)
	{
		m_elements.push_back(JsonValue(pValue));
	}

	void JsonArray::AddElement(JsonArray* pValue)
	{
		m_elements.push_back(JsonValue(pValue));
	}

	const std::vector<JsonValue>& JsonArray::GetElements() const
	{
		return m_elements;
	}
}
