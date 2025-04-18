/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Container/ContainerId.h"

#include <cstdint>
#include <string>

namespace Systems
{
	// The asset id is a 64 bits number made of 2 parts : the container id and the object id.
	// The container id is made of the 56 lsb of the asset id. The container id identify uniquely a container
	// in the entire project.
	// The object id is made of the 8 msb of the asset id. The object id identify an object inside a container.
	class NewAssetId
	{
	public:
		NewAssetId();
		explicit NewAssetId(uint64_t id);
		explicit NewAssetId(ContainerId containerId, uint64_t objectId );
		~NewAssetId();

		std::string ToString() const;

		ContainerId GetContainerId() const;
		uint64_t GetObjectId() const;

		bool operator<(NewAssetId other) const;

		static const NewAssetId INVALID;
	private:
		uint64_t m_id;
	};
}
