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
	class Container;

	class ContainerJsonDeserializer
	{
	public:
		ContainerJsonDeserializer() = default;
		~ContainerJsonDeserializer() = default;

		bool Deserialize(const Core::JsonObject& jsonObj, Container& pContainer);
	};
}
