/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetRef/HardAssetRefRaw.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"

namespace Systems
{
	HardAssetRefRaw::HardAssetRefRaw()
		: m_id()
		, m_pPtr(nullptr)
		, m_domain(LoadingDomain::UNKNOWN)
	{ }

	HardAssetRefRaw::HardAssetRefRaw(NewAssetId id)
		: m_id(id)
		, m_pPtr(nullptr)
		, m_domain(LoadingDomain::UNKNOWN)
	{ }

	HardAssetRefRaw::~HardAssetRefRaw()
	{
		AssetUtil::UnloadAsset(m_id, m_domain);
		m_id = NewAssetId::INVALID;
		m_pPtr = nullptr;
	}

	bool HardAssetRefRaw::Resolve(LoadingDomain domain)
	{
		m_domain = domain;
		m_pPtr = AssetUtil::GetAsset(m_id, domain);
		return m_pPtr != nullptr;
	}

	bool HardAssetRefRaw::IsResolved() const
	{
		return m_pPtr != nullptr;
	}

	NewAssetId HardAssetRefRaw::GetAssetId() const
	{
		return m_id;
	}

	bool HardAssetRefRaw::Load(LoadingDomain domain)
	{
		m_domain = domain;
		m_pPtr = AssetUtil::LoadAsset(m_id, domain);
		return IsResolved();
	}
}
