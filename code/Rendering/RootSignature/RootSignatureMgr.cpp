/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/RootSignature/RootSignatureMgr.h"

#include "RootSignature.h"
#include "RootSignatureId.h"

#include <assert.h>

namespace Rendering
{
	RootSignatureMgr::RootSignatureMgr()
		: Core::Singleton<RootSignatureMgr>()
		, m_rootSignatures()
	{}

	RootSignatureMgr::~RootSignatureMgr()
	{
		for (RootSignature* pRootSignature : m_rootSignatures)
			delete pRootSignature;
	}

	void RootSignatureMgr::Init(const std::string& binPath)
	{
		m_rootSignatures.resize(static_cast<size_t>(EngineRootSigs::COUNT));

		m_rootSignatures[static_cast<size_t>(EngineRootSigs::SHADOWMAP_SPOTLIGHT)] = new RootSignature(binPath + "\\shadowmap.rs.cso");
		m_rootSignatures[static_cast<size_t>(EngineRootSigs::SHADOWMAP_DIRLIGHT)] = new RootSignature(binPath + "\\shadowmap_dirlight.rs.cso");
	}

	void RootSignatureMgr::Shutdown()
	{ }

	RootSignature* RootSignatureMgr::CreateRootSignature(const std::string& path, RootSignatureId& id)
	{
		id.m_id = m_rootSignatures.size();
		RootSignature* pRootSignature = new RootSignature(path);
		m_rootSignatures.push_back(pRootSignature);
		return pRootSignature;
	}

	RootSignature* RootSignatureMgr::GetRootSignature(RootSignatureId id) const
	{
		assert(id.m_id < m_rootSignatures.size());
		return m_rootSignatures[id.m_id];
	}

	RootSignature* RootSignatureMgr::GetRootSignature(EngineRootSigs id) const
	{
		return m_rootSignatures[static_cast<size_t>(id)];
	}
}
