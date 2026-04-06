/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetRef/HardAssetRefRaw.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Container/ContainerMgr.h"

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
		if(m_id != NewAssetId::INVALID)
			AssetUtil::UnloadAsset(m_id, m_domain);

		m_id = NewAssetId::INVALID;
		m_pPtr = nullptr;
	}

	HardAssetRefRaw& HardAssetRefRaw::operator=(const HardAssetRefRaw& other)
	{
		if (IsResolved())
			AssetUtil::UnloadAsset(m_id, m_domain);

		m_id = other.m_id;
		m_pPtr = other.m_pPtr;
		if (IsResolved())
			ContainerMgr::Get().IncRefCount(other.m_id.GetContainerId(), m_domain);

		return *this;
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
