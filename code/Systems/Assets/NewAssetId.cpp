/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/NewAssetId.h"

#include "Core/String/BytesToHexa.h"

namespace Systems
{
	const NewAssetId NewAssetId::INVALID = NewAssetId();

	NewAssetId::NewAssetId()
		: m_id(-1)
	{ }

	NewAssetId::NewAssetId(uint64_t id)
		: m_id(id)
	{ }

	NewAssetId::NewAssetId(ContainerId containerId, uint64_t objectId)
	{
		m_id = containerId.ToUint64() | objectId << 56;
	}

	NewAssetId::~NewAssetId()
	{ }

	ContainerId NewAssetId::GetContainerId() const
	{
		return ContainerId(m_id & 0x00FFFFFFFFFFFFFF);
	}

	uint64_t NewAssetId::GetObjectId() const
	{
		return (m_id & 0xFF00000000000000) >> 56;
	}

	std::string NewAssetId::ToString() const
	{
		return Core::Uint64ToHexa(m_id);
	}

	bool NewAssetId::operator<(NewAssetId other) const
	{
		return m_id < other.m_id;
	}
}
