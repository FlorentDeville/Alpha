/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>
#include <string>

namespace Systems
{
	// A container id is a unique identifier for container. It uses the first 56 lsb
	// which gives us 2^56 possible value.
	// The top 8 bits are reserved for ObjectId. An ObjectId is local to a container and represents
	// the id of a root object in the package. It means a container can store 2^8 = 256 root objects.
	// The ObjectId and ContainerId together makes the AssetId.
	class ContainerId
	{
	public:
		ContainerId();
		explicit ContainerId(uint64_t id);
		explicit ContainerId(const char* strId);
		~ContainerId() = default;

		bool operator==(ContainerId other) const;
		bool operator!=(ContainerId other) const;
		bool operator<(ContainerId other) const;

		bool IsValid() const;

		std::string ToString() const;
		uint64_t ToUint64() const;

		static const ContainerId INVALID;

	private:
		uint64_t m_id;
	};
}
