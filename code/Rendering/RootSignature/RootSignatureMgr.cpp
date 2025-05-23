/********************************************************************/
/* � 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

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

	RootSignatureId RootSignatureMgr::CreateRootSignature(const std::string& path)
	{
		RootSignatureId id;
		id.m_id = m_rootSignatures.size();
		m_rootSignatures.push_back(new RootSignature(path));
		return id;
	}
	RootSignature* RootSignatureMgr::GetRootSignature(RootSignatureId id) const
	{
		assert(id.m_id < m_rootSignatures.size());
		return m_rootSignatures[id.m_id];
	}
}
