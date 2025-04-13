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

	// Class to serialize an object in json using the reflection
	class ObjectJsonSerializer
	{
	public:
		ObjectJsonSerializer() = default;
		~ObjectJsonSerializer() = default;

		bool Serialize(const Object* pObject, Core::JsonObject& jsonObj);
	};
}
