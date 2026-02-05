/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetRef/HardAssetRefRaw.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"

namespace Systems
{
	HardAssetRefRaw::HardAssetRefRaw()
		: m_id()
		, m_pPtr(nullptr)
	{ }

	HardAssetRefRaw::HardAssetRefRaw(NewAssetId id)
		: m_id(id)
		, m_pPtr(nullptr)
	{ }

	HardAssetRefRaw::~HardAssetRefRaw()
	{
		m_id = NewAssetId::INVALID;
		m_pPtr = nullptr;
	}

	bool HardAssetRefRaw::Resolve()
	{
		m_pPtr = AssetUtil::GetAsset(m_id);
		return m_pPtr != nullptr;
	}

	bool HardAssetRefRaw::IsResolved() const
	{
		return m_pPtr != nullptr;
	}
}
