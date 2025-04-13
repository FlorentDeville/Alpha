/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Core
{
	class JsonObject;
}

namespace Systems
{
	class Object;

	class ObjectJsonDeserializer
	{
	public:
		ObjectJsonDeserializer() = default;
		~ObjectJsonDeserializer() = default;

		bool Deserialize(const Core::JsonObject& jsonObject, Object** ppObject);
	};
}
