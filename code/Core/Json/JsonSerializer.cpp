/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Json/JsonSerializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonMember.h"
#include "Core/Json/JsonObject.h"

#include <assert.h>
#include <vector>

namespace Core
{
	void WriteArray(const JsonArray& array, std::stringstream& output);
	void WriteObject(const JsonObject& input, std::stringstream& output);

	void WriteBoolean(const JsonValue& value, std::stringstream& output)
	{
		if (value.GetValueAsBool())
			output << "true";
		else
			output << "false";
	}

	void WriteNumber(const JsonValue& value, std::stringstream& output)
	{
		output << value.GetValueAsDouble();
	}

	void WriteString(const JsonValue& value, std::stringstream& output)
	{
		output << "\"" << value.GetValueAsString() << "\"";
	}

	void WriteNull(std::stringstream& output)
	{
		output << "null";
	}

	void WriteValue(const JsonValue& value, std::stringstream& output)
	{
		switch (value.GetType())
		{
		case JsonType::Boolean:
			WriteBoolean(value, output);
			break;

		case JsonType::Number:
			WriteNumber(value, output);
			break;

		case JsonType::String:
			WriteString(value, output);
			break;

		case JsonType::Array:
			WriteArray(*value.GetValueAsArray(), output);
			break;

		case JsonType::Object:
			WriteObject(*value.GetValueAsObject(), output);
			break;

		case JsonType::Null:
			WriteNull(output);
			break;

		default:
			assert(false);
			break;
		}
	}

	void WriteArray(const JsonArray& array, std::stringstream& output)
	{
		output << "[\n";

		bool isFirst = true;

		const std::vector<JsonValue>& elements = array.GetElements();
		for (const JsonValue& value : elements)
		{
			if (isFirst)
				isFirst = false;
			else
				output << ",\n";

			WriteValue(value, output);
		}

		output << "\n]";
	}

	void WriteObject(const JsonObject& input, std::stringstream& output)
	{
		output << "{ \n";

		bool isFirst = true;
		const std::vector<JsonMember*>& members = input.GetMembers();
		for (const JsonMember* pMember : members)
		{
			if (isFirst)
				isFirst = false;
			else
				output << ", \n";
	
			output << "\"" << pMember->GetName() << "\": ";
			WriteValue(pMember->GetValue(), output);
		}

		output << "\n}";
	}

	JsonSerializer::JsonSerializer()
		: m_stream()
	{}

	JsonSerializer::~JsonSerializer()
	{}

	bool JsonSerializer::Serialize(const JsonObject& input, std::string& output)
	{
		m_stream.clear();
		WriteObject(input, m_stream);

		output = m_stream.str();
		return true;
	}
}
