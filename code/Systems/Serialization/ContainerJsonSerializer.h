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

	class ContainerJsonSerializer
	{
	public:
		ContainerJsonSerializer() = default;
		~ContainerJsonSerializer() = default;

		bool Serialize(const Container* pContainer, Core::JsonObject& pJsonObject);
	};
}
