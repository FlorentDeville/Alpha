/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <vector>

namespace Core
{
	class JsonArray;
	class JsonMember;

	class JsonObject
	{
	public:
		JsonObject();
		~JsonObject();

		void AddMember(const std::string& name, int32_t value);
		void AddMember(const std::string& name, float value);
		void AddMember(const std::string& name, bool value);
		void AddMember(const std::string& name, const char* pValue);
		void AddMember(const std::string& name, const std::string& value);

		JsonObject* AddObject(const std::string& name);
		JsonArray* AddArray(const std::string& name);

	private:
		std::vector<JsonMember*> m_members;
	};
}
