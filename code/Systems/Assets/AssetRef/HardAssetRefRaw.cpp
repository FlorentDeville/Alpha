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

	HardAssetRefRaw::HardAssetRefRaw(const HardAssetRefRaw& copy)
		: m_id(copy.m_id)
		, m_pPtr(copy.m_pPtr)
		, m_domain(copy.m_domain)
	{
		if (m_id != NewAssetId::INVALID)
			ContainerMgr::Get().IncRefCount(m_id.GetContainerId(), m_domain);
	}

	HardAssetRefRaw::~HardAssetRefRaw()
	{
		if(m_id != NewAssetId::INVALID)
			AssetUtil::UnloadAsset(m_id, m_domain);

		m_id = NewAssetId::INVALID;
		m_pPtr = nullptr;
	}

	HardAssetRefRaw& HardAssetRefRaw::operator=(const HardAssetRefRaw& other)
	{
		new (this) HardAssetRefRaw(other);
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
