/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Json/JsonType.h"

#include <string>
#include <vector>

namespace Core
{
	class JsonObject;
	class JsonValue;

	class JsonArray
	{
	public:
		JsonArray();
		~JsonArray();

		void AddElement(int32_t value);
		void AddElement(float value);
		void AddElement(double value);
		void AddElement(bool value);
		void AddElement(const std::string& value);
		void AddElement(JsonObject* pValue);
		void AddElement(JsonArray* pValue);

		const std::vector<JsonValue*>& GetElements() const;

	private:
		std::vector<JsonValue*> m_elements;
	};
}
