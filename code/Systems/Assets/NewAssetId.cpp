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

	NewAssetId::NewAssetId(ContainerId containerId, ObjectId objectId)
	{
		m_id = containerId.ToUint64() | objectId.ToUint64() << 56;
	}

	NewAssetId::~NewAssetId()
	{ }

	ContainerId NewAssetId::GetContainerId() const
	{
		return ContainerId(m_id & 0x00FFFFFFFFFFFFFF);
	}

	ObjectId NewAssetId::GetObjectId() const
	{
		return ObjectId((m_id & 0xFF00000000000000) >> 56);
	}

	bool NewAssetId::IsValid() const
	{
		return *this != NewAssetId::INVALID;
	}

	std::string NewAssetId::ToString() const
	{
		return Core::Uint64ToHexa(m_id);
	}

	uint64_t NewAssetId::ToUint64() const
	{
		return m_id;
	}

	bool NewAssetId::operator<(NewAssetId other) const
	{
		return m_id < other.m_id;
	}

	bool NewAssetId::operator==(NewAssetId other) const
	{
		return m_id == other.m_id;
	}

	bool NewAssetId::operator!=(NewAssetId other) const
	{
		return !(*this == other);
	}
}
